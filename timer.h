/* 
 * File:   timer.h
 * Author: andresjc
 *
 * Created on July 25, 2025, 1:29 AM
 */

#ifndef TIMER_H
#define	TIMER_H

#include <xc.h>
#define _XTAL_FREQ 8000000
#define LED_LAT LATAbits.LATA1
#define LED_TRIS TRISAbits.TRISA1

void timer1_init(void);

#endif	/* TIMER_H */