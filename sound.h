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

