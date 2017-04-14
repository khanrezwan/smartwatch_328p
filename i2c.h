/*
 * i2c.h
 *
 *  Created on: Apr 15, 2017
 *      Author: rezwan
 */

#ifndef I2C_H_
#define I2C_H_

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
extern unsigned char i2c_transmit(unsigned char type);
extern char i2c_start(unsigned int I2C_Address, unsigned char rw_type);
extern char i2c_start(unsigned int I2C_Address, unsigned char rw_type);
extern void i2c_stop(void);
extern char i2c_write(char data);
extern char i2c_read(char *data, char ack_type);
#endif /* I2C_H_ */
