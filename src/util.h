#ifndef ASM_UTIL_H
#define ASM_UTIL_H

	#ifdef __ASSEMBLER__	//assembler shared defines

	#else  /* !ASSEMBLER */
		#include <stdint.h>

		#ifdef __cplusplus
		extern "C" {
		#endif // __cplusplus

		extern volatile bool newSecond;

		#ifdef __cplusplus
		}
		#endif // __cplusplus
	#endif /* ASSEMBLER */
#endif // ASM_UTIL_H