#include <avr/io.h>
#include "util.h"

.MACRO saveContext
        push r0
        in r0, _SFR_IO_ADDR(SREG)
        cli
        push r0
        push r1
        clr r1
        push r2
        push r3
        push r4
        push r5
        push r6
        push r7
        push r8
        push r9
        push r10
        push r11
        push r12
        push r13
        push r14
        push r15
        push r16
        push r17
        push r18
        push r19
        push r20
        push r21
        push r22
        push r23
        push r24
        push r25
        push r26
        push r27
        push r28
        push r29
        push r30
        push r31
.ENDM

.MACRO	restoreContext
        pop r31
        pop r30
        pop r29
        pop r28
        pop r27
        pop r26
        pop r25
        pop r24
        pop r23
        pop r22
        pop r21
        pop r20
        pop r19
        pop r18
        pop r17
        pop r16
        pop r15
        pop r14
        pop r13
        pop r12
        pop r11
        pop r10
        pop r9
        pop r8
        pop r7
        pop r6
        pop r5
        pop r4
        pop r3
        pop r2
        pop r1
        pop r0
        out _SFR_IO_ADDR(SREG), r0
        pop r0
.ENDM

; End of Macro(s)

.section .data
.global newSecond
newSecond:	.byte 0
miliSec:		.word 0

.section .text
.global TIMER0_COMPA_vect		; define ISR
TIMER0_COMPA_vect:saveContext
                  lds r24, miliSec
                  lds r25, miliSec+1
                  adiw r24, 1 ; miliSec++;
                  cpi r24, 232 ; (3*256)+232 = 1000
                  brne msExit
                  cpi r25, 3 ; 3*256
                  brne msExit
                  ldi r24, 0xFF ; r24 = -1
                  sts newSecond, r24 ; newSecond = true;
                  clr r24 ; miliSec = 0 (reset miliSec)
                  clr r25
msExit:						sts miliSec, r24
                  sts miliSec+1, r25
                  restoreContext
                  reti
