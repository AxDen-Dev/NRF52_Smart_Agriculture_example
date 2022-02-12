#include "vcnl4040.h"
#include "string.h"

#include "nrf_pwr_mgmt.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define VCNL4040_ADDRESS 0x60
#define ALS_CONF 0x00
#define PS_CONF1_2 0x03
#define PS_CONF3_PS_MS 0x04
#define PS_Data 0x08
#define ALS_Data 0x09
#define White_Data 0x0A
#define ID 0x0C

typedef union {

	struct {

		uint16_t ALS_SD :1;
		uint16_t ALS_INT_EN :1;
		uint16_t ALS_PERS :2;
		uint16_t Reserved0 :2;
		uint16_t ALS_IT :2;
		uint16_t Reserved1 :8;

	} bits;

	uint16_t value;

} als_conf_t;

typedef union {

	struct {

		uint16_t PS_SD :1;
		uint16_t PS_IT :3;
		uint16_t PS_PERS :2;
		uint16_t PS_Duty :2;

		uint16_t PS_INT :2;
		uint16_t Reserved0 :1;
		uint16_t PS_HD :1;
		uint16_t Reserved1 :4;

	} bits;

	uint16_t value;

} ps_conf1_2;

typedef union {

	struct {

		uint16_t PS_SC_EN :1;
		uint16_t Reserved0 :1;
		uint16_t PS_TRIG :1;
		uint16_t PS_AF :1;
		uint16_t PS_SMART_PERS :1;
		uint16_t PS_MPS :2;
		uint16_t Reserved1 :1;

		uint16_t LED_I :3;
		uint16_t Reserved2 :3;
		uint16_t PS_MS :1;
		uint16_t White_EN :1;

	} bits;

	uint16_t value;

} ps_conf3_ps_ms;

static nrf_drv_twi_t m_twi;

static void wait_100_ms() {

	vcnl4040_100ms_timer = 0x00;

	while (!vcnl4040_100ms_timer) {

		if (NRF_LOG_PROCESS() == false) {

			nrf_pwr_mgmt_run();

		}

	}

}

void set_vcnl4040_twi_instance(nrf_drv_twi_t twi_instance) {

	m_twi = twi_instance;

}

static uint8_t i2c_write8(uint8_t address, uint8_t reg) {

	uint16_t timeout = UINT16_MAX;
	uint8_t txBuffer = reg;

	twi_write_done = 0x00;
	twi_read_done = 0x00;
	twi_address_nack = 0x00;

	ret_code_t result = nrf_drv_twi_tx(&m_twi, address, &txBuffer, 1, true);

	if (result == NRF_SUCCESS) {

		while (!twi_write_done && --timeout) {

		}

		if (!timeout || twi_address_nack) {

			return 0x00;

		} else {

			return 0x01;

		}

	} else {

		return 0x00;

	}

}

static uint8_t i2c_write_buffer(uint8_t address, uint8_t reg, uint8_t *value,
		const uint8_t buffer_size) {

	uint16_t timeout = UINT16_MAX;
	uint8_t txBuffer[buffer_size + 1];

	txBuffer[0] = reg;
	memcpy(txBuffer + 1, value, buffer_size);

	twi_write_done = 0x00;
	twi_read_done = 0x00;
	twi_address_nack = 0x00;

	ret_code_t result = nrf_drv_twi_tx(&m_twi, address, txBuffer,
			sizeof(txBuffer), false);

	if (result == NRF_SUCCESS) {

		while (!twi_write_done && --timeout) {

		}

		if (!timeout || twi_address_nack) {

			return 0x00;

		} else {

			return 0x01;

		}

	} else {

		return 0x00;

	}

}

static uint8_t i2c_read_buffer(uint8_t address, uint8_t *read_value,
		const uint8_t buffer_size) {

	uint16_t timeout = UINT16_MAX;
	uint8_t rxBuffer[buffer_size];

	twi_write_done = 0x00;
	twi_read_done = 0x00;
	twi_address_nack = 0x00;

	ret_code_t result = nrf_drv_twi_rx(&m_twi, address, rxBuffer, buffer_size);

	if (result == NRF_SUCCESS) {

		while (!twi_read_done && --timeout) {

		}

		memcpy(read_value, rxBuffer, buffer_size);

		if (!timeout || twi_address_nack) {

			return 0x00;

		} else {

			return 0x01;

		}

	} else {

		return 0x00;

	}

}

uint8_t init_vcnl4040() {

	uint8_t i2c_state = 0x00;
	uint8_t init_value[2] = { 0x00 };

	als_conf_t als_conf;
	als_conf.value = 0x00;

	init_value[0] = (als_conf.value & 0xFF);
	init_value[1] = (als_conf.value >> 8);

	for (uint8_t i = 0; i < 2; i++) {

		wait_100_ms();

	}

	i2c_state = i2c_write_buffer(VCNL4040_ADDRESS, ALS_CONF, init_value, 2);

	return i2c_state;

}

uint8_t get_vcnl4040_ambient_light(uint16_t *ambient_light) {

	uint8_t i2c_state = 0x00;
	uint8_t read_value[2] = { 0x00 };

	i2c_state = i2c_write8(VCNL4040_ADDRESS, ALS_Data);

	i2c_state = i2c_state & i2c_read_buffer(VCNL4040_ADDRESS, read_value, 2);

	uint16_t value = ((uint16_t) read_value[1] << 8 | read_value[0]);
	*ambient_light = value * 0.1;

	return i2c_state;

}
