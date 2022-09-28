//
// Created by svetoslav on 14-07-22.
//

#ifndef HELLO_WORLD_STRUCTS_H
#define HELLO_WORLD_STRUCTS_H


#include <zephyr.h>

//data structures for the different values the program needs to read or save in a queue
struct data_eda_ma {
    int32_t data1;
    int32_t data2;
    int32_t data3;
    int32_t data4;
};


struct data_z {
    int32_t data1;
    int32_t data2;
    int32_t data3;
    int32_t data4;
};

struct data_eda_mv {
    int32_t data1;
    int32_t data2;
    int32_t data3;
    int32_t data4;
};

struct sensor_q_adrrs {
    struct k_msgq* msgq_data_pointer_eda_ma;
    struct k_msgq* msgq_data_pointer_eda_mv;
    struct k_msgq* msgq_data_pointer_z;
};

#endif //HELLO_WORLD_STRUCTS_H
