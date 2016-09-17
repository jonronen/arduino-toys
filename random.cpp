/*
 *
 * Arduino Toys
 * Copyright 2012-2013 Jon Ronen-Drori
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
static uint8_t g_state[STATE_LEN] = {
  44, 226, 119, 75, 185, 243, 15, 41,
  140, 113, 208, 67, 234, 89, 20, 221,
  54, 55, 21, 117, 135, 172, 47, 122,
  84, 116, 125, 32, 37, 40, 239, 127,
  63, 230, 31, 39, 169, 35, 82, 202,
  70, 233, 157, 88, 101, 254, 112, 62,
  171, 203, 65, 148, 170, 188, 237, 29,
  199, 178, 235, 245, 253, 43, 165, 161,
  249, 229, 2, 197, 180, 156, 187, 56,
  164, 155, 228, 1, 223, 94, 104, 10,
  27, 225, 74, 216, 175, 250, 179, 38,
  238, 120, 126, 33, 60, 146, 3, 177,
  42, 232, 189, 184, 158, 28, 153, 212,
  214, 217, 166, 105, 30, 192, 22, 150,
  79, 130, 241, 91, 168, 149, 193, 109,
  49, 134, 5, 143, 24, 103, 96, 97,
  191, 100, 71, 159, 93, 57, 219, 14,
  86, 76, 8, 248, 73, 190, 151, 181,
  64, 132, 7, 13, 128, 12, 11, 66,
  240, 0, 173, 45, 247, 205, 80, 6,
  222, 198, 231, 85, 68, 19, 147, 200,
  210, 154, 108, 252, 23, 72, 26, 51,
  136, 50, 17, 4, 137, 183, 107, 215,
  242, 111, 186, 209, 213, 174, 61, 81,
  25, 142, 36, 207, 59, 152, 99, 162,
  114, 69, 160, 224, 251, 124, 87, 83,
  255, 118, 138, 195, 102, 133, 201, 163,
  34, 123, 139, 18, 53, 246, 77, 131,
  48, 110, 90, 182, 46, 52, 211, 78,
  115, 9, 95, 58, 176, 141, 167, 220,
  227, 196, 145, 206, 194, 106, 16, 204,
  244, 218, 144, 236, 129, 92, 121, 98
};


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

void put_key(const uint8_t key[], uint8_t key_len)
{
  uint8_t i, tmp;
  uint8_t j=0;
  for (i=0; i<256; i++) {
    j += g_state[i];
    j += key[i%key_len];
    tmp = g_state[i];
    g_state[i] = g_state[j];
    g_state[j] = tmp;
  }
}

uint8_t get_rand()
{
  uint8_t tmp;

  g_i = g_i + 1;
  g_j = g_j + g_state[g_i];

  tmp = g_state[g_i];
  g_state[g_i] = g_state[g_j];
  g_state[g_j] = tmp;

  return g_state[g_state[g_i] + tmp];
}

void random_init()
{
  uint16_t tmp;

  set_adc_prescale(32);

  // enable A2D conversions
  sbi (ADCSRA, ADEN);

  // AREF pin is connected to AVCC
  analogReference (0);
  
  tmp = analogRead(RANDOM_PIN);
  put_key((const uint8_t*)&tmp, 2);
}

