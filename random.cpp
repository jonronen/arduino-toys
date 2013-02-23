#include <Arduino.h>


#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif


#define STATE_LEN 16
static uint8_t g_state[STATE_LEN] = {
  2,  6,  8, 15,  1,  9,  7,  3,
  5,  4, 13, 10, 11,  0, 12, 14
};
static uint8_t g_i = 0;
static uint8_t g_j = 0;


void set_prescale(uint8_t prescale_val)
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
  for (i=0; i<16; i++) {
    j += g_state[i];
    j += key[i%key_len];
    j &= 0xf;
    tmp = g_state[i];
    g_state[i] = g_state[j];
    g_state[j] = tmp;
  }
}

uint8_t get_rand()
{
  uint8_t tmp;

  g_i = (g_i + 1)&0xf;
  g_j = (g_j + g_state[g_i]) & 0xf;

  tmp = g_state[g_i];
  g_state[g_i] = g_state[g_j];
  g_state[g_j] = tmp;

  return g_state[(g_state[g_i] + tmp) & 0xf];
}

