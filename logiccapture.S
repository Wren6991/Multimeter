#define _SFR_ASM_COMPAT 1
#define __SFR_OFFSET 0
#include <avr/io.h>


.global logic_acquire_fast

; parameters: uint8_t* address, uint16_t count
logic_acquire_fast:
    mov XH, r25         ; X points to the current position in the data array
    mov XL, r24
    mov ZH, r23         ; Z counts the remaining samples
    mov ZL, r22
    sbiw ZL, 2          ; test is at end of loop, so decrement the count by 2 since always executes
    andi ZL, 0xfe       ; Z must be even, because we subtract two and test for zero.
    in r18, DDRB
    andi r18, 0xf0
    out DDRB, r18       ; Set logic pins to input
logic_loop:
    in r18, PINB
    swap r18
    andi r18, 0xf0      ; read in from the port, swap to top and mask off bottom (3cyc)
    sbiw ZL, 2          ; decrement count by 2 (2cyc)
    in r20, SREG
    andi r20, 0x02      ; read status register and mask for the Z bit (2cyc)
    nop                 ; nop to bring to a round 8 cycles
    in r19, PINB
    andi r19, 0x0f
    or r18, r19         ; read, mask and combine the input into r18 (3cyc)
    st X+, r18          ; store the result in the output buffer and postincrement (2cyc)
    tst r20             ; check for our zero flag from earlier (1cyc)
    breq logic_loop     ; branch to top of loop (2cyc) if our saved flag is zero, i.e. Z was not set

    ret
