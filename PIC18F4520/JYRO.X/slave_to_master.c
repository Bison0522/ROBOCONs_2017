#pragma config OSC		= HSPLL		// Oscillator Selection bits
#pragma config FCMEN	= OFF	// Fail-Safe Clock Monitor Enable bit
#pragma config IESO	= OFF	// Internal/External Oscillator Switchover bit
#pragma config PWRT	= ON		// Power-up Timer Enable bit
#pragma config BOREN	= OFF	// Brown-out Reset Enable bits
#pragma config BORV	= 0		// Brown-out Reset Voltage bit
#pragma config WDT		= OFF	// Watchdog Timer Enable bit
#pragma config WDTPS	= 1		// Watchdog Timer Postscale Select bits
#pragma config MCLRE	= ON		// MCLR Pin Enable bit
#pragma config LPT1OSC	= OFF	// Low-Power Timer1 Oscillator Enable bit
#pragma config PBADEN	= OFF	// PORTB A/D Enable bit
#pragma config CCP2MX	= PORTBE	// CCP2 MUX bit
#pragma config STVREN	= OFF	// Stack Full/Underflow Reset Enable bit
#pragma config LVP		= OFF	// Single-Supply ICSP Enable bit
#pragma config XINST	= OFF	// Extended Instruction Set Enable bit
#pragma config DEBUG	= OFF	// Background Debugger Enable bit
#pragma config CP0		= OFF	// Code Protection bit Block 0
#pragma config CP1		= OFF	// Code Protection bit Block 1
#pragma config CP2		= OFF	// Code Protection bit Block 2
#pragma config CP3		= OFF	// Code Protection bit Block 3
#pragma config CPB		= OFF	// Boot Block Code Protection bit
#pragma config CPD		= OFF	// Data EEPROM Code Protection bit
#pragma config WRT0	= OFF	// Write Protection bit Block 0
#pragma config WRT1	= OFF	// Write Protection bit Block 1
#pragma config WRT2	= OFF	// Write Protection bit Block 2
#pragma config WRT3	= OFF	// Write Protection bit Block 3
#pragma config WRTB	= OFF	// Boot Block Write Protection bit
#pragma config WRTC	= OFF	// Configuration Register Write Protection bit
#pragma config WRTD	= OFF	// Data EEPROM Write Protection bit
#pragma config EBTR0	= OFF	// Table Read Protection bit Block 0
#pragma config EBTR1	= OFF	// Table Read Protection bit Block 1
#pragma config EBTR2	= OFF	// Table Read Protection bit Block 2
#pragma config EBTR3	= OFF	// Table Read Protection bit Block 3
#pragma config EBTRB	= OFF	// Boot Block Table Read Protection bit

#include <xc.h>

#define BITE 2
#define SLAVE_ADDR 0x30
#define JYRO_ADDR 0x10

char cnt1 = 0,
     cnt2 = 0,
     Jyro_Get[2] = {0},
     Jyro_Send[2] = {0},
     Get_Data[64] = {0},
     Send_Data[64] = {1,2,3,4,5,6,7,8,9,0}; 
void init(){
    TRISA = TRISB = TRISC = TRISD = TRISE = 0;
	LATA  = LATB  = LATC  = LATD  = LATE  = 0;
    TRISC = 0b00011000;   //RC3 = SCL, RC4 = SDA
    SSPSTAT = 0b00000100;
    SSPCON1 = 0b00100110;
    SSPCON2 = 0b00001001;
    SSPADD = SLAVE_ADDR;
    RCONbits.IPEN = 0;
    IPR1bits.SSPIP  = 1;
    PIE1bits.SSPIE  = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE  = 1;
}

void interrupt slave_inter(){
    if(PIR1bits.SSPIF){
        if(SSPSTATbits.R_W){
            LATBbits.LB0 = 1;
            SSPBUF = Send_Data[cnt1];
            cnt1++;
            if(cnt1 == BITE){
                cnt1 = 0;
            }
            PIR1bits.SSPIF = 0;
            SSPCON1bits.CKP = 1;
        }
        else{ 
            if(SSPSTATbits.BF){
                char read = SSPBUF;
            }
            SSPCON1bits.CKP = 1;
            PIR1bits.SSPIF = 0;
        }
    }
}

void main(){
    init();
    while(1){
        LATBbits.LB0 = 0;
    }
}
