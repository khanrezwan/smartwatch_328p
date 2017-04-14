/*
 * i2c.c
 *
 *  Created on: Apr 15, 2017
 *      Author: rezwan
 */
#include "i2c.h"
#include <compat/twi.h>
#include <math.h>
#include <inttypes.h>
//////////////I2C Functions start////////////////////////////////
void init_I2C() {
	TWBR = twBR;
	TWSR = 0x00; // no prescaler
//	TWCR |= (1<<TWEN);

}

unsigned char i2c_transmit(unsigned char type) {
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

char i2c_start(unsigned int I2C_Address, unsigned char rw_type) {
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

			return -1;
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

				return -1;
			} else {
				//return success
				return 0;
			}
		}

//	  twi_status=i2c_transmit(I2C_START);
		n++;
	}
	return -1;
}

void i2c_stop(void) {

	// Transmit I2C Data
	i2c_transmit(I2C_STOP);
}

char i2c_write(char data) {
	unsigned char twi_status;
//  char r_val = -1;
	// Send the Data to I2C Bus
	TWDR = data;
	// Transmit I2C Data
	twi_status = i2c_transmit(I2C_DATA);
	// Check the TWSR status
	if (twi_status != TW_MT_DATA_ACK) {
		return -1; // failure
	}

	return 0; //success
}

char i2c_read(char *data, char ack_type) {
	unsigned char twi_status;

	if (ack_type) {
		// Read I2C Data and Send Acknowledge
		twi_status = i2c_transmit(I2C_DATA_ACK);
		if (twi_status != TW_MR_DATA_ACK) {
			return -1; //failure
		}
	} else {
		// Read I2C Data and Send No Acknowledge
		twi_status = i2c_transmit(I2C_DATA);
		if (twi_status != TW_MR_DATA_NACK) {
			return -1; //failure
		}
	}
	// Get the Data
	*data = TWDR;

	return 0; //success
}
//////////////I2C Functions end////////////////////////////////
