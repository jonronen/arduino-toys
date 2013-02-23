#include "Arduino.h"


#define BLOCK_SIZE 512

/* registers */
#define NUM_REGS 16
static uint8_t g_regs[NUM_REGS];

/* global array */
#define GLOBAL_ARRAY_LEN 128
static uint8_t g_array[GLOBAL_ARRAY_LEN];


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
uint16_t process_instruction_block(
  uint8_t ins_block[BLOCK_SIZE],
  uint16_t* p_block_offset /* both input and output */
)
{
  opcode_t op;
  uint8_t param1,param2,param3;

  while (1) {
    if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
    
    op = get_opcode(ins_block[*p_block_offset]);
    switch (op) {
    
    case OP_SET_REG:
      param1 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
      g_regs[param1] = ins_block[*p_block_offset];
      *p_block_offset = *p_block_offset+1;
      break;
    
    case OP_ADD_REGS:
      param1 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
      param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
      param3 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      g_regs[param1] = g_regs[param2] + g_regs[param3];
      break;
    
    case OP_SUB_REGS:
      param1 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
      param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
      param3 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      g_regs[param1] = g_regs[param2] - g_regs[param3];
      break;
    
    case OP_MUL_REGS:
      param1 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
      param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
      param3 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      g_regs[param1] = g_regs[param2] * g_regs[param3];
      break;
    
    case OP_DIV_REGS:
      param1 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
      param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
      param3 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      /* check for zero */
      if (g_regs[param3] == 0) return 0xffff;
      g_regs[param1] = g_regs[param2] / g_regs[param3];
      break;
    
    case OP_LIGHT_ON:
      /* TODO */
      break;
    
    case OP_LIGHT_OFF:
      /* TODO */
      break;
    
    case OP_PLAY_SOUND:
      /* TODO */
      break;
    
    case OP_PLAY_SOUND_BLOCKING:
      /* TODO */
      break;
    
    case OP_GET_INPUT:
      /* TODO */
      break;
    
    case OP_DELAY:
      /* TODO */
      break;
    
    case OP_LOAD:
      *p_block_offset = *p_block_offset+1;
      if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
      param1 = (ins_block[*p_block_offset] & 0xf0) >> 4;
      param2 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      /* check for an invalid offset */
      if (g_regs[param2] >= GLOBAL_ARRAY_LEN) return 0xffff;
      g_regs[param1] = g_array[g_regs[param2]];
      break;
    
    case OP_STORE:
      *p_block_offset = *p_block_offset+1;
      if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
      param1 = (ins_block[*p_block_offset] & 0xf0) >> 4;
      param2 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      /* check for an invalid offset */
      if (g_regs[param2] >= GLOBAL_ARRAY_LEN) return 0xffff;
      g_array[g_regs[param2]] = g_regs[param1];
      break;
    
    case OP_JUMP:
      param1 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      return g_regs[param1];
      break;
    
    case OP_JUMP_EQUAL:
      param1 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
      param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
      param3 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      if (g_regs[param1] == g_regs[param2]) return g_regs[param3];
      break;
    
    case OP_JUMP_GREATER:
      param1 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      if (*p_block_offset >= BLOCK_SIZE) return 0xffff;
      param2 = (ins_block[*p_block_offset] & 0xf0) >> 4;
      param3 = ins_block[*p_block_offset] & 0x0f;
      *p_block_offset = *p_block_offset+1;
      if (g_regs[param1] > g_regs[param2]) return g_regs[param3];
      break;
    
    case OP_RANDOM:
      /* TODO */
      break;
    default:
      /* unknown instruction */
      return 0xffff;
      break;
    }
  }
}

