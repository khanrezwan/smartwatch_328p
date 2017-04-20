/*
 * i2c.c
 *
 *  Created on: Apr 15, 2017
 *      Author: rezwan
 */
#include "i2c.h"

//////////////I2C Functions start////////////////////////////////
void init_I2C() {
	TWBR = twBR;
	TWSR = 0x00; // no prescaler
//	TWCR |= (1<<TWEN);

}

uint8_t i2c_transmit(uint8_t type) {
	switch (type) {
	case I2C_START:    // Send Start Condition
		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
		break;
	case I2C_DATA:     // Send Data with No-Acknowledge
		TWCR = (1 << TWINT) | (1 << TWEN);
		break;
	case I2C_DATA_ACK: // Send Data with Acknowledge
		TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
		break;
	case I2C_STOP:     // Send Stop Condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		return 0;
	}
	// Wait for TWINT flag set on Register TWCR
	while (!(TWCR & (1 << TWINT)))
		;

	// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
	return (TWSR & 0xF8);

}

uint8_t i2c_start(uint8_t I2C_Address, uint8_t rw_type) {
	unsigned char n = 0;
	unsigned char twi_status;
//  char r_val = -1;
	twi_status = i2c_transmit(I2C_START);
	while (n < MAX_TRIES) {
		if (twi_status == TW_MT_ARB_LOST) {
			//retry
			twi_status = i2c_transmit(I2C_START);
		} else if ((twi_status != TW_START) && (twi_status != TW_REP_START)) {
			//quit failure

			return 0;
		} else {
			// Send slave address (SLA_W)
			TWDR = (I2C_Address<<1) | rw_type;
			// Transmit I2C Data
			twi_status = i2c_transmit(I2C_DATA);
			if ((twi_status == TW_MT_SLA_NACK)
					|| (twi_status == TW_MT_ARB_LOST)) {
				//retry
				twi_status = i2c_transmit(I2C_START);
			} else if (twi_status != TW_MT_SLA_ACK) {
				//quit failure

				return 0;
			} else {
				//return success
				return 1;
			}
		}

//	  twi_status=i2c_transmit(I2C_START);
		n++;
	}
	return 0;
}

void i2c_stop(void) {

	// Transmit I2C Data
	i2c_transmit(I2C_STOP);
}

uint8_t i2c_write(uint8_t data) {
	unsigned char twi_status;
//  char r_val = -1;
	// Send the Data to I2C Bus
	TWDR = data;
	// Transmit I2C Data
	twi_status = i2c_transmit(I2C_DATA);
	// Check the TWSR status
	if (twi_status != TW_MT_DATA_ACK) {
		return 0; // failure
	}

	return 1; //success
}

uint8_t i2c_read(uint8_t *data, uint8_t ack_type) {
	unsigned char twi_status;

	if (ack_type) {
		// Read I2C Data and Send Acknowledge
		twi_status = i2c_transmit(I2C_DATA_ACK);
		if (twi_status != TW_MR_DATA_ACK) {
			return 0; //failure
		}
	} else {
		// Read I2C Data and Send No Acknowledge
		twi_status = i2c_transmit(I2C_DATA);
		if (twi_status != TW_MR_DATA_NACK) {
			return 0; //failure
		}
	}
	// Get the Data
	*data = TWDR;

	return 1; //success
}
//////////////I2C Functions end////////////////////////////////
