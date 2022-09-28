#include "eda.h"
#include "devicetree.h"
#include "drivers/gpio.h"
#include "queue_func.h"
#include "structs.h"
#include <string.h>
#include <drivers/adc.h>

struct sensor_q_adrrs sensor_q1;
struct k_work_delayable adc_read_work;

#define SH_R 100 //chunt resistor value

#define ADC_READ_MSECONDS 250 //the reading ms of the ADC

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define ADC_NUM_CHANNELS DT_PROP_LEN(DT_PATH(zephyr_user), io_channels)
#define ADC_NODE DT_PHANDLE(DT_PATH(zephyr_user), io_channels)

/* Common settings supported by most ADCs */
#define ADC_RESOLUTION		12
#define ADC_GAIN		    ADC_GAIN_1
#define ADC_REFERENCE		ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 40)  //ADC_ACQ_TIME_DEFAULT

/* Get the numbers of up to two channels */
static uint8_t channel_ids[ADC_NUM_CHANNELS] = {
        DT_IO_CHANNELS_INPUT_BY_IDX(DT_PATH(zephyr_user), 0),
        DT_IO_CHANNELS_INPUT_BY_IDX(DT_PATH(zephyr_user), 1)
};

static int16_t sample_buffer[ADC_NUM_CHANNELS];

struct adc_channel_cfg channel_cfg = {
        .gain = ADC_GAIN,
        .reference = ADC_REFERENCE,
        .acquisition_time = ADC_ACQUISITION_TIME,
        /* channel ID will be overwritten below */
        .channel_id = 0,
        .differential = 0
};

struct adc_sequence sequence = {
        /* individual channels will be added below */
        .channels    = 0,
        .buffer      = sample_buffer,
        /* buffer size in bytes, not number of samples */
        .buffer_size = sizeof(sample_buffer),
        .resolution  = ADC_RESOLUTION,
};

typedef struct {
    const struct device* adc_device;
    bool is_initialized;
} adc_drv_instance;

static adc_drv_instance instance = {
        .is_initialized = false,
};


//initialization function for the 2 ADC  channels used
void eda_adc_init() {
    int err;
    instance.adc_device = DEVICE_DT_GET(ADC_NODE);

    if (!device_is_ready(instance.adc_device)) {
       // printk("ADC device not found\n");
        return;
    }

    //Configure channels individually prior to sampling
    for (uint8_t i = 0; i < ADC_NUM_CHANNELS; i++) {
        channel_cfg.channel_id = channel_ids[i];
#ifdef CONFIG_ADC_NRFX_SAADC
        channel_cfg.input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0 + channel_ids[i];
#endif
        err = adc_channel_setup(instance.adc_device, &channel_cfg);
        if (err) {
           // printk("Error %d: unable to setup adc channel", err);
            instance.adc_device = NULL;
        }
        sequence.channels |= BIT(channel_ids[i]);
    }
    memset(sample_buffer, 0, sizeof(sample_buffer));
    instance.is_initialized = true;
}


//reading raw value of the shunt resistor
//converting value to mV and sending it to shunt_current()
static int32_t eda_adc_read_sh_out(){
    int err;
    int32_t mv_value;

    __ASSERT(instance.is_initialized, "ADC measure called while uninitialized");
    // choose a value that cannot be measured if nothing goes wrong, to check if measurement failed
    sample_buffer[0] = -BAD_ADC_READING;
    if (!instance.adc_device) {
        //printk("adc device is null");
        return -EIO;
    }

    //Read sequence of channels (fails if not supported by MCU)
    err = adc_read(instance.adc_device, &sequence);
    if (err != 0) {
        printk("ADC reading failed with error %d.\n", err);
        return -EIO;
    }

    int32_t raw_val = sample_buffer[0];
    if (raw_val == -BAD_ADC_READING) {
        printk("EDA reading == BAD_ADC_READING");
        return -EIO;
    }
    if (raw_val < 0) {
        raw_val = 0;
    }
//    printk(" Raw value: %ld \n", raw_val);
    return raw_val;
}

//shunt current is calculated from the raw value of the ADC at certain periods of time
//it saves the value in a queue
static void shunt_current(){
    int32_t mv_value;
    int32_t adc_vref = adc_ref_internal(instance.adc_device);
    if (adc_vref > 0) {
        //convert raw reading to millivolts if drive supports reading of ADC reference voltage
        mv_value = eda_adc_read_sh_out();
        adc_raw_to_millivolts(adc_vref, ADC_GAIN, ADC_RESOLUTION, &mv_value);
//        printk("sh mv = %ld mV  \n", mv_value);
    }

    int32_t current = (mv_value*1000)/SH_R;
//    printk("sh ua = %ld uA  \n", current);
    put_to_edaq_ma(current);
}


//reading raw value of the electrode connected to the body and converting it to mV
//activated with a timer
//values are saved in a queue
static int16_t eda_adc_read_el_out(){
    int err;
    int32_t mv_value;

    __ASSERT(instance.is_initialized, "ADC measure called while uninitialized");
    // choose a value that cannot be measured if nothing goes wrong, to check if measurement failed
    sample_buffer[0] = -BAD_ADC_READING;
    if (!instance.adc_device) {
        printk("adc device is null");
        return -EIO;
    }

    //Read sequence of channels (fails if not supported by MCU)
    err = adc_read(instance.adc_device, &sequence);
    if (err != 0) {
        printk("ADC reading failed with error %d.\n", err);
        return -EIO;
    }

    int32_t raw_val = sample_buffer[1];
    if (raw_val == -BAD_ADC_READING) {
        printk("EDA reading == BAD_ADC_READING");
        return -EIO;
    }
    if (raw_val < 0) {
        raw_val = 0;
    }

    int32_t adc_vref = adc_ref_internal(instance.adc_device);
    if (adc_vref > 0) {
        //convert raw reading to millivolts if drive supports reading of ADC reference voltage
        mv_value = raw_val;
        adc_raw_to_millivolts(adc_vref, ADC_GAIN, ADC_RESOLUTION, &mv_value);
    }
//    printk("el mv = %ld mV  \n", mv_value);
    put_to_edaq_mv(mv_value);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////work functions

//work scheduler of data generate function
void work_scheduler_adc_read() {
    k_work_schedule(&adc_read_work, K_USEC(ADC_READ_MSECONDS));
}

//function which calls the data generation and reschedules it
static void adc_read_work_fn() {
    shunt_current();

    int err = eda_adc_read_el_out();
    if (err) {
       printk("adc read, error: %d\n", err);
    }
    work_scheduler_adc_read();
}

//function for initialization of delayable work for data generation
void adc_read_init() {
    k_work_init_delayable(&adc_read_work, adc_read_work_fn);
}

//cancels the delayable if no more data needs to be scheduled for generation
void cancel_delayable_adc_read() {
    (void)k_work_cancel_delayable(&adc_read_work);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// register data, time queues
void msgq_addres1(struct sensor_q_adrrs* ptr_from_main_data) {
    sensor_q1.msgq_data_pointer_eda_mv = ptr_from_main_data->msgq_data_pointer_eda_mv;
    sensor_q1.msgq_data_pointer_eda_ma = ptr_from_main_data->msgq_data_pointer_eda_ma;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
