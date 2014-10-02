#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "screen.h"
#include "util.h"
#include "measurement.h"
#include "spi.h"

extern uint8_t resistance_range;

extern "C" void logic_acquire_fast(uint8_t* address, uint16_t count);
extern "C" void oscilloscope_fast(uint8_t* address, uint16_t count);

void draw_top_bar()
{
    scrn_print("\004\004\004\004\004\004\004\004\004\004\004\004\004\004\004\004\004\004\004", 0, 0);
    uint16_t battery = get_battery_voltage();
    if (battery < (uint16_t)(3.6 * 256))
        scrn_print("\010\011", 76, 0);
    else if (battery < (uint16_t)(3.8 * 256))
        scrn_print("\012\013", 76, 0);
    else if (battery < (uint16_t)(4.0 * 256))
        scrn_print("\014\015", 76, 0);
    else
        scrn_print("\016\017", 76, 0);
    scrn_print(fxp88toa(battery), 67, 1);
}

void draw_digit_screen(const char *param_type, float value, const char *unit)
{
    scrn_clear();
    draw_top_bar();
    scrn_print(param_type, 0, 1);
    scrn_print_digits(ftoa(value), 0, 2);
    scrn_print_digits(unit, 64, 2);
    scrn_print("Megameter   Luke Wren", 0, 5);

}

int main()
{
    switch_current(CURRENT_ON);
    scrn_backlight(true);
    scrn_init();
    button_isdown(BTN_DOWN);    // forces button setup code to run
    uint8_t func = 0;
    while (true)
    {
        switch (func)
        {
        case 0:
            draw_digit_screen("Voltage", get_voltage(), "V");
            break;
        case 1:
            draw_digit_screen("Current", get_current(), "A");
            break;
        case 2: {
            float cap = get_capacitance();
            if (cap < 1.f)
                draw_digit_screen("Capacitance", cap * 1000.f, "nF");
            else
                draw_digit_screen("Capacitance", cap, "uF");
            break;}
        case 3:
            {
                float resistance = get_resistance();
                if (resistance > 1000000.f)
                {
                    draw_digit_screen("Resistance", resistance * 0.000001f, "MO");
                }
                else if (resistance > 1000.f)
                {
                    draw_digit_screen("Resistance", resistance * 0.001f, "kO");
                }
                else
                {
                    draw_digit_screen("Resistance", resistance, "O");
                }

            }
            break;
        }
        _delay_ms(250);

        if (button_isdown(BTN_UP))
            func = (func + 1) & 0x03;
        if (button_isdown(BTN_DOWN))
            func = (func - 1) & 0x03;
    }
}

