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
#include "sound.h"
#include "engine.h"
#include "random.h"
#include "filesystem.h"
#include "io_pins.h"


void setup();
void loop();


void setup()
{
#ifdef DEBUG_FS
  Serial.begin(115200);
#endif
  if (init_fs()) {
#ifdef DEBUG_FS
    Serial.println("Error initialising filesystem");
#endif
    return;
  }
  
  random_init();
  sound_setup();

  /* set the pins */
  pinMode(INPUT0_PIN, INPUT);
  pinMode(INPUT1_PIN, INPUT);
  pinMode(INPUT2_PIN, INPUT);
  pinMode(INPUT3_PIN, INPUT);
  /* input4 is analog because we ran out of pins */

  pinMode(LED0_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
}


void loop()
{
  /*
   * we need to make sure the sound blocks are always ready.
   * this means we don't allow reading more than one file system block
   * between sound blocks
   */
  read_sound_block();
  read_instruction_block();
  read_sound_block();
  process_instruction_block();
}

