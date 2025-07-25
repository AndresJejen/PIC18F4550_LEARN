/* 
 * File:   adc.c
 * Author: andresjc
 *
 * Created on July 24, 2025, 10:41 PM
 */

#ifndef ADC_C
#define	ADC_C

#include <xc.h>
#define _XTAL_FREQ 8000000

void adc_init(void);
unsigned int adc_read(void);

#endif	/* ADC_C */

