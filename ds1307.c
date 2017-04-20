/*
 * ds1307.c
 *
 *  Created on: Apr 20, 2017
 *      Author: rezwan
 */

#include "ds1307.h"
char* weekdayname[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Saturday" };

uint8_t BcdToDec(uint8_t value) {
	return ((value / 16) * 10 + value % 16);
}
;
uint8_t DecToBcd(uint8_t value) {
	return (value / 10 * 16 + value % 10);
}
;
uint8_t DS1307_Write(Time_t *time) {
	if (i2c_start(DS1307_I2C_Address, TW_WRITE)) {
		if (i2c_write(DS1307_SECOND)) {
			if (i2c_write(DecToBcd(time->minute))) {
				if (i2c_write(DecToBcd(time->hour))) {
					if (i2c_write(DecToBcd(time->weekday))) {
						if (i2c_write(DecToBcd(time->date))) {
							if (i2c_write(DecToBcd(time->month))) {
								if (i2c_write(DecToBcd(time->year))) {
									i2c_stop();
									return 1;
								} else {
									i2c_stop();
									return 0;
								}
							} else {
								i2c_stop();
								return 0;
							}
						} else {
							i2c_stop();
							return 0;
						}
					} else {
						i2c_stop();
						return 0;
					}

				} else {
					i2c_stop();
					return 0;
				}
			} else {
				i2c_stop();
				return 0;
			}

		} else {
			i2c_stop();
			return 0;
		}
	} else {
		i2c_stop();
		return 0;
	}

//	i2c_write(DecToBcd(time->second));
//	i2c_write(DecToBcd(time->minute));
//	i2c_write(DecToBcd(time->hour));
//	i2c_write(DecToBcd(time->weekday));
//	i2c_write(DecToBcd(time->date));
//	i2c_write(DecToBcd(time->month));
//	i2c_write(DecToBcd(time->year));
//	i2c_stop();

	return 0;
}
;
uint8_t DS1307_Read(Time_t *time) {
	uint8_t data = 0;
	i2c_start(DS1307_I2C_Address, TW_WRITE);
	i2c_write(DS1307_SECOND);
	i2c_stop();
	i2c_start(DS1307_I2C_Address, TW_READ);
	if (i2c_read(&data, ACK)) {
		time->second = BcdToDec(data);
		if (i2c_read(&data, ACK)) {
			time->minute = BcdToDec(data);
			if (i2c_read(&data, ACK)) {
				time->hour = BcdToDec(data);
				if (i2c_read(&data, ACK)) {
					time->weekday = BcdToDec(data);
					if (i2c_read(&data, ACK)) {
						time->date = BcdToDec(data);
						if (i2c_read(&data, ACK)) {
							time->month = BcdToDec(data);
							if (i2c_read(&data, NACK)) {
								time->year = BcdToDec(data);
								i2c_stop();
								return 1;
							} else {
								i2c_stop();
								return 0;
							}
						} else {

							i2c_stop();
							return 0;
						}
					} else {
						i2c_stop();
						return 0;

					}
				} else {
					i2c_stop();
					return 0;

				}
			} else {
				i2c_stop();
				return 0;

			}
		} else {
			i2c_stop();
			return 0;

		}
	} else {
		i2c_stop();
		return 0;

	}
//	return 0;

}

uint8_t DS1307_read_Reg(uint8_t address) {
	uint8_t data;
	i2c_start(DS1307_I2C_Address, TW_WRITE);
	i2c_write(address);
	i2c_stop();
	i2c_start(DS1307_I2C_Address, TW_READ);
	i2c_read(&data, NACK);
	return data;
}

uint8_t DS1307_write_Reg(uint8_t address, uint8_t data) {

	i2c_start(DS1307_I2C_Address, TW_WRITE);
	i2c_write(address);
	if (i2c_write(DecToBcd(data))) {
		i2c_stop();
		return 1;
	} else {
		i2c_stop();
		return 0;
	}

}

uint8_t DS1307_check() {
	if (i2c_start(DS1307_I2C_Address, TW_WRITE)) {
		i2c_stop();
		return 1;

	} else {
		i2c_stop();
		return 0;
	}
}
