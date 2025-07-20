/*
 * Hardware.c
 *
 *  Created on: 29 июн. 2025 г.
 *      Author: Victor
 */
#include "driver/i2c_master.h"
#include "Hardware.h"

    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 0,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .flags.enable_internal_pullup = false,  // no pull-up
    };
    
    
    i2c_master_bus_handle_t bus_handle;


void HardwareInit()
{
	i2c_new_master_bus(&i2c_mst_config_1, &bus_handle);
	
}

void I2C_Write(uint8_t I2C_address, uint8_t reg, uint8_t val)
{
	
}

uint8_t I2C_Read(uint8_t I2C_address, uint8_t reg)
{
	return reg;
}


