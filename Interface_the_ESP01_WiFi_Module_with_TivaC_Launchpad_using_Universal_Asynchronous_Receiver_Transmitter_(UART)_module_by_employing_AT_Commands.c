/******************************************************************************
*Macros for Register Addresses and Values
*******************************************************************************/
//Register definitions for ClockEnable
#define 	SYSCTL_RCGC_UART_R				(*((volatile unsigned long*)0x400FE618))
#define 	SYSCTL_RCGC_GPIO_R				(*((volatile unsigned long*)0x400FE608))

//Register definitions for GPIOPortA
#define 	GPIO_PORTA_AFSEL_R				(*((volatile unsigned long*)0x40004420))
#define 	GPIO_PORTA_PCTL_R				(*((volatile unsigned long*)0x4000452C))
#define 	GPIO_PORTA_DEN_R				(*((volatile unsigned long*)0x4000451C))
#define 	GPIO_PORTA_DIR_R				(*((volatile unsigned long*)0x40004400))

//Register definitions for GPIOPortA
#define 	GPIO_PORTB_AFSEL_R				(*((volatile unsigned long*)0x40005420))
#define 	GPIO_PORTB_PCTL_R				(*((volatile unsigned long*)0x4000552C))
#define 	GPIO_PORTB_DEN_R				(*((volatile unsigned long*)0x4000551C))
#define 	GPIO_PORTB_DIR_R				(*((volatile unsigned long*)0x40005400))

//Register definitions for UART0_ module
#define 	UART0_CTL_R						(*((volatile unsigned long*)0x4000C030))
#define  	UART0_IBRD_R					(*((volatile unsigned long*)0x4000C024))
#define  	UART0_FBRD_R					(*((volatile unsigned long*)0x4000C028))
#define 	UART0_LCRH_R					(*((volatile unsigned long*)0x4000C02C))
#define 	UART0_CC_R						(*((volatile unsigned long*)0x4000CFC8))
#define 	UART0_FR_R						(*((volatile unsigned long*)0x4000C018))
#define 	UART0_DR_R						(*((volatile unsigned long*)0x4000C000))

//Register definitions for UART1_ module
#define 	UART1_CTL_R						(*((volatile unsigned long*)0x4000D030))
#define  	UART1_IBRD_R					(*((volatile unsigned long*)0x4000D024))
#define  	UART1_FBRD_R					(*((volatile unsigned long*)0x4000D028))
#define 	UART1_LCRH_R					(*((volatile unsigned long*)0x4000D02C))
#define 	UART1_CC_R						(*((volatile unsigned long*)0x4000DFC8))
#define 	UART1_FR_R						(*((volatile unsigned long*)0x4000D018))
#define 	UART1_DR_R						(*((volatile unsigned long*)0x4000D000))

//Macros
#define 	UART_FR_TX_FF				0x20								//UART Transmit FIFO Full
#define 	UART_FR_RX_FE				0x10								//UART Receive FIFO Empty



#include "TM4C123.h"
#include "top.h"

//System Initialization for Floating Point Unit
void SystemInit (void)
{
    #if (__FPU_USED == 1)
        SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
    #endif
}

unsigned char UART1Rx(void);
void UART1Tx(unsigned char data);
void UART1TxString(char*pt);
void UART1RxString(char*bufPt, unsigned short max);
void UART0TxString(char *pt);
void UART0Tx(unsigned char data);
unsigned char UART0Rx(void);
void UART0RxString(char*bufPt, unsigned short max);

char response;

//Intialize and configure UART
void UARTInit(void)
{
    // Enable clocks
    SYSCTL_RCGC_UART_R |= 0x03;  // UART0 + UART1
    SYSCTL_RCGC_GPIO_R |= 0x03;  // PORTA + PORTB

    // -------- UART0 on PA0, PA1 --------
    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_DEN_R   |= 0x03;
    GPIO_PORTA_DIR_R   &= ~0x01;
    GPIO_PORTA_DIR_R   |=  0x02;
    GPIO_PORTA_PCTL_R   = (GPIO_PORTA_PCTL_R & ~0xFF) | 0x11;

    // -------- UART1 on PB0, PB1 --------
    GPIO_PORTB_AFSEL_R |= 0X03;
    GPIO_PORTB_DEN_R   |= 0x03;     // FIX: was enabling only PB0
    GPIO_PORTB_DIR_R   &= ~0x01;
    GPIO_PORTB_DIR_R   |=  0x02;
    GPIO_PORTB_PCTL_R   = (GPIO_PORTB_PCTL_R & ~0xFF) | 0x11;

    //Disable UART
    UART0_CTL_R &= ~0x01;
    UART1_CTL_R &= ~0x01;

    //Select PIOSC
    UART0_CC_R =0x05;
    UART1_CC_R =0x05;

    //Set Baud Rate 
    UART0_IBRD_R = 104;
    UART0_FBRD_R = 10;
    UART1_IBRD_R = 8;
    UART1_FBRD_R = 44;

    //8bit, FIFO
    UART0_LCRH_R = 0x70;
    UART1_LCRH_R = 0x70;

    //Enable UART
    UART0_CTL_R |= 0X301;
    UART1_CTL_R |= 0X301;
}

//Wait for input and returns ASCII
unsigned char UART1Rx(void)
{
    while((UART1_FR_R & UART_FR_RX_FE)!=0);
    return((unsigned char)(UART1_DR_R & 0x0FF));
}

//ESP01 response reader
void UART1RxString (char*bufPt, unsigned short max)
{
    unsigned short i = 0;
    unsigned long timeout;
    unsigned char c;

    while(1)
    {
        timeout = 500000;

        while((UART1_FR_R & UART_FR_RX_FE) != 0)
        {
            if(timeout-- == 0)
            {
                bufPt[i] = '\0';
                return;
            }
        }

        c = UART1Rx();
        UART0Tx(c);  // show on UART0

        if(i < max-1)
        {
            bufPt[i] = c;
            i++;
        }
    }
}

//UART1 Tx
void UART1Tx(unsigned char data)
{
    while((UART1_FR_R & UART_FR_TX_FF)!=0);
    UART1_DR_R=data;
}

//UART1 Tx STRING
void UART1TxString(char *pt)
{
    while(*pt){
        UART1Tx(*pt);
        pt++;
    }
}

unsigned char UART0Rx(void)
{
    while((UART0_FR_R & UART_FR_RX_FE)!=0);
    return((unsigned char)(UART0_DR_R & 0x0FF));
}

void UART0RxString(char*bufPt, unsigned short max){
    int i;
    for (i=0; i<max; i++)
    {
        bufPt[i] = UART0Rx();
        UART0Tx(bufPt[i]); 
        if (bufPt[i] == '\r') break;
    }
}

void UART0Tx(unsigned char data)
{
    while((UART0_FR_R & UART_FR_TX_FF)!=0);
    UART0_DR_R=data;
}

void UART0TxString(char *pt){
    int i = 0;
    while (pt[i] != '\0')
    {
        UART0Tx(pt[i]);
        i++;
    }
}

int main(void)

{
    char string[17];
	int ulLoop;
// send AT
    UARTInit();
	 //UART1TxString(" AT+CIPSEND=4\r\n");
	//UART1RxString(string, 1000); // read response
	//for ( ulLoop = 0 ; ulLoop < 1000 ; ulLoop++ ) ;
	//UART1TxString("AT+CWJAP=\"V2043_21\",\"12345678\"\r\n");
	//UART1RxString(string, 1000); // read response
		//for ( ulLoop = 0 ; ulLoop < 1000; ulLoop++ ) ;
	//UART1TxString("AT+CIFSR\r\n");
	//UART1RxString(string, 1000); // read response
	
  UART1TxString("AT+CIPSTART=\"TCP\",\"192.168.4.1\",8080\r\n");

	UART1RxString(string, 10000);// read response
	
 
	
	
	  // send AT
        UART1RxString(string, 1000); // read response
 
        UART0TxString(string);     // print response on UART0
	

   
}
