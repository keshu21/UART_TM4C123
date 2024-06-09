#include "TM4C123.h" 
#include <stdlib.h>
#include <stdint.h>

// Define the necessary registers
#define RCGCGPIO    (*((volatile unsigned long*) (0x400FE608)))
#define RCGCUART    (*((volatile unsigned long*) (0x400FE618)))
#define PORTB_BASE  0x40005000
#define PORTB_AFSEL_R  (*((volatile unsigned long*) (PORTB_BASE + 0x420)))
#define PORTB_PCTL_R   (*((volatile unsigned long*) (PORTB_BASE + 0x52C)))
#define PORTB_DIR_R    (*((volatile unsigned long*) (PORTB_BASE + 0x400)))
#define PORTB_DEN_R    (*((volatile unsigned long*) (PORTB_BASE + 0x51C)))
#define PORTB_DATA_R   (*((volatile unsigned long*) (PORTB_BASE + 0x3FC)))
#define PORTB_AMSEL_R  (*((volatile unsigned long*) (PORTB_BASE + 0x528)))

#define UART1_BASE    0x4000D000
#define UART_DR_R     (*((volatile unsigned long*) (UART1_BASE + 0x00)))
#define UART_FR_R     (*((volatile unsigned long*) (UART1_BASE + 0x18)))
#define UART_IBRD_R   (*((volatile unsigned long*) (UART1_BASE + 0x24)))
#define UART_FBRD_R   (*((volatile unsigned long*) (UART1_BASE + 0x28)))
#define UART_LCRH_R   (*((volatile unsigned long*) (UART1_BASE + 0x2C)))
#define UART_CTL_R    (*((volatile unsigned long*) (UART1_BASE + 0x30)))
#define UART_CC_R     (*((volatile unsigned long*) (UART1_BASE + 0xFC8)))

#define PORTF_BASE    0x40025000
#define PORTF_DIR_R   (*((volatile unsigned long*) (PORTF_BASE + 0x400)))
#define PORTF_DEN_R   (*((volatile unsigned long*) (PORTF_BASE + 0x51C)))
#define PORTF_DATA_R  (*((volatile unsigned long*) (PORTF_BASE + 0x3FC)))
#define PORTF_AFSEL_R (*((volatile unsigned long*) (PORTF_BASE + 0x420)))
#define PORTF_PCTL_R  (*((volatile unsigned long*) (PORTF_BASE + 0x52C)))
#define PORTF_AMSEL_R (*((volatile unsigned long*) (PORTF_BASE + 0x528)))

void UART_Init(void);
void PORTF_Init(void);
char UART_ReadChar(void);
void UART_WriteChar(char c);
void delay(void);

void UART_Init(void) {
    RCGCUART |= 0x02; // enable UART1 clock
    RCGCGPIO |= 0x02; // enable GPIO Port B clock
    
    PORTB_AFSEL_R |= 0x03; // PB0 and PB1
    PORTB_PCTL_R = 0x00000011; // configure PB0 and PB1 for UART
    PORTB_DEN_R |= 0x03; // digital enable PB0 and PB1
    PORTB_DIR_R |= 0x02; // PB1 as TX
    PORTB_DIR_R &= ~0x01; // PB0 as RX
    
    UART_CTL_R &= ~0x01; // disable UART
    UART_IBRD_R = 104; // 9600 baud rate (assuming 16 MHz clock)
    UART_FBRD_R = 11;
    UART_LCRH_R = 0x60 | 0x10; // 8-bit, FIFO enable
    UART_CC_R = 0x00; // use system clock
    UART_CTL_R = 0x301; // enable UART, TX and RX
}
void PORTF_Init() {
    RCGCGPIO |= 0x20; // enable GPIO Port F clock
    PORTF_DIR_R = 0x0E; // PF1, PF2, and PF3 as output (LEDs)
    PORTF_DEN_R |= 0x0E; // digital enable PF1, PF2, and PF3
    PORTF_AFSEL_R &=~0x0E; // disable alternate function
    PORTF_PCTL_R &=~0xFFF0; // configure as GPIO
    PORTF_AMSEL_R &= ~0x0E; // disable analog function
	  
}

#define UART_FR_RXFE 0x00000010 // UART Receive FIFO Empty
#define UART_FR_TXFF 0x00000020 // UART Transmit FIFO Full

int main() {
    char c[3] = {'B','R','G'};
    UART_Init();
    PORTF_Init();
    int i = 0;
    PORTF_DATA_R = 0;
    
		
    while(1) {
        UART_WriteChar(c[i]);
        char data = UART_ReadChar();
        
        if (data == 'R') {
            PORTF_DATA_R = 0x02; // Red LED on PF1
        } else if (data == 'G') {
            PORTF_DATA_R = 0x08; // Green LED on PF3
        } else if (data == 'B') {
            PORTF_DATA_R = 0x04; // Blue LED on PF2
        }
        i =i+1;
				if(i==3)
					i=0;
        delay();
				
    }
}

char UART_ReadChar(void) {
    while (UART_FR_R & UART_FR_RXFE); // Wait until the receive FIFO is not empty
    return (char)(UART_DR_R & 0xFF);
}

void UART_WriteChar(char c) {
    while (UART_FR_R & UART_FR_TXFF); // Wait until the transmit FIFO is not full
    UART_DR_R = c;
}

void delay(void) {
    unsigned long i = 8000000;
    while (i > 0) {
        i--;
    }
}
