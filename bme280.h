#ifndef BME280_H_
#define BME280_H_

#include <stdint.h>
#include "board_define.h"
#include "nrf_drv_twi.h"

extern volatile uint8_t bme280_100ms_timer;
extern volatile uint8_t twi_read_done;
extern volatile uint8_t twi_write_done;
extern volatile uint8_t twi_address_nack;

void set_bme280_twi_instance(nrf_drv_twi_t twi_instance);

uint8_t init_bme280();

uint8_t get_bme280_temperature(int16_t *temperature_output);

uint8_t get_bme280_pressure(uint32_t *pressure_output);

#endif /* BME280_H_ */
