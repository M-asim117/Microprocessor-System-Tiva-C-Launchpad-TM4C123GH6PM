#include "TM4C123.h"

void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}

#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long*)0x400FE608))

#define GPIO_PORTF_AFSEL_R (*((volatile unsigned long*)0x40025420))
#define GPIO_PORTF_DEN_R (*((volatile unsigned long*)0x4002551C))
#define GPIO_PORTF_DIR_R (*((volatile unsigned long*)0x40025400))
#define GPIO_PORTF_DATA_R (*((volatile unsigned long*)0x400253FC))
/*use values 0x04 to blink Blue and 0x08 to blink Green in place of 0x02*/
#define GPIO_PORTF_CLK_EN 0X20
#define GPIO_PORTF_PIN3_EN 0X02  
#define LED_ON        0X02
#define LED_OFF       ~(0X02)
#define DELAY         200000

int main(void) {
	volatile unsigned long ulLoop;
	/*enable the GPIO PORT THAT IS USED FOR THE ON BOARD LED*/
	SYSCTL_RCGCGPIO_R |=GPIO_PORTF_CLK_EN;
	/*DO A DUMMY READ TO INSERT A FEW CYCLES AFTER ENABLING THE PERIPHERAL*/
	ulLoop=SYSCTL_RCGCGPIO_R;
	/*ENABLE THE GPIO PIN PF3 ENABLE THE GPIO PIN FOR DIGITAL FUNCTION AND SET THE DIRECTION OUTPUT*/
	GPIO_PORTF_AFSEL_R &=~(GPIO_PORTF_PIN3_EN);
	GPIO_PORTF_DEN_R |=GPIO_PORTF_PIN3_EN;
	GPIO_PORTF_DIR_R |=GPIO_PORTF_PIN3_EN;
	/*LOOP FOREVER*/
	while(1){
		//turn on the led
		GPIO_PORTF_DATA_R |=LED_ON;
		//delay for a bit
		for(ulLoop=0;ulLoop<DELAY;ulLoop++){};
		//turn of the led
		GPIO_PORTF_DATA_R |=LED_OFF;	
		//delay for a bit
		for(ulLoop=0;ulLoop<DELAY;ulLoop++){};
		}
	}
	
