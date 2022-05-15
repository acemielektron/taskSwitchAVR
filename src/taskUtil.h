#ifndef ASM_UTIL_H
#define ASM_UTIL_H

	#ifdef __ASSEMBLER__	//assembler shared defines

	#else  /* !ASSEMBLER */
		#include <stdint.h>

		// Macro for yielding a task's timeslot to other tasks 
		#define YIELD() \
    	asm("cli\n"); \
			TIMER0_COMPA_vect()

		#ifdef __cplusplus
		extern "C" {
		#endif // __cplusplus

		extern volatile bool newSecond; // flagged true every new second (for time)
		void TIMER0_COMPA_vect(); // defined here for yield macro
		void setTimer0msInt(); // set up milisecond interupt

		#ifdef __cplusplus
		}
		#endif // __cplusplus
	#endif /* ASSEMBLER */
#endif // ASM_UTIL_H