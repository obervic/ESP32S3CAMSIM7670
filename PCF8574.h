/*
 * PCF8574.h
 *
 *  Created on: 8 июл. 2025 г.
 *      Author: Victor
 */

#ifndef MAIN_PCF8574_H_
#define MAIN_PCF8574_H_
#include "driver/i2c_master.h"
#include "I2C_routines.h"

#define PCF8574_I2C_ADDR_DEFAULT 0x36
#define PCF8574_I2C_NUMBER 0
#define PCF8574_I2C_ADDR PCF8574_I2C_ADDR_DEFAULT+PCF8574_I2C_NUMBER 

#endif /* MAIN_PCF8574_H_ */
