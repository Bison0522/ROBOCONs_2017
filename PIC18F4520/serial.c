#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "serial.h"

char timeout( char v )
{
	static unsigned long time = 0;
	
	if (++time > TIMEOUT_LIMIT)
	{
		// ??????
		time = 0;
		return -1;
	}
	else if (v == 0)
	{
		// ??
		time = 0;
		return 1;
	}
	// ??
	return 0;
}

void serial_init()
{
	TRISCbits.RC6 = 0;
	TRISCbits.RC7 = 1;
	
	TXSTA	 = 0b10100110;
	RCSTA	 = 0b10110000;
	SPBRG	 = 255;
}	

void serial_baud(unsigned long baudrate)
{
	switch(baudrate)
	{
		case 9600:
			SPBRG = 255;
			break;
			
		case 19200:
			SPBRG = 129;
			break;
			
		case 57600:
			SPBRG = 42;
			break;
			
		case 76800:
			SPBRG = 32;
			break;
			
		case 115200:
			SPBRG = 21;
			break;
			
		case 230400:
			SPBRG = 10;
			break;
			
	}	
}	

void serial_fomat()
{
	
}

int serial_write(int sendData)
{
	int result;
	while((result = timeout(!TXSTAbits.TRMT)) == 0);
	if(result == -1)
	{
		return -1;
	}	
	TXREG = sendData & 0xff;
	if( TXSTAbits.TX9 )
	{
		TXSTAbits.TX9D = sendData >> 8;
	}	
	return 0;
}
/*
int serial_puts(rom char *str)
{
	int i, result = 0;
	for(i = 0; str[i] != '\0'; i++)
	{
		if(serial_write(str[i]) == -1)
		{
			result = -1;
		}
	}
	if(serial_write('\n') == -1)
	{
		result = -1;
	}
	return result;
}

int serial_printf(rom char *format, ...)
{
	int i, result = 0;
	va_list ptr;
	va_start(ptr, format);
	
	for(i = 0; format[i] != '\0'; i++)
	{
		char c;
		if((c = format[i]) == '%')
		{
			char s1[N] = {'\0'};
			int j;
			double d;
			switch(c = format[++i])
			{
				case 'd':
					sprintf(s1, "%d", va_arg(ptr, int));
					break;
					
				case 'f':
					// ?????
					//sprintf(s1, "%f", va_arg(ptr, double));
					
					break;
					
				case 'c':
					sprintf(s1, "%c", va_arg(ptr, char));
					break;
					
			}
			for(j = 0; s1[j] != '\0'; j++)
			{
				if(serial_write(s1[j]) == -1)
				{
					result = -1;
				}	
			}
		}
		else
		{
			if(serial_write(c) == -1)
			{
				result = -1;
			}	
		}
	}	
	va_end(ptr);
	return result;
}*/