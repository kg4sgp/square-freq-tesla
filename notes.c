/* 
This function computes the number of overflows and match value needed for a toggle if using a timer in "normal" mode (AVR uC) to produce frequncies as they relate to the keys of a panio 1-88 
*/ 

#include <stdio.h>
#include <math.h>

int main()
{
  double i = 1;
  printf("int notes[88][2] = {\n");
  for (i = 1; i <= 88; i++) {
    double freq, cycles, fract, integ;
    double bits = 8;
    double clock = 16000000;
    double prescale = 1;
    freq = (pow(2, ((i-49)/12)) * 440);
    cycles = (clock/pow(2,bits)/freq/prescale/2);
    fract = modf(cycles, &integ);
    fract = round(pow(2,bits)*fract);
    printf("{ %d, %d }, /* Key %d */ \n", (int)integ, (int)fract, (int)i);
  }
  printf("};\n");
  
  return 0;
}
