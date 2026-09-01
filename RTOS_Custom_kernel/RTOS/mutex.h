/*
 * mutex.h
 *
 *  Created on: 30 Aug 2026
 *      Author: samfishy
 */

#ifndef RTOS_CUSTOM_KERNEL_RTOS_MUTEX_H_
#define RTOS_CUSTOM_KERNEL_RTOS_MUTEX_H_

#include "stdint.h"
#include "Custom_RTOS.h"

typedef struct {
	uint32_t *t_addr;
	void     *sp    ;
	uint8_t priro   ;
}Mutex_History;

typedef struct {
	OS_Thread    *owner       ;
	uint8_t       orignal_prior;
	uint32_t      wait_set     ;
	Mutex_History M1_h[32]     ;
}Mutex;

void Mutex_init(Mutex *mt)  ;
void Mutex_Lock(Mutex *mt)  ;
void Mutex_Unlock(Mutex *mt);

#endif /* RTOS_CUSTOM_KERNEL_RTOS_MUTEX_H_ */
