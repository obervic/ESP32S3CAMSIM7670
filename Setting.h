/*
 * Setting.h
 *
 *  Created on: 3 June 2025 г.
 *      Author: Victor Oberemok
 *  obervic@gmail.com
 *  +380931630905
 *    
 *  Active setting of device
 *  if not exist loading default settings
 */

#ifndef MAIN_SETTING_H_
#define MAIN_SETTING_H_

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>
#include "init.h"

struct WiFiSetting{
	
};
 typedef struct WiFiSetting tWiFiSetting;//Device setting structure

struct CODECSetting{
	
};

 typedef struct CODECSetting tCODECSetting;//Device setting structure

struct DeviceSetting{
	tWiFiSetting WiFi;
	tCODECSetting CODEC;
};

typedef struct DeviceSetting tDeviceSetting;//Device setting structure
typedef tDeviceSetting *pDeviceSetting;//pointer to device setting structure

pDeviceSetting GetDevSetting(void);
esp_err_t LoadSettingDefault(void);


#endif /* MAIN_SETTING_H_ */
