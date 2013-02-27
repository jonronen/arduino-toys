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

