/*
 * adxl.c
 *
 *  Created on: Apr 15, 2017
 *      Author: rezwan
 */

#include "adxl345.h"

void init_ADXL345_double_tap(void) {
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x01D); // THRESH_TAP Register
	i2c_write(0x50); //5g
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x022); //Latent register
	i2c_write(0x05);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x023); //window register
	i2c_write(0xFF);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x021); //duration register
	i2c_write(0x10);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02A); //tap axis register
	i2c_write(0b00000111); // all axes
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02E); //Interrupt Enable
	i2c_write(0b00100000); // all axes
	i2c_stop();

}
void init_ADXL345() {
	init_ADXL345_double_tap();
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02D);
	i2c_write(0);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02D);
	i2c_write(16);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x02D);
	i2c_write(8);
	i2c_stop();

	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x31);
	i2c_write(0x08); //full range +/-2g
	i2c_stop();

}

uint8_t check_ADXL345() {
	uint8_t data;
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x00);
	i2c_stop();
	i2c_start(ADXL345_I2C_Address, TW_READ);
	i2c_read(&data, NACK);
	return data;

}

void read_ADXL345(int *x, int* y, int *z) {
	uint8_t buffer[6];
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x32);
	i2c_stop();
	i2c_start(ADXL345_I2C_Address, TW_READ);
	i2c_read(&buffer[0], ACK);
	i2c_read(&buffer[1], ACK);
	i2c_read(&buffer[2], ACK);
	i2c_read(&buffer[3], ACK);
	i2c_read(&buffer[4], ACK);
	i2c_read(&buffer[5], NACK);
	*x = buffer[1] << 8 | buffer[0];
	*y = buffer[3] << 8 | buffer[2];
	*z = buffer[5] << 8 | buffer[4];
}

void read_interrupt_source_ADXL345() {
	i2c_start(ADXL345_I2C_Address, TW_WRITE);
	i2c_write(0x30); //Source of interrupts.
	i2c_stop();
	i2c_start(ADXL345_I2C_Address, TW_READ);
	i2c_read(NULL, NACK);
}
