#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "screen.h"
#include "util.h"
#include "measurement.h"


int main()
{

    /*ADCSRA = 0x00;
    ADMUX = VIN_RC;
    ADCSRB = 0x40;
    ACSR = 0x40;*/

    switch_current(CURRENT_ON);
    scrn_wake();
    while (true)
    {
        /*float cap = get_capacitance();
        scrn_clear();
        scrn_print("Cap: ");
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
        scrn_cursorpos(0, 1);
        scrn_print("Range: ");
        scrn_print(itoa(cap_range));
        _delay_ms(300);
        PORTB &= ~0x01;*/

        scrn_clear();
        scrn_print(ftoa(get_current()));
        scrn_print("A");
        _delay_ms(300);
    }
}

