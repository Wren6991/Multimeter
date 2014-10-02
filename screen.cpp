#include "screen.h"

#include "spi.h"

#include <util/delay.h>
#include <avr/pgmspace.h>

#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data"))) // Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734#c4
#ifdef PSTR
#undef PSTR
#define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];})) // Copied from pgmspace.h in avr-libc source
#endif
#endif

#include "font_35.inc"
#include "font_digits.inc"

void scrn_backlight(bool onoff)
{
    DDRA |= 0x01;
    if (!onoff)
    {
        PORTA |= 0x01;
    }
    else
    {
        PORTA &= ~0x01;
    }
}

inline void scrn_dc(bool dc)
{
    DDRC |= 0x40;
    if (dc)
        PORTC |= 0x40;
    else
        PORTC &= ~0x40;
}

void scrn_assert_reset(bool onoff)
{
    DDRC |= 0x80;
    if (onoff)
    {
        PORTC &= ~0x80; // active low signal
    }
    else
    {
        PORTC |= 0x80;
    }
}

void scrn_data8(uint8_t data)
{
    scrn_dc(1);
    spi_send_byte(data);
}

void scrn_cmd(uint8_t cmd)
{
    scrn_dc(0);
    spi_send_byte(cmd);
}

inline void scrn_address_x(uint8_t x)
{
    if (x < 84)
        scrn_cmd(0x80 | x);
    else
        scrn_cmd(0x80 | 83);
}

inline void scrn_address_y(uint8_t y)
{
    if (y < 6)
        scrn_cmd(0x40 | y);
    else
        scrn_cmd(0x40 | 5);
}

void scrn_address(uint8_t x, uint8_t y)
{
    scrn_address_x(x);
    scrn_address_y(y);
}

void scrn_print_char(char c, uint8_t x, uint8_t y)
{
    scrn_address_y(y);
    uint16_t byteaddress = (uint16_t)font_88 + (((uint16_t)c) << 2);
    for (uint8_t i = 0; i < 4; ++i)
    {
        scrn_address_x(x);
        scrn_data8(pgm_read_byte(byteaddress));
        ++x;
        ++byteaddress;
    }
}

void scrn_print(const char *str, uint8_t x, uint8_t y)
{
    char c;
    //uint8_t xstart = x;
    while ((c = *str++))
    {
        /*if (c == '\n')
        {
            x = xstart;
            y++;
            if (y >= 6)
                break;
        }
        else */if (x < 84)
        {
            scrn_print_char(c, x, y);
            x += 4;
        }
    }
}

void scrn_print_digits(const char *str, uint8_t x, uint8_t y)
{
    char buffer[11];
    char c;
    char *bufptr = buffer;
    uint8_t bufcount = 0;
    while (++bufcount <= 10 && (c = *str++))
    {
        if (c >= '0' && c <= '9')
            c -= 48;
        else if (c == '.')
            c = 10;
        else if (c == 'V')  // upper case only!
            c = 11;
        else if (c == 'A')
            c = 12;
        else if (c == 'O')
            c = 13;
        else if (c == 'F')
            c = 14;
        else if (c == 'n')
            c = 15;
        else if (c == 'u')
            c = 16;
        else if (c == 'k')
            c = 17;
        else if (c == 'M')
            c = 18;
        else
            c = 10;
        *bufptr++ = c + 1;
    }
    *bufptr = 0;
    bufptr = buffer;
    uint8_t xstart = x;

    scrn_address_y(y);
    while ((c = *bufptr++))
    {
        --c;
        uint16_t byteaddress = (uint16_t)font_digits + (((uint16_t)c) << 3);
        for (uint8_t i = 0; i < 8; ++i)
        {
            scrn_address_x(x);
            scrn_data8(pgm_read_byte(byteaddress));
            x++;
            byteaddress++;
        }
    }

    bufptr = buffer;
    x = xstart;
    scrn_address_y(y + 1);
    while ((c = *bufptr++))
    {
        --c;
        uint16_t byteaddress = (uint16_t)font_digits + (n_font_digit << 3) + (((uint16_t)c) << 3);
        for (uint8_t i = 0; i < 8; ++i)
        {
            scrn_address_x(x);
            scrn_data8(pgm_read_byte(byteaddress));
            x++;
            byteaddress++;
        }
    }
}

void scrn_clear()
{
    for (uint8_t y = 0; y < 6; ++y)
    {
        scrn_address_y(y);
        for (uint8_t x = 0; x < 84; ++x)
        {
            scrn_address_x(x);
            scrn_data8(0);
        }
    }
}

void scrn_init()
{
    scrn_assert_reset(true);
    spi_init();
    spi_deassert();
    _delay_ms(10);
    scrn_assert_reset(false);
    _delay_ms(10);
    spi_assert(spi_assert_lcd);

    uint8_t init_seq[] = {0x21, 0xa2, 0x04, 0x12, 0x20, 0x0c};
    for (uint8_t i = 0; i < 6; ++i)
        scrn_cmd(init_seq[i]);

    scrn_clear();
}

