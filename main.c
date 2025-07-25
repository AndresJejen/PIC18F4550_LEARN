#include <xc.h>
#include "lcd.h"
#include "keypad.h"

#define _XTAL_FREQ 8000000
#define LED_LAT LATAbits.LATA1
#define LED_TRIS TRISAbits.TRISA1
#define LED_LBK_LAT LATAbits.LATA3
#define LED_LBK_TRIS TRISAbits.TRISA3
#define INPUT_TRIS TRISCbits.TRISC1

volatile unsigned char piezas_contadas = 0;
unsigned char piezas_objetivo = 0;
char *status = "welcome";
volatile unsigned int led_estado = 0;
volatile unsigned int led_lbk_estado = 0;
volatile unsigned char int0_triggered = 0;

#pragma config FOSC = INTOSCIO_EC
#pragma config WDT = OFF
#pragma config PWRT = ON
#pragma config MCLRE = ON
#pragma config LVP = OFF
#pragma config PBADEN = OFF

void __interrupt() isr(void) {
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;          // Clear Timer1 interrupt flag

        // Reload Timer1 (for 1s delay with 8MHz and prescaler 1:8)
        TMR1H = 0x0B;                 // High byte
        TMR1L = 0xDC;                 // Low byte  => preload = 0x0BDC = 3036

        led_estado ^= 1;             // Toggle LED state
        LED_LAT = led_estado;        // Write to LED pin
    }
}

void set_status(char *new_status) {
    status = new_status;
    if (new_status == "welcome"){
        
    }
    else if (new_status == "ask") {
        
    }
    else if (new_status == "fail_input") {
        
    }
    else if (new_status == "count") {
        
    }
    else if (new_status == "end_count") {
        
    }
    else if (new_status == "stop_emergency") {
        lcd_clear();
        lcd_set_cursor(1, 1);
        lcd_write("  !!!PARADA!!!  ");
        lcd_set_cursor(2, 1);
        lcd_write("!!!EMERGENCIA!!!");
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
                led_estado ^= 1;             // Toggle LED state
                LED_LBK_LAT = led_estado;        // Write to LED pin
            }
        }
    }
}

void count_update_screen(void) {
    // Mostrar mensaje inicial
    lcd_clear();
    lcd_set_cursor(1, 1);
    lcd_write("Objetivo:");
    lcd_set_cursor(1, 11);
    lcd_data((piezas_objetivo / 10) + '0');
    lcd_data((piezas_objetivo % 10) + '0');

    lcd_set_cursor(2, 1);
    lcd_write("Faltan:");
    lcd_set_cursor(2, 9);
    lcd_data(( (piezas_objetivo-piezas_contadas) / 10) + '0');
    lcd_data(( (piezas_objetivo-piezas_contadas) % 10) + '0');
}

void loop(void) {
    while(status == "ask") {
        get_valid_input_count();
        count_update_screen();
        
        while (1) {
            char key = keypad_get_key();
            if (key == '*') { // REINICIAR
                piezas_contadas = 0;
                set_status("count");
                count_update_screen();
            }
            if (key == '#') { // PREGUNTAR DE NUEVO
                piezas_contadas = 0;
                set_status("ask");
                break;
            }
            else if (key == 'D') {
                led_estado ^= 1;             // Toggle LED state
                LED_LBK_LAT = led_estado;        // Write to LED pin
            }
            else if (key == 'A' && status == "end_count") {  // OK
                piezas_contadas = 0;
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
                    count_update_screen();
                }
            }
            else if (PORTCbits.RC1 == 0 && int0_triggered == 1) {
                int0_triggered = 0;
            }
        }
    }
}

void timer_init(void) {
    LED_TRIS = 0;       // Set RC2 as output
    LED_LAT = 0;        // Start LED off

    // Timer1 setup: Fosc = 8MHz -> Ftimer = 1MHz with prescaler 1:8
    T1CON = 0b00110001; // TMR1ON=1, T1CKPS=11 (1:8 prescaler), internal clock

    TMR1H = 0x19;  // High byte of 0x1948
    TMR1L = 0x48;  // Low byte

    PIR1bits.TMR1IF = 0; // Clear interrupt flag
    PIE1bits.TMR1IE = 1; // Enable Timer1 interrupt
    INTCONbits.PEIE = 1; // Enable peripheral interrupts
    INTCONbits.GIE = 1;  // Global interrupt enable
}


void main(void) {
    //OSCCON = 0b01110000;
    OSCCON = 0b01110010;
    while (!OSCCONbits.IOFS);
    __delay_ms(50);
    
    INPUT_TRIS = 1;
    LED_LBK_TRIS = 0;
    lcd_init();
    keypad_init();
    timer_init();
    
    lcd_set_cursor(1, 1);
    lcd_write("Bienvenido a ");
    lcd_data(0);

    lcd_set_cursor(2, 1);
    lcd_write("contador play...");
    __delay_ms(1000);
    set_status("ask");
    loop();

}
