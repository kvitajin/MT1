// *******   PWM2  (RC1, pin 16)   *******

//There are used a little different names of configuration bits here,
//for example WDTEN from datasheet.....................here is named WDTE
//See the 
//      file:///C:/Program%20Files%20(x86)/Microchip/xc8/v1.38/docs/chips/16f877a.html
//or see 
//       _readme_configuration_bits_PIC16F877A.docx
#pragma config CP=1
#pragma config DEBUG=1
#pragma config WRT=OFF
#pragma config CPD=1
#pragma config LVP=0
#pragma config BOREN=1
#pragma config PWRTE=OFF // if we want to debug program by the PICKIT3, 
                         //  then this bit PWRTE must be OFF
#pragma config WDTE=OFF
#pragma config FOSC=HS

//Prescale factor, meaning have only bits b1, b0
//#define PRESCALE_1 0b00000000
//#define PRESCALE_4 0b00000001
//#define PRESCALE_16 0b00000010
#include <xc.h>
#include <pic.h>

void waiting_for_blue_button_pressing_releasing(void)
{ unsigned int local_integer;

// blue button released.....PORTB.4 = 1
//while(PORTB & 0b00010000)   // if blue button released then cycling here
while(PORTBbits.RB4)   // if blue button released then cycling here
    PORTC ^= 0b00000100;

// it was just detected  blue button pressed.....PORTB.4 = 0

// waiting for continuous releasing of blue button (for continuous 1  in button signal)
for(local_integer=0-10000; local_integer++; PORTC ^= 0b00000100  )
    if(!RB4)
        local_integer=0-10000;

asm("NOP");
asm("NOP");
asm("NOP");

 }




void set_PWM2(unsigned char value_for_PR2, unsigned int value_for_duty, unsigned char value_of_prescale)
{
///////////////////////////////////////////////////////////////////////////////////////////
    PR2 = value_for_PR2;
///////////////////////////////////////////////////////////////////////////////////////////
    value_for_duty &= 0b0000001111111111;
    
//*******  b0 of duty  ( bit CCP2Y, contained in  SFR CCP2CON.4 ) *******
/*    if(value_for_duty & 0b0000000000000001)
        CCP2CON |= 0b00010000;  // CCP2CON.4 = 1
    else
        CCP2CON &= 0b11101111;// CCP2CON.4 = 0
*/
//    CCP2CONbits.CCP2Y=(value_for_duty & 0b0000000000000001)?1:0;
    CCP2Y=(value_for_duty & 0b0000000000000001)?1:0;

//*******  b1 of duty  ( bit CCP2X, contained in  SFR CCP2CON.5 ) *******
/*    if(value_for_duty & 0b0000000000000010)
        CCP2CON |= 0b00100000;  // CCP2CON.5 = 1
    else
        CCP2CON &= 0b11011111;// CCP2CON.5 = 0
*/
//    CCP2CONbits.CCP2X=(value_for_duty & 0b0000000000000010)?1:0;
    CCP2X=(value_for_duty & 0b0000000000000010)?1:0;

    // rest  bits  b9 - b2  of  duty  ( i.e.  SFR  CCPR2L )
   value_for_duty >>= 2;
   CCPR2L = value_for_duty;

   // ******* prescale factor *******
   switch(value_of_prescale)
   {   case 1: T2CKPS1 = 0;     // prescale 1:1
                    T2CKPS0 = 0;     //
                    break;
       case 4: T2CKPS1 = 0;     // prescale 1:4
                    T2CKPS0 = 1;     //
                    break;
       case 16: T2CKPS1 = 1;    // prescale 1:16
                    break;
       default: break;
   }

   asm("NOP");
   asm("NOP");
   asm("NOP");
}




void main(void)
{ unsigned char value_for_PR2_main, value_for_prescale_main;
    unsigned int value_for_duty_main;

TRISC &= 0b11111001;
OPTION_REG &= 0b01111111;   // internal PORTB pull-up resistors ON (for blue button, RB4)
T2CON = 0b00000100;
CCP2CON = 0b00001111;   //regime of CCP2,  CCP2 will work as PWM


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
value_for_PR2_main = 0x3F;
value_for_duty_main = 1;   // maximal value is 0000 0011 1111 1111,  10-bit value (b9 - b0)
value_for_prescale_main = 4;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
while(1)
{
set_PWM2(value_for_PR2_main, value_for_duty_main, value_for_prescale_main);

asm("NOP");
asm("NOP");
asm("NOP");

waiting_for_blue_button_pressing_releasing();

asm("NOP");
asm("NOP");
asm("NOP");
}
}