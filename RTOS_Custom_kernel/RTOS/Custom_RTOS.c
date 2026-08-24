/*
 * Custom_RTOS.c
 *
 *  Created on: 24 Aug 2026
 *      Author: samfishy
 */
#include "main.h"
#include "stdint.h"
#include "Custom_RTOS.h"

OS_Thread * volatile OS_Curr;
OS_Thread * volatile OS_Nxt;

void OS_init(void)
{
	  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}

void OS_schd(void)
{
	if(OS_Nxt != OS_Curr)
	{
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}

}

void OS_Thread_Start(OS_Thread *ot, OS_Thread_Handler th, void *sp_ini,
		uint32_t stkSize) {
	uint32_t *sp = (uint32_t*) ((((uint32_t) sp_ini + stkSize) / 8) * 8); // for rounding off
	uint32_t *stkLim;

	*(--sp) = (1u << 24);
	*(--sp) = (uint32_t) th;
    *(--sp) = 0x0000000EU; /* LR  */
    *(--sp) = 0x0000000CU; /* R12 */
    *(--sp) = 0x00000003U; /* R3  */
    *(--sp) = 0x00000002U; /* R2  */
    *(--sp) = 0x00000001U; /* R1  */
    *(--sp) = 0x00000000U; /* R0  */
    /* additionally, fake registers R4-R11 */
    *(--sp) = 0x0000000BU; /* R11 */
    *(--sp) = 0x0000000AU; /* R10 */
    *(--sp) = 0x00000009U; /* R9 */
    *(--sp) = 0x00000008U; /* R8 */
    *(--sp) = 0x00000007U; /* R7 */
    *(--sp) = 0x00000006U; /* R6 */
    *(--sp) = 0x00000005U; /* R5 */
    *(--sp) = 0x00000004U; /* R4 */

	ot->sp = sp;

	stkLim = (uint32_t *) (((((uint32_t) sp_ini - 1) / 8) + 1) * 8);

	for (sp = sp - 1; sp >= stkLim; --sp) {
		*sp = 0xDEAD;
	}
}

__attribute__((naked)) void PendSV_Handler(void)
{
	__asm__ volatile (
		"cpsid   i                             \n\t" // Disable interrupts

	    // if(OS_Curr != (OS_Thread *)0)
	    "ldr     r3, =OS_Curr                  \n\t"
	    "ldr     r3, [r3, #0]                  \n\t"
	    "cmp     r3, #0                        \n\t"
	    "beq     PendSV_Restore                \n\t"

	    // Save current thread context
	    "push    {r4-r11}                      \n\t"
	    "ldr     r1, =OS_Curr                  \n\t" // Fixed typo (was OS_curr)
	    "ldr     r1, [r1, #0]                  \n\t"
	    "str     sp, [r1, #0]                  \n\t" // OS_Curr->sp = sp;

	"PendSV_Restore:                           \n\t" // Added missing colon
	    // sp = OS_Nxt->sp;
	    "ldr     r3, =OS_Nxt                   \n\t"
	    "ldr     r3, [r3, #0]                  \n\t"
	    "ldr     r2, [r3, #0]                  \n\t" // Fixed logic: read sp from struct
		"mov     sp,r2                         \n\t"

	    // OS_Curr = OS_Nxt;
	    "ldr     r1, =OS_Nxt                   \n\t"
	    "ldr     r1, [r1, #0]                  \n\t"
	    "ldr     r2, =OS_Curr                  \n\t"
	    "str     r1, [r2, #0]                  \n\t" // Fixed logic: store Nxt into Curr

	    "pop     {r4-r11}                      \n\t"
	    "cpsie   i                             \n\t" // Enable interrupts
	    "bx      lr                            \n\t"
	);

}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  __disable_irq();
  OS_schd();
  __enable_irq();
  /* USER CODE END SysTick_IRQn 1 */
}
