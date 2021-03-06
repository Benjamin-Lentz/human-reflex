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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LED PORTBbits.RB4
#define BUTT PORTBbits.RB1

//GLOBAL VARIABLE declaration
unsigned int counter = 0;
//END GLOBAL VARIABLE declaration

//FUNCTION PROTOTYPES declaration
void UART_Send(int data);
int UART_Recieve();
unsigned int getTimeSpent_ms();
void timeSpentToString(unsigned int time, char tab[4]);
int randomGenerator(int min_number, int max_number);
void sleep_ms(int sleeptime);
void displayTime(int timeMS, char time[4]);
//END FUNCTION PROTOTYPES declaration

void main(void) 
{
    
    TRISB = 0b00000000; //Setting port Bs to output
    TRISBbits.TRISB1 = 1;//setting the port B1 to input
	PORTB = 0; //Setting port Bs value to 1
	ADCON1 = 0b00000110;//Disabling analog inputs
    
    //UART Conf.
	TXSTA = 0;
	RCSTA = 0b10000000;
	SPBRG = 25;
    //END UART Conf.
    
    //Timer 1 Conf.
    T1CON = 255;
    T1CONbits.TMR1CS = 0;
    
    TMR1ON = 0;     //Disable timer
    TMR1IE=1;       //Enable timer interrupt bit in PIE1 register
    GIE=1;          //Enable Global Interrupt
    PEIE=1;         //Enable the Peripheral Interrupt
    
    TMR1H=0b00111100;
    TMR1L=0b10101111;
    //END Timer 1 Conf.
    char time[4] = {'0','0','0','0'};
    
    static __bit isRunning;
    static __bit hasBegun;
    
    isRunning = 1;
    hasBegun = 0; 
    
    LED = 1;
    
    while(1)
    {
        if(hasBegun)
        {
            int randomNumber = randomGenerator(1, 3000);
            isRunning = 1;
            
            //reset timer 1
            counter = 0;
            TMR1H=0b00111100;
            TMR1L=0b10101111;
            
            __delay_ms(3000);
            sleep_ms(randomNumber);
            
            LED = 1;
            TMR1ON = 1;     //Start Timer1 
            while (isRunning) 
            {
                if(BUTT == 0)
                {
                    //stop timer
                    TMR1ON = 0;
                    //stop loop
                    isRunning = 0;
                    //get time
                    int t = getTimeSpent_ms();
                    //display time
                    displayTime(t, time);
                    
                    LED = 0;
                }
            }

            
        }
        else
        {
            if(BUTT == 0)
            {
                hasBegun = 1;
                LED = 0;
            }
        }
        
    }
    return;
}

void UART_Send(int data)
{
    PIR1bits.TXIF = 0;
    TXSTAbits.TXEN = 1;
    TXREG = data;
    while(!PIR1bits.TXIF){}
    __delay_ms(1);
}

int UART_Recieve()
{
    RCSTAbits.CREN = 1;
    while(!PIR1bits.RCIF){}
    int data = RCREG;
    RCSTAbits.CREN = 0;
    return data;
}

__interrupt() void MY_ISR(void)
{
    if(TMR1IF==1)
    {
        counter++;

        
        //put the register to count only 50k values before interrupt
        TMR1H=0b00111100;
        TMR1L=0b10101111;
        TMR1IF=0;       // Clear timer interrupt flag
    } 
}

unsigned int getTimeSpent_ms()
{
    //counter increase every 100 ms and the register every 2us
    unsigned int i = (((unsigned int)TMR1H << 8) + TMR1L);
    
    return (unsigned int)(counter*100 + (i-13535)/500);
}

void timeSpentToString(unsigned int time, char tab[4])
{
    tab[0] = (char)(48 + (time%10000 - time%1000)/1000);
    tab[1] = (char)(48 + (time%1000 - time%100)/100);
    tab[2] = (char)(48 + (time%100 - time %10)/10);
    tab[3] = (char)(48 + time%10);
    
}

int randomGenerator(int min_number, int max_number)
{
    return (rand() % (max_number + 1 - min_number) + min_number);
}

void sleep_ms(int sleeptime)
{
    int i;
    for (i = 0; i < sleeptime; i++) 
    {
        __delay_ms(1);
    }
}

void displayTime(int timeMS, char time[4])
{
    UART_Send('R');
    UART_Send('e');
    UART_Send('s');
    UART_Send('u');
    UART_Send('l');
    UART_Send('t');
    UART_Send('a');
    UART_Send('t');
    UART_Send(' ');
    UART_Send(':');
    UART_Send(' ');
    
    timeSpentToString(timeMS, time);
    
    for (int i = 0; i < 4; i++) 
    {
       UART_Send(time[i]);
    }
    UART_Send('m');
    UART_Send('s');
    UART_Send('\n');
    
}