#define _SFR_ASM_COMPAT 1
#define __SFR_OFFSET 0
#include <avr/io.h>

#include "pins.h"

.global oscilloscope_fast

; parameters: uint8_t* address, uint16_t count
oscilloscope_fast:
    mov XH, r25         ; X points to the current position in the data array
    mov XL, r24
    mov ZH, r23         ; Z counts the remaining samples
    mov ZL, r22
    sbiw ZL, 1
    push YL
    push YH
    ldi YH, 0x00
    ldi YL, ADMUX
    ldi r18, (0x60 | VIN_V)
    st Y, r18      ; AREF = VCC, left adjust result, mux = VIN_V
    ldi YL, ADCSRB
    ldi r18, 0x00
    st Y, r18
    ldi YL, ADCSRA
    ldi r18, 0xc0
    st Y, r18     ; ADC enabled, ck/2, start first conversion
    ldi r19, 20
osc_fast_delay_loop:
    dec r19
    brne osc_fast_delay_loop
oscilloscope_fast_loop:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop                         ; nop x17
    ldi YL, ADCSRA
    st Y, r18                   ; start the next conversion (3 cyc)
    ldi YL, ADCH
    ld r19, Y                   ; result read happens here (3cyc)
    st X+, r19                  ; store the result in the data array (2cyc)
    sbiw ZL, 1                  ; decrement remaining count (2cyc)
    brne oscilloscope_fast_loop ; jump back to start (2cyc)

    pop YH
    pop YL

    ret
