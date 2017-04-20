/*
 * i2c.h
 *
 *  Created on: Apr 15, 2017
 *      Author: rezwan
 */

#ifndef I2C_H_
#define I2C_H_
#include <compat/twi.h>
#include <math.h>
#include <inttypes.h>
//////////////I2C related Defines////////////////
#define F_OSC 16000000 //hz
#define SCL_Freq 100000 //hz
#define twPS 0 //no prescaler
#define twBR ((F_OSC/SCL_Freq)-16)/(2*pow(4,twPS))
#define I2C_START 0
#define I2C_DATA 1
#define I2C_DATA_ACK 2
#define I2C_STOP 3
#define ACK 1
#define NACK 0
#define MAX_TRIES 50

extern void init_I2C(void);
extern uint8_t i2c_transmit(unsigned char type);
extern uint8_t i2c_start(uint8_t I2C_Address, uint8_t rw_type);
//extern uint8_t i2c_start(uint8_t I2C_Address, uint8_t rw_type);
extern void i2c_stop(void);
extern uint8_t i2c_write(uint8_t data);
extern uint8_t i2c_read(uint8_t *data, uint8_t ack_type);
#endif /* I2C_H_ */
