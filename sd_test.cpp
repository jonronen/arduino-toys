#include "Arduino.h"
#include <Sd2Card.h>

#define BUFF_HALF_SIZE 256

static uint8_t g_buff[BUFF_HALF_SIZE*2];
static uint8_t g_buff_pos;

/*
 * buffer flags:
 * - bit #0: first 256 bytes of last 256 bytes
 * - bit #1: buffer should be updated
 * - bit #2: playing?
 */
#define BUFF_FLAG_SECOND_HALF (1<<0)
#define BUFF_FLAG_NEEDS_FETCH (1<<1)
#define BUFF_FLAG_IS_PLAYING (1<<2)
static uint8_t g_buff_flags;

void setup();
void loop();

static Sd2Card g_sdc;

static uint32_t g_partition_start;
static uint32_t g_partition_len;

static uint16_t g_num_sounds;
static uint32_t g_sound_table_sector;

static uint32_t g_curr_sound_block;
static uint16_t g_curr_sound_len;
static void start_playing(uint32_t start_sector, uint16_t num_sectors);


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


static void sound_setup()
{
  pinMode(11, OUTPUT);
  
  // Set up Timer 2 to do pulse width modulation on the speaker
  // pin.
  
  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));
  
  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);
  
  // Do non-inverting PWM on pin OC2A (p.155)
  // On the Arduino this is pin 11.
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
  TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));
  
  // No prescaler (p.158)
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // Set initial pulse width to the first sample.
  OCR2A = 0x80;
  
  
  // Set up Timer 1 to send a sample every interrupt.
  
  cli();
  
  // Set CTC mode (Clear Timer on Compare Match) (p.133)
  // Have to set OCR1A *after*, otherwise it gets reset to 0!
  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));
  
  // No prescaler (p.134)
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // Set the compare register (OCR1A).
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = 500;    // 16e6 / 32000
  
  // Enable interrupt when TCNT1 == OCR1A (p.136)
  TIMSK1 |= _BV(OCIE1A);
  sei();
}


void setup()
{
  uint8_t res, i;
  partitionEntry* p_part;
  partitionToc* p_toc;
  soundEntry* p_sound_entry;
  
  g_sdc.init(SPI_HALF_SPEED);
  g_sdc.partialBlockRead(1);
  
  Serial.begin(115200);
  
  res = g_sdc.readBlock(0, g_buff);
  if (!res) {
    Serial.println("Error reading partition table");
    return;
  }

  for (i=0; i<4; i++) {
    p_part = (partitionEntry*)&g_buff[0x1be + 0x10*i];
    if ((p_part->boot == 0) &&
        (p_part->type == PARTITION_TYPE)) {
      g_partition_start = p_part->sector_start;
      g_partition_len = p_part->sector_cnt;
      Serial.print("Found partition #");
      Serial.print(i);
      Serial.print(" starting at sector ");
      Serial.print(g_partition_start);
      Serial.println(".");
      break;
    }
  }
  if (i==4) {
    Serial.println("Couldn't find the partition");
    return;
  }

  res = g_sdc.readBlock(g_partition_start, g_buff);
  if (!res) {
    Serial.println("Error reading partition's index block");
    return;
  }

  p_toc = (partitionToc*)g_buff;
  if ((p_toc->magic == PARTITION_MAGIC_NUM) &&
      (p_toc->version == 1)) {
    g_num_sounds = p_toc->num_sounds;
    g_sound_table_sector = p_toc->sound_table_start + g_partition_start;
    Serial.print("Sound table starts at sector ");
    Serial.print(g_sound_table_sector);
    Serial.println(".");
  }
  else {
    Serial.println("Malformed sound partition");
    return;
  }

  res = g_sdc.readBlock(g_sound_table_sector, g_buff);
  if (!res) {
    Serial.println("Error reading sound table");
    return;
  }

  sound_setup();

  p_sound_entry = (soundEntry*)g_buff;
  start_playing(
    p_sound_entry->sector_offset + g_partition_start,
    p_sound_entry->sector_cnt
  );
}

static void start_playing(uint32_t start_sector, uint16_t num_sectors)
{
  uint8_t res;

  // disable interrupts
  cli();

  g_curr_sound_block = start_sector;
  g_curr_sound_len = num_sectors;

  res = g_sdc.readBlock(start_sector, g_buff);
  if (!res) {
    Serial.println("Error reading first sound block");
    g_curr_sound_block = 0;
    g_curr_sound_len = 0;
    return;
  }
  
  g_buff_pos = 0;
  g_buff_flags =
    BUFF_FLAG_NEEDS_FETCH |
    BUFF_FLAG_SECOND_HALF |
    BUFF_FLAG_IS_PLAYING;

  sei();
}

static void read_next_block_if_needed()
{
  uint8_t res;

  if ((g_buff_flags & (BUFF_FLAG_NEEDS_FETCH|BUFF_FLAG_IS_PLAYING)) !=
      (BUFF_FLAG_NEEDS_FETCH|BUFF_FLAG_IS_PLAYING)) {
    return;
  }

  if (g_buff_flags & BUFF_FLAG_SECOND_HALF) {
    res = g_sdc.readData(
      g_curr_sound_block, BUFF_HALF_SIZE, BUFF_HALF_SIZE,
      g_buff+BUFF_HALF_SIZE
    );
    if (!res) {
      Serial.println("Error reading second half of block");
      g_curr_sound_block = 0;
      g_curr_sound_len = 0;
      cli();
      g_buff_flags = 0;
      sei();
      return;
    }
  }
  else {
    // move to next block
    g_curr_sound_block++;
    g_curr_sound_len--;

    // is there a next block?
    if (g_curr_sound_len == 0) {
      // finish playing after the current block
      g_curr_sound_block = 0;
      g_curr_sound_len = 0;
      cli();
      g_buff_flags = 0;
      sei();
      return;
    }

    // there is a next block. read it
    res = g_sdc.readData(g_curr_sound_block, 0, BUFF_HALF_SIZE, g_buff);
    if (!res) {
      Serial.println("Error reading first half of block");
      g_curr_sound_block = 0;
      g_curr_sound_len = 0;
      cli();
      g_buff_flags = 0;
      sei();
      return;
    }
  }

  // now clear the "needs fetch" bit and switch to the other half of the buffer
  cli();
  g_buff_flags ^= (BUFF_FLAG_NEEDS_FETCH|BUFF_FLAG_SECOND_HALF);
  sei();
}


void loop()
{
  read_next_block_if_needed();
}

ISR(TIMER1_COMPA_vect)
{
  if ((g_buff_flags & BUFF_FLAG_IS_PLAYING) || g_buff_pos) {
    OCR2A = g_buff[
      (uint16_t)g_buff_pos +
      BUFF_HALF_SIZE*(g_buff_flags & BUFF_FLAG_SECOND_HALF)
    ];
    g_buff_pos++;
    if (g_buff_pos == 0) {
      g_buff_flags ^= BUFF_FLAG_SECOND_HALF;
      g_buff_flags |= BUFF_FLAG_NEEDS_FETCH;
    }
  }
}

