#include "ad9850.h"
#include "devicetree.h"
#include "drivers/gpio.h"
#include "devicetree/gpio.h"

//clk pin setup
#define AD8950_W_CLK_PIN DT_NODELABEL(ad9850_w_clk)
#if DT_NODE_HAS_STATUS(AD8950_W_CLK_PIN, okay)
#define AD8950_W_CLK_WAKE_UP DT_GPIO_LABEL(AD8950_W_CLK_PIN, gpios)
#define AD8950_W_CLK_WAKE_UP_PIN DT_GPIO_PIN(AD8950_W_CLK_PIN, gpios)
#define AD8950_W_CLK_WAKE_UP_FLAGS DT_GPIO_FLAGS(AD8950_W_CLK_PIN, gpios)
#else
#error "Unsupported board: I2C_SPI_SEL_wake_up is not defined"
#define AD8950_W_CLK_WAKE_UP ""
#define AD8950_W_CLK_WAKE_UP_PIN 0
#define AD8950_W_CLK_WAKE_UP_FLAGS 0
#endif

//frequency pin setup
#define AD8950_FQ_UD_PIN DT_NODELABEL(ad9850_fq_ud)
#if DT_NODE_HAS_STATUS(AD8950_FQ_UD_PIN, okay)
#define AD8950_FQ_UD_WAKE_UP DT_GPIO_LABEL(AD8950_FQ_UD_PIN, gpios)
#define AD8950_FQ_UD_WAKE_UP_PIN DT_GPIO_PIN(AD8950_FQ_UD_PIN, gpios)
#define AD8950_FQ_UD_WAKE_UP_FLAGS DT_GPIO_FLAGS(AD8950_FQ_UD_PIN, gpios)
#else
#error "Unsupported board: I2C_SPI_SEL_wake_up is not defined"
#define AD8950_FQ_UD_WAKE_UP ""
#define AD8950_FQ_UD_WAKE_UP_PIN 0
#define AD8950_FQ_UD_WAKE_UP_FLAGS 0
#endif

//data pin setup
#define AD8950_D7_PIN DT_NODELABEL(ad9850_d7)
#if DT_NODE_HAS_STATUS(AD8950_D7_PIN, okay)
#define AD8950_D7_WAKE_UP DT_GPIO_LABEL(AD8950_D7_PIN, gpios)
#define AD8950_D7_WAKE_UP_PIN DT_GPIO_PIN(AD8950_D7_PIN, gpios)
#define AD8950_D7_WAKE_UP_FLAGS DT_GPIO_FLAGS(AD8950_D7_PIN, gpios)
#else
#error "Unsupported board: I2C_SPI_SEL_wake_up is not defined"
#define AD8950_D7_WAKE_UP ""
#define AD8950_D7_WAKE_UP_PIN 0
#define AD8950_D7_WAKE_UP_FLAGS 0
#endif

//reset pin setup
#define AD8950_RESET_PIN DT_NODELABEL(ad9850_reset)
#if DT_NODE_HAS_STATUS(AD8950_RESET_PIN, okay)
#define AD8950_RESET_WAKE_UP DT_GPIO_LABEL(AD8950_RESET_PIN, gpios)
#define AD8950_RESET_WAKE_UP_PIN DT_GPIO_PIN(AD8950_RESET_PIN, gpios)
#define AD8950_RESET_WAKE_UP_FLAGS DT_GPIO_FLAGS(AD8950_RESET_PIN, gpios)
#else
#error "Unsupported board: I2C_SPI_SEL_wake_up is not defined"
#define AD8950_RESET_WAKE_UP ""
#define AD8950_RESET_WAKE_UP_PIN 0
#define AD8950_RESET_WAKE_UP_FLAGS 0
#endif


#define CAL_FREQ 125000000 //calibration frequency
#define TWO_32 4294967296

const struct device* clk_w;
const struct device* freq_ud;
const struct device* data;
const struct device* res;

//send 1 high pulse
static void freq_update(){
    bool on = true;
    gpio_pin_set_raw(freq_ud, AD8950_FQ_UD_WAKE_UP_PIN, (int)on);
    gpio_pin_set_raw(freq_ud, AD8950_FQ_UD_WAKE_UP_PIN, !(int)on);
}

// 0 to set pin low 1 to set pin high for 1 pulse to reset device and return again to 0
static void reset_pin1(int on){
    if(on == 0) {
        gpio_pin_set_raw(res, AD8950_FQ_UD_WAKE_UP_PIN, on);
    }
    else if(on == 1){
        gpio_pin_set_raw(res, AD8950_FQ_UD_WAKE_UP_PIN, on);
        gpio_pin_set_raw(res, AD8950_FQ_UD_WAKE_UP_PIN, !on);
    }
}

//clock pulse for loading bit
static void clk_pulse(){
    bool on = true;
    gpio_pin_set_raw(clk_w, AD8950_W_CLK_WAKE_UP_PIN, (int)on);
    gpio_pin_set_raw(clk_w, AD8950_W_CLK_WAKE_UP_PIN, !(int)on);
}


int ad9850_hardware_init(){
    int ret;
    //////clk pin
    clk_w = device_get_binding(AD8950_W_CLK_WAKE_UP);
    if (clk_w == NULL) {
        printk("Error failed to set word_clk pin\n");
        return -1;
    }
    ret = gpio_pin_configure(clk_w, AD8950_W_CLK_WAKE_UP_PIN, GPIO_OUTPUT_ACTIVE | AD8950_W_CLK_WAKE_UP_FLAGS);
    if (ret < 0) {
        printk("Error failed to configure word_clk pin\n");
        return -1;
    }

    //////frequency update pin
    freq_ud = device_get_binding(AD8950_FQ_UD_WAKE_UP);
    if (freq_ud == NULL) {
        printk("Error failed to set freq_ud  pin\n");
        return -1;
    }
    ret = gpio_pin_configure(freq_ud, AD8950_FQ_UD_WAKE_UP_PIN, GPIO_OUTPUT_ACTIVE | AD8950_FQ_UD_WAKE_UP_FLAGS);
    if (ret < 0) {
        printk("Error failed to configure freq_ud pin\n");
        return -1;
    }

    //////D7 serial data pin
    data = device_get_binding(AD8950_D7_WAKE_UP);
    if (data == NULL) {
        printk("Error failed to set D7 pin\n");
        return -1;
    }
    ret = gpio_pin_configure(data, AD8950_D7_WAKE_UP_PIN, GPIO_OUTPUT | AD8950_D7_WAKE_UP_FLAGS);
    if (ret < 0) {
        printk("Error failed to configure D7 pin\n");
        return -1;
    }

    /////reset pin
    res = device_get_binding(AD8950_RESET_WAKE_UP);
    if (res == NULL) {
        printk("Error failed to set reset pin\n");
        return -1;
    }
    ret = gpio_pin_configure(res, AD8950_RESET_WAKE_UP_PIN, GPIO_OUTPUT | AD8950_RESET_WAKE_UP_FLAGS);
    if (ret < 0) {
        printk("Error failed to configure reset pin\n");
        return -1;
    }

    // set pin low so the device won't reset
    reset_pin1(0);
    //enable serial data read
    freq_update();
    return 0;
}

//shifts the data out to the device in serial mode
static void data_shift(uint32_t tuning_w, int ctr, int pow_down, uint8_t phase){
    for(int i = 0; i <= 31; i++){
        int f = (int)(tuning_w >> i) & 1;
        gpio_pin_set_raw(data, AD8950_D7_WAKE_UP_PIN, f);
        clk_pulse(); // send clk pulse to tell device to recieve bit
    }

    //send control bits
    for(int i = 0; i <= 1; i++){
        gpio_pin_set_raw(data, AD8950_D7_WAKE_UP_PIN, (ctr >> i) & 1);
        clk_pulse(); // send clk pulse to tell device to recieve bit
    }

    //send power down bit
    gpio_pin_set_raw(data, AD8950_D7_WAKE_UP_PIN, pow_down);
    clk_pulse(); // send clk pulse to tell device to recieve bit

    //send phase bits
    for(int i = 0; i <= 3; i++){
        gpio_pin_set_raw(data, AD8950_D7_WAKE_UP_PIN, (phase >> i) & 1);
        clk_pulse(); // send clk pulse to tell device to recieve bit
    }

    //set the  freq update pin to high for 7ns to update the new value
    freq_update();
}

//sets frequency, control, power down options, phase
void set_freq(int f, int ctr, int pow_down, uint8_t phase){
    uint32_t tuning_w = ((f*TWO_32)/CAL_FREQ)/2;
    data_shift(tuning_w, ctr, pow_down, phase);
}

