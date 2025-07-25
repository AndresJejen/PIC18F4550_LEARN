#include "motor.h"

void motor_control(unsigned int value) {
    if (value > 512) {
        MOTOR = 1; // Enciende el motor
    } else {
        MOTOR = 0; // Apaga el motor
    }
}

void motor_init(void) {
    MOTOR_TRIS = 0;          // RC0 como salida
    MOTOR = 0;               // Motor apagado inicialmente
}
