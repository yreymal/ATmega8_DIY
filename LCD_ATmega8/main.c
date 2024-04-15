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

#define	ENABLE_HIGHT	PORTB|=(1<<2)	/* set 1 or 0 to E pin */
#define ENABLE_LOW		PORTB&=~(1<<2)

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
	DDRD|=(1<<0)|(1<<1)|(1<<2)|(1<<3);
}

void sendData(char data){
	PORTD=(data>>4);
	RS_DATA;
	ENABLE_HIGHT;  /* rise voltage pulse */
	_delay_us(50);
	ENABLE_LOW;    /* data transits with falling voltage pulse */

	PORTD=data;
	ENABLE_HIGHT;
	_delay_us(50);
	ENABLE_LOW;    /* data transits with falling voltage pulse */
	_delay_us(50);
}

void sendInstraction(char command){
	PORTD=(command>>4);
	RS_INSTRACTION;
	ENABLE_HIGHT;  /* rise voltage pulse */
	_delay_us(50);
	ENABLE_LOW;    /* data transits with falling voltage pulse */
	
	PORTD=command;
	ENABLE_HIGHT;
	_delay_us(50);
	ENABLE_LOW;    /* data transits with falling voltage pulse */
	_delay_us(50);
}

void displayInit(){
	_delay_ms(15);
	sendInstraction(0b0011);
	_delay_ms(5);
	sendInstraction(0b0011);
	_delay_us(100);
	sendInstraction(0b0011); /* initialization */
	_delay_ms(1);
	sendInstraction(0b000010);
	_delay_ms(2);
	sendInstraction(0b000010);
	_delay_ms(1);
	sendInstraction(0b00101000); /* 5X8 dots format display mode, 2 line, 4-bit bus mode */
	_delay_ms(1);
	sendInstraction(0b00001100); /* display ON, cursor OFF */
	_delay_ms(1);
	sendInstraction(0b00000001); /*clear display*/
	_delay_ms(2);
	sendInstraction(0b00000110);  /* moves to right and DDRAM address is increased by 1 */
	_delay_ms(1);

}

int main(void)
{
	/*ATmega ports init*/
	portInit();
	
	displayInit();
	
	sendData('H');
	
	
}

