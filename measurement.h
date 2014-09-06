#ifndef _MEASUREMENT_H_
#define _MEASUREMENT_H_

#include <avr/io.h>

void set_vatten(uint8_t vatten);

uint16_t get_adc_10bit_fast(uint8_t pin);
uint16_t get_adc_12bit(uint8_t pin);

uint16_t get_battery_voltage(); // in fxp 8.8 format

float get_voltage();        // voltage in volts
float get_current();        // current in amperes
float get_capacitance();    // capacitance in microfarads
float get_resistance();

void switch_current(bool onoff);

#endif // _MEASUREMENT_H_
