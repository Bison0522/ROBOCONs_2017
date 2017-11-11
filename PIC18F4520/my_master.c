// CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = ON        // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 0         // Brown Out Reset Voltage bits (Maximum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 1        // Watchdog Timer Postscale Select bits (1:1)

// CONFIG3H
#pragma config CCP2MX = PORTBE  // CCP2 MUX bit (CCP2 input/output is multiplexed with RB3)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

#include<xc.h>
#include"serial.h"

#define SLAVE_ADDR 0b01101010
#define TIME_OUT 1000

long long int out = 0;
int error = 0x00;
long long int cnt0 = 0;
double time = 0;

void interrupt Timer0(){
    if(INTCONbits.TMR0IF){
        cnt0++;
        INTCONbits.TMR0IF = 0;
        TMR0H	= 0xFC;
        TMR0L	= 0x18;
        LATBbits.LB0 = 1;
    }
}

void timer_start(){
    TMR0H	= 0xFC;
    TMR0L	= 0x18;
    INTCONbits.TMR0IE = 1;
}

void timer_stop(){
    INTCONbits.TMR0IE = 0;
    time = cnt0 * 0.0001;
}

void timer_reset(){
    cnt0 = 0;
}

char Timeout(char a){
    out++;
    if(a == 1 && out < TIME_OUT){
        out = 0;
        return 0;
    }
    else if(out > TIME_OUT){
        return -1;
    }
    return 1;
}

void i2c_write(int addr, char *data, char num, char start, char stop){
    char ans = 0;
    int cnt = 0;
    if(start){
        SSPCON2bits.SEN = 1;
        while(ans = Timeout(SSPCON2bits.SEN == 0));
        if(ans == -1){
            return;
        }
    }
    PIR1bits.SSPIF = 0;
    SSPBUF = (addr << 1);
    while(ans = Timeout(PIR1bits.SSPIF));
    if(ans == -1){
        return;
    }
    PIR1bits.SSPIF = 0;
    while(ans = Timeout(SSPCON2bits.ACKSTAT == 0));
    if(ans == -1){
        return;
    }
    for(cnt = 0; cnt < num; cnt++){
        SSPBUF = data[cnt];
        while(ans = Timeout(PIR1bits.SSPIF));
        if(ans == -1){
            return;
        }
        PIR1bits.SSPIF = 0;
        while(ans = Timeout(SSPCON2bits.ACKSTAT == 0));
        if(ans == -1){
            return;
        }
        PIR1bits.SSPIF = 0;
    }
    if(stop){
        SSPCON2bits.PEN = 1;
        while((ans = Timeout(SSPCON2bits.PEN)) == 0);
        if(ans == -1){
            return;
        }
    }
    else if(stop == 2){
        SSPCON2bits.RSEN = 1;
        while((ans = Timeout(SSPCON2bits.RSEN)) == 0);
        if(ans == -1){
            return;
        }
    }
}

void i2c_read(int addr, char *data, char num){
    char ans = 0;
    int cnt = 0;
    SSPCON2bits.SEN = 1;
    while(ans = Timeout(SSPCON2bits.SEN == 0));
    if(ans == -1){
        return;
    }
    PIR1bits.SSPIF = 0;
    SSPBUF = (addr << 1) + 1;
    while(ans = Timeout(PIR1bits.SSPIF));
    if(ans == -1){
        return;
    }
    PIR1bits.SSPIF = 0;
    while(ans = Timeout(SSPCON2bits.ACKSTAT == 0));
    if(ans == -1){
        return;
    }
    SSPCON2bits.RCEN = 1;
    for(cnt = 0; cnt < num; cnt++){
        while(ans = Timeout(SSPSTATbits.BF));
        if(ans == -1){
            return;
        }
        if(cnt == (num - 1)){
            SSPCON2bits.ACKDT = 1;
        }
        else if(cnt < (num - 1)){
            SSPCON2bits.ACKDT = 0;
        }
        SSPCON2bits.ACKEN = 1;
        while(ans = Timeout(SSPCON2bits.ACKEN == 0));
        if(ans == -1){
            return;   
        }
        data[cnt] = SSPBUF;
        if(PIR1bits.SSPIF){
            PIR1bits.SSPIF = 0;
        }
        else{
            return;
        }
    }
    SSPCON2bits.PEN = 1;
    //SSPCON2bits.RSEN = 1;
    while(ans = Timeout(SSPCON2bits.PEN == 0));
    if(ans == -1){
        return;
    }
}

void init(){
    TRISA	= TRISB	= TRISC	= TRISD	= TRISE	= 0;
	LATA	= LATB	= LATC	= LATD	= LATE	= 0;
	TRISC	= 0b00011000;
    SSPSTAT = 0b00100000;
    SSPCON1 = 0b00101000;
    SSPCON2 = 0b00001000;
    /**********************************************
     400(kbps) = FOSC / ((SSPADD + 1) * 4)           
               = 40(MHz) / ((14 + 1) * 4)
    ***********************************************/
    SSPADD = 24;
    T0CON = 0b10001000;
    RCONbits.IPEN = 0;
    INTCON2bits.TMR0IP = 1;
    INTCONbits.TMR0IE = 0;
    INTCONbits.GIE = 0;
    TMR0H = 0xFC;
    TMR0L = 0x18;
    serial_baud(9600);
    serial_init();
    INTCONbits.GIE = 1;
}

void main(){
    init();
    char tmp = 0;
    int jyro = 0,
        JYRO = 0,
        cnt = 0,
        turn_value = 0;
    float J_deg = 0,
          MAE = 0,
          deg = 0;
    double before = 0,
           Jyro = 0;
    char W_data[1] = {0x0F},
         R_data[1] = {0};
    i2c_write(SLAVE_ADDR, W_data, 1, 1, 0);
    i2c_read(SLAVE_ADDR, R_data, 1);
    if(R_data[0] == 0xD4){
        LATBbits.LB1 = 1;
        serial_write(R_data[0]);
    }
    else {
        LATBbits.LB1 = 0;
        return;
    }
    W_data[0] = 0x20;
    char W_data2[1] = {0x0F};
    i2c_write(SLAVE_ADDR, W_data, 1, 1, 0);
    i2c_write(SLAVE_ADDR, W_data2, 1, 0, 1);
    while(1){
        W_data[0] = 0x2D;
        i2c_write(SLAVE_ADDR, W_data, 1, 1, 0);
        i2c_read(SLAVE_ADDR, R_data, 1);
        jyro = R_data[0];
        jyro <<= 8;
        W_data[0] = 0x2C;
        i2c_write(SLAVE_ADDR, W_data, 1, 1, 0);
        i2c_read(SLAVE_ADDR, R_data, 1);
        jyro |= R_data[0];
        timer_stop();
        W_data[0] = 0x26;
        i2c_write(SLAVE_ADDR, W_data, 1, 1, 0);
        i2c_read(SLAVE_ADDR, R_data, 1);
        tmp = 40 - R_data[0];
        J_deg = jyro;
        if(J_deg > 0x8000){
            J_deg = -(0xFFFF - J_deg);
        }
        J_deg = J_deg * 0.00875;
        //serial_write(cnt0);
        deg += ((J_deg + before) * time / 2) / 75 * 900;
        before = J_deg;
        if(deg > 360 || deg < -360){
            turn_value = (int)deg / 360;
            deg -= (360 * turn_value);
        }
        if(deg - MAE < 0.01 && deg - MAE > -0.01){
            deg = MAE;
        }
        MAE = deg;
        cnt++;
        JYRO = deg;
        serial_write(0xAA);
        serial_write(0xBB);
        serial_write(JYRO >> 8);
        serial_write(JYRO);
        timer_reset();
        timer_start();
        LATBbits.LB0 = 0;
    }
}
