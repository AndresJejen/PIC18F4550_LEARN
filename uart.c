#include "uart.h"

void UART_Init() {
    // Set baud rate to 9600
    SPBRG = 12;           // Para 9600 bps con 8MHz y BRGH = 0
    TXSTAbits.BRGH = 0;   // Baja velocidad
    RCSTAbits.SPEN = 1;   // Habilita el módulo serial
    TXSTAbits.TXEN = 1;   // Habilita transmisor
    RCSTAbits.CREN = 1;   // Habilita receptor continuo
    
    // Set RC6/RC7 as digital and UART pins
    TRISCbits.TRISC6 = 0;      // TX as output
    TRISCbits.TRISC7 = 1;      // RX as input
}

void UART_Write(char data) {
    while (!TXSTAbits.TRMT); // Esperar a que el buffer esté libre
    TXREG = data;
}

void UART_Write_Text(const char* text) {
    while (*text != '\0') {
        UART_Write(*text++);
    }
}

char UART_Read(void) {
    while (!PIR1bits.RCIF); // Esperar dato
    return RCREG; // Retornar dato recibido
}
