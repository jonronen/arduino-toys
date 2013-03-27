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


#include "Arduino.h"
#include <Sd2Card.h>
#include "sound.h"
#include "filesystem.h"
#include "engine.h"


#define SOUND_FREQ 32000


static uint8_t g_play_buff[BUFF_HALF_SIZE*2];
static uint8_t g_play_buff_pos;
uint8_t g_play_buff_flags;

static uint32_t g_curr_sound_block;
uint16_t g_curr_sound_len;

/* keep this for the delay functionality */
static uint16_t g_interrupt_cnt;


void sound_setup()
{
  pinMode(6, OUTPUT);
  
  // Set up Timer 0 to do pulse width modulation on the speaker
  // pin.
  
  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));
  
  // disable Timer0 interrupts - delay() is no longer available
  TIMSK0 &= (~TOIE0);

  // Set fast PWM mode  (p.157)
  TCCR0A |= _BV(WGM01) | _BV(WGM00);
  TCCR0B &= ~_BV(WGM02);
  
  // Do non-inverting PWM on pin OC0A (p.155)
  // On the Arduino this is pin 6.
  TCCR0A = (TCCR0A | _BV(COM0A1)) & ~_BV(COM0A0);
  TCCR0A &= ~(_BV(COM0B1) | _BV(COM0B0));
  
  // No prescaler (p.158)
  TCCR0B = (TCCR0B & ~(_BV(CS02) | _BV(CS01))) | _BV(CS00);
  
  // Set initial pulse width to the first sample.
  OCR0A = 0x80;
  
  
  // Set up Timer 1 to send a sample every interrupt.
  
  cli();
  
  // Set CTC mode (Clear Timer on Compare Match) (p.133)
  // Have to set OCR1A *after*, otherwise it gets reset to 0!
  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));
  
  // No prescaler (p.134)
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // Set the compare register (OCR1A).
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = (F_CPU / SOUND_FREQ);    // 16e6 / 32000 for Arduino Uno
  
  // Enable interrupt when TCNT1 == OCR1A (p.136)
  TIMSK1 |= _BV(OCIE1A);
  sei();
}


void play_sectors(uint32_t start_sector, uint16_t num_sectors)
{
  uint8_t res;

#ifdef DEBUG_FS
  Serial.print("Starting to play on sector ");
  Serial.print(start_sector);
  Serial.print(" for ");
  Serial.print(num_sectors);
  Serial.println(" sectors.");
#endif

  // sanity check
  if (num_sectors == 0) {
#ifdef DEBUG_FS
    Serial.println("Error: zero sectors to play. aborting.");
#endif
    return;
  }

  // disable interrupts
  cli();

  g_curr_sound_block = start_sector;
  g_curr_sound_len = num_sectors;

  res = g_sdc.readBlock(start_sector, g_play_buff);
  if (!res) {
#ifdef DEBUG_FS
    Serial.println("Error reading first sound block");
#endif
    g_curr_sound_block = 0;
    g_curr_sound_len = 0;
    sei();
    return;
  }
  
  g_play_buff_pos = 0;
  g_play_buff_flags =
    BUFF_FLAG_NEEDS_FETCH |
    BUFF_FLAG_SECOND_HALF |
    BUFF_FLAG_IS_PLAYING;

  sei();
}

void read_sound_block()
{
  uint8_t res;

  if ((g_play_buff_flags & (BUFF_FLAG_NEEDS_FETCH|BUFF_FLAG_IS_PLAYING)) !=
      (BUFF_FLAG_NEEDS_FETCH|BUFF_FLAG_IS_PLAYING)) {
    return;
  }

  if (g_play_buff_flags & BUFF_FLAG_SECOND_HALF) {
#ifdef DEBUG_FS
    Serial.println("Fetching second half");
#endif
    res = g_sdc.readData(
      g_curr_sound_block, BUFF_HALF_SIZE, BUFF_HALF_SIZE,
      g_play_buff+BUFF_HALF_SIZE
    );
    if (!res) {
#ifdef DEBUG_FS
      Serial.println("Error reading second half of block");
#endif
      g_curr_sound_block = 0;
      g_curr_sound_len = 0;
      cli();
      g_play_buff_flags = 0;
      sei();
      return;
    }
  }
  else {
#ifdef DEBUG_FS
    Serial.println("Fetching first half");
#endif
    // move to next block
    g_curr_sound_block++;
    g_curr_sound_len--;

    // is there a next block?
    if (g_curr_sound_len == 0) {
#ifdef DEBUG_FS
      Serial.println("Done playing");
#endif
      // finish playing after the current block
      g_curr_sound_block = 0;
      g_curr_sound_len = 0;
      cli();
      g_play_buff_flags = 0;
      sei();
      return;
    }

    // there is a next block. read it
    res = g_sdc.readData(g_curr_sound_block, 0, BUFF_HALF_SIZE, g_play_buff);
    if (!res) {
#ifdef DEBUG_FS
      Serial.println("Error reading first half of block");
#endif
      g_curr_sound_block = 0;
      g_curr_sound_len = 0;
      cli();
      g_play_buff_flags = 0;
      sei();
      return;
    }
  }

  // now clear the "needs fetch" bit and switch to the other half of the buffer
  cli();
  g_play_buff_flags ^= BUFF_FLAG_NEEDS_FETCH;
  sei();
}


ISR(TIMER1_COMPA_vect)
{
  /* handle the delay counter */
  g_interrupt_cnt++;
  /* delay resolution is 1/10 sec */
  if (g_interrupt_cnt == SOUND_FREQ/10) {
    g_interrupt_cnt = 0;
    if (g_delay_cnt) g_delay_cnt--;
  }

  if ((g_play_buff_flags & BUFF_FLAG_IS_PLAYING) || g_play_buff_pos) {
    OCR0A = g_play_buff[
      (uint16_t)g_play_buff_pos +
      ((g_play_buff_flags & BUFF_FLAG_SECOND_HALF) ? 0 : BUFF_HALF_SIZE)
    ];
    g_play_buff_pos++;
    if (g_play_buff_pos == 0) {
      g_play_buff_flags ^= (BUFF_FLAG_NEEDS_FETCH|BUFF_FLAG_SECOND_HALF);
    }
  }
}

