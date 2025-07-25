#include <xc.h>
#include <stdio.h>     // ? THIS is required for sprintf()
#include <stdint.h>
#include "motor.h"
#include "lcd.h"
#include "keypad.h"
#include "display.h"
#include "rgb.h"
#include "adc.h"
#include "uart.h"
#include "timer.h"

#define _XTAL_FREQ 8000000
#define LED_LBK_LAT LATAbits.LATA3
#define LED_LBK_TRIS TRISAbits.TRISA3
#define INPUT_TRIS TRISCbits.TRISC1

unsigned char piezas_contadas = 0;
unsigned char piezas_objetivo = 0;
char *status = "welcome";
volatile unsigned int led_estado = 0;
volatile unsigned int led_lbk_estado = 0;
volatile unsigned char int0_triggered = 0;
volatile unsigned char t3_interrupts = 0;

#pragma config FOSC = INTOSCIO_EC
#pragma config WDT = OFF
#pragma config PWRT = ON
#pragma config MCLRE = ON
#pragma config LVP = OFF
#pragma config PBADEN = OFF

void motor_control_with_level(unsigned int value);

void IntToString(unsigned int value, char* buffer) {
    sprintf(buffer, "Valor del ADC: %u\r\n", value); // Añade retorno de carro y salto de línea
}

void check_reset_cause() {
    if (RCONbits.POR) {
        lcd_clear();
        lcd_set_cursor(1, 1);
        lcd_write("Falla de energia");
    } else if (RCONbits.RI) {
        lcd_clear();
        lcd_set_cursor(1, 1);
        lcd_write("Falla de energia");
    }
    
    // Limpiar flags
    RCONbits.POR = 0;
    RCONbits.RI = 0;
}

void set_status(char *new_status) {
    status = new_status;
    send_display(0);
    if (new_status == "welcome"){
        send_rgb(6); // NEGRO
    }
    else if (new_status == "ask") {
        piezas_contadas = 0;
        send_rgb(6); // NEGRO
    }
    else if (new_status == "fail_input") {
        send_rgb(6); // NEGRO
    }
    else if (new_status == "count") {
        piezas_contadas = 0;
        count_update_screen(piezas_objetivo, piezas_contadas);
        send_rgb(0); // NEGRO
    }
    else if (new_status == "end_count") {
        send_rgb(6); // NEGRO
    }
    else if (new_status == "stop_emergency") {
        send_rgb(7); // ROJO
        lcd_clear();
        lcd_set_cursor(1, 1);
        lcd_write("  !!!PARADA!!!  ");
        lcd_set_cursor(2, 1);
        lcd_write("!!!EMERGENCIA!!!");
        motor_control(0);
        // Entra a modo SLEEP
        Sleep();
    }
}

unsigned char get_valid_input_count(void) {
    char buffer[3] = {0};  // para 2 dígitos + '\0'
    char key;
    int index;
    int value;

    while (status == "ask") {
        index = 0;
        lcd_clear();
        lcd_write("Piezas a contar:");
        lcd_set_cursor(2, 1);
        lcd_cmd(0x0E);  // Cursor ON

        while (status == "ask") {
            key = keypad_get_key();
            if (key >= '0' && key <= '9' && index < 2) {
                buffer[index++] = key;
                lcd_data(key);
            } else if (key == 'C' && index > 0) {  // tecla delete last
                lcd_set_cursor(2, index);
                lcd_data(' ');
                lcd_set_cursor(2, index);
                index--;
            } else if (key == 'A') {  // tecla OK
                buffer[index] = '\0';  // terminar cadena
                value = atoi(buffer);  // convertir a número
                if (value >= 1 && value <= 59) {
                    lcd_cmd(0x0C); // cursor OFF
                    set_status("count");
                    piezas_objetivo = value;
                } else {
                    set_status("fail_input");
                    lcd_clear();
                    lcd_write("Valor Invalido");
                    lcd_set_cursor(2, 1);
                    lcd_write("Solo 1 a 59");
                    __delay_ms(5000);
                    set_status("ask");
                    break;  // volver a pedir
                }
            } else if (key == 'B') {
                set_status("stop_emergency");
            }
            else if (key == 'D') {
                led_lbk_estado ^= 1;             // Toggle LED state
                LED_LBK_LAT = led_lbk_estado;        // Write to LED pin
            }
            if (PIR1bits.RCIF) {
                char c = UART_Read();
                UART_Write(c);  // Eco al terminal
                handle_serial_command(c);
            }
        }
    }
}

void handle_serial_command(char cmd) {
    switch(cmd) {
        case 'P':
        case 'p':
            set_status("stop_emergency");
            break;
        case 'E':
        case 'e':
            // Encender motor (si no está en emergencia)
            motor_control(1);
            break;
        case 'A':
        case 'a':
            motor_control(0);
            // Apagar motor (si no está en emergencia)
            break;
        case 'R':
        case 'r':
            if (status == "count") {
                set_status("count");
            }
            // Reiniciar conteo (si está contando)
            break;
        default:
            // Comando no reconocido
            break;
    }
}

void __interrupt() isr(void) {
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;          // Clear Timer1 interrupt flag

        // Reload Timer1 (for 1s delay with 8MHz and prescaler 1:8)
        TMR1H = 0x0B;                 // High byte
        TMR1L = 0xDC;                 // Low byte  => preload = 0x0BDC = 3036

        led_estado ^= 1;             // Toggle LED state
        LED_LAT = led_estado;        // Write to LED pin
        
        t3_interrupts++;
        if (t3_interrupts >= 2) {  // 2 * 250ms = 1 second
            char buffer[20];
            unsigned int value = adc_read();
            motor_control_with_level(value);
            IntToString(value, buffer);
            UART_Write_Text(buffer);             // Enviar por RS232
        }
    }
}

void loop(void) {
    while(status == "ask") {
        get_valid_input_count();
        count_update_screen(piezas_objetivo, piezas_contadas);
        
        while (1) {
            if (PIR1bits.RCIF) {
                char c = UART_Read();
                UART_Write(c);  // Eco al terminal
                handle_serial_command(c);
            }
            char key = keypad_get_key();
            if (key == '*') { // REINICIAR
                set_status("count");
            }
            if (key == '#') { // REINICIAR
                set_status("ask");
                break;
            }
            else if (key == 'D') {
                led_lbk_estado ^= 1;             // Toggle LED state
                LED_LBK_LAT = led_lbk_estado;        // Write to LED pin
            }
            else if (key == 'A' && status == "end_count") {  // OK
                set_status("ask");
                break;
            }
            else if (key == 'B') {  // STOP
                set_status("stop_emergency");
            }
            
            if ((piezas_contadas >= piezas_objetivo) && status == "count") {
                set_status("end_count");
                lcd_clear();
                lcd_set_cursor(1, 1);
                lcd_write("ObjetivoCumplido");
                lcd_set_cursor(2, 1);
                lcd_write("Presione OK...");
                lcd_data(1);
            }
            
            if (PORTCbits.RC1 == 1 && int0_triggered == 0) {
                int0_triggered = 1;
                if (piezas_contadas < piezas_objetivo) {
                    piezas_contadas++;
                    count_update_screen(piezas_objetivo, piezas_contadas);
                }
            }
            else if (PORTCbits.RC1 == 0 && int0_triggered == 1) {
                int0_triggered = 0;
            }
        }
    }
}

void main(void) {
    //OSCCON = 0b01110000;
    OSCCON = 0b01110010;
    while (!OSCCONbits.IOFS);
    __delay_ms(50);
    
    INPUT_TRIS = 1;
    LED_LBK_TRIS = 0;
    LED_LBK_LAT = 0;
    lcd_init();
    keypad_init();
    timer1_init();
    adc_init();
    motor_init();
    UART_Init();
    
    check_reset_cause();
    __delay_ms(5000);
    lcd_clear();
    lcd_set_cursor(1, 1);
    lcd_write("Bienvenido a ");
    lcd_data(0);

    lcd_set_cursor(2, 1);
    lcd_write("contador play...");
    __delay_ms(5000);
    set_status("ask");
    loop();

}
