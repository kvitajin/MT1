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




#include <pic.h>
#define _XTAL_FREQ 18432000


//void interrupt timer (void){
//    if (PORTBbits.RB2)
//        ;
//}
int flag=0;
int time, iter;
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
void btn_check(){
    if (!PORTBbits.RB4 && !flag){       //tlacitko je zmacknute
        flag=1;
    }
    else if (PORTBbits.RB4 && flag){    //tlacitko je uvolneno
        flag =0;
        iter =iter*2;
    }
}

void data_up(){
    PORTBbits.RB1=1;
    PORTBbits.RB2=1;
    asm("NOP");
    PORTBbits.RB2=0;
    PORTBbits.RB1=0;
}

void data_down(){
    asm("NOP");
    PORTBbits.RB2=1;
    asm("NOP");
    PORTBbits.RB2=0;
    asm("NOP");
}
void num9(){
    data_up();
    data_up();
    data_up();
    data_up();
    data_down();
    data_up();
    data_up();
    data_down();
}

void num1(){
    data_down();
    data_up();
    data_up();
    data_down();
    data_down();
    data_down();
    data_down();
    data_down();
}



void end(){
    data_down();
    data_down();
    data_down();
}

//data= rb1, clock rb2
void main(void)
{ unsigned char value_for_PR2_main, value_for_prescale_main;
    unsigned int value_for_duty_main;

TRISC &= 0b11111001;
OPTION_REG &= 0b01111111;   // internal PORTB pull-up resistors ON (for blue button, RB4)
T2CON = 0b00000100;
CCP2CON = 0b00001111;   //regime of CCP2,  CCP2 will work as PWM
PORTB &= 0b11111001;
TRISB &= 0b11111001;

time=900;
iter=1;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
value_for_PR2_main = 0x3F;
value_for_duty_main = 1;   // maximal value is 0000 0011 1111 1111,  10-bit value (b9 - b0)
value_for_prescale_main = 4;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//
//inicializace displeje
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
data_up();

for(int i =0; i<40;++i){
    data_down();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

while(1){
    data_up();
    num9();
    num9();
    num9();
    num9();
    end();
    btn_check();
    for(int i =0; i<time;++i){
        for(int j=0; j<iter; ++j){
            __delay_ms(1);
            btn_check();    
        }
    }
    
    data_up();
    num1();
    num1();
    num1();
    num1();
    end();
    for(int i =0; i<time;++i){
        __delay_ms(1);
    }
   
}
}