/* 
 * File:   uart.h
 * Author: andresjc
 *
 * Created on July 25, 2025, 12:19 AM
 */

#ifndef UART_H
#define	UART_H

#include <xc.h>
#define _XTAL_FREQ 8000000

void UART_Init(void);

void UART_Write(char data);

void UART_Write_Text(const char* text);

void handle_serial_command(char cmd);

char UART_Read(void);

#endif	/* UART_H */

