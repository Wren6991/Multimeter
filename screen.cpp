#include "screen.h"

#include <util/delay.h>

const int s_rs = 0x80;
const int s_enable = 0x40;

uint8_t screensignal = 0;

void shiftout(uint8_t data)
{
    PORTB &= ~0x0c;
    uint8_t i = 8;
    while (i--)
    {
        PORTB = (PORTB & ~0x02) | ((data & 0x80) >> 6);
        data <<= 1;
        PORTB |= 0x04;
        PORTB &= ~0x04;
    }
    PORTB |= 0x88;
}

void scrn_write4bits(uint8_t data)
{
    shiftout(screensignal | data);
    //_delay_us(20);
    shiftout(screensignal | data | s_enable);
    //_delay_us(20);
    shiftout(screensignal | data);
    //_delay_us(20);
}

void scrn_write8bits(uint8_t data)
{
    scrn_write4bits(data >> 4);
    scrn_write4bits(data & 0x0f);
}

void scrn_cursorpos(uint8_t x, uint8_t y)
{
    screensignal = 0x00;
    uint8_t rows[] = {0, 64, 20, 84};
    scrn_write8bits((x + rows[y]) | 0x80);
    screensignal = s_rs;
}

void scrn_clear()
{
    scrn_cursorpos(0, 0);
    for (int i = 16; i; --i)
        scrn_write8bits(' ');
    scrn_cursorpos(0, 1);
    for (int i = 16; i; --i)
        scrn_write8bits(' ');
    scrn_cursorpos(0, 0);
}

void scrn_print(const char *str)
{
    char c;
    while ((c = *str++))
        scrn_write8bits(c);
}

void scrn_wake()
{
    DDRB |= 0x0e;
    screensignal = 0x00;
    scrn_write4bits(0x3);
    _delay_ms(5);
    scrn_write4bits(0x3);
    _delay_ms(3);
    scrn_write4bits(0x3);
    scrn_write4bits(0x2);
    scrn_write8bits(0x28);
    scrn_write8bits(0x0c);
    scrn_write8bits(0x01);
    _delay_ms(2);
    screensignal = s_rs;
}

