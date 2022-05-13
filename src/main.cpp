#include "simpleUART.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <stdio.h>

#define portSAVE_CONTEXT() \
asm volatile ( \
				"push r0\n" \
				"in r0, __SREG__\n" \
				"cli\n" \
				"push r0\n" \
				"push r1\n" \
				"clr r1\n" \
				"push r2\n" \
				"push r3\n" \
				"push r4\n" \
				"push r5\n" \
				"push r6\n" \
				"push r7\n" \
				"push r8\n" \
				"push r9\n" \
				"push r10\n" \
				"push r11\n" \
				"push r12\n" \
				"push r13\n" \
				"push r14\n" \
				"push r15\n" \
				"push r16\n" \
				"push r17\n" \
				"push r18\n" \
				"push r19\n" \
				"push r20\n" \
				"push r21\n" \
				"push r22\n" \
				"push r23\n" \
				"push r24\n" \
				"push r25\n" \
				"push r26\n" \
				"push r27\n" \
				"push r28\n" \
				"push r29\n" \
				"push r30\n" \
				"push r31\n");

#define portRESTORE_CONTEXT() \
asm volatile ( \
				"pop r31\n" \
				"pop r30\n" \
				"pop r29\n" \
				"pop r28\n" \
				"pop r27\n" \
				"pop r26\n" \
				"pop r25\n" \
				"pop r24\n" \
				"pop r23\n" \
				"pop r22\n" \
				"pop r21\n" \
				"pop r20\n" \
				"pop r19\n" \
				"pop r18\n" \
				"pop r17\n" \
				"pop r16\n" \
				"pop r15\n" \
				"pop r14\n" \
				"pop r13\n" \
				"pop r12\n" \
				"pop r11\n" \
				"pop r10\n" \
				"pop r9\n" \
				"pop r8\n" \
				"pop r7\n" \
				"pop r6\n" \
				"pop r5\n" \
				"pop r4\n" \
				"pop r3\n" \
				"pop r2\n" \
				"pop r1\n" \
				"pop r0\n" \
				"out __SREG__, r0\n" \
				"pop r0\n");

//extern "C" volatile int16_t miliSec;
//extern "C" volatile bool newSecond;
volatile int16_t miliSec = 0;
volatile bool newSecond = false;

ISR(TIMER0_COMPA_vect, ISR_NAKED){//timer0 interrupt 2kHz toggles pin 8
//generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)
	portSAVE_CONTEXT();	
	asm(		
		"						lds r24, miliSec\n"
		"						lds r25, miliSec+1\n"
		"						adiw r24, 1\n" // miliSec++;
		"						cpi r24, 232\n" // (3*256)+232 = 1000
		"						brne msExit\n"
		"						cpi r25, 3\n" // 3*256
		"						brne msExit\n"
		"						ldi r24, 0xFF\n" // r24 = -1
		"						sts newSecond, r24\n" // newSecond = true;
		"						clr r24\n" // miliSec = 0 (reset miliSec)
		"						clr r25\n"
		"msExit:		sts miliSec, r24\n"
		"						sts miliSec+1, r25\n");
	portRESTORE_CONTEXT();
	asm("reti\n");
}

void setup()
{
	asm("cli\n"); // Disable interrupts
	TCCR0A = 0; // COM0A1|COM0A0|COM0B1|COM0B0|–|–|WGM01|WGM00
	TCCR0B = 0;	// FOC0A|FOC0B|–|–|WGM02|CS02|CS01|CS00
	TCCR0A |= (1 << WGM01); // WGM=0b010 -> turn on CTC (Clear Timer on Compare match) mode
	TCCR0B |= (1 << CS01) | (1 << CS00); // 1/64 prescaler (16.000.000 / 64 = 250.000)	
	OCR0A = 249; // OCR0A – Output Compare Register A (250.000/250 = 1000 = 1ms)
	OCR0B = 0; // OCR0B – Output Compare Register B
	TCNT0 = 0; // clear timer 0 counter
	//TIFR0 => Timer/Counter 0 Interrupt Flag Register –|–|–|–|–|OCF0B|OCF0A|TOV0
	TIMSK0 |= (1 << OCIE0A); // –|–|–|–|–|OCIE0B|OCIE0A|TOIE0 -> Enable interupt on timer0 compare A match interrupt flag
	asm("sei\n"); // Enable interrupts
}

int main()
{
	int8_t seconds = 0, minutes = 0, hours = 0;
	setup();
	Serial.print("Start\n");	
	for (;;) // main loop
	{
		if (newSecond)
		{						
			newSecond = false;
			//check time
			seconds ++;
			if (seconds == 60)
			{
				seconds = 0;
				minutes++;
				if (minutes == 60)
				{
					minutes = 0;
					hours++;
				}				
			}
			//print time
			Serial.write(' ');
			if (hours < 10) Serial.write('0');
			Serial.print(hours);			
			Serial.write(':');
			if (minutes < 10) Serial.write('0');
			Serial.print(minutes);
			Serial.write(':');
			if (seconds < 10) Serial.write('0');
			Serial.print(seconds);
			Serial.write('\r');					
		}		
		//char ch = Serial.read();
		//Serial.write(ch);
	}
	return 0;
}