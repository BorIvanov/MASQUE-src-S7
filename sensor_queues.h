#ifndef HELLO_WORLD_SENSOR_QUEUES_H
#define HELLO_WORLD_SENSOR_QUEUES_H

#include "structs.h"
#include <zephyr.h>

K_MSGQ_DEFINE(data_edaq_mv, sizeof(struct data_eda_mv), 60, 4); // the .max_msgs can be changed to the number of messages you need to store, this might need to change in eda_calculations also
K_MSGQ_DEFINE(data_edaq_ma, sizeof(struct data_eda_ma), 60, 4); // the .max_msgs can be changed to the number of messages you need to store, this might need to change in eda_calculations also
K_MSGQ_DEFINE(dataq_z, sizeof(struct data_z), 60, 4); // the .max_msgs can be changed to the number of messages you need to store, this might need to change in eda_calculations also

#endif //HELLO_WORLD_SENSOR_QUEUES_H
