/*
test.c
*/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "squarewave.h"
#include <math.h>

int main()
{
  sei();
  square_init();

  while(1){
 
    uint8_t i = 21;
    for(i = 21; i < 109; i++) {
      square_play(i);
      _delay_ms(100);
      square_stop();
      _delay_ms(1);
    }

  }
  
  return 0;  
}
    
