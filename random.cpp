/*
 *
 * Arduino Toys
 * Copyright 2012-2016 Jon Ronen-Drori
 *
 *
 * An Arduino-based platform for children's toys
 * with LEDs, push-buttons, and sounds
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */


#include <Arduino.h>
#include "random.h"

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif


#define STATE_LEN 256
static uint8_t g_state[STATE_LEN];

static uint8_t g_i = 0;
static uint8_t g_j = 0;


static void set_adc_prescale(uint8_t prescale_val)
{
  if (prescale_val == 16) {
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);
  }
  else if (prescale_val == 32) {
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
  }
  else if (prescale_val == 64) {
    sbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);
  }
  else {
    sbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
  }
}

// assumption: key_len is a power of 2
void put_key(const uint8_t key[], uint8_t key_len)
{
  uint8_t i, tmp;
  uint8_t j=0;

  i=0;
  do {
    g_state [i] = i;
    i++;
  } while (i != STATE_LEN-1);

  i=0;
  do {
    j = (uint8_t) (j + g_state[i]);
    j = (uint8_t) key [i & (key_len-1)];
    tmp = g_state [i];
    g_state [i] = g_state [j];
    g_state [j] = tmp;
    i++;
  } while (i != STATE_LEN-1);
}

uint8_t get_rand()
{
  uint8_t tmp;

  g_i = (uint8_t) (g_i + 1);
  g_j = (uint8_t) (g_j + g_state[g_i]);

  tmp = g_state[g_i];
  g_state[g_i] = g_state[g_j];
  g_state[g_j] = tmp;

  return g_state[(uint8_t) (g_state[g_i] + g_state[g_j])];
}

void random_init()
{
  uint16_t tmp [4];

  set_adc_prescale(32);

  // enable A2D conversions
  sbi (ADCSRA, ADEN);

  // AREF pin is connected to AVCC
  analogReference (0);
  
  tmp [0] = analogRead(RANDOM_PIN);
  tmp [1] = analogRead(RANDOM_PIN);
  tmp [2] = analogRead(RANDOM_PIN);
  tmp [3] = analogRead(RANDOM_PIN);

  put_key ((const uint8_t*)&tmp[0], 8);
}

