
#include <avr/io.h>
#include <util/delay.h>
#include "scale16.h"

#define F_CPU 16000000
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD)-1)

#define BUZZER_PORT PORTB
#define BUZZER 0
#define BUZZER_DDR DDRB
 
void USART_Init(void);
void transmit_byte(uint8_t data);
uint8_t receive_byte(void);
void print_string(const char string[]);


#define NOTE_DURATION 0xF000 // long note duration

void playNote(uint16_t period, uint16_t duration);//Plays a note for given duration

// Does nothing for a time equal to the passed duration.
void rest(uint16_t duration);
int main(void)
{
	USART_Init();
	BUZZER_DDR |= (1 << BUZZER)|(1<<5);
	print_string("START");
	
	char ComputerInput;
	uint16_t currentnotlength = NOTE_DURATION/ 2;
	const uint8_t keys[]={'a', 'w', 's', 'e', 'd', 'f', 't', 'g', 'y', 'h', 'j', 'i', 'k', 'o', 'l', 'p', ';', '\''};
	const uint16_t notes[]={G4, Gx4, A4, Ax4, B4, C5, Cx5, D5, Dx5, E5, F5, Fx5, G5, Gx5, A5, Ax5, B5, C6};
	uint8_t isNote;
	uint8_t t;
	
	
    while(1)
    {
        ComputerInput = receive_byte();
		
		print_string("YO");
		transmit_byte('N');
		
		isNote=0;
		for(t=0;t<sizeof(keys);t++)
		{
			if(ComputerInput == keys[t])
			{
				playNote(notes[t],currentnotlength);
				isNote=1;
				break;
            }
      }
	  if(!isNote)
	  {
		  if(ComputerInput =='['){
			  currentnotlength = NOTE_DURATION / 2;
	      }
		  else if(ComputerInput == ']')
		  {
			  currentnotlength = NOTE_DURATION;
		  }
		  else{
			  rest(currentnotlength);
		  }
	 }
	}
	return 0;
}
	
void USART_Init(void){
	UBRR0H = BRC >> 8;
	UBRR0L = BRC;
	
	UCSR0B =(1 << TXEN0) | (1 << RXEN0);
	UCSR0C =(1 << UCSZ00)| (1<<UCSZ01);
}
void transmit_byte(uint8_t data){
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}
uint8_t receive_byte(void){
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}
void print_string(const char string[]){
	uint8_t i=0;
	while(string[i]){
		transmit_byte(string[i]);
		i++;
	}
}
void playNote(uint16_t period, uint16_t duration) {
	uint16_t elapsed;
	uint16_t u;
	for (elapsed = 0; elapsed < duration; elapsed += period) {
		/* For loop with variable delay selects the pitch */
		for (u = 0; u < period; u++) {
			_delay_us(1);
		}
		
		BUZZER_PORT ^= (1 << BUZZER);
	}
}
void rest(uint16_t duration) {
	do {
		_delay_us(1);
	} while (--duration);
}
