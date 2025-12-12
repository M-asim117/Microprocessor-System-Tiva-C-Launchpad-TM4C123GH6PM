/*Add the following line to startup file before Reset Handler service routine in the startup file 
EXPORT DisableInterrupts
				EXPORT EnableInterrupts
				EXPORT EnablePriorityInterrupts
				EXPORT WaitForInterrupt
DisableInterrupts
				CPSID I ; set PRIMASK to disable interrupts
				BX LR
EnableInterrupts
				CPSIE I ; clear PRIMASK to enable interrupts
				BX LR
EnablePriorityInterrupts
				MOV R0 , #0x40 ; set base priority 2
				MSR BASEPRI , R0
				BX LR
WaitForInterrupt
				WFI
				BX LR */
//Then add the following c code file 

/******************************************************************************
*	Project name:Configuration of Timer
*	File name:Lab9 
*	Author:Muhammad Asim
*	Date:5 Dec 2025
*	Description: This is a template file for Timer0A configuration. 
*                It toggles Red LED at 2Hz.
 ******************************************************************************/

/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/

#include "TM4C123.h"

/* GPIO registers */
#define		SYSCTL_RCGCGPIO_R			(*((volatile unsigned long*)0x400FE608))
#define GPIO_PORTF_DATA_R (*((volatile unsigned long*) 0x400253FC))
#define GPIO_PORTF_DEN_R (*((volatile unsigned long*) 0x4002551C))
#define GPIO_PORTF_DIR_R (*((volatile unsigned long*) 0x40025400))

/* Timer registers */
#define		SYSCTL_RCGCTIMER_R			(*((volatile unsigned long*)0x400FE604))
#define		TIMER0_CTL_R				(*((volatile unsigned long*)0x4003000C))
#define		TIMER0_CFG_R				(*((volatile unsigned long*)0x40030000))
#define		TIMER0_TAMR_R				(*((volatile unsigned long*)0x40030004))
#define		TIMER0_TAILR_R				(*((volatile unsigned long*)0x40030028))
#define		TIMER0_TAPR_R				(*((volatile unsigned long*)0x40030038))
#define		TIMER0_ICR_R				(*((volatile unsigned long*)0x40030024))
#define		TIMER0_IMR_R				(*((volatile unsigned long*)0x40030018))

/* NVIC registers for Timer0A */
#define		NVIC_EN__R					(*((volatile unsigned long*)0xE000E100))
#define		NVIC_PRI__R					(*((volatile unsigned long*)0xE000E410))

/* Constant values */
/* Enable clock for Timer0 */
#define		TIM0_CLK_EN	 0X01
/* Enable Timer0 */							
#define		TIM0_EN	     0X01
/* 16-bit Timer mode */									
#define		TIM_16_BIT_EN 0X04
/* Periodic mode */								
#define		TIM_TAMR_PERIODIC_EN  0X02
/* Prescaler value for 100kHz frequency */				
#define		TIM_FREQ_10usec	    160

/* Clear timeout interrupt */						
#define		TIM0_INT_CLR  0X01
/* Enable Timer0A interrupt */								
#define		EN__INT	 0x01
/* Enable clock for portF */							
#define		PORTF_CLK_EN 0x20
/* Toggle red led(PF1) */								
#define		TOGGLE_PF1	 GPIO_PORTF_DATA_R ^=(0x02)
/* Configure red led(PF1) */								
#define		LED_RED		0x02								

/* Function headers */
void GPIO_Init(void);
void Timer_Init(unsigned long period);
void DisableInterrupts(void);
void EnableInterrupts(void);
void WaitForInterrupt(void);

//System Initialization for Floating Point Unit
void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}

/* Timer 0A Configuration */
void Timer_Init(unsigned long period)
{
	/* Enable clock for Timer0 */
	SYSCTL_RCGCTIMER_R |=TIM0_CLK_EN ;

	/* Disable Timer0 before setup */
	TIMER0_CTL_R &=~(TIM0_EN) ;											
	/* Configure 16-bit timer mode */
	TIMER0_CFG_R =TIM_16_BIT_EN;									
	/* Configure periodic mode */
	TIMER0_TAMR_R|=TIM_TAMR_PERIODIC_EN;					
	/* Set initial load value */
	TIMER0_TAILR_R =0xFFFF ;												
	/* Set prescalaer for desired frequency 100kHz */
	TIMER0_TAPR_R =TIM_FREQ_10usec;								
	
	/* Interrupt Configuration */											
	DisableInterrupts();
	/* Enable interrupt for Timer0A */
	NVIC_EN__R |=0X00080000;
	/* Set priority for interrupt */
	NVIC_PRI__R = (NVIC_PRI__R & 0x00FFFFFF)|0XA0000000;
	/* Clear timeout interrupt */
	TIMER0_ICR_R |=TIM0_INT_CLR ;										
	/* Enable interrupt mask for Timer_0A */
	TIMER0_IMR_R |=EN__INT ;	
	
	/* Enable Timer_0A */										
	TIMER0_CTL_R |= TIM0_EN;
	
	EnableInterrupts();
}


void GPIO_Init()
{
	/* Enable clock for PortF
	* enable digital IO for Red LED
	* and set its direction */
 	SYSCTL_RCGCGPIO_R |=PORTF_CLK_EN; 
	GPIO_PORTF_DEN_R  |=0x02;
	GPIO_PORTF_DIR_R  |=0x02;
	
	
	
}


void TIMER0A_Handler(void)
{
	/* Clear timeout interrupt */
	TIMER0_ICR_R |=TIM0_INT_CLR;
	
	/* Toggle Red LED */
	TOGGLE_PF1;
	
}



int main(void)
{
	/* Generate a square wave for 2Hz */
	Timer_Init(50000);					
	/* Initialize PF1 as digital output */
	GPIO_Init();								
	while(1);
	 
	
}
