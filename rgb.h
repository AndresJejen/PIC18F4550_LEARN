/* 
 * File:   rgb.h
 * Author: andresjc
 *
 * Created on July 24, 2025, 10:02 PM
 */

#ifndef RGB_H
#define	RGB_H


#include <xc.h>
#define _XTAL_FREQ 8000000

#define RED LATEbits.LATE0
#define GREEN LATEbits.LATE1
#define BLUE LATEbits.LATE2

void rgb_init(void);
void send_rgb(unsigned char units);

#endif	/* RGB_H */

