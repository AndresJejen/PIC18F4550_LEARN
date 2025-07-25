// keypad.c
#include "keypad.h"

const char keymap[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

void keypad_init(void) {
    TRISB = 0xF0;  // RB4?RB7 inputs (cols), RB0?RB3 outputs (rows)
    LATB = 0x0F;   // Initialize rows high
}

char keypad_get_key() {
    for (char row = 0; row < 4; row++) {
        LATB = ~(1 << row); // pull one row low, others high
        __delay_us(5);

        for (char col = 0; col < 4; col++) {
            if (!(PORTB & (1 << (col + 4)))) { // column pin LOW = key pressed
                while (!(PORTB & (1 << (col + 4)))) ; // wait for release
                __delay_ms(10);
                return keymap[row][col];
            }
        }
    }
    return 0;
}
