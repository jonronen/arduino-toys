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

