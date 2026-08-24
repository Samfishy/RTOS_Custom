/*
 * Custom_RTOS.h
 *
 *  Created on: 24 Aug 2026
 *      Author: samfishy
 */

#ifndef RTOS_CUSTOM_KERNEL_RTOS_CUSTOM_RTOS_H_
#define RTOS_CUSTOM_KERNEL_RTOS_CUSTOM_RTOS_H_

typedef struct {
	void *sp;
} OS_Thread;

typedef void(*OS_Thread_Handler)();

void OS_init(void);
void OS_schd(void);
void OS_Thread_Start(OS_Thread *ot, OS_Thread_Handler th, void *sp, uint32_t stkSize);

#endif /* RTOS_CUSTOM_KERNEL_RTOS_CUSTOM_RTOS_H_ */
