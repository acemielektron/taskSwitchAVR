#include "simpleUART.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <stdio.h>
#include "taskCtl.h"

#define DATASIZE 1000 // size of allocated global or static variables
#define MAIN_STACK_SIZE  200 // stack reserved for main task
#define TIME_TASK_STACK_SIZE	48

/*
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
*/

int8_t seconds = 0, minutes = 0, hours = 0;

void printTime()
{
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

void calcTime()
{
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
				printTime();
			}		
			YIELD();
		}
}

int main()
{	
	Serial.begin(115200);
	Serial.print("Start\n");	
	initSwitcher(DATASIZE, MAIN_STACK_SIZE); // start task switcher
	printTime();
	startNewTask(&calcTime, TIME_TASK_STACK_SIZE);
	for (;;) // main loop = task0
	{
		//char ch = Serial.read();
		//Serial.write(ch);
		YIELD();
	}
	return 0;
}