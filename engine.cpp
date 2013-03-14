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
#include "engine.h"
#include "random.h"
#include "sound.h"
#include "filesystem.h"
#include "io_pins.h"


/* registers */
#define NUM_REGS 16
static uint8_t g_regs[NUM_REGS];

/* global array */
#define GLOBAL_ARRAY_LEN 128
static uint8_t g_array[GLOBAL_ARRAY_LEN];

/* delay counter */
uint8_t g_delay_cnt;

/* are we in the middle of PLAY_SOUND_BLOCKING? */
static uint8_t g_f_playing_sound_block;

/* are we waiting for an input? */
static uint8_t g_f_waiting_for_input;
static uint8_t g_input_register;

/* opcode enumerations */
enum opcode_t {
  OP_INCOMPLETE,
  OP_SET_REG,
  OP_ADD_REGS,
  OP_SUB_REGS,
  OP_MUL_REGS,
  OP_DIV_REGS,
  OP_AND_REGS,
  OP_OR_REGS,
  OP_XOR_REGS,
  OP_LIGHT_ON,
  OP_LIGHT_OFF,
  OP_PLAY_SOUND,
  OP_PLAY_SOUND_BLOCKING,
  OP_GET_INPUT,
  OP_DELAY,
  OP_LOAD,
  OP_STORE,
  OP_JUMP,
  OP_JUMP_EQUAL,
  OP_JUMP_GREATER,
  OP_RANDOM,
  OP_MAX_VALUE
};

/* opcode parameters */
static const uint8_t g_num_reg_params[OP_MAX_VALUE+1] = {
  0, /* OP_INCOMPLETE */
  1, /* OP_SET_REG */
  3, /* OP_ADD_REGS */
  3, /* OP_SUB_REGS */
  3, /* OP_MUL_REGS */
  3, /* OP_DIV_REGS */
  3, /* OP_AND_REGS */
  3, /* OP_OR_REGS */
  3, /* OP_XOR_REGS */
  1, /* OP_LIGHT_ON */
  1, /* OP_LIGHT_OFF */
  1, /* OP_PLAY_SOUND */
  1, /* OP_PLAY_SOUND_BLOCKING */
  2, /* OP_GET_INPUT */
  1, /* OP_DELAY */
  2, /* OP_LOAD */
  2, /* OP_STORE */
  1, /* OP_JUMP */
  3, /* OP_JUMP_EQUAL */
  3, /* OP_JUMP_GREATER */
  2, /* OP_RANDOM */
  0, /* OP_MAX_VALUE */
};


/* short opcodes */
static const opcode_t g_short_opcodes[] = {
  OP_INCOMPLETE,            /* 0x0X */
  OP_SET_REG,               /* 0x10 */
  OP_LIGHT_ON,              /* 0x20 */
  OP_LIGHT_OFF,             /* 0x30 */
  OP_PLAY_SOUND,            /* 0x40 */
  OP_PLAY_SOUND_BLOCKING,   /* 0x50 */
  OP_DELAY,                 /* 0x60 */
  OP_JUMP,                  /* 0x70 */
  OP_JUMP_EQUAL,            /* 0x80 */
  OP_JUMP_GREATER,          /* 0x90 */
  OP_MAX_VALUE,             /* 0xA0 */
  OP_MAX_VALUE,             /* 0xB0 */
  OP_MAX_VALUE,             /* 0xC0 */
  OP_MAX_VALUE,             /* 0xD0 */
  OP_MAX_VALUE,             /* 0xE0 */
  OP_MAX_VALUE,             /* 0xF0 */
};
/* medium opcodes */
static const opcode_t g_medium_opcodes[] = {
  OP_INCOMPLETE,            /* 0x00 */
  OP_GET_INPUT,             /* 0x01 */
  OP_LOAD,                  /* 0x02 */
  OP_STORE,                 /* 0x03 */
  OP_RANDOM,                /* 0x04 */
  OP_MAX_VALUE,             /* 0x05 */
  OP_MAX_VALUE,             /* 0x06 */
  OP_MAX_VALUE,             /* 0x07 */
  OP_MAX_VALUE,             /* 0x08 */
  OP_MAX_VALUE,             /* 0x09 */
  OP_MAX_VALUE,             /* 0x0A */
  OP_MAX_VALUE,             /* 0x0B */
  OP_MAX_VALUE,             /* 0x0C */
  OP_MAX_VALUE,             /* 0x0D */
  OP_MAX_VALUE,             /* 0x0E */
  OP_MAX_VALUE,             /* 0x0F */
};
/* long opcodes */
static const opcode_t g_long_opcodes[] = {
  OP_MAX_VALUE,             /* 0x000 */
  OP_ADD_REGS,              /* 0x001 */
  OP_SUB_REGS,              /* 0x002 */
  OP_MUL_REGS,              /* 0x003 */
  OP_DIV_REGS,              /* 0x004 */
  OP_AND_REGS,              /* 0x005 */
  OP_OR_REGS,               /* 0x006 */
  OP_XOR_REGS,              /* 0x007 */
  OP_MAX_VALUE,             /* 0x008 */
  OP_MAX_VALUE,             /* 0x009 */
  OP_MAX_VALUE,             /* 0x00A */
  OP_MAX_VALUE,             /* 0x00B */
  OP_MAX_VALUE,             /* 0x00C */
  OP_MAX_VALUE,             /* 0x00D */
  OP_MAX_VALUE,             /* 0x00E */
  OP_MAX_VALUE,             /* 0x00F */
};


static uint8_t get_input()
{
  uint8_t ans = 0;
  uint16_t key;

  if (digitalRead(INPUT0_PIN) == 0)
    ans |= 1;
  if (digitalRead(INPUT1_PIN) == 0)
    ans |= 2;
  if (digitalRead(INPUT2_PIN) == 0)
    ans |= 4;
  if (digitalRead(INPUT3_PIN) == 0)
    ans |= 8;
  if (analogRead(INPUT4_PIN) <= 0x200) /* input4 is analog */
    ans |= 16;

  if (ans != 0) {
    /* there is an input. seed the random number generator */
    key = TCNT1;
    put_key((const uint8_t*)&key, 2);
  }

#ifdef DEBUG_FS
  Serial.print("Input: ");
  Serial.println((int)ans);
#endif

  return ans;
}


static void set_led(uint8_t led_num, uint8_t set_not_clear)
{
  if (led_num == 0) {
    digitalWrite(LED0_PIN, set_not_clear);
  }
  else if (led_num == 1) {
    digitalWrite(LED1_PIN, set_not_clear);
  }
  else if (led_num == 2) {
    digitalWrite(LED2_PIN, set_not_clear);
  }
  else if (led_num == 3) {
    digitalWrite(LED3_PIN, set_not_clear);
  }
  else if (led_num == 4) {
    digitalWrite(LED4_PIN, set_not_clear);
  }
}


static uint16_t get_opcode_and_params(
    uint8_t* ins_block,
    uint16_t* p_block_offset,
    opcode_t* p_opcode,
    uint8_t* p_param1,
    uint8_t* p_param2,
    uint8_t* p_param3
)
{
  uint16_t retval = 0;
  uint8_t ins;
  uint8_t num_params = 0;

  ins = ins_block[*p_block_offset];

  if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;

  do {
    /* is it a short opcode? */
    *p_opcode = g_short_opcodes[(ins & 0xf0) >> 4];
    if (*p_opcode != OP_INCOMPLETE) {
      if (*p_opcode == OP_MAX_VALUE) retval = PROCESSING_ERROR;
      break;
    }

    *p_block_offset = *p_block_offset+1;

    /* is it a medium opcode? */
    *p_opcode = g_medium_opcodes[ins & 0x0f];
    if (*p_opcode != OP_INCOMPLETE) {
      if (*p_opcode == OP_MAX_VALUE) retval = PROCESSING_ERROR;
      break;
    }

    /* it's probably a long opcode */
    if (*p_block_offset >= CODE_BLOCK_SIZE) {
      retval = PROCESSING_ERROR;
      break;
    }
    ins = ins_block[*p_block_offset];

    *p_opcode = g_long_opcodes[(ins&0xf0) >> 4];
    if (*p_opcode != OP_INCOMPLETE) {
      if (*p_opcode == OP_MAX_VALUE) retval = PROCESSING_ERROR;
      break;
    }

    /* no opcode? that's an error */
    retval = PROCESSING_ERROR;
  } while (0);

  /* error? */
  if (retval != 0) return retval;

  /* get the parameters */
  num_params = g_num_reg_params[*p_opcode];
  if ((num_params == 1) || (num_params == 3)) {
    /* param1 is always the least-significant nibble */
    *p_param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (num_params == 3) {
      if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
      *p_param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
      *p_param3 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
    }
  }
  else if (num_params == 2) {
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    *p_param1 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    *p_param2 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
  }

  return 0;
}

/*
 * return value:
 * 0x00 to 0xff - next instruction block
 * 0xffff       - error
 * 0xfffe       - yield (for delays / playbacks)
 */
uint16_t process_instruction(
  uint8_t ins_block[CODE_BLOCK_SIZE],
  uint16_t* p_block_offset /* both input and output */
)
{
  opcode_t op;
  uint8_t param1,param2,param3;

  if (g_f_waiting_for_input) {
    g_regs[g_input_register] = get_input();
    if (g_regs[g_input_register] != 0) {
      g_f_waiting_for_input = 0;
      g_delay_cnt = 0;
    }
    else {
      if (g_delay_cnt) return PROCESSING_YIELD;
    }
  }
  else if (g_delay_cnt) return PROCESSING_YIELD;

  if (g_f_playing_sound_block &&
      (g_play_buff_flags & BUFF_FLAG_IS_PLAYING)) {
    /* wait till we finish playing */
    return PROCESSING_YIELD;
  }
  
  /* playing is over - clear the flag */
  g_f_playing_sound_block = 0;

  /* get the opcode and change p_block_offset if necessary */
  if (get_opcode_and_params(
        ins_block, p_block_offset, &op,
        &param1, &param2, &param3)
     ) {
#ifdef DEBUG_FS
    Serial.println("get_opcode_and_params returned an error!");
#endif
    return PROCESSING_ERROR;
  }

#ifdef DEBUG_FS
  Serial.print("block offset: ");
  Serial.println((int)*p_block_offset);
  Serial.print("opcode: ");
  Serial.println((int)op);
#endif

  switch (op) {
  
  case OP_SET_REG:
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    g_regs[param1] = ins_block[*p_block_offset];
    *p_block_offset = *p_block_offset+1;
    break;
  
  case OP_ADD_REGS:
    g_regs[param1] = g_regs[param2] + g_regs[param3];
    break;
  
  case OP_SUB_REGS:
    g_regs[param1] = g_regs[param2] - g_regs[param3];
    break;
  
  case OP_MUL_REGS:
    g_regs[param1] = g_regs[param2] * g_regs[param3];
    break;
  
  case OP_DIV_REGS:
    /* check for zero */
    if (g_regs[param3] == 0) return PROCESSING_ERROR;
    g_regs[param1] = g_regs[param2] / g_regs[param3];
    break;
  
  case OP_AND_REGS:
    g_regs[param1] = g_regs[param2] & g_regs[param3];
    break;
  
  case OP_OR_REGS:
    g_regs[param1] = g_regs[param2] | g_regs[param3];
    break;
  
  case OP_XOR_REGS:
    g_regs[param1] = g_regs[param2] ^ g_regs[param3];
    break;
  
  case OP_LIGHT_ON:
    set_led(g_regs[param1], HIGH);
    break;
  
  case OP_LIGHT_OFF:
    set_led(g_regs[param1], LOW);
    break;
  
  case OP_PLAY_SOUND_BLOCKING:
    g_f_playing_sound_block = 1;
    /* fall through */
  case OP_PLAY_SOUND:
    play_sound(g_regs[param1]);
    break;
  
  case OP_GET_INPUT:
    /* get the input if there is one */
    g_regs[param1] = get_input();
    if (g_regs[param1] == 0) {
      g_delay_cnt = g_regs[param2];

      /* no input - set the delay and wait */
      if (g_delay_cnt) {
        g_f_waiting_for_input = 1;
        g_input_register = param1;
      }
    }
    break;
  
  case OP_DELAY:
    g_delay_cnt = g_regs[param1];
    break;
  
  case OP_LOAD:
    /* check for an invalid offset */
    if (g_regs[param2] >= GLOBAL_ARRAY_LEN) return PROCESSING_ERROR;
    g_regs[param1] = g_array[g_regs[param2]];
    break;
  
  case OP_STORE:
    /* check for an invalid offset */
    if (g_regs[param2] >= GLOBAL_ARRAY_LEN) return PROCESSING_ERROR;
    g_array[g_regs[param2]] = g_regs[param1];
    break;
  
  case OP_JUMP:
    *p_block_offset = 0;
    return g_regs[param1];
    break;
  
  case OP_JUMP_EQUAL:
    if (g_regs[param1] == g_regs[param2]) {
      *p_block_offset = 0;
      return g_regs[param3];
    }
    break;
  
  case OP_JUMP_GREATER:
    if (g_regs[param1] > g_regs[param2]) {
      *p_block_offset = 0;
      return g_regs[param3];
    }
    break;
  
  case OP_RANDOM:
    /* get the range for the randomness */
    param3 = 1;
    while (param3 < g_regs[param2]) {
      param3 <<= 1;
    }
    while (1) {
      /* get a random, truncate it, and make sure it's in the range */
      g_regs[param1] = get_rand() & (param3-1);
      if (g_regs[param1] <= g_regs[param2]) break;
    }
    break;
  default:
    /* unknown instruction */
    return PROCESSING_ERROR;
    break;
  }

  /*
   * finished handling the instruction,
   * yield to do some other stuff and
   * continue processing other instructions afterwards
   */
  return PROCESSING_YIELD;
}

