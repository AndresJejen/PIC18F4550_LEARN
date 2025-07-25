#include "timer.h"

void timer1_init(void) {
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
