/*
 * Custom_RTOS.h
 *
 *  Created on: 24 Aug 2026
 *      Author: samfishy
 */

#ifndef RTOS_CUSTOM_KERNEL_RTOS_CUSTOM_RTOS_H_
#define RTOS_CUSTOM_KERNEL_RTOS_CUSTOM_RTOS_H_

#define MAX_THREAD 32

typedef struct {
	void *sp;
	uint32_t timeout;
} OS_Thread;

typedef void(*OS_Thread_Handler)();

void OS_init(void *stack, uint32_t stack_size);
void OS_schd(void);
void OS_IdleTsk(void);
void OS_Delay(int MsDelay);
void OS_Thread_Start(OS_Thread *ot, OS_Thread_Handler th, void *sp, uint32_t stkSize);

#endif /* RTOS_CUSTOM_KERNEL_RTOS_CUSTOM_RTOS_H_ */
