#ifndef __SOUND_H__
#define __SOUND_H__


/*
 * buffer flags:
 * - bit #0: first 256 bytes of last 256 bytes
 * - bit #1: buffer should be updated
 * - bit #2: playing?
 */
#define BUFF_FLAG_SECOND_HALF (1<<0)
#define BUFF_FLAG_NEEDS_FETCH (1<<1)
#define BUFF_FLAG_IS_PLAYING (1<<2)
extern uint8_t g_play_buff_flags;

void sound_setup();
void read_sound_block();
void play_sectors(uint32_t start_sector, uint16_t num_sectors);

extern uint16_t g_curr_sound_len;


#endif /* __SOUND_H__ */

