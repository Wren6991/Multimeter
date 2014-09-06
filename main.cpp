#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "screen.h"
#include "util.h"
#include "measurement.h"
#include "spi.h"

extern uint8_t resistance_range;


int main()
{
    switch_current(CURRENT_ON);
    spi_init();
    scrn_wake();
    uint8_t func = 0;
    while (true)
    {
        switch (func)
        {
        case 0:
            scrn_clear();
            scrn_print("Voltage");
            scrn_cursorpos(0, 1);
            scrn_print(ftoa(get_voltage()));
            scrn_print("V");
            _delay_ms(300);
            break;
        case 1:
            scrn_clear();
            scrn_print("Current");
            scrn_cursorpos(0, 1);
            scrn_print(ftoa(get_current()));
            scrn_print("A");
            _delay_ms(300);
            break;
        case 2: {
            float cap = get_capacitance();
            scrn_clear();
            scrn_print("Capacitance ");
            scrn_cursorpos(0, 1);
            if (cap < 1.f)
            {
                scrn_print(ftoa(cap * 1000.f));
                scrn_print("nF");
            }
            else
            {
                scrn_print(ftoa(cap));
                scrn_print("\344F");
            }
            _delay_ms(300);
            break;}
        case 3:
            scrn_clear();
            scrn_print("Resistance");
            scrn_cursorpos(0, 1);
            {
                float resistance = get_resistance();
                if (resistance > 1000000.f)
                {
                    scrn_print(ftoa(get_resistance() * 0.000001f));
                    scrn_print("M\364");
                }
                else if (resistance > 1000.f)
                {
                    scrn_print(ftoa(get_resistance() * 0.001f));
                    scrn_print("k\364");
                }
                else
                {
                    scrn_print(ftoa(get_resistance()));
                    scrn_print("\364");
                }

            }

            scrn_cursorpos(14, 1);
            scrn_print(itoa(resistance_range));
            _delay_ms(300);
            break;
        }

        if (button_isdown(BTN_UP))
            func = (func + 1) & 0x03;
        if (button_isdown(BTN_DOWN))
            func = (func - 1) & 0x03;

        spi_assert(spi_assert_flash);
        uint8_t x = 0;
        while (1)
        {
            for (uint8_t i = 8; i; --i)
                x = (x << 1) | ((uint8_t)get_adc_10bit_fast(0x0a) & 0x01);
             spi_send_byte(x);
        }

        spi_deassert();


    }
}

