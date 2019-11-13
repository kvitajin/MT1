/*
 * File:   newmain.c
 *
 * Created on Nede?a, 2018, okt€bra 7, 16:48
 */


#include <xc.h>

#define _XTAL_FREQ 18432000

#define C5 RC5
#define D0 RD0
#define R0 RE0
#pragma config FOSC = HS, WDTE = OFF, PWRTE = OFF, BOREN = ON, LVP = OFF, CPD = OFF, WRT = OFF, DEBUG = ON, CP = OFF

int pocitadlo = 2;
int pom = 0;

void __interrupt () Prerusenie(){
	  TMR1IF=0;
      //TMR1ON=0;
      TMR1H=0xEE;
      //TMR1ON=1; 
      
	  pocitadlo--;	  
}

void delay_ms(int x) {
    for (int i = 0; i < x; i++)
        __delay_ms(1);
}

void main(void) {
    
    TRISD = 0;
    PORTD = 0;
    TRISE0 = 0;
    TRISC5 = 0;
    TMR1H = 0xEE;
    T1CON = 1;
    PIE1 = 1;
    INTCON = 0b11000000;
    TMR1ON=1;
  
    asm("nop");
    
    OPTION_REG=0b00000000;
    
    int value;
    int address = 0b1010;
    //eeprom_write(address,0);
    int out = eeprom_read(address);

    //delay_ms(999);

    value = out + 50;
    eeprom_write(address,value);
    //out = eeprom_read(address);
    
    while(1){
        if(out < 128){
            if(pocitadlo < 2){
                R0 = 1;
                asm("nop");
                D0 = 1;
                R0 = 0;
                if(pocitadlo == 0)
                    pocitadlo = 2;
            }
            else if(pocitadlo > 1){
                R0 = 1;
                asm("nop");
                D0 = 0;
                R0 = 0;
            }
        }
        else{
            if(pocitadlo < 4){
                R0 = 1;
                asm("nop");
                D0 = 1;
                R0 = 0;
                if(pocitadlo == 0)
                    pocitadlo = 4;
            }
            else if(pocitadlo > 1){
                R0 = 1;
                asm("nop");
                D0 = 0;
                R0 = 0;
            }
        }
    }
}