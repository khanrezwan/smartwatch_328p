/*
 * ds1307.h
 *
 *  Created on: Apr 20, 2017
 *      Author: rezwan
 */

#ifndef DS1307_H_
#define DS1307_H_
#include "i2c.h"
#include <inttypes.h>
#define DS1307_I2C_Address    0x68        // I2C DS1307 Device 7bit

#define DS1307_SECOND 0x00
#define DS1307_MINUTE 0x01
#define DS1307_HOUR 0x02
#define DS1307_WEEKDAY 0x03
#define DS1307_DATE 0x04
#define DS1307_MONTH 0x05
#define DS1307_YEAR 0x06

struct {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t weekday;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} typedef Time_t;


uint8_t BcdToDec(uint8_t value);
uint8_t DecToBcd(uint8_t value);
uint8_t DS1307_Write(Time_t *time);
uint8_t DS1307_Read(Time_t *time);
uint8_t DS1307_read_Reg(uint8_t address);
uint8_t DS1307_write_Reg(uint8_t address, uint8_t data);
uint8_t DS1307_check();
#endif /* DS1307_H_ */
