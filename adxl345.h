/*
 * adxl345.h
 *
 *  Created on: Apr 15, 2017
 *      Author: rezwan
 */

#ifndef ADXL345_H_
#define ADXL345_H_
#include "i2c.h"
#include <compat/twi.h>
#include <stddef.h>

#define ADXL345_I2C_Address    0x53        // I2C ADXL345 Device 7bit
#define ADXL345_Dev_ID 0b11100101
#define ADXL345_TO_READ (6) //read 6 bytes from x_low

void init_ADXL345_double_tap(void);
void init_ADXL345();
char check_ADXL345();

void read_ADXL345(int *x, int* y, int *z);
void read_interrupt_source_ADXL345();

#endif /* ADXL345_H_ */
