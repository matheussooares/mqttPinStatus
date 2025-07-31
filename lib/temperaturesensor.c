#include "temperaturesensor.h"

void init_sensor(void) {
    adc_init();
    adc_set_temp_sensor_enabled(true);
}

float temperature_sensor_read(void) {
    adc_select_input(4);
    uint16_t raw = adc_read();

    float voltage = raw * 3.3f / 4095.0f;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;

    return temperature;

}