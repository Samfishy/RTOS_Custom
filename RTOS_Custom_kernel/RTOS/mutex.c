/*
 * mutex.c
 *
 *  Created on: 30 Aug 2026
 *      Author: samfishy
 */
#include "mutex.h"

extern OS_Thread * volatile OS_Curr;
extern uint32_t OS_ReadySet;
extern OS_Thread *OS_thread_array[32+1];

void Mutex_init(Mutex *mt)
{
	mt->owner         = (OS_Thread *)0;
	mt->orignal_prior = 0             ;
	mt->wait_set      = 0U            ;
}

void Mutex_Lock(Mutex *mt)
{
	__disable_irq();

	if(mt->owner == (OS_Thread *)0)
	{
		mt->owner         = OS_Curr       ;
		mt->orignal_prior = OS_Curr->priro;
		__enable_irq();
	}
	else
	{
		if(OS_Curr->priro > mt->owner->priro)
		{
			if(OS_ReadySet & (1UL << (mt->owner->priro-1)))
			{
				OS_ReadySet &= ~(1UL << (mt->owner->priro-1));
				OS_ReadySet |=  (1UL << (OS_Curr->priro-1))  ;
			}

			mt->M1_h[OS_Curr->priro-1].thread = OS_Curr;
			mt->M1_h[OS_Curr->priro-1].priro  = OS_Curr->priro     ;

			mt->owner->priro = OS_Curr->priro            ;
			OS_thread_array[mt->owner->priro] = mt->owner;
		}
		else
		{
			OS_ReadySet  &= ~(1UL << (OS_Curr->priro-1));
			mt->wait_set |=  (1UL << (OS_Curr->priro-1));
		}

		__enable_irq();
		OS_schd();
	}
}

void Mutex_Unlock(Mutex *mt)
{
	__disable_irq();

	if(mt->owner != OS_Curr)
	{
		__enable_irq();
	}
	else
	{
		if(OS_Curr->priro != mt->orignal_prior)
		{
			uint8_t    tsk_prior = OS_Curr->priro;
			OS_Thread* rep_tread = mt->M1_h[tsk_prior-1].thread ;

			OS_Curr->priro  =  mt->orignal_prior          ;
			OS_ReadySet    |=  (1UL << (OS_Curr->priro-1));

			OS_thread_array[OS_Curr->priro] = OS_Curr  ;
			OS_thread_array[tsk_prior]      = rep_tread;
		}

		if(mt->wait_set == 0U)
		{
			mt->owner = (OS_Thread *)0;
		}
		else
		{
			uint8_t hPriro  = LOG2(mt->wait_set);
			mt->wait_set   &= ~(1UL << (hPriro+1U));
			OS_ReadySet    |=  ((1UL << hPriro+1U));

			mt->owner = OS_thread_array[hPriro];
		}

		__enable_irq();
		OS_schd();
	}
}
