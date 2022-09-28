#ifndef HELLO_WORLD_AD9850_H
#define HELLO_WORLD_AD9850_H

#include "zephyr/types.h"

int ad9850_hardware_init();
void set_freq(int f, int ctr, int pow_down, uint8_t phase);

#endif //HELLO_WORLD_AD9850_H
