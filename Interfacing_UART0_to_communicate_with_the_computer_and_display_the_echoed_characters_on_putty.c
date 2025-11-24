/******************************************************************************
*	Project name:
*	File name:
*	Author:
*	Date:
*	Description: This is a template file for UART0 configuration. It echoes back
*				 the character sent.
 ******************************************************************************/

#include "TM4C123.h"

/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/
//Register definitions for ClockEnable
#define 	SYSCTL_RCGC_UART_R				(*((volatile unsigned long*)0x400FE618))
#define 	SYSCTL_RCGC_GPIO_R				(*((volatile unsigned long*)0x400FE608))

//Register definitions for GPIOPortA
#define 	GPIO_PORT_A_AFSEL_R				(*((volatile unsigned long*)0x40004420))
#define 	GPIO_PORTA_PCTL_R				(*((volatile unsigned long*)0x4000452C))
#define 	GPIO_PORTA_DEN_R				(*((volatile unsigned long*)0x4000451C))
#define 	GPIO_PORTA_DIR_R				(*((volatile unsigned long*)0x40004400))

//Register definitions for UART0_ module
#define 	UART0_CTL_R						(*((volatile unsigned long*)0x4000C030))
#define  	UART0_IBRD_R					(*((volatile unsigned long*)0x4000C024))
#define  	UART0_FBRD_R					(*((volatile unsigned long*)0x4000C028))
#define 	UART0_LCRH_R					(*((volatile unsigned long*)0x4000C02C))
#define 	UART0_CC_R						(*((volatile unsigned long*)0x4000CFC8))
#define 	UART0_FR_R						(*((volatile unsigned long*)0x4000C018))
#define 	UART0_DR_R						(*((volatile unsigned long*)0x4000C000))

//Macros
#define 	UART_FR_TX_FF				0x20								//UART Transmit FIFO Full
#define 	UART_FR_RX_FE				0x10								//UART Receive FIFO Empty

//Function definitions
unsigned char UARTRx(void);
void UARTTx(unsigned char data);
void UARTTxString(char*pt);
void UARTRxString(char*bufPt, unsigned short max);

//System Initialization for Floating Point Unit
void SystemInit (void)
{
	  /* --------------------------FPU settings ----------------------------------*/
	#if (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                  (3UL << 11*2)  );               /* set CP11 Full Access */
	#endif
}

//Intialize and configure UART
void UARTInit(void)
{
	//Enable clock for UART0_ and GPIO PortA
	SYSCTL_RCGC_UART_R |=0x01;
	SYSCTL_RCGC_GPIO_R |=0x01;
	// Configure GPIO as UART (AFSEL,PCTL,DEN)
	GPIO_PORT_A_AFSEL_R |=0x03; 
	GPIO_PORTA_PCTL_R |=0x11;
	GPIO_PORTA_DEN_R |=0x03;
	GPIO_PORTA_DIR_R &=~(0x01);
	GPIO_PORTA_DIR_R |=0x02;
	//Disable UART
	UART0_CTL_R &= ~0x01;
	//Select system clock/PIOSC as UART Baud clock-> i am using PIOSC
	UART0_CC_R =0x05;
	//Set Baud Rate
	UART0_IBRD_R = 104;
	UART0_FBRD_R =10;
	
	//8bit word length,no parity bit,one stop bit,FIFOs enable
	UART0_LCRH_R=0x70;
	
	//Enable UART
	UART0_CTL_R |= 0X301;

}

//Wait for input and returns its ASCII value
unsigned char UARTRx(void)
{
	while((UART0_FR_R & UART_FR_RX_FE)!=0);
	return((unsigned char)(UART0_DR_R & 0x0FF));
}

/*Accepts ASCII characters from the serial port and
adds them to a string.It echoes each character as it
is inputted.*/
void UARTRxString (char *pt, unsigned short max)
{
    unsigned short i = 0;
    unsigned char c;

    while(1)
    {
        c = UARTRx();      // wait for character from PuTTY
        UARTTx(c);         // echo it back to PuTTY

        if(c == '\r')      // ENTER key pressed
        {
            pt[i] = '\0';  // terminate string
            return;
        }

        if(i < max)        // store in buffer
        {
            pt[i] = c;
            i++;
        }
    }
}


//Output 8bit to serial port
void UARTTx(unsigned char data)
{  
	while((UART0_FR_R & UART_FR_TX_FF)!=0);
	UART0_DR_R=data;
}


//Output a character string to serial port
void UARTTxString(char *pt)
{
    while(*pt)           // run until null terminator
    {
        UARTTx(*pt);     // transmit one character
        pt++;            // move to next char
    }
}


int main(void)
{

	char string[17];

	UARTInit();

	//The input given using keyboard is displayed on hyper terminal
	//.i.e.,data is echoed
	UARTTxString("EnterText:");

while(1)
	{
		UARTRxString(string,16);
		UARTTxString("\r\n");
	}
}
