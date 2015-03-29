/* 

The size of the counter (8b vs 16b) determines the amplitude resolution of our DAC, if we are using it as such. However, because we are working with squarewaves we really probably shouldn't be using it as such. 

Instead of using the timer for pulse width modulation we can use it as a toggle (toggle on compaire) and set the timer to toggle at half the period of the wave we are trying to produce.

If we run any length counter at 16MHz it will take 400 cycles to toggle if we wanted to produce a 20kHz square wave (16MHz/20kHz/2). Unfortunately if we wanted to produce a 20Hz square wave we would need 400k cycles. This is a bit bigger than our 2^16 = 65536 length timer. Also unfortunately if we prescale the frequency of the counter we will loose frequency resolution.

To try and combat this I'm going to use the timer as is at 16MHz and keep track of roll overs. I'm going to note notes as how many times the counter sould roll over and then how many cycles it will need. I'll then compare the values in software and toggle myself. We should also be able to use 8bit counters with the same frequency resolution the only thing that will change is how many roll overs + remainders we will need for the note. (The note notation).

I'm going to start off with an 8 bit counter, because I'm more familliar with them, just to see if this approach works and then write it for a 16 bit counter.

*/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t overflows = 0;

uint16_t num_roll = 0;
uint8_t num_match = 0;

int main(void) 
{
  /* setup pins for output */
  DDRD = _BV(3);

  /* setup counter 2 for fast PWM output on pin 3 (arduino) */
  /*TCCR2A = _BV(COM2B1); */ /* | _BV(COM2B0); */ /* _BV(WGM21) | _BV(WGM20); */
  TCCR2B |= _BV(CS20); /* No prescale */
  TIMSK2 |= _BV(TOIE2) | _BV(OCIE2B); /* Enable Counter */
  
  sei();
  
  /* Set the counter for 440Hz */
  num_roll = 71;
  num_match = 6;
  OCR2B = num_match;
  
  /* Does NOT Terminate! */
  for(;;) {
    /* Idle here while interrupts take care of things */
  }
}

/* Keep track of overflows */
ISR(TIMER2_OVF_vect) 
{
  overflows++;
}

/* Compare match */
ISR(TIMER2_COMPB_vect)
{
  /* If we havent overflowed enough yet, exit interrupt */
  if (overflows != num_roll) return;
  
  /* Otherwise, toggle pin and reset counter */
  PORTD =  PORTD^(1<<3);
  TCNT0 = 0;
  overflows = 0;
  num_roll++;
}
