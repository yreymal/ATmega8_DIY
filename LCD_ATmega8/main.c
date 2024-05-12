/*
 * LCD_ATmega8.c
 *
 * Created: 4/15/2024 7:35:43 PM
 * Author : Roman Yarmak
 */ 

# define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RS_DATA			PORTB|=(1<<1)  /*select DATA register*/
#define RS_INSTRACTION	PORTB&=~(1<<1) /*select INSTRACTION register*/

#define	ENABLE_HIGHT	PORTB|=(1<<2)	/* set 1 or 0 to E pin */
#define ENABLE_LOW		PORTB&=~(1<<2)
#define X50_SCHEDULER_CONST (0xFF - 32) /* 255 -50 in DEC, for TCTN0)


/* custom character bytes */ 
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
char human[] = {0x0E, 0x0E, 0x04, 0x0E, 0x15, 0x04, 0x0A, 0x0A};
char csmile[] = {0x00, 0x00, 0x0A, 0x00, 0x11, 0x0E, 0x00, 0x00};  
char emptyHeart[] = {0x00, 0x00, 0x0A, 0x15, 0x11, 0x0E, 0x04, 0x00}; 

void config_Timer_0(void );
void config_Timer_1(void );
char wait_ms(unsigned int amountOfMs);

void portInit(){
	/* pins 1,2 PORTB are for execute commands */
	DDRB|=(1<<1)|(1<<2);
	
	/* pins 0-3 PORTD for byte sending */
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
	
	_delay_ms(100);
	sendInstraction(0b00110000);
	_delay_ms(100);
	sendInstraction(0b00110000);
	_delay_us(100);
	sendInstraction(0b00110000); /* initialization */
	_delay_ms(100);
	sendInstraction(0b00001000);
	_delay_ms(100);
	sendInstraction(0b00000010); /* return home */
	_delay_ms(100);
	sendInstraction(0b00101000); /* 5X8 dots format display mode, 2 line, 4-bit bus mode */
	_delay_ms(100);
	sendInstraction(0b00001100); /* display ON, cursor OFF */
	_delay_ms(100);
	sendInstraction(0b00000001); /* clear display */
	_delay_ms(100);
	sendInstraction(0b00000110);  /* moves to right and DDRAM address is increased by 1 */
	_delay_ms(100);
	
}

char wait_us(unsigned char amountOfUs){			/* A 0 timer for us with overflow interrupt */
	unsigned char sreg;
		if(amountOfUs<=0xFF){					    /* jle 1 byte */
			config_Timer_0();
			sreg = SREG;						/* save global interrupt Flag, SREG - HW register */
			cli();								/* disable interrupts */
			TCNT0 = (0xFF-amountOfUs);				/* timer will start to calc not from 0, but from the diff, so it will be the put time to overflow */
		
			SREG = sreg;						/* restore global interrupt flag */
		
			return 0;
	}
	return 1;
}

char wait_ms(unsigned int amountOfMs){			/* A 1 timer for ms with compare interrupt */
	unsigned char sreg;
		if(amountOfMs<=0xFFFF){					/* jle 16 bit */
			config_Timer_1();
			sreg = SREG;						/* save global interrupt Flag, SREG - HW register */
			
			cli();								/* disable interrupts */
			OCR1AH = (amountOfMs>>8);			/* put 16 bit var into two 8 bit registers */
			OCR1AL = (amountOfMs<<8);
 			//sei();							/* enable interrupts */
			SREG = sreg;						/* restore global interrupt flag */
			
			return 0;
		}
		return 1;
}


ISR(TIMER0_OVF_vect){

	
	
	TCCR0&=~(1<<CS00);  /* turn off 0 Timer */
}


ISR(TIMER1_COMPA_vect){
	TCNT1=0;             /*set to 0  1st timer register */
	TCCR1B&=~(1<<CS11);  /*turn off 1st Timer */
	sei();
}




void config_Timer_0(void ){
	TCCR0=(1<<CS00);			/* No prescaling */
	TIMSK |= (1<<TOIE0);		/* enable overflow interrupt */
	sei();
	
}

void config_Timer_1(void ){
	TCCR1B=(1<<CS11);			/* clk/8 prescaling */
	TIMSK |= (1<<OCIE1A);		/* enable compare interrupt */
	
}








int main(void)
{
	/*ATmega ports init*/
	portInit();
	
	displayInit();
	
	sendInstraction(0b01001000);       /* Write to CGRAM with 8 value address */
	
	for(int i =0; i<7; ++i){
		sendData(emptyHeart[i]);
	}
	sendInstraction(0b10000000|0x45);   /* set DDRAM address (0b10000000) and select position with logic add ( | ), pos = middle of a 2 line */
	sendData(0b1);                      /* display character */
	
	

	sei();  /* set global interrupt enable */
}

