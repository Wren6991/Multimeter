#ifndef _PINS_H_
#define _PINS_H_

#define PORT_VATTEN PORTA
#define DDR_VATTEN DDRA
#define VATTEN_1M 0x20
#define VATTEN_100k 0x10
#define VATTEN_5k 0x04
#define VATTEN_ALL (VATTEN_1M | VATTEN_100k | VATTEN_5k)

#define DDR_IGATE DDRC
#define PORT_IGATE PORTC
#define IGATE 0x20
#define CURRENT_ON true
#define CURRENT_OFF false

#define PORT_RC PORTC
#define DDR_RC DDRC
#define RC_HI 0x10
#define RC_MID 0x08
#define RC_LOW 0x04

#define PORT_RCSENSE PORTA
#define DDR_RCSENSE DDRA
#define PIN_RCSENSE PINA
#define RC_SENSE 0x08

#define VIN_V 6
#define VIN_I 7
#define VIN_RC 3
#define VIN_BANDGAP 0x1e

#endif // _PINS_H_
