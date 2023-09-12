/*
 * Mini_Project2.c
 *
 *  Created on: Sep 10, 2023
 *      Author: Youssif Hossam
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
int seconds = 0 , minutes = 0 , hours = 0 ;
unsigned char change_alarm_mode = 0 ;	// flag for changing alarm time mode
unsigned char alarm_mode = 0;			// flag for alarm mode
ISR (TIMER1_COMPA_vect){
	if(!change_alarm_mode){		// only change time when device is not in change alarm time mode
		if(alarm_mode)			// alarm mode case : decrement the time
			seconds--;
		else
			seconds++;			// normal mode : increment the time
	}
}
ISR (INT0_vect){
	seconds = 0; minutes = 0; hours = 0;	// Reseting the stop watch
	TCNT1 = 0 ;								// Start counting from zero.
}
ISR (INT1_vect){
	TCCR1B &=~ (1<<CS10 | 1<<CS11 | 1<<CS12);	// Stop the stop watch
}
ISR (INT2_vect){
	TCCR1B = 1<<WGM12 | 1<< CS10 | 1<<CS11 ;	// turn on the stop watch
}
void TIMER1_CTC_INIT(unsigned short compare_value){
	SREG |= (1<<7); 		// Enable global interrupts in MC
	TCCR1A = 1<<FOC1A ;		// non-PWM mode.
	TCCR1B = 1<<WGM12 | 1<< CS10 | 1<<CS11 ;
	/* --> CTC mode.
	 * --> clk/64 (From prescaler).
	 * --> T(timer) = 64us
	 * --> compare value = 1 ms / 64 us = 15625
	 */
	TCNT1 = 0 ;					// Start counting from zero.
	OCR1A = compare_value ; 	// Compare value = 15625
	TIMSK |= 1<<OCIE1A ;		// Output Compare A Match Interrupt Enable

}
void INT0_INIT(void){
	SREG |= (1<<7); 		// Enable global interrupts in MC
	MCUCR |= 1<<ISC01;		// The falling edge of INT0 generates an interrupt request
	GICR  |= 1<<INT0;		// External Interrupt Request 0 Enable
	DDRD &=~ 1<<PD2;		// Set INT0 as input pin
}
void INT1_INIT(void){
	SREG |= (1<<7); 					// Enable global interrupts in MC
	MCUCR |= 1<<ISC11 | 1<<ISC11;		// The rising edge of INT1 generates an interrupt request
	GICR  |= 1<<INT1;					// External Interrupt Request 1 Enable
	DDRD &=~ 1<<PD3;					// Set INT1 as input pin
}
void INT2_INIT(void){
	SREG |= (1<<7); 					// Enable global interrupts in MC
	MCUCSR	&=~ 1<<ISC2;				// The falling edge of INT2 generates an interrupt request
	GICR  |= 1<<INT2;					// External Interrupt Request 1 Enable
	DDRB &=~ 1<<PB2;					// Set INT2 as input pin
}
int main (void){
	unsigned char common_selector = 0;	// for selecting the 7-segment using the Multiplexing method.
	unsigned char dp_selector = 7;		// for selecting the 7-segment that will show the dp
	unsigned char set_flag = 0 , plus_flag = 0 , minus_flag = 0 , go_flag = 0 , arrow_flag = 0;
	// flag for the bush buttons
	DDRC |= 0x0F;			// decoder 4-pins to the first 4-pins in PORTC.
	DDRC |= 1<<PC4;			// dp pin as output pin
	DDRA |= 0x3F;			// first 6-pins in PORTA as the enable/disable pins for the six 7-segments
	DDRD &=~ 1<<PD4;		// Set button as input pin
	DDRD &=~ 1<<PD5;		// + button as input pin
	DDRD &=~ 1<<PD6;		// - button as input pin
	DDRD &=~ 1<<PD7;		// start button as input pin
	DDRD &=~ 1<<PD0;		// <-- button as input pin
	DDRA |= 1<<PA6;			// buzzer pin as output pin
	PORTD |= 0b11110001;	// internal pull activation
	PORTC &=~ 0x0F;			// number zero initially
	PORTC |= 1<<PC4;		// turn off dp led initially
	PORTA &=~ 0<<PA6;		// turn off the buzzer initially
	PORTA &=~ 0x3F;			// all 7-segment are off initially
	PORTD |= 1<<PD2;		// Internal pull-up resistor on INT0
	PORTB |= 1<<PB2;		// Internal pull-up resistor on INT2
	INT0_INIT();			// Interrupt 0 initialization
	INT1_INIT();			// Interrupt 1 initialization
	INT2_INIT();			// Interrupt 2 initialization
	TIMER1_CTC_INIT(15625);
	while(1){
		if(seconds > 59){
			minutes++;
			seconds = 0;
		}
		if(minutes > 59){
			hours++;
			minutes = 0;
		}
		if(hours > 23){
			seconds = 0 ; minutes = 0 ; hours = 0;
		}

		common_selector = 1;	// selecting the first 7-segment

		PORTA = (PORTA & 0xC0) | (common_selector & 0x3F);	// lighting up the 1st 7-segment
		PORTC = (PORTC & 0xF0) | (seconds%10 & 0x0F);		// showing the one's of seconds
		if(change_alarm_mode)		// if the system in changing time mode
			PORTC = (PORTC | 0x10) & ~((dp_selector==0) << PC4);	// light up the dp
		common_selector <<= 1;		// select the next 7-segment
		_delay_ms(6);

		PORTA = (PORTA & 0xC0) | (common_selector & 0x3F);	// lighting up the 2nd 7-segment
		PORTC = (PORTC & 0xF0) | (seconds/10 & 0x0F);		// showing the ten's of seconds
		if(change_alarm_mode)		// if the system in changing time mode
			PORTC = (PORTC | 0x10) & ~((dp_selector==1) << PC4);	// light up the dp
		common_selector <<= 1;		// select the next 7-segment
		_delay_ms(6);

		PORTA = (PORTA & 0xC0) | (common_selector & 0x3F);	// lighting up the 3rd 7-segment
		PORTC = (PORTC & 0xF0) | (minutes%10 & 0x0F);		// showing the one's of minutes
		if(change_alarm_mode)		// if the system in changing time mode
			PORTC = (PORTC | 0x10) & ~((dp_selector==2) << PC4);	// light up the dp
		common_selector <<= 1;		// select the next 7-segment
		_delay_ms(6);

		PORTA = (PORTA & 0xC0) | (common_selector & 0x3F);	// lighting up the 4th 7-segment
		PORTC = (PORTC & 0xF0) | (minutes/10 & 0x0F);		// showing the ten's of minutes
		if(change_alarm_mode)		// if the system in changing time mode
			PORTC = (PORTC | 0x10) & ~((dp_selector==3) << PC4);	// light up the dp
		common_selector <<= 1;		// select the next 7-segment
		_delay_ms(6);

		PORTA = (PORTA & 0xC0) | (common_selector & 0x3F);	// lighting up the 5th 7-segment
		PORTC = (PORTC & 0xF0) | (hours%10 & 0x0F);			// showing the one's of hours
		if(change_alarm_mode)		// if the system in changing time mode
			PORTC = (PORTC | 0x10) & ~((dp_selector==4) << PC4);	// light up the dp
		common_selector <<= 1;		// select the next 7-segment
		_delay_ms(6);
		PORTA = (PORTA & 0xC0) | (common_selector & 0x3F);	// lighting up the 6th 7-segment
		PORTC = (PORTC & 0xF0) | (hours/10 & 0x0F);			// showing the ten's of hours
		if(change_alarm_mode)		// if the system in changing time mode
			PORTC = (PORTC | 0x10) & ~((dp_selector==5) << PC4);	// light up the dp
		common_selector <<= 1;		// select the next 7-segment
		_delay_ms(6);

		if(((seconds + minutes + hours) == 0)  && alarm_mode){	// if the time of alarm reach zero
			for(int i = 0 ; i < 8 ; i++){	// flashing the 7-segments and the buzzer
				PORTA  |= 1<<PA6;
				PORTA |= 0x3F;
				_delay_ms(200);
				PORTA &=~ 1<<PA6;
				PORTA &=~ 0x3F;
				_delay_ms(200);
			}
			alarm_mode = 0;		// return back to the normal mode
			seconds = 0; minutes = 0; hours = 0;	// reset the time to zero
		}
		if (!(PIND & 1<<PD4)){	// if the user press set button
			_delay_ms(30);		// delay for debouncing
			if (!(PIND & 1<<PD4) && !set_flag){
				set_flag = 1;
				seconds = 0; minutes = 0; hours = 0;	// reset the time to zero
				dp_selector = 0;		// select the 1st 7-segment for the dp
				change_alarm_mode = 1;	// change the mode to changing the alarm time
			}
		}
		else
			set_flag = 0;

		if (!(PIND & 1<<PD5) && change_alarm_mode){	// if the user press + button
			_delay_ms(30);		// delay for debouncing
			if (!(PIND & 1<<PD5) && change_alarm_mode && !plus_flag){
				plus_flag = 1;
				// changing the 7-segments digits
				switch(dp_selector){
				case 0:
					seconds += 1;
					break;
				case 1:
					seconds += 10;
					break;
				case 2:
					minutes += 1;
					break;
				case 3:
					minutes += 10;
					break;
				case 4:
					hours += 1;
					break;
				case 5:
					hours += 10;
					break;
				}

			}
		}
		else
			plus_flag = 0;
		if (!(PIND & 1<<PD6) && change_alarm_mode){		// if the user press - button
			_delay_ms(30);		// delay for debouncing
			if (!(PIND & 1<<PD6) && change_alarm_mode && !minus_flag){
				minus_flag = 1;
				// changing the 7-segments digits
				switch(dp_selector){
				case 0:
					seconds -= 1;
					break;
				case 1:
					seconds -= 10;
					break;
				case 2:
					minutes -= 1;
					break;
				case 3:
					minutes -= 10;
					break;
				case 4:
					hours -= 1;
					break;
				case 5:
					hours -= 10;
					break;
				}
				if(seconds < 0)
					seconds = 0;
				if(minutes < 0)
					minutes = 0;
				if(hours < 0)
					hours = 0;
			}
		}
		else
			minus_flag = 0;

		if (!(PIND & 1<<PD7) && change_alarm_mode){	// if the user press go button
			_delay_ms(30);		// delay for debouncing
			if (!(PIND & 1<<PD7) && change_alarm_mode && !go_flag){
				dp_selector = 7; // turn off all dp
				go_flag = 1;
				change_alarm_mode = 0;	// turn off alarm changing time mode
				alarm_mode = 1;			// start the alarm
				TCNT1 = 0 ;				// Start timer from zero.

			}
		}
		else
			go_flag = 0;
		if (!(PIND & 1<<PD0) && change_alarm_mode){		// if the user press <-- button
			_delay_ms(30);		// delay for debouncing
			if (!(PIND & 1<<PD0) && change_alarm_mode && !arrow_flag){
				arrow_flag = 1;
				dp_selector++;	// shifting left the dp to select the digit which will be changed
				if (dp_selector == 6)
					dp_selector = 0;
			}
		}
		else
			arrow_flag = 0;
	}
	return 0;
}




