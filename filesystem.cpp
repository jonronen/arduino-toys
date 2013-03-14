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
#include <Sd2Card.h>
#include "sound.h"
#include "engine.h"
#include "filesystem.h"


Sd2Card g_sdc;
static uint8_t g_fs_buf[BUFF_HALF_SIZE*2];
static uint16_t g_code_buf_offset;
static uint8_t g_code_block_index;

static uint32_t g_partition_start;
static uint32_t g_partition_len;

static uint16_t g_num_sounds;
static uint32_t g_sound_table_sector;
static uint16_t g_num_code_blocks;
static uint32_t g_code_sector;


#define PARTITION_TYPE 0x6a
typedef struct partition_entry_ {
  uint8_t boot;
  uint8_t phys_start[3];
  uint8_t type;
  uint8_t phys_end[3];
  uint32_t sector_start;
  uint32_t sector_cnt;
} partitionEntry;

typedef struct sound_entry_ {
  uint16_t index;
  uint16_t sector_cnt;
  uint32_t sector_offset;
} soundEntry;

#define PARTITION_MAGIC_NUM 0x616e616d
typedef struct partition_toc_ {
  uint32_t magic;
  uint16_t version;

  uint16_t code_start;
  uint16_t code_len;

  uint16_t sound_table_start;
  uint16_t num_sounds;
} partitionToc;


int8_t init_fs()
{
  uint8_t res, i;
  partitionEntry* p_part;
  partitionToc* p_toc;
  
  g_sdc.init(SPI_HALF_SPEED);
  g_sdc.partialBlockRead(1);
  
  res = g_sdc.readBlock(0, g_fs_buf);
  if (!res) {
#ifdef DEBUG_FS
    Serial.println("Error reading partition table");
#endif
    return -1;
  }

  for (i=0; i<4; i++) {
    p_part = (partitionEntry*)&g_fs_buf[0x1be + 0x10*i];
    if ((p_part->boot == 0) &&
        (p_part->type == PARTITION_TYPE)) {
      g_partition_start = p_part->sector_start;
      g_partition_len = p_part->sector_cnt;
#ifdef DEBUG_FS
      Serial.print("Found partition #");
      Serial.print(i);
      Serial.print(" starting at sector ");
      Serial.print(g_partition_start);
      Serial.println(".");
#endif
      break;
    }
  }
  if (i==4) {
#ifdef DEBUG_FS
    Serial.println("Couldn't find the partition");
#endif
    return -1;
  }

  res = g_sdc.readBlock(g_partition_start, g_fs_buf);
  if (!res) {
#ifdef DEBUG_FS
    Serial.println("Error reading partition's index block");
#endif
    return -1;
  }

  p_toc = (partitionToc*)g_fs_buf;
  if ((p_toc->magic == PARTITION_MAGIC_NUM) &&
      (p_toc->version == 1)) {
    g_num_sounds = p_toc->num_sounds;
    g_sound_table_sector = p_toc->sound_table_start + g_partition_start;
    g_code_sector = p_toc->code_start + g_partition_start;
    g_num_code_blocks = p_toc->code_len + g_partition_start;
#ifdef DEBUG_FS
    Serial.print("Sound table starts at sector ");
    Serial.print(g_sound_table_sector);
    Serial.println(".");
#endif
  }
  else {
#ifdef DEBUG_FS
    Serial.println("Malformed sound partition");
#endif
    return -1;
  }
  return 0;
}


#define SOUND_ENTRIES_IN_BLOCK ((sizeof(g_fs_buf) / sizeof(soundEntry)))
int8_t play_sound(uint8_t sound_index)
{
  soundEntry* p_sound_entry;
  uint8_t res;

  res = g_sdc.readBlock(
    g_sound_table_sector + sound_index/SOUND_ENTRIES_IN_BLOCK,
    g_fs_buf
  );
  if (!res) {
#ifdef DEBUG_FS
    Serial.println("Error reading sound table");
#endif
    return -1;
  }

  p_sound_entry = ((soundEntry*)g_fs_buf) +
    (sound_index % SOUND_ENTRIES_IN_BLOCK);
  play_sectors(
    p_sound_entry->sector_offset + g_partition_start,
    p_sound_entry->sector_cnt
  );

  return 0;
}

int8_t read_instruction_block()
{
  uint8_t res;

#ifdef DEBUG_FS
  Serial.print("Block ");
  Serial.println((int)g_code_block_index);
#endif
  res = g_sdc.readBlock(g_code_sector + g_code_block_index, g_fs_buf);
  if (!res) {
#ifdef DEBUG_FS
    Serial.println("Error reading partition table");
#endif
    return -1;
  }
  
  return 0;
}

int8_t process_instruction_block()
{
  uint16_t res;

  res = process_instruction(g_fs_buf, &g_code_buf_offset);
  if (res == PROCESSING_ERROR) {
#ifdef DEBUG_FS
    Serial.println("ERROR!");
#endif
    return -1;
  }
  else if (res < 0x100) g_code_block_index = (uint8_t)res;

  return 0;
}

