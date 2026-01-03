#include "TM4C123.h"
/******************************************************************************
*	Project name: PWM EVALUATION 
*	File name:PWM
*	Author:MUHAMMAD ASIM
*	Date:15 DEC 2025
*	Description: THIS FILE IS PWM TRIANGULAR WAVEFORM GENERATOR
 ******************************************************************************/

void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}

/* ================= SYSTEM CONTROL REGISTERS ================= */
#define SYSCTL_RCGCPWM_R   (*((volatile unsigned long *)0x400FE640))
#define SYSCTL_RCGCGPIO_R  (*((volatile unsigned long *)0x400FE608))

/* ================= GPIO PORT F REGISTERS(THE PIN IS PF1 ) ================ */
#define GPIO_PORTF_AFSEL_R (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PCTL_R  (*((volatile unsigned long *)0x4002552C))
#define GPIO_PORTF_DEN_R   (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_DIR_R (*((volatile unsigned long*) 0x40025400))

/* ================= PWM MODULE 1 REGISTERS ==============================
========since on pwm5  pwm2B'signal is passed to the M1PWM5pin============
====================so generator 2 is being used =========================*/
#define PWM1_2_CTL_R      (*((volatile unsigned long *)0x400290C0))
#define PWM1_2_GENB_R     (*((volatile unsigned long *)0x400290E4))
#define PWM1_2_LOAD_R    (*((volatile unsigned long *)0x400290D0))
#define PWM1_2_CMPB_R     (*((volatile unsigned long *)0x400290DC))
#define PWM_ENABLE_R     (*((volatile unsigned long *)0x40029008)) 

#define LOAD_VALUE 1600

/* ================= FUNCTION PROTOTYPES ================= */
void PWM_Init(void);
void delay(unsigned long i);
/* ================= PWM INITIALIZATION ================= */
void PWM_Init(void)
{
    SYSCTL_RCGCPWM_R |= 0x02;     // enable PWM1 clock   pg354
    SYSCTL_RCGCGPIO_R |= 0x20;    // enable GPIOF clock pg340

   // SYSCTL_RCC_R |= (1 << 20);   // enable PWM clock divider
    //SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000E0000) | 0x00060000; // divide by 64

    GPIO_PORTF_AFSEL_R |= 0x02;  // PF1 alternate function
    //GPIO_PORTF_PCTL_R &= ~0x000000F0;
    GPIO_PORTF_PCTL_R |= 0x00000050; // PF1 ? M1PWM5
    GPIO_PORTF_DEN_R |= 0x02;    // enable PF1 digital
    GPIO_PORTF_DIR_R |= 0x02;    // enable PF1 as an output

    PWM1_2_CTL_R &= ~(0x03 );         // disable generator 2, select mode up/down, 
    PWM1_2_GENB_R = 0x00000F0C;  // set on LOAD, clear on CMPA, 
    PWM1_2_LOAD_R =LOAD_VALUE;    // 16MHz /1KHz
    PWM1_2_CMPB_R = 0;           // start at 0% duty

    PWM1_2_CTL_R |=0x03;        // enable generator
    PWM_ENABLE_R |= 0x20;       // enable M1PWM5/pg1247
}

void delay(unsigned long value) {
    unsigned long i;
    for (i = 0; i < value; i++);
}
/* ================= MAIN FUNCTION ================= */
int main(void)
{
    unsigned int value;

    PWM_Init();

     while (1) {
        // Increase brightness
        for (value = 0; value < LOAD_VALUE; value++) {
            PWM1_2_CMPB_R = value;  // Increase duty cycle for Comparator B
            delay(1000);            
        }
        delay(10000);  

        // Decrease brightness
        for (value = LOAD_VALUE; value > 0; value--) {
            PWM1_2_CMPB_R = value;  // Decrease duty cycle for Comparator B
           delay(1000);           
        }
        delay(10000);  
    }
}



 
