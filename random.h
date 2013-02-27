#ifndef __RANDOM_H__
#define __RANDOM_H__


#define RANDOM_PIN 4

void random_init();
void put_key(const uint8_t key[], uint8_t key_len);
uint8_t get_rand();


#endif /* __RANDOM_H__ */

