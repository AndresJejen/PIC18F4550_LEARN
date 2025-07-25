// lcd.h
#ifndef LCD_H
#define LCD_H

#include <xc.h>
#define _XTAL_FREQ 8000000  // Required for delays

// === LCD pin definitions ===
#define RS LATAbits.LATA4
#define EN LATAbits.LATA5
#define D4 LATDbits.LATD4
#define D5 LATDbits.LATD5
#define D6 LATDbits.LATD6
#define D7 LATDbits.LATD7

// === LCD Function Prototypes ===
void lcd_init(void);
void lcd_clear(void);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_set_cursor(unsigned char row, unsigned char col);
void lcd_write(const char *str);
void lcd_create_char(unsigned char location, unsigned char *charmap);
void load_custom_characters(void);


#endif
