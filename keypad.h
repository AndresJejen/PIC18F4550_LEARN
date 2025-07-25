// keypad.h
#ifndef KEYPAD_H
#define KEYPAD_H

#include <xc.h>
#define _XTAL_FREQ 8000000

void keypad_init(void);
char keypad_get_key(void);

#endif
