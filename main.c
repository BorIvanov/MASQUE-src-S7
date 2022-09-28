#include "eda.h"
#include "ad9850.h"
#include "queue_func.h"
#include "structs.h"
#include "sensor_queues.h"
#include "eda_calculations.h"

struct sensor_q_adrrs sensor_q = {
        .msgq_data_pointer_eda_mv = &data_edaq_mv,
        .msgq_data_pointer_eda_ma = &data_edaq_ma,
        .msgq_data_pointer_z = &dataq_z,
};


int main(){
    calc_work_init();
    adc_read_init();

    //initialize the queues for data and timestamp
    msgq_addres(&sensor_q);
    msgq_addres1(&sensor_q);
    msgq_addres2(&sensor_q);

    ad9850_hardware_init(); //initialize the ad9850 hardware settings
    eda_adc_init(); // initialize adc readings of mV and uA
    set_freq(1000, 0, 0, 0); //set the frequency of the ad9850 to 1kHz
    work_scheduler_adc_read(); //schedule the readings of the adc to be fs = 4kHz
    work_scheduler_calc();  // Schedule calculations to be performed after 60 signals are recorded (for testing purposes only 60)

    return 0;
}