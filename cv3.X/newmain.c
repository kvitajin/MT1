/*
 The PIC16F877A communicates with SAA1064(LED display controller) using I2C BUS.
 Crystal 18.432MHz.
 The contain of variable abc(16bit  uint) is displayed in hexadecimal system, initial value is 0x5678.
  A man handles it by pressing of blue button(connected to pin PORTB.4).
               R.Novak    VSB-TU Ostrava
                       program works OK
*/


#include <xc.h>
#include <pic.h>



#define MASK_PORTC_SDA      0b00010000
#define MASK_PORTC_SCL      0b00001000
#define MASK_CLEARING_SDA_SCL_as_PINS_of_PORTC      0b11100111
#define blue_button_pressed (!(PORTB & 0b00010000))
#define blue_button_released (PORTB & 0b00010000)


//__CONFIG _CP_OFF & _WDT_OFF & _BODEN_ON & _PWRTE_OFF & _HS_OSC & _WRT_OFF & _LVP_OFF & _CPD_OFF 

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


bit I2CStarted=0;
bit I2CError=0;
bit I2C_SDA_free;

unsigned char field[4];

//REM keyword "const" - value is created in FLASH (not in RAM)
//          it can be only read (writing some value to it is impossible)
const unsigned char codes_for_display[] = {
0b00111111,	// code for displaying of  0
0b00000110,	// code for displaying of  1
0b01011011,	// code for displaying of  2
0b01001111,	// code for displaying of  3
0b01100110,	// code for displaying of  4
0b01101101,	// code for displaying of  5
0b01111101,	// code for displaying of  6
0b00000111,	// code for displaying of  7
0b01111111,	// code for displaying of  8
0b01101111,	// code for displaying of  9
0b01110111,	// code for displaying of  A
0b01111100,	// code for displaying of  b
0b00111001,	// code for displaying of  C
0b01011110,	// code for displaying of  d
0b01111001,	// code for displaying of  E
0b01110001	// code for displaying of  F
};


void Delay_pause(unsigned char var)
{
    while(var--)
        ;
}


//It is doing  START Condition on the I2C
void I2CStart(void)
{

    if(!I2CStarted)
        goto    I2CSta01;

    RSEN = 1;   // RESTART Condition on the I2C
    goto    I2CSta02;            
            
I2CSta01:
if(!(PORTC & MASK_PORTC_SDA))
    goto    I2CSta03;   // SDA==0.....BUS is busy, go to I2CSta03
    
if(!(PORTC & MASK_PORTC_SCL))
    goto    I2CSta03;   // SCL==0.....BUS is busy, go to I2CSta03

SEN = 1;    // START Condition on the I2C
    
I2CSta02:	
I2CStarted = 1;

while(!SSPIF)
        ;
    SSPIF = 0;
    goto    I2CStart_conclusion;

I2CSta03:	
    I2CError = 1;

I2CStart_conclusion:
	asm("NOP");
	asm("NOP");
	asm("NOP");
}




// It is  STOP condition on the I2C
void I2CStop(void)
{
    PEN = 1;                // STOP condition on the I2C
    I2CStarted = 0;
    while(!SSPIF)
        ;
    SSPIF = 0;
}




void I2COut(unsigned char sended_value)
{
    SSPBUF = sended_value;

    while(!SSPIF)
        ;
    SSPIF = 0;
}
    
    

// config. of the   SAA1064
void ConfSAA1064(void)
{
I2CStart();      //	; I2C Start

asm("NOP");
asm("NOP");
asm("NOP");
asm("NOP");
asm("NOP");
asm("NOP");

I2COut(0x70);   //ADDRESS 70h, WRite bit active

asm("NOP");
asm("NOP");
asm("NOP");

I2COut(0x00);   //SUBADDRESS

asm("NOP");
asm("NOP");
asm("NOP");

I2COut(0x77);   //Control Byte

asm("NOP");
asm("NOP");
asm("NOP");

I2CStop();

asm("NOP");
asm("NOP");
asm("NOP");
}
    
    
    
void I2CSetup(void)     // I2C BUS hardware in processor is activated, works on 100kHz speed
{ unsigned char Count;

    SSPCON = 0x38;
    I2CStarted = I2CError = 0;
    SSPSTAT = 0x80;
    SSPCON2 = 0;
    SSPADD = 0x2D;
    if(PORTC & MASK_PORTC_SDA)
        goto    I2CSetup_conclusion;    // OK  signal SDA is "1"
    I2C_SDA_free=0;
    PORTC &= MASK_CLEARING_SDA_SCL_as_PINS_of_PORTC;
    SSPEN = 0;

// 9-times attempt to release the BUS
for(Count = 9; Count && (!I2C_SDA_free);   )
{   TRISC &= 0b11110111;    // SCL
    Delay_pause(240);
    TRISC |= 0b00001000;    //  SCL
    Delay_pause(240);
    if(PORTC & MASK_PORTC_SDA)
        I2C_SDA_free = 1;
    else
         --Count;
}
I2CSet02:
    SSPEN = 1;

I2CSetup_conclusion:
    asm("NOP");
    asm("NOP");
    asm("NOP");
}


// WrDis - function sends 4 values of field[] to SAA1064 controller
//              for displaying onto 4-position LED display
void WrDis(void)
{ unsigned char local_index;

    asm("NOP");
    asm("NOP");
    asm("NOP");

    I2CStart();

    asm("NOP");
    asm("NOP");
    asm("NOP");

    I2COut(0x70);   // ;address of SAA1064

    asm("NOP");
    asm("NOP");
    asm("NOP");

    I2COut(0x01);   //subaddress

    asm("NOP");
    asm("NOP");
    asm("NOP");

    // REM field[] should contain codes for displaying
    //  e.g.    abc = 0x5678
    //  then    field[0] =  01111111,	// code for displaying of  8
    //              field[1] = 00000111,	// code for displaying of  7
    //              field[2] = 01111101,	// code for displaying of  6
    //              field[3] = 01101101,	// code for displaying of  5
    for(local_index=0; local_index != 4; ++local_index)
	{
        I2COut(field[local_index]);                                       //zde vypisuju (asi) cisla num
        asm("NOP");
        asm("NOP");
        asm("NOP");
        }

    I2CStop();

    asm("NOP");
    asm("NOP");
    asm("NOP");

}
 
 

void main(void)
{
unsigned char index;
unsigned int    value_16bit,  abc;

//TRISC = 0b11111111;
TRISD = 0x00;
TRISE &= 0b11111110;
PORTE |= 0b00000001;

//blinking by LEDS connected on PORTD
PORTD = 0xFF;
for(index=20; index--; )
{   for(value_16bit=0; ++value_16bit; )
        ;
    PORTD ^= 0b11111111;
}
PORTD=0;


  

    
I2CSetup();             //  I2C setup
ConfSAA1064();      // configuring of the  SAA1064


abc = 0x9999;
while(1)
{
value_16bit = abc;
for(index=0; index != sizeof(field); ++index, value_16bit >>= 4)
    field[index] = codes_for_display[value_16bit & 0x000F];

WrDis();

Delay_pause(255);
abc=0x111;
value_16bit = abc;
for(index=0; index != sizeof(field); ++index, value_16bit >>= 4)
    field[index] = codes_for_display[value_16bit & 0x000F];

WrDis();
Delay_pause(255);

}

}