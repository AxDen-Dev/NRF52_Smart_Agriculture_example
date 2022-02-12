#ifndef VCNL4040_H_
#define VCNL4040_H_

#include <stdint.h>
#include "board_define.h"
#include "nrf_drv_twi.h"

extern volatile uint8_t vcnl4040_100ms_timer;
extern volatile uint8_t twi_read_done;
extern volatile uint8_t twi_write_done;
extern volatile uint8_t twi_address_nack;

void set_vcnl4040_twi_instance(nrf_drv_twi_t twi_instance);

uint8_t init_vcnl4040();

uint8_t get_vcnl4040_ambient_light(uint16_t *ambient_light);

#endif /* VCNL4040_H_ */
