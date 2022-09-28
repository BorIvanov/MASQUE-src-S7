//
// Created by svetoslav on 21-07-22.
//

#ifndef HELLO_WORLD_FFT_H
#define HELLO_WORLD_FFT_H

#include <math.h>
#include <zephyr.h>

#define PI 3.141592

void calculate_DFT(int len, int32_t *xn, int N, double *Xr, double *Xi);
void calculate_IDFT(int len, double *Xr, double *Xi, int N, int32_t *x);

#endif //HELLO_WORLD_FFT_H
