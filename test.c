/*
test.c
*/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "squarewave.h"

int main()
{

  square_init();

  while(1){
    uint16_t i;
    uint16_t j;  
    for(i = 21; i < 109; i++) {
      square_play(i);
      for(j = 0; j< 65000; j++);
      square_stop();
      for(j = 0; j< 65000; j++);
    }
  }
  
  return 0;  
}
    
