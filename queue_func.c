#include "queue_func.h"
#include "structs.h"

struct sensor_q_adrrs sensor_q0;

//register the message queues
void msgq_addres(struct sensor_q_adrrs* ptr_from_main_data) {
    sensor_q0.msgq_data_pointer_eda_mv = ptr_from_main_data->msgq_data_pointer_eda_mv;
    sensor_q0.msgq_data_pointer_eda_ma = ptr_from_main_data->msgq_data_pointer_eda_ma;
    sensor_q0.msgq_data_pointer_z = ptr_from_main_data->msgq_data_pointer_z;
}

//puts mv data into eda queue
void put_to_edaq_mv(int32_t put_data) {
    if (sensor_q0.msgq_data_pointer_eda_mv == NULL) {
        printk("EDA msgq is null! Did you forget to call register_msgq_address?");
        return;
    }
    struct data_eda_mv sensor_eda0;
    sensor_eda0.data1 = put_data;
//    printk("put to queue mv = %ld mV  \n", sensor_eda0.data1);
    k_msgq_put(sensor_q0.msgq_data_pointer_eda_mv, &sensor_eda0, K_NO_WAIT);
}

//puts ma data into eda queue
void put_to_edaq_ma(int32_t put_data) {
    if (sensor_q0.msgq_data_pointer_eda_ma == NULL) {
        printk("EDA msgq is null! Did you forget to call register_msgq_address?");
        return;
    }
    struct data_eda_ma sensor_eda0;
    sensor_eda0.data1 = put_data;
//    printk("put to queue ma = %ld mA  \n", sensor_eda0.data1);
    k_msgq_put(sensor_q0.msgq_data_pointer_eda_ma, &sensor_eda0, K_NO_WAIT);
}

//puts ma data into eda queue
void put_to_z(int32_t put_data) {
    if (sensor_q0.msgq_data_pointer_z == NULL) {
        printk("EDA msgq is null! Did you forget to call register_msgq_address?");
        return;
    }
    struct data_z sensor_z;
    sensor_z.data1 = put_data;
    k_msgq_put(sensor_q0.msgq_data_pointer_z, &sensor_z, K_NO_WAIT);
}

//gets the data from the eda queue mv
void get_from_edaq_mv(int32_t* get_data) {
    struct data_eda_mv sensor_eda0;
    int err = k_msgq_get(sensor_q0.msgq_data_pointer_eda_mv, &sensor_eda0, K_NO_WAIT);
    if(err != 0){
        printk("Error with receiving queue message");
        return;
    }
    *get_data = sensor_eda0.data1;
//    printk("get from queue mv = %ld mV  \n", *get_data);
}

//gets the data from the eda queue ma
void get_from_edaq_ma(int32_t* get_data) {
    struct data_eda_ma sensor_eda0;
    int err = k_msgq_get(sensor_q0.msgq_data_pointer_eda_ma, &sensor_eda0, K_NO_WAIT);
    if(err != 0){
        printk("Error with receiving queue message");
        return;
    }
    *get_data = sensor_eda0.data1;
//    printk("get from queue ma = %ld mA  \n", *get_data);
}

//gets the data from the eda queue ma
void get_from_z(int32_t* get_data) {
    struct data_z sensor_z;
    int err = k_msgq_get(sensor_q0.msgq_data_pointer_z, &sensor_z, K_NO_WAIT);
    if(err != 0){
        printk("Error with receiving queue message");
        return;
    }
    *get_data = sensor_z.data1;

}



//
//
////puts mv data into eda queue
//void put_to_edaq_mv(int32_t put_data[]) {
//    if (sensor_q0.msgq_data_pointer_eda_mv == NULL) {
//        printk("EDA msgq is null! Did you forget to call register_msgq_address?");
//        return;
//    }
//    struct data_eda_mv sensor_eda0;
//    for (int i = 0; i < LEN_EDA_MV; i++) {
//        sensor_eda0.data1[i] = put_data[i];
//    }
//    k_msgq_put(sensor_q0.msgq_data_pointer_eda_mv, &sensor_eda0, K_NO_WAIT);
//}
//
////puts ma data into eda queue
//void put_to_edaq_ma(int32_t put_data[]) {
//    if (sensor_q0.msgq_data_pointer_eda_ma == NULL) {
//        printk("EDA msgq is null! Did you forget to call register_msgq_address?");
//        return;
//    }
//    struct data_eda_ma sensor_eda0;
//    for (int i = 0; i < LEN_EDA_MA; i++) {
//        sensor_eda0.data1[i] = put_data[i];
//    }
//    k_msgq_put(sensor_q0.msgq_data_pointer_eda_ma, &sensor_eda0, K_NO_WAIT);
//}
//
////puts ma data into eda queue
//void put_to_z(int32_t put_data[]) {
//    if (sensor_q0.msgq_data_pointer_z == NULL) {
//        printk("EDA msgq is null! Did you forget to call register_msgq_address?");
//        return;
//    }
//    struct data_z sensor_z;
//    for (int i = 0; i < LEN_EDA_MA; i++) {
//        sensor_z.data1[i] = put_data[i];
//    }
//    k_msgq_put(sensor_q0.msgq_data_pointer_z, &sensor_z, K_NO_WAIT);
//}
//
////gets the data from the eda queue mv
//void get_from_edaq_mv(int32_t* get_data) {
//    struct data_eda_mv sensor_eda0;
//    k_msgq_get(sensor_q0.msgq_data_pointer_eda_mv, &sensor_eda0, K_NO_WAIT);
//    for (int i = 0; i < LEN_EDA_MV; i++) {
//        get_data[i] = sensor_eda0.data1[i];
//    }
//}
//
////gets the data from the eda queue ma
//void get_from_edaq_ma(int32_t* get_data) {
//    struct data_eda_ma sensor_eda0;
//    k_msgq_get(sensor_q0.msgq_data_pointer_eda_ma, &sensor_eda0, K_NO_WAIT);
//
//    for (int i = 0; i < LEN_EDA_MA; i++) {
//        get_data[i] = sensor_eda0.data1[i];
//    }
//}
//
////gets the data from the eda queue ma
//void get_from_z(int32_t* get_data) {
//    struct data_z sensor_z;
//    k_msgq_get(sensor_q0.msgq_data_pointer_z, &sensor_z, K_NO_WAIT);
//
//    for (int i = 0; i < LEN_EDA_MA; i++) {
//        get_data[i] = sensor_z.data1[i];
//    }
//}