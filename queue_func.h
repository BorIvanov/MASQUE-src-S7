#ifndef HELLO_WORLD_QUEUE_FUNC_H
#define HELLO_WORLD_QUEUE_FUNC_H

#include <zephyr.h>
#include "queue_func.h"
#include "structs.h"

void msgq_addres(struct sensor_q_adrrs* ptr_from_main_data);
void put_to_edaq_mv(int32_t put_data);
void put_to_edaq_ma(int32_t put_data);
void get_from_edaq_mv(int32_t* get_data);
void get_from_edaq_ma(int32_t* get_data);
void put_to_z(int32_t put_data);
void get_from_z(int32_t* get_data);
#endif //HELLO_WORLD_QUEUE_FUNC_H
