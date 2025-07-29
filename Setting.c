#include "Setting.h"
#include "esp_err.h"

tDeviceSetting ActiveSetting;

pDeviceSetting GetDevSetting(void)
{
	return &ActiveSetting;
}

esp_err_t LoadSettingDefault(void)
{
	return ESP_OK;
}