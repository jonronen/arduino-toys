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

