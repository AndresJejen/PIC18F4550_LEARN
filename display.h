/* 
 * File:   display.h
 * Author: andresjc
 *
 * Created on July 24, 2025, 8:02 PM
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#include <xc.h>
#define _XTAL_FREQ 8000000

#define D0 LATDbits.LATD0
#define D1 LATDbits.LATD1
#define D2 LATDbits.LATD2
#define D3 LATDbits.LATD3


void send_display(unsigned char units);
void display_init(void);

#endif	/* DISPLAY_H */

