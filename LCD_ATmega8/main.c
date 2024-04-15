/*
 * LCD_ATmega8.c
 *
 * Created: 4/15/2024 7:35:43 PM
 * Author : Roman Yarmak
 */ 

# define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#define RS_DATA			PORTB|=(1<<1)  /*select DATA register*/
#define RS_INSTRACTION	PORTB&=~(1<<1) /*select INSTRACTION register*/

#define	ENABLE_HIGHT			PORTB|=(1<<2)	/* set 1 or 0 to E pin */
#define ENABLE_LOW				PORTB&=~(1<<2)

char customChar[8] = {
	0b00100,
	0b00100,
	0b00100,
	0b01110,
	0b11111,
	0b11111,
	0b11111,
	0b01110
};
char heart[8] = {
	0b00000,
	0b00000,
	0b01010,
	0b11011,
	0b11111,
	0b11111,
	0b01110,
	0b00100
};

void displayInit(){
	_delay_ms(15);
}

void portInit(){
	/* pins 1,2 PORTB are for execute commands */
	DDRB|=(1<<1)|(1<<2);
	
	/* pins 1-4 PORTD for byte sending */
	/*
	screen PIN number		ATmega PIN number
	  D4			   ->      PORTD1
	  D5			   ->      PORTD2
	  D6			   ->      PORTD3
	  D7			   ->      PORTD4
	  */
	DDRD|=(1<<1)|(1<<2)|(1<<3)|(1<<4);
}

void sendData(char data){
	RS_DATA;
	_delay_us(15);
	ENABLE_HIGHT;  /* rise voltage pulse */
	_delay_us(15);
	PORTB=data>>4;
	ENABLE_LOW;    /* data transits with falling voltage pulse */
	_delay_us(15);
	ENABLE_HIGHT;  /* rise voltage pulse */
	_delay_us(15);
	PORTB=data>>4;
	_delay_us(15);
	ENABLE_LOW;    /* data transits with falling voltage pulse */
	_delay_us(15);
}

void sendInstraction(char command){
	RS_INSTRACTION;
	_delay_us(15);
	ENABLE_HIGHT;  /* rise voltage pulse */
	_delay_us(15);
	PORTB=command>>4;
	ENABLE_LOW;    /* data transits with falling voltage pulse */
	_delay_us(15);
	ENABLE_HIGHT;  /* rise voltage pulse */
	_delay_us(15);
	PORTB=command>>4;
	_delay_us(15);
	ENABLE_LOW;    /* data transits with falling voltage pulse */
	_delay_us(15);
}
int main(void)
{
	/*ATmega ports init*/
	portInit();
	
	
}

