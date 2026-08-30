/*
 * semaphores.c
 *
 *  Created on: 30 Aug 2026
 *      Author: samfishy
 */
#include "semaphores.h"

extern OS_Thread * volatile OS_Curr;
extern uint32_t OS_ReadySet;


void Semaphore_Init(Semaphore *sph, uint32_t max, uint32_t ini_cnt)
{
	sph->max_count = max    ;
	sph->count     = ini_cnt;
	sph->wait_set  = 0U     ;
}

void Semaphore_Wait(Semaphore *sph)
{
	__disable_irq();

	if(sph->count > 0U)
	{
		sph->count--;
		__enable_irq();
	}
	else
	{
		uint8_t cur_priority = OS_Curr->priro-1;

		OS_ReadySet   &= ~(1U << cur_priority);
		sph->wait_set |=  (1U << cur_priority);

		__enable_irq();
		OS_schd();
	}
}

void Semaphore_Signal(Semaphore *sph)
{
	__disable_irq();

	if(sph->wait_set == 0)
	{
		if(sph->count < sph->max_count)
		{
			sph->count++;
		}
		__enable_irq();
	}
	else
	{
		uint8_t highest_piro_tsk = LOG2(sph->wait_set)-1;

		sph->wait_set &= ~(1U << highest_piro_tsk);
		OS_ReadySet   |=  (1U << highest_piro_tsk);

		__enable_irq();
		OS_schd();
	}
}
