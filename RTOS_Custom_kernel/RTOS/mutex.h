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
	OS_Thread *thread_addr ;
	uint8_t   orignal_prior;
}Owner_Details;

typedef struct {
	Owner_Details *own;
	uint32_t wait_set;
}Mutex;

void Mutex_init(Mutex *mt)  ;
void Mutex_Lock(Mutex *mt)  ;
void Mutex_Unlock(Mutex *mt);

#endif /* RTOS_CUSTOM_KERNEL_RTOS_MUTEX_H_ */
