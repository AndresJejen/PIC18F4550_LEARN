
// lcd.c
#include "lcd.h"
#include "rgb.h"
#include "display.h"

void lcd_pulse() {
    EN = 1; __delay_us(1);
    EN = 0; __delay_us(100);
}

void lcd_send_nibble(unsigned char nibble) {
    D4 = (nibble >> 0) & 1;
    D5 = (nibble >> 1) & 1;
    D6 = (nibble >> 2) & 1;
    D7 = (nibble >> 3) & 1;
    lcd_pulse();
}

void lcd_cmd(unsigned char cmd) {
    RS = 0;
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd & 0x0F);
    __delay_ms(2);
}

void lcd_data(unsigned char data) {
    RS = 1;
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
    __delay_us(50);
}

void lcd_clear() {
    lcd_cmd(0x01);
    __delay_ms(2);
}

void lcd_set_cursor(unsigned char row, unsigned char col) {
    unsigned char addr = (row == 1) ? 0x80 + (col - 1) : 0xC0 + (col - 1);
    lcd_cmd(addr);
}

void lcd_write(const char *str) {
    while (*str) lcd_data(*str++);
}

void lcd_create_char(unsigned char location, unsigned char *charmap) {
    location &= 0x07;  // Only 0?7 allowed
    lcd_cmd(0x40 | (location << 3));  // Set CGRAM address

    for (int i = 0; i < 8; i++) {
        lcd_data(charmap[i]);
    }
}

void load_custom_characters(void) {
    unsigned char smiley[8] = {
        0b00000,
        0b01010,
        0b01010,
        0b00000,
        0b10001,
        0b01110,
        0b00000,
        0b00000
    };
    
    lcd_create_char(0, smiley);
    unsigned char cool_dude[8] = {
        0b00000,
        0b11111,
        0b10101,
        0b11111,
        0b10001,
        0b11111,
        0b00000,
        0b00000
    };
    lcd_create_char(1, cool_dude);
}

void lcd_init() { 
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;
    
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD6 = 0;
    TRISDbits.TRISD7 = 0;
    __delay_ms(20);
        
    RS = 0;
    lcd_send_nibble(0x03); __delay_ms(5);
    lcd_send_nibble(0x03); __delay_us(100);
    lcd_send_nibble(0x03); __delay_us(100);
    lcd_send_nibble(0x02); __delay_us(100); // 4-bit mode

    lcd_cmd(0x28); // 2-line, 5x8 font
    lcd_cmd(0x0C); // Display ON, cursor OFF
    lcd_cmd(0x06); // Entry mode
    lcd_clear();
    
    load_custom_characters();
    
    display_init();
    rgb_init();
}

void count_update_screen(unsigned char piezas_obj, unsigned char piezas_cont) {
    // Mostrar mensaje inicial
    lcd_clear();
    lcd_set_cursor(1, 1);
    lcd_write("Objetivo:");
    lcd_set_cursor(1, 11);
    lcd_data((piezas_obj / 10) + '0');
    lcd_data((piezas_obj % 10) + '0');

    unsigned char units_pending = ( (piezas_obj-piezas_cont) % 10) + '0';
    unsigned char dec_pending = ( (piezas_obj-piezas_cont) / 10) + '0';
    
    unsigned char units_counted = ( (piezas_cont) % 10);
    unsigned char dec_counted = ( (piezas_cont) / 10);
    
    lcd_set_cursor(2, 1);
    lcd_write("Faltan:");
    lcd_set_cursor(2, 9);
    lcd_data(dec_pending);
    lcd_data(units_pending);
    
    send_display(units_counted);
    send_rgb(dec_counted);
}

