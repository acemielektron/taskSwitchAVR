#include <avr/io.h>
#include "taskUtil.h"

; saves working registers and status register to stack
.MACRO saveContext
        push    r0
        in      r0, _SFR_IO_ADDR(SREG)
        cli             ; became redundant due to "cli" in YIELD() macro
        push    r0
        push    r1
        clr     r1
        push    r2
        push    r3
        push    r4
        push    r5
        push    r6
        push    r7
        push    r8
        push    r9
        push    r10
        push    r11
        push    r12
        push    r13
        push    r14
        push    r15
        push    r16
        push    r17
        push    r18
        push    r19
        push    r20
        push    r21
        push    r22
        push    r23
        push    r24
        push    r25
        push    r26
        push    r27
        push    r28
        push    r29
        push    r30
        push    r31
.ENDM

; restores working registers and status register from stack
.MACRO	restoreContext
        pop     r31
        pop     r30
        pop     r29
        pop     r28
        pop     r27
        pop     r26
        pop     r25
        pop     r24
        pop     r23
        pop     r22
        pop     r21
        pop     r20
        pop     r19
        pop     r18
        pop     r17
        pop     r16
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     r7
        pop     r6
        pop     r5
        pop     r4
        pop     r3
        pop     r2
        pop     r1
        pop     r0
        out     _SFR_IO_ADDR(SREG), r0
        pop     r0
.ENDM

; End of Macro(s)

.section .data
.extern pCurrentTask;
.global newSecond
newSecond:	.byte 0
miliSec:	.word 0

.section .text
.global TIMER0_COMPA_vect		; define ISR
TIMER0_COMPA_vect:saveContext
                  lds   r16, TCNT0      ; did we enter here by ms interrupt or YIELD() ?
                  and   r16, r16        ; if TIMER0 counter is 0 we entered by interrupt
                  brne  loadNextTask    ; if not jump to loadNextTask
msStart:          lds   r24, miliSec
                  lds   r25, miliSec+1
                  adiw  r24, 1 ; miliSec++;
                  cpi   r24, 232 ; (3*256)+232 = 1000
                  brne  msExit
                  cpi   r25, 3 ; 3*256
                  brne  msExit
                  ldi   r24, 0xFF ; r24 = -1
                  sts   newSecond, r24 ; newSecond = true;
                  clr   r24 ; miliSec = 0 (reset miliSec)
                  clr   r25
msExit:		  sts miliSec, r24
                  sts   miliSec+1, r25
loadNextTask:     lds   r30, pCurrentTask      
                  lds   r31, pCurrentTask+1               
                  ld    r28, Z+         ; load nextTask pointer to Y register
                  ld    r29, Z+
                  in    r24, _SFR_IO_ADDR(SPL) ; load current stack pointer to r24:r25
                  in    r25, _SFR_IO_ADDR(SPH)
                  st    Z+, r24         ; store current task's stack pointer
                  st    Z+, r25
                  sts   pCurrentTask, r28 ; store next task's pointer to pCurrentTask
                  sts   pCurrentTask+1, r29                  
                  ldd   r24, Y+4        ; load next task's ready status to r24
                  and   r24, r24        ; check if next task is ready
                  breq  loadNextTask    ; if not ready check the task after next
startNextTask:    ldd   r24, Y+2        ; load next task's stack pointer to r24:25
                  ldd   r25, Y+3
                  out   _SFR_IO_ADDR(SPL), r24        ; update stack pointer
                  out   _SFR_IO_ADDR(SPH), r25
intrExit:         restoreContext
                  reti


.global setTimer0msInt
setTimer0msInt:   cli             
                  clr   r24
                  ori	r24, (1 << WGM01)
                  out	_SFR_IO_ADDR(TCCR0A), r24
                  clr   r24
                  ori	r24, (1 << CS01) | (1 << CS00)
                  out	_SFR_IO_ADDR(TCCR0B), r24
                  ldi	r24, 0xF9	; 249
                  out	_SFR_IO_ADDR(OCR0A), r24                 
                  clr   r24
                  out	_SFR_IO_ADDR(TCNT0), r24
                  ldi	r30, TIMSK0
                  ldi	r31, 0
                  ld	r24, Z
                  ori	r24, (1 << OCIE0A)
                  st	Z, r24
                  sei
                  ret
                  