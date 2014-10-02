#include "measurement.h"
#include "pins.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

uint16_t get_adc_10bit_fast(uint8_t pin)
{
    ADMUX = 0x40 | pin; // AREF = VCC
    ADCSRA = 0x85;      // Enabled, clk/64 = 125kHz
    ADCSRA |= 0x40;
    while (ADCSRA & 0x40);
    uint8_t result = ADCL;
    return result | (((uint16_t)ADCH) << 8);
}

uint16_t get_battery_voltage()
{
    ADMUX = 0x5e;   // AREF = VCC, input = 1.1v bandgap reference
    ADCSRA = 0x86;  // ADC enabled, don't start conversion yet, auto trigger off, no interrupt, ck/128 = 62.5 kHz
    _delay_us(500);
    uint32_t result = 0;
    for (int i = 16; i; --i)
    {
        result += get_adc_10bit_fast(VIN_BANDGAP);
    }
    return 4464085 / result;   // = 1024 (number of ADC steps) * 1.1 (bandgap voltage) * 16 (number of readings) * 256 (fxp88 fixed point)
}

uint16_t get_adc_12bit(uint8_t pin)
{
    uint16_t result = 0;
    for (uint8_t i = 16; i; --i)
    {
        result += get_adc_10bit_fast(pin);
    }
    return result >> 2;
}

void set_vatten(uint8_t vatten)
{
    DDR_VATTEN |= VATTEN_ALL;
    PORT_VATTEN = (PORT_VATTEN & ~VATTEN_ALL) | vatten;
}

uint8_t voltage_range = 0;
void set_voltage_range(uint8_t range)
{
    uint8_t range_attens[] = {VATTEN_1M, VATTEN_100k, VATTEN_5k};
    DDR_VATTEN |= VATTEN_ALL;
    PORT_VATTEN &= ~VATTEN_ALL;
    PORT_VATTEN |= range_attens[range];
}

float get_voltage()
{
    set_voltage_range(voltage_range);
    uint16_t lower_limits[] = {0, 151, 49};
    uint16_t upper_limits[] = {941, 805, 1024};
    uint16_t multipliers[] = {2, 11, 201};

    uint16_t result = get_adc_10bit_fast(VIN_V);
    if (result < lower_limits[voltage_range])
        voltage_range--;
    else if (result > upper_limits[voltage_range])
        voltage_range++;

    _delay_us(10);

    uint32_t reading = get_adc_12bit(VIN_V);
    uint32_t battery = get_battery_voltage();
    reading = (reading * multipliers[voltage_range] * battery);
    reading >>= 7;
    return reading * (1/16384.f);
}

void switch_current(bool onoff)
{
    DDR_IGATE |= IGATE;
    if (onoff)
        PORT_IGATE |= IGATE;
    else
        PORT_IGATE &= ~IGATE;
}

float get_current()
{
    switch_current(CURRENT_ON);
    _delay_us(50);
    uint32_t reading = get_adc_12bit(VIN_I);
    reading = (reading * 19 * get_battery_voltage()) / 46;
    reading >>= 6;
    return reading * (1/16384.f);
}

inline void set_rc_range(uint8_t range)
{
    uint8_t ranges[] = {RC_LOW, RC_MID, RC_HI, 0};
    DDR_RC &= ~(RC_LOW | RC_MID | RC_HI);
    PORT_RC &= ~(RC_LOW | RC_MID | RC_HI);
    DDR_RC |= ranges[range];
    PORT_RC |= ranges[range];
}

uint8_t cap_range = 2;
volatile uint8_t cap_aco_mask = 0x20;

// Timer 1 overflow vector: stop the timer, set its result back to a large value,
// jump up a range, and force the wait loop to exit.
ISR(TIMER1_OVF_vect) {
    TCCR1B = 0x00;
    cap_aco_mask = 0x00;
    cap_range--;
    TCNT1 = 0xfff0;
}

void choose_cap_range(float result)
{
    // all values are in microfarads
    switch(cap_range)
    {
    case 0:
        cap_range++;
        break;
    case 1:
        if (result < 5.f)
            cap_range++;
        break;
    case 2:
        if (result > 6.f)
            cap_range--;
        else if (result < 0.5f)
            cap_range++;
        break;
    case 3:
        if (result > 0.6f)
            cap_range--;
        break;
    }
}

float get_capacitance()
{
    uint16_t resistances[] = {47, 470, 20000, 20000};
    uint8_t divisors[] = {1, 1, 1, 8};          // compensates for timer clock frequency differences
    uint8_t tccrs[] = {0x02, 0x02, 0x02, 0x01}; // selects the clock frequency input to the timer
    uint8_t subtractors[] = {1, 1, 1, 10};      // compensates for code execution time

    // let the RC_SENSE net float by switching off the current pass MOSFET
    switch_current(CURRENT_OFF);

    // Get values ready to plug into registers (timing is critical later)
    uint8_t rc_range = cap_range;
    if (rc_range > 2)
        rc_range = 2;
    uint8_t tccr_start = tccrs[cap_range];
    cap_aco_mask = 0x20;

    // Switch off the ADC so we can use its multiplexer, and set up the comparator:
    ADCSRA = 0x00;  // ADC off
    ADCSRB = 0x40;  // ACME = 1 (connect ADC multiplexer to the comparator)
    ADMUX = VIN_RC; // RC pin connected to comparator negative input
    ACSR = 0x40;    // 1.1v bandgap reference connected to comparator positive input

    // connect RC -> 47 ohms -> ground, float other pins, then wait 50ms for discharge
    set_rc_range(0);
    PORT_RC &= ~RC_LOW;
    DDR_RCSENSE &= ~RC_SENSE;
    PORT_RCSENSE &= ~RC_SENSE;
    _delay_ms(50);

    // set up timer1
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCNT1 = 0;
    TIMSK1 = 0x01; // overflow interrupt enabled
    sei();

    // connect rc -> resistance -> vcc and start timer.
    set_rc_range(rc_range);
    TCCR1B = tccr_start;

    // exits when comparator outputs 0 (voltage has reached 1.1v) or the timer overflow interrupt has fired
    while (ACSR & cap_aco_mask);
    // stop timer and float pins
    TCCR1B = 0x00;
    DDR_RC |= RC_LOW;
    PORT_RC &= ~RC_LOW;
    // C = -t / (R * ln(1 - Vtrig / Vcc)
    float result = -((float)(TCNT1 - subtractors[cap_range]) / divisors[cap_range]) / (log(1 - 1.03f / (get_battery_voltage() * (1/256.f))) * (float)resistances[cap_range]);
    choose_cap_range(result);
    return result;
}

uint8_t resistance_range = 1;

float get_resistance()
{
    uint16_t r[] = {47, 500, 20000};
    switch_current(CURRENT_OFF);
    set_rc_range(resistance_range);
    _delay_us(100);
    uint16_t reading = 0;
    for (uint8_t i = 4; i; --i)
        reading += get_adc_12bit(VIN_RC);
    set_rc_range(3);
    float  result = reading * (1/16384.f);
    result = r[resistance_range] * (result / (1.f - result));

    if (resistance_range == 1 and result > 4000.f)
        resistance_range++;
    else if (resistance_range == 2 and result < 3000.f)
        resistance_range--;

    return result;
}
