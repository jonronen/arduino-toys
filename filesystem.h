#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <Sd2Card.h>
#include "sound.h"


#define BUFF_HALF_SIZE 256

extern Sd2Card g_sdc;


int8_t init_fs();
int8_t read_instruction_block();
int8_t process_instruction_block();
int8_t play_sound(uint8_t sound_index);


#endif /* __FILESYSTEM_H__ */

