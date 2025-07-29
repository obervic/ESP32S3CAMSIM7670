/*
 * Storage.c
 *
 *  Created on: 9 июн. 2025 г.
 *      Author: Victor
 */


 
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "ff.h"
#include "sdmmc_cmd.h"
#include "Hardware.h"
#include "Storage.h"
#include "Setting.h"


#define MOUNT_POINT "/sd_card"
FILE *fVideoOut;
FILE *fVideoCFG;
FILE *fAudioOut;
FILE *fVideoDefGFG;
FILE *fVideoSND;
FILE *fAudioSND;
FILE *fCONFIG;



struct SavState{
off_t lastSentALMVideo;
off_t lastSentALMAudio;
bool AlarmSend;
off_t lastSentframe;
off_t lastSaveFrame;
int lastSentFrameOfset;
off_t lastSentAudio;
uint32_t FrameSize;
} TFSaveState;





static const char *TAG = "SD_CARD";
static const char *VideoFN=MOUNT_POINT"/video.dat";
static const char *AudioFN=MOUNT_POINT"/audio.snd";
static const char *FNconfig=MOUNT_POINT"/video.cfg";
static const char *DEVconfig=MOUNT_POINT"/devconfig.cfg";
static const char *DEVstate=MOUNT_POINT"/devstate.cfg";
sdmmc_card_t *card;
const char mount_point[] = MOUNT_POINT;
sdmmc_host_t host = SDSPI_HOST_DEFAULT();
spi_bus_config_t bus_cfg = {
        .mosi_io_num = TF_CMD,
        .miso_io_num = TF_D0,
        .sclk_io_num = TF_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();    
    
esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 10,
        .allocation_unit_size = 16 * 1024
    };
    
    
void SetFrameSize(size_t size)
{
	TFSaveState.FrameSize=size;
}
  
void FileSaveState()
{
	fVideoCFG=fopen(FNconfig, "w");
	fwrite(&TFSaveState,sizeof(TFSaveState), 1, fVideoCFG);
	fclose(fVideoCFG);
}  
    
int InitStorage()
{
	esp_err_t ret;
   ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return ret;
    }
      
    slot_config.gpio_cs = TF_CD;
    slot_config.host_id = host.slot;  
    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
                }
     }
     
     fVideoOut=fopen(VideoFN, "a");
     fAudioOut=fopen(AudioFN, "a");
     fVideoCFG=fopen(FNconfig, "r");
     fCONFIG=fopen(DEVconfig, "r");
 
 //if config exist reading device setting
 if (fCONFIG!=NULL)  {
	  fread(GetDevSetting(), sizeof(tDeviceSetting), 1, fCONFIG);
      fclose(fCONFIG);
  }
   else {
	  LoadSettingDefault();
  }
 //read last state
 
 //size_t res;
 if (fVideoCFG!=NULL){
   fread(&TFSaveState, sizeof(TFSaveState), 1, fVideoCFG);
   fclose(fVideoCFG);
 }
 //set current state    
     
 
  return ESP_OK;	
  
  
}

size_t GetLastSentFrame()
{
	return 0;
}

size_t GetLastSentAudioByte()
{
	return 0;
}

size_t SaveVideoBuffer(uint8_t *Buffer,size_t size)
{
	uint32_t res=0;
	res=fwrite(Buffer, 1, size, fVideoOut);
	return res;
}

size_t SaveAudioBuffer(uint8_t *Buffer,size_t size)
{
	uint32_t res=0;
	res=fwrite(Buffer, 1, size, fAudioOut);
	return res;
}