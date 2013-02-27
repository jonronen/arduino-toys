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
/* long opcodes */
static const opcode_t g_long_opcodes[] = {
  OP_MAX_VALUE,             /* 0x00 */
  OP_ADD_REGS,              /* 0x01 */
  OP_SUB_REGS,              /* 0x02 */
  OP_MUL_REGS,              /* 0x03 */
  OP_DIV_REGS,              /* 0x04 */
  OP_GET_INPUT,             /* 0x05 */
  OP_LOAD,                  /* 0x06 */
  OP_STORE,                 /* 0x07 */
  OP_RANDOM,                /* 0x08 */
  OP_MAX_VALUE,             /* 0x09 */
  OP_MAX_VALUE,             /* 0x0A */
  OP_MAX_VALUE,             /* 0x0B */
  OP_MAX_VALUE,             /* 0x0C */
  OP_MAX_VALUE,             /* 0x0D */
  OP_MAX_VALUE,             /* 0x0E */
  OP_MAX_VALUE,             /* 0x0F */
};


static uint8_t get_input()
{
  uint8_t ans = 0xff;
  uint16_t key;

  if (digitalRead(INPUT0_PIN) == 0)
    ans = 0;
  else if (digitalRead(INPUT1_PIN) == 0)
    ans = 1;
  else if (digitalRead(INPUT2_PIN) == 0)
    ans = 2;
  else if (digitalRead(INPUT3_PIN) == 0)
    ans = 3;
  else if (analogRead(INPUT4_PIN) <= 0x200) /* input4 is analog */
    ans = 4;

  if (ans != 0xff) {
    /* there is an input. seed the random number generator */
    key = TCNT1;
    put_key((const uint8_t*)&key, 2);
  }

  return ans;
}


static opcode_t get_opcode(uint8_t ins)
{
  opcode_t retval = g_short_opcodes[(ins & 0xf0) >> 4];
  if (retval != OP_INCOMPLETE) return retval;
  return g_long_opcodes[ins&0x0f];
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
    if (g_regs[g_input_register] != 0xff) {
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

  op = get_opcode(ins_block[*p_block_offset]);
  switch (op) {
  
  case OP_SET_REG:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    g_regs[param1] = ins_block[*p_block_offset];
    *p_block_offset = *p_block_offset+1;
    break;
  
  case OP_ADD_REGS:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param3 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    g_regs[param1] = g_regs[param2] + g_regs[param3];
    break;
  
  case OP_SUB_REGS:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param3 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    g_regs[param1] = g_regs[param2] - g_regs[param3];
    break;
  
  case OP_MUL_REGS:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param3 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    g_regs[param1] = g_regs[param2] * g_regs[param3];
    break;
  
  case OP_DIV_REGS:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param3 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    /* check for zero */
    if (g_regs[param3] == 0) return PROCESSING_ERROR;
    g_regs[param1] = g_regs[param2] / g_regs[param3];
    break;
  
  case OP_LIGHT_ON:
    /* TODO */
    break;
  
  case OP_LIGHT_OFF:
    /* TODO */
    break;
  
  case OP_PLAY_SOUND_BLOCKING:
    g_f_playing_sound_block = 1;
    /* fall through */
  case OP_PLAY_SOUND:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    play_sound(g_regs[param1]);
    break;
  
  case OP_GET_INPUT:
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param1 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param2 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;

    /* get the input if there is one */
    g_regs[param1] = get_input();
    if (g_regs[param1] == 0xff) {
      g_delay_cnt = g_regs[param2];

      /* no input - set the delay and wait */
      if (g_delay_cnt) {
        g_f_waiting_for_input = 1;
        g_input_register = param1;
      }
    }
    break;
  
  case OP_DELAY:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    g_delay_cnt = g_regs[param1];
    break;
  
  case OP_LOAD:
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param1 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param2 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    /* check for an invalid offset */
    if (g_regs[param2] >= GLOBAL_ARRAY_LEN) return PROCESSING_ERROR;
    g_regs[param1] = g_array[g_regs[param2]];
    break;
  
  case OP_STORE:
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param1 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param2 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    /* check for an invalid offset */
    if (g_regs[param2] >= GLOBAL_ARRAY_LEN) return PROCESSING_ERROR;
    g_array[g_regs[param2]] = g_regs[param1];
    break;
  
  case OP_JUMP:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = 0;
    return g_regs[param1];
    break;
  
  case OP_JUMP_EQUAL:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param3 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (g_regs[param1] == g_regs[param2]) {
      *p_block_offset = 0;
      return g_regs[param3];
    }
    break;
  
  case OP_JUMP_GREATER:
    param1 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param3 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    if (g_regs[param1] > g_regs[param2]) {
      *p_block_offset = 0;
      return g_regs[param3];
    }
    break;
  
  case OP_RANDOM:
    *p_block_offset = *p_block_offset+1;
    if (*p_block_offset >= CODE_BLOCK_SIZE) return PROCESSING_ERROR;
    param1 = (ins_block[*p_block_offset] & 0xf0) >> 4;
    param2 = ins_block[*p_block_offset] & 0x0f;
    *p_block_offset = *p_block_offset+1;
    
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

