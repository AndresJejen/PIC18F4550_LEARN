#include "rgb.h"

void rgb_init(void) {
    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
    send_rgb(6);
    __delay_ms(20);
}

void send_rgb(unsigned char units) {
    if (units == 0) { // magenta = RED + BLUE
        RED = 1;
        GREEN = 0;
        BLUE = 1;
    }
    else if (units == 1) { // azul (blue)
        RED = 0;
        GREEN = 0;
        BLUE = 1;
    }
    else if (units == 2) { // cyan = GREEN + BLUE
        RED = 0;
        GREEN = 1;
        BLUE = 1;
    }
    else if (units == 3) { // verde (green)
        RED = 0;
        GREEN = 1;
        BLUE = 0;
    }
    else if (units == 4) { // amarillo (yellow) = RED + GREEN
        RED = 1;
        GREEN = 1;
        BLUE = 0;
    }
    else if (units == 5) { // blanco (white) = RED + GREEN + BLUE
        RED = 1;
        GREEN = 1;
        BLUE = 1;
    }
    else if (units == 6) { // START
        RED = 0;
        GREEN = 0;
        BLUE = 0;
    }
    else if (units == 7) { // STOP
        RED = 1;
        GREEN = 0;
        BLUE = 0;
    }
}