#include "util.h"

#include <util/delay.h>

bool button_isdown(button_t button)
{
    return !(PIND & button);
}

void bpmc_send(uint8_t *data, uint8_t count)
{
    DDRB |= 0x01;
    for (uint8_t i = count; i; --i)
    {
        uint8_t byte = *data;
        ++data;
        for (uint8_t bit = 8; bit; --bit)
        {
            PORTB ^= 0x01;
            _delay_us(4);
            if (byte & 0x01)
                PORTB ^= 0x01;
            byte >>= 1;
            _delay_us(4);
        }
    }
}

void bpmc_sendstring(const char *str)
{
    const char *ptr = str;
    uint8_t length = 0;
    while (*ptr++)
        ++length;
    bpmc_send((uint8_t*)str, length);
}

char* fxp88toa(uint16_t value)
{
    uint32_t x = ((uint32_t)value) * 0x3d09 / 4000;  // 0xf42 = 1000000 / 256
    bool onleft = true;
    static char outputbuffer[8];
    char *optr = outputbuffer;
    uint32_t order = 100000;
    while (order)
    {
        if (x > order || !onleft || order <= 1000)
        {
            uint8_t digit = x / order;
            *optr++ = '0' + (char)digit;
            x -= digit * order;
            onleft = false;
        }
        order /= 10;
        if (order == 100)
            *optr++ = '.';
    }
    *optr = 0;
    return outputbuffer;
}

char* itoa(uint16_t value)
{
    uint16_t order = 10000;
    static char outputbuffer[6];
    char *optr = outputbuffer;
    bool onleft = true;
    while (order)
    {
        if (value >= order || !onleft || order == 1)
        {
             uint8_t digit = value / order;
            *optr++ = '0' + (char)digit;
            value -= digit * order;
            onleft = false;
        }
        order /= 10;
    }
    *optr = 0;
    return outputbuffer;
}

char* ftoa(float value)
{
    bool onleft = true;
    static char outputbuffer[10];
    char *optr = outputbuffer;
    float order = 10000.f;
    uint8_t i = 0;
    if (value < 0.f)
    {
        *optr++ = '-';
        value = -value;
    }
    while (order > 0.0009f)
    {
        if (value >= order || !onleft || order <= 1.f)
        {
            uint8_t digit = value / order;
            *optr++ = '0' + (char)digit;
            value -= digit * order;
            onleft = false;
        }
        order *= 0.1f;
        ++i;
        if (i == 5)
            *optr++ = '.';
    }
    *optr = 0;
    return outputbuffer;
}
