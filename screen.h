#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <avr/io.h>

void scrn_assert_reset(bool onoff);
void scrn_backlight(bool onoff);
void scrn_cmd(uint8_t cmd);
void scrn_data8(uint8_t data);
void scrn_address(uint8_t x, uint8_t y);
void scrn_print_char(char c, uint8_t x = 0, uint8_t y = 0);
void scrn_print(const char *str, uint8_t x = 0, uint8_t y = 0);
void scrn_print_digits(const char *str, uint8_t x, uint8_t y);
void scrn_clear();
void scrn_init();

#endif // _SCREEN_H_
