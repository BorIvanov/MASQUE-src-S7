#include "FFT.h"

// Function to calculate the DFT
//N - num of points in DFT, xn - values of the signal
//only calculating real part
void calculate_DFT(int len, int32_t *xn, int N, double *Xr, double *Xi){
    int k, n = 0;
    for (k = 0; k < N; k++) {
        Xr[k] = 0;
        Xi[k] = 0;
        for (n = 0; n < len; n++) {
            Xr[k] = (Xr[k] + xn[n] * cos(2 * PI * k * n / N));
            Xi[k] = (Xi[k] - xn[n] * sin(2 * PI * k * n / N));
        }
        printk("h %f \n", Xr[k]);
    }
}

// Function to calculate the inverse
// discrete fourier transformation
//only real part
void calculate_IDFT(int len, double *Xr, double *Xi, int N, int32_t *x){
    int k, n= 0;

    for (n = 0; n < N; n++) {
        x[n] = 0;
        for (k = 0; k < N; k++) {
            int theta = (2 * 3.141592 * k * n) / N;
            x[n] = x[n] + Xr[k] * cos(theta) + Xi[k] * sin(theta);
        }
        x[n] = x[n] / N;
        printk("\n h x[%d] = %ld\n", n, x[n]);
    }
}


