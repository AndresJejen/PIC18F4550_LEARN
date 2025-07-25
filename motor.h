/* 
 * File:   motor.h
 * Author: andresjc
 *
 * Created on July 24, 2025, 10:44 PM
 */

#ifndef MOTOR_H
#define	MOTOR_H

#include <xc.h>
#define _XTAL_FREQ 8000000

#define MOTOR LATCbits.LATC0
#define MOTOR_TRIS TRISCbits.TRISC0

void motor_control(unsigned int value);
void motor_init(void);

#endif	/* MOTOR_H */

