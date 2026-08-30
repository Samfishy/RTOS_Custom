/*
 * semaphores.h
 *
 *  Created on: 30 Aug 2026
 *      Author: samfishy
 */

#ifndef RTOS_CUSTOM_KERNEL_RTOS_SEMAPHORES_H_
#define RTOS_CUSTOM_KERNEL_RTOS_SEMAPHORES_H_

#include "main.h"
#include "stdint.h"
#include "Custom_RTOS.h"

typedef struct
{
	uint32_t max_count;
	uint32_t count;
	uint32_t wait_set;
} Semaphore;

void Semaphore_Init(Semaphore *sph, uint32_t max, uint32_t ini_cnt);
void Semaphore_Wait(Semaphore *sph);
void Semaphore_Signal(Semaphore *sph);

#endif /* RTOS_CUSTOM_KERNEL_RTOS_SEMAPHORES_H_ */
