#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H
    #include "hardware/adc.h"
    void init_sensor(void);
    float temperature_sensor_read(void);
#endif