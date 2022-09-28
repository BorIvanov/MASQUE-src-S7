//
// Created by svetoslav on 14-07-22.
//

#ifndef HELLO_WORLD_EDA_CALCULATIONS_H
#define HELLO_WORLD_EDA_CALCULATIONS_H

#include <zephyr.h>
#include "queue_func.h"
#include "structs.h"


int32_t calc_z();
void msgq_addres2(struct sensor_q_adrrs* ptr_from_main_data);
void calc_work_init(void);
void cancel_delayable_calc();
void work_scheduler_calc();
#endif //HELLO_WORLD_EDA_CALCULATIONS_H
