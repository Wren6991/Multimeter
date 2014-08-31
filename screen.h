#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <avr/io.h>

void scrn_write8bits(uint8_t data);
void scrn_cursorpos(uint8_t x, uint8_t y);
void scrn_clear();
void scrn_print(const char *str);
void scrn_wake();

#endif // _SCREEN_H_
