/** @file
* @defgroup eda_adc_driver eda_adc_driver.h
* @{
* @ingroup zephyr-senti-firmware
*
* @brief Driver to use the nrf52840 ADC for measuring EDA
*
*/

#ifndef ZEPHYR_SENTI_EDA_H
#define ZEPHYR_SENTI_EDA_H

#include <zephyr.h>
#include "queue_func.h"
#include "structs.h"
/**
 * magic number for a bad adc reading
 */
#define BAD_ADC_READING 111

void eda_adc_init();
void work_scheduler_adc_read();
void adc_read_init();
void msgq_addres1(struct sensor_q_adrrs* ptr_from_main_data);
#endif //EPHYR_SENTI_EDA_H

