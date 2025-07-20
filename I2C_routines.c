/*
 * I2C_routines.c
 *
 *  Created on: 16 июл. 2025 г.
 *      Author: Victor
 */
#include "driver/i2c_master.h"

esp_err_t i2c_bus_read_byte(i2c_master_dev_handle_t handle,uint8_t ADDR,  uint8_t *data)
{
	
	 return ESP_OK;
	
}

esp_err_t i2c_bus_read_word(i2c_master_dev_handle_t handle,uint8_t ADDR,  uint16_t *data)
{
	return ESP_OK;
}

esp_err_t i2c_bus_write_byte(i2c_master_dev_handle_t handle,uint8_t ADDR,  uint8_t *data)
{
	uint8_t buf[] = {ADDR,*data};
    return i2c_master_transmit(handle,&buf[0],3,-1);
   
}

esp_err_t i2c_bus_write_word(i2c_master_dev_handle_t handle,uint8_t ADDR,  uint16_t *data)
{
	uint8_t buf[] = {ADDR,*data};
    return i2c_master_transmit(handle,&buf[0],3,-1);
   
}

