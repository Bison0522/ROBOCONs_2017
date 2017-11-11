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


#include <xc.h>

#define  I2C_SLAVE_ADDRESS 0xD4
#define TIMEOUT_LIMIT 1000
char timeout( char v );

void i2cSendSingleByte( char address, char data );
void i2cSendMultiByte( char address, char* data, unsigned char num );


char timeout( char v ){
	static unsigned long long time = 0; //???????????
	
	if ( ++time > TIMEOUT_LIMIT ){
		// ????????????????-1???
		time = 0;
		return -1;
	} else if ( v == 0 ){
		// ????
		time = 0;
		return 1;
	}
	
	// ??
	return 0;
}

void i2cSendSingleByte( char address, char data ){
	i2cSendMultiByte( address, &data, 1 );
}

void i2cSendMultiByte( char address, char* data, unsigned char num ){
	int i = 0;
	char result = 0;
	
	// Start Condition ???
	SSPCON2bits.SEN = 1;
	// SEN?????? Start Condition ????????
	while ( ( result = timeout( SSPCON2bits.SEN ) ) == 0 );
	if ( result == -1 ){
		return;
	}
	PIR1bits.SSPIF = 0;
	
	// ????????????
	SSPBUF = address;
	
	// SSPIF??????????????
	while ( ( result = timeout( PIR1bits.SSPIF ) ) == 1 );
	if ( result == -1 ){
		return;
	}
	PIR1bits.SSPIF = 0;
	
	// ACKSTAT?????????????ACK???
	while ( ( result = timeout( SSPCON2bits.ACKSTAT ) ) == 0 );
	if ( result == -1 ){
		return;
	}
	PIR1bits.SSPIF = 0;
	
	for ( i = 0; i < num; i++ ){
		// ??????
		SSPBUF = data[ i ];
		LATB = data[ i ];
		// SSPIF?????????????
		while ( ( result = timeout( PIR1bits.SSPIF  ) ) == 1 );
		if ( result == -1 ){
			return;
		}
		PIR1bits.SSPIF = 0;
		
		// ACKSTAT?????????????ACK???
		while ( ( result = timeout( SSPCON2bits.ACKSTAT ) ) == 0 );
        if(SSPCON2bits.ACKSTAT)LATBbits.LB0 = 1;
		if ( result == -1 ){
			return;
		}
		PIR1bits.SSPIF = 0;
	}
	
	// Stop Condition ???
	SSPCON2bits.PEN = 1;
	
	// PEN?????? Stop Condition ????????
	while ( ( result = timeout( SSPCON2bits.PEN ) ) == 0 );
	if ( result == -1 ){
		return;
	}
}

void init(){
	TRISA	= TRISB	= TRISC	= TRISD	= TRISE	= 0;
	LATA	= LATB	= LATC	= LATD	= LATE	= 0;
	
	// SDA???SCL??
	TRISC	= 0b00011000;
	// ???
	TRISD	= 0xFF;
	
	// I2C
	//	?400kbps
	//	?SDA?SCL???????
	//	?????
	SSPSTAT	= 0b00100000;
	SSPCON1	= 0b00101000;
	SSPCON2	= 0b00000000;
	
	// 400kbps = 40MHz / ( 4 * ( x + 1 ) )
	//		  x = 24
	SSPADD	= 24;
}

void main(){
	init();
	char data[ 2 ] = {0};
	//d[ 0 ] = s;
	//d[  ] = {100};
	//char date[8]={0b00000000,0b00001100,0b01010010,0b01011000,0b01010100,0b11111110,0b01001010,0b00000100};
	while ( 1 ){
        //i2cSendSingleByte( I2C_SLAVE_ADDRESS, 0xAE );
	    i2cSendMultiByte( I2C_SLAVE_ADDRESS, &data, 2 );
	}
}