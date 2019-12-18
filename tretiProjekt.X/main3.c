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

#include <pic.h>
#define _XTAL_FREQ 18432000
#define SBIT_PS1  5
#define SBIT_PS0  4

// Enable the Global Interrupts
INTERRUPT_GlobalInterruptEnable();

// Enable the Peripheral Interrupts
INTERRUPT_PeripheralInterruptEnable();




int flag=0;
int iter;
int cisla=1111;
int cislo[4];
int predDelic=92;
int flagDelic=1;            /// default sekundove casovani

int znaky[16]={0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE4, 0xFE, 0xF6, 0xEE, 0x3E, 0x9C, 0x7A, 0x9E, 0x8E };
unsigned int Delic20;

void __interrupt() Preruseni (void){
    TMR1IF = 0;
    TMR1ON = 0;
    TMR1-=50000-7;
    TMR1ON = 1;
        
    if(!(--Delic20)){
        Delic20 = predDelic;
        ++cisla;
    }
}
void stridani_rychlosti(){
    if (predDelic>80 && !flagDelic){
        predDelic/=2;
        flagDelic=1;
    }
    else if (predDelic<80 && !flagDelic){
        predDelic*=2;
        flagDelic=1;
    }

}

void btn_check(){
    if (!PORTBbits.RB4 && !flag){       //tlacitko je zmacknute
        flag=1;
    }
    else if (PORTBbits.RB4 && flag){    //tlacitko je uvolneno
        flag =0;
        iter *=2;
        flagDelic=0;
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

void end(){
    data_down();
    data_down();
    data_down();
}

void parse_num(){
    int tmp=cisla;
    for(int i= 4; i;){
        cislo[--i]=tmp%10;
        tmp/=10;
    }
}

void num_2_time(){          //prevod cisla na hodinovy format
    if (cisla>=6000){
        cisla-=6000;
    }
    if ((cisla/10)%10==6){      //druha cifra
        cisla+=40;              //-60+100
    }
}

void main(void){
    //unsigned int value_for_duty_main;
    
    TRISD=0;
    TRISE0=0;
    RE0=0;
    asm("NOP");
    RE0=1;
    
    TRISC &= 0b11111001;
    OPTION_REG &= 0b01111111;   // internal PORTB pull-up resistors ON (for blue button, RB4)
    T2CON = 0b00000100;
    PORTB &= 0b11111001;
    TRISB &= 0b11111000;

    TRISB1=0;
    Delic20=20;
    TMR1=65536-50000;
    T1CON=0b00000001;
    PIE1=1;
    INTCON=0b11000000;

    TRISD=0x00;    //Configure PORTD as output to blink the LEDs
    TMR1IE=1;       //Enable timer interrupt bit in PIE1 register
    GIE=1;          //Enable Global Interrupt
    PEIE=1;         //Enable the Peripheral Interrupt
    TMR1ON = 1;     //Start Timer1

    iter=1;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///inicializace displeje
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    data_up();

    for(int i =0; i<40;++i){
        data_down();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    while(1){
        num_2_time();
        parse_num();
        btn_check();
        stridani_rychlosti();
        data_up();
        for(int j=0; j<4; ++j ){                //4 cifry
            for(int i = 0b10000000; i;i/=2){
                if ((j==0 || j==2) && cislo[j]==0){
                    data_down();
                }
                else if(znaky[cislo[j]]&i){
                    data_up();
                }
                else if (j==1 && i==0b00000001){
                    data_up();
                }
                else{
                    data_down();
                }
            }
        }
        end();
    }
}