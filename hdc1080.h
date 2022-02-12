#ifndef HDC1080_H_
#define HDC1080_H_

#include <stdint.h>
#include "board_define.h"
#include "nrf_drv_twi.h"

extern volatile uint8_t hdc1080_100ms_timer;
extern volatile uint8_t twi_read_done;
extern volatile uint8_t twi_write_done;
extern volatile uint8_t twi_address_nack;

void set_hdc1080_twi_instance(nrf_drv_twi_t twi_instance);

uint8_t init_hdc1080();

uint8_t get_hdc1080_temperature_humidity(int16_t *temperature_output,
		uint8_t *humidity_output);

#endif /* HDC1080_H_ */
