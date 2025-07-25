#include "adc.h"

void adc_init(void) {
    TRISAbits.TRISA0 = 1;    // RA0 como entrada
    ADCON1 = 0x0E;     // RA0 como entrada analógica (AN0), Vref = Vdd
    ADCON2 = 0b10110110; // A/D result right-justified, 20 Tad, Fosc/64
    ADCON0 = 0b00000001; // Selecciona canal AN0, enciende el módulo ADC
}

unsigned int adc_read(void) {
    ADCON0bits.GO = 1;             // Inicia conversión
    while (ADCON0bits.GO);         // Espera que termine
    return ((ADRESH << 8) + ADRESL);  // Devuelve valor de 10 bits
}