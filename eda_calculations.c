#include "eda_calculations.h"
#include <math.h>
#include "FFT.h"


struct sensor_q_adrrs sensor_q2;
struct k_work_delayable calc_work;

#define NUM_MEASUREMENTS 60
#define FR 1000
#define AMPLITUDE 1000

//create a reference table form a sine wave wimth A = 100
int32_t ref_sine_table[] = { 50,55,60,65,70,75,79,83,
                             87,90,93,96,98,99,100,100,
                             100,99,98,96,93,90,87,83,
                             79,75,70,65,60,55,50,45,
                             40,35,30,25,21,17,13,10,
                             7,4,2,1,0,0,0,1,
                             2,4,7,10,13,17,21,25,
                             30,35,40,45};


//lowpass
static void lpf(double *x, int M, double *y){
    int n;
    y[0] = 0;
    x[0] = 0;
    for (n=1; n < M ; n++) {
        y[n] =  0.992*y[n-1] + 0.0039*x[n] + 0.0039*x[n-1]; // the coefficients of the lwpass are chosen for fs = 4kHz and cutoff frequency of 5Hz
        //these can be recalculated with the python code written for this in the project
    }
}

//doing the multiplication of the values of the two signals
static int32_t multiplex_signal(int32_t mv_val, int32_t ref_val){
    int32_t demod_val = mv_val*ref_val*ref_val;
    return demod_val;
}

//calculating the impeadance of the skin
int32_t lock_in_amp_calc(){
    int32_t mv_val[NUM_MEASUREMENTS], ua_val[NUM_MEASUREMENTS], multiplex_mv_val[NUM_MEASUREMENTS],z[NUM_MEASUREMENTS];
    double fft_signal[NUM_MEASUREMENTS];
    int i = 0;
    double lpf_signal[NUM_MEASUREMENTS];
    int32_t clear_signal[NUM_MEASUREMENTS];

    //getting data about the measured mV from queue and putting it to an array of 60
    while (k_msgq_num_used_get(sensor_q2.msgq_data_pointer_eda_mv) > 0){ //if there is any data in the queue this is executed
        get_from_edaq_mv(&mv_val[i]);
        printk("queue mv = %ld mV  \n", mv_val[i]);
        k_sleep(K_SECONDS(1));
        //multiplying the mV values with the reference signal
        multiplex_mv_val[i] = multiplex_signal(mv_val[i], ref_sine_table[i]);
        i++;
    }
    i = 0;

    //getting data about the current from queue and putting it to an array of 60
    while (k_msgq_num_used_get(sensor_q2.msgq_data_pointer_eda_ma) > 0){ //if there is any data in the queue this is executed
        get_from_edaq_ma(&ua_val[i]);
//        printk("queue ua = %ld uA  \n", ua_val[i]);
//        k_sleep(K_SECONDS(1));
        i++;
    }

    //calculating average value of shunt current
    int32_t sh_curr = 0;
    for (int j = 0; j < i; j++){
        sh_curr +=ua_val[j];
    }
    sh_curr = sh_curr/i;
    //printk("final current is %ld \n", sh_curr);


/////////////this part still needs changes to work properly...the sequence might not be correct and changes might need to be made on the formulas
    //doing furrier transform on the multiplied signal
    //calculate_DFT(i, &mv_val[0], i, &fft_signal[0]);

    //doing inverse fft on the signal after LPF
    //calculate_IDFT(i, &fft_signal[0], i,&clear_signal[0]);

    //passing the signal trough LPF after it has gone trough fft
    //lpf(&clear_signal[0],i, &lpf_signal[0]);
//////////////////////////////////////////////////


////    calulating impeadance of the signal for all the values in the array
//    for(int j = 1; j < (i-1); j++){
//        z[j] = clear_signal[j]/(1000*sh_curr);
//    }
//
//    //calculating average value of impeadace
//    int32_t impeadance;
//    for (int j = 1; j < i; j++){
//        impeadance +=z[j];
//    }
//    impeadance /= (i-1);
//    printk("final impeadance is %ld", impeadance);
}


/////////////////////////////////////////////////////////////////////////////////////////
///register the message queues
void msgq_addres2(struct sensor_q_adrrs* ptr_from_main_data) {
    sensor_q2.msgq_data_pointer_eda_mv = ptr_from_main_data->msgq_data_pointer_eda_mv;
    sensor_q2.msgq_data_pointer_eda_ma = ptr_from_main_data->msgq_data_pointer_eda_ma;
}
/////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
void work_scheduler_calc() {
    k_work_schedule(&calc_work, K_MSEC(15));
}

//Gets data from queues and calculates the result every 30s
static void calk_work_fn(struct k_work* work) {
    int err;
    err = lock_in_amp_calc();
    if (err) {
        printk("data publish, error: %d\n", err);
    }
    work_scheduler_calc();
}

//initialize the scheduled work
void calc_work_init(void) {
    k_work_init_delayable(&calc_work, calk_work_fn);
}

void cancel_delayable_calc() {
    (void)k_work_cancel_delayable(&calc_work);
}
///////////////////////////////////////////////////////////////////////////////////

