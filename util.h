#ifndef _MM_UTIL_H_
#define _MM_UTIL_H_

#include <avr/io.h>

enum button_t
{
    BTN_UP = 0x01,
    BTN_BACK = 0x02,
    BTN_FWD = 0x04,
    BTN_DOWN = 0x08
};

bool button_isdown(button_t button);

void bpmc_send(uint8_t *data, uint8_t count);
void bpmc_sendstring(const char *str);

char* fxp88toa(uint16_t value);
char* itoa(uint16_t value);
char* ftoa(float value);

#endif // _MM_UTIL_H_

