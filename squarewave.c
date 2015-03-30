/*
squarewave.c
*/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "squarewave.h"

volatile int _pin1 = 3;
volatile int _pin2 = 4;
volatile int _pin3 = 5;
volatile int _pin4 = 6;
volatile int _pin5 = 7;
volatile int _pin6 = 8;

uint16_t overflows = 0;

uint16_t num_roll = 0;
uint8_t num_match = 0;

uint16_t _notes[88][2] = {
  { 1136, 93 }, /* Key 1 */ 
  { 1072, 150 }, /* Key 2 */ 
  { 1012, 99 }, /* Key 3 */ 
  { 955, 144 }, /* Key 4 */ 
  { 901, 239 }, /* Key 5 */ 
  { 851, 80 }, /* Key 6 */ 
  { 803, 136 }, /* Key 7 */ 
  { 758, 111 }, /* Key 8 */ 
  { 715, 221 }, /* Key 9 */ 
  { 675, 176 }, /* Key 10 */ 
  { 637, 195 }, /* Key 11 */ 
  { 601, 248 }, /* Key 12 */ 
  { 568, 47 }, /* Key 13 */ 
  { 536, 75 }, /* Key 14 */ 
  { 506, 49 }, /* Key 15 */ 
  { 477, 200 }, /* Key 16 */ 
  { 450, 247 }, /* Key 17 */ 
  { 425, 168 }, /* Key 18 */ 
  { 401, 196 }, /* Key 19 */ 
  { 379, 55 }, /* Key 20 */ 
  { 357, 239 }, /* Key 21 */ 
  { 337, 216 }, /* Key 22 */ 
  { 318, 226 }, /* Key 23 */ 
  { 300, 252 }, /* Key 24 */ 
  { 284, 23 }, /* Key 25 */ 
  { 268, 37 }, /* Key 26 */ 
  { 253, 25 }, /* Key 27 */ 
  { 238, 228 }, /* Key 28 */ 
  { 225, 124 }, /* Key 29 */ 
  { 212, 212 }, /* Key 30 */ 
  { 200, 226 }, /* Key 31 */ 
  { 189, 156 }, /* Key 32 */ 
  { 178, 247 }, /* Key 33 */ 
  { 168, 236 }, /* Key 34 */ 
  { 159, 113 }, /* Key 35 */ 
  { 150, 126 }, /* Key 36 */ 
  { 142, 12 }, /* Key 37 */ 
  { 134, 19 }, /* Key 38 */ 
  { 126, 140 }, /* Key 39 */ 
  { 119, 114 }, /* Key 40 */ 
  { 112, 190 }, /* Key 41 */ 
  { 106, 106 }, /* Key 42 */ 
  { 100, 113 }, /* Key 43 */ 
  { 94, 206 }, /* Key 44 */ 
  { 89, 124 }, /* Key 45 */ 
  { 84, 118 }, /* Key 46 */ 
  { 79, 184 }, /* Key 47 */ 
  { 75, 63 }, /* Key 48 */ 
  { 71, 6 }, /* Key 49 */ 
  { 67, 9 }, /* Key 50 */ 
  { 63, 70 }, /* Key 51 */ 
  { 59, 185 }, /* Key 52 */ 
  { 56, 95 }, /* Key 53 */ 
  { 53, 53 }, /* Key 54 */ 
  { 50, 56 }, /* Key 55 */ 
  { 47, 103 }, /* Key 56 */ 
  { 44, 190 }, /* Key 57 */ 
  { 42, 59 }, /* Key 58 */ 
  { 39, 220 }, /* Key 59 */ 
  { 37, 159 }, /* Key 60 */ 
  { 35, 131 }, /* Key 61 */ 
  { 33, 133 }, /* Key 62 */ 
  { 31, 163 }, /* Key 63 */ 
  { 29, 221 }, /* Key 64 */ 
  { 28, 47 }, /* Key 65 */ 
  { 26, 154 }, /* Key 66 */ 
  { 25, 28 }, /* Key 67 */ 
  { 23, 179 }, /* Key 68 */ 
  { 22, 95 }, /* Key 69 */ 
  { 21, 29 }, /* Key 70 */ 
  { 19, 238 }, /* Key 71 */ 
  { 18, 208 }, /* Key 72 */ 
  { 17, 193 }, /* Key 73 */ 
  { 16, 194 }, /* Key 74 */ 
  { 15, 210 }, /* Key 75 */ 
  { 14, 238 }, /* Key 76 */ 
  { 14, 24 }, /* Key 77 */ 
  { 13, 77 }, /* Key 78 */ 
  { 12, 142 }, /* Key 79 */ 
  { 11, 218 }, /* Key 80 */ 
  { 11, 47 }, /* Key 81 */ 
  { 10, 143 }, /* Key 82 */ 
  { 9, 247 }, /* Key 83 */ 
  { 9, 104 }, /* Key 84 */ 
  { 8, 225 }, /* Key 85 */ 
  { 8, 97 }, /* Key 86 */ 
  { 7, 233 }, /* Key 87 */ 
  { 7, 119 }, /* Key 88 */ 
};

void square_init(void)
{

  /* setup pins for output */
  DDRD = _BV(3);

  /* Counter 2, Pin 3 */
  TCCR2B |= _BV(CS20); /* No prescale */
  TIMSK2 |= _BV(TOIE2) | _BV(OCIE2B); /* Enable Counter */
  
  sei();
  
}

void square_play(uint16_t midi_pitch)
{
  uint16_t _pitch = midi_pitch - 21;
  num_roll = _notes[_pitch][0];
  num_match = (uint8_t)_notes[_pitch][1];
  OCR2B = num_match;
  TIMSK2 |= _BV(OCIE2B);
}

void square_stop(void)
{
  TIMSK2 ^= _BV(OCIE2B);
  PORTD = PORTD&((1<<3)&0xff);
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

