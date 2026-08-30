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
	mt->own = (Owner_Details *)0;
	mt->wait_set = 0U;
}

void Mutex_Lock(Mutex *mt)
{
	__disable_irq();

	if(mt->own == (Owner_Details *)0U)
	{
		mt->own->thread_addr = OS_Curr;
		__enable_irq();
	}
	else
	{
		if(OS_Curr->priro > mt->own->thread_addr->priro)
		{
			if(OS_ReadySet & (1UL << mt->own->thread_addr->priro))
			{
				OS_ReadySet &= ~(1UL << mt->own->thread_addr->priro);
				OS_ReadySet |=  (1UL << OS_Curr->priro);
			}
			mt->own->orignal_prior = mt->own->thread_addr->priro;
			mt->own->thread_addr->priro = OS_Curr->priro;
		}
		else
		{
			OS_ReadySet  &= ~(1UL << OS_Curr->priro);
			mt->wait_set |=  (1UL << OS_Curr->priro);
		}

		__enable_irq();
		OS_schd();
	}
}

void Mutex_Unlock(Mutex *mt)
{
	__disable_irq();

	if(mt->own->thread_addr != OS_Curr)
	{
		__enable_irq();
	}
	else
	{
		if(OS_Curr->priro != mt->own->orignal_prior)
		{
			OS_ReadySet    &= ~(1UL << OS_Curr->priro);
			OS_Curr->priro  = mt->own->orignal_prior  ;
			OS_ReadySet    |=  (1UL << OS_Curr->priro);
		}

		if(mt->wait_set == 0U)
		{
			mt->own = (Owner_Details *)0;
		}
		else
		{
			uint8_t hPriro  = LOG2(mt->wait_set)-1;
			mt->wait_set   &= ~(1UL << hPriro)    ;
			OS_ReadySet    |=  (1UL << hPriro)    ;

			mt->own->thread_addr = OS_thread_array[hPriro];
			OS_schd();
		}

		__enable_irq();
	}
}
