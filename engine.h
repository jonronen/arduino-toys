#ifndef __ENGINE_H__
#define __ENGINE_H__

#define CODE_BLOCK_SIZE 512


/* return values when processing an instruction */
#define PROCESSING_ERROR 0xffff
#define PROCESSING_YIELD 0xfffe

/*
 * return value:
 * 0x00 to 0xff - next instruction block
 * 0xffff       - error
 * 0xfffe       - yield (for delays / playbacks)
 */
uint16_t process_instruction(
  uint8_t ins_block[CODE_BLOCK_SIZE],
  uint16_t* p_block_offset /* both input and output */
);


extern uint8_t g_delay_cnt;


#endif /* __ENGINE_H__ */

