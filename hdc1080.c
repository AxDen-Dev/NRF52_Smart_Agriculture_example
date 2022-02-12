#include "hdc1080.h"
#include "string.h"

#include "nrf_pwr_mgmt.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define HDC1080_ADDRESS 0x40

static nrf_drv_twi_t m_twi;

static void wait_100_ms() {

	hdc1080_100ms_timer = 0x00;

	while (!hdc1080_100ms_timer) {

		if (NRF_LOG_PROCESS() == false) {

			nrf_pwr_mgmt_run();

		}

	}

}

void set_hdc1080_twi_instance(nrf_drv_twi_t twi_instance) {

	m_twi = twi_instance;

}

static uint8_t i2c_write8(uint8_t address, uint8_t reg) {

	uint16_t timeout = UINT16_MAX;
	uint8_t txBuffer = reg;

	twi_write_done = 0x00;
	twi_read_done = 0x00;
	twi_address_nack = 0x00;

	ret_code_t result = nrf_drv_twi_tx(&m_twi, address, &txBuffer, 1, false);

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

		while (!twi_write_done && --timeout) {

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

uint8_t init_hdc1080() {

	uint8_t i2c_state = 0x00;
	uint8_t init_value[2] = { 0x00 };

	//Reset
	init_value[0] = 0b10000000;
	init_value[1] = 0b00000000;

	i2c_state = i2c_write_buffer(HDC1080_ADDRESS, 0x02, init_value, 2);

	for (uint8_t i = 0; i < 2; i++) {

		wait_100_ms();

	}

	init_value[0] = 0b00010010;
	init_value[1] = 0b00000000;

	i2c_state = i2c_state
			& i2c_write_buffer(HDC1080_ADDRESS, 0x02, init_value, 2);

	return i2c_state;

}

uint8_t get_hdc1080_temperature_humidity(int16_t *temperature_output,
		uint8_t *humidity_output) {

	uint8_t i2c_state = 0x00;
	uint8_t read_buffer[4] = { 0x00 };

	i2c_state = i2c_write8(HDC1080_ADDRESS, 0x00);

	for (uint8_t i = 0; i < 2; i++) {

		wait_100_ms();

	}

	i2c_state = i2c_state & i2c_read_buffer(HDC1080_ADDRESS, read_buffer, 4);

	uint16_t value = read_buffer[0] << 8 | read_buffer[1];
	float temperature = ((float) value / 65536.0f) * 165.0f - 40.0f;
	temperature *= 10;

	value = read_buffer[2] << 8 | read_buffer[3];
	float humidity = ((float) value / 65536.0f) * 100.0f;

	if (humidity > 100) {

		humidity = 100;

	} else if (humidity < 0) {

		humidity = 0;

	}

	*temperature_output = (int16_t) temperature;
	*humidity_output = (uint8_t) humidity;

	return i2c_state;

}

