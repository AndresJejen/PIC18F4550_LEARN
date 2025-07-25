
#include "display.h"

void display_init(void) {
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    __delay_ms(20);
    send_display(0);
}

void send_display(unsigned char units) {
    D0 = (units >> 1) & 1;
    D1 = (units >> 2) & 1;
    D2 = (units >> 3) & 1;
    D3 = (units >> 0) & 1;
}
