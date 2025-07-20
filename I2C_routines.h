/*
 * I2C_routines.h
 *
 *  Created on: 10 июл. 2025 г.
 *      Author: Victor
 */

#ifndef MAIN_I2C_ROUTINES_H_
#define MAIN_I2C_ROUTINES_H_

#include "driver/i2c_master.h"

esp_err_t i2c_bus_read_byte(i2c_master_dev_handle_t handle,uint8_t ADDR,  uint8_t *data);
esp_err_t i2c_bus_read_word(i2c_master_dev_handle_t handle,uint8_t ADDR,  uint16_t *data);
esp_err_t i2c_bus_write_byte(i2c_master_dev_handle_t handle,uint8_t ADDR,  uint8_t *data);
esp_err_t i2c_bus_write_word(i2c_master_dev_handle_t handle,uint8_t ADDR,  uint16_t *data);




#endif /* MAIN_I2C_ROUTINES_H_ */
