/*
 * File:   mymain.c
 * Author: LentzTech
 *
 * Created on March 18, 2021, 10:14 AM
 */

// PIC16F876A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ 16000000

#include <xc.h>

#define LED PORTBbits.RB4

void UART_Send(int data);

void main(void) 
{
    
    TRISB = 0b00000000; //Setting port Bs to output
	PORTB = 255; //Setting port Bs value to 1
	ADCON1 = 0b00000110;//Disabling analog inputs
    
    //UART Conf.
	TXSTA = 0;
	RCSTA = 0b10000000;
	SPBRG = 25;
    //END UART Conf.
    
    while(1)
    {
        LED = !LED;
        UART_Send(LED);
        __delay_ms(500);
    }
    return;
}

void UART_Send(int data)
{
    PIR1bits.TXIF = 0;
    TXSTAbits.TXEN = 1;
    TXREG = data;
    while(!PIR1bits.TXIF){}
}
