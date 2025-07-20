/*
 * Audio.c
 *
 *  Created on: 12 июн. 2025 г.
 *      Author: Victor
 */

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "Storage.h"
#include "hal/i2s_types.h"
#include "init.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_check.h"
#include "esp_system.h"
#include "driver/i2s_std.h"
#include "NAU881x.h"
#include "Audio.h"
#include "esp_audio_enc_default.h"
#include "esp_audio_enc_reg.h"
#include "esp_audio_enc.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "MemoryMgt.h"
#include "Communication.h"


NAU881x_t hNAU8814;

static const char *TAG = "i2s_NAU8814";
/*
static const char err_reason[][30] = {"input param is invalid",
                                      "operation timeout"
                                     };
                                     
*/
static i2s_chan_handle_t tx_handle = NULL;
static i2s_chan_handle_t rx_handle = NULL;

bool I2S_buffer_empty=true;


void initSound()
{
	
	//codec powering up 
	NAU881x_Init( &hNAU8814);
	//set output 
	
	//set output parameters 
	
	//set microphone parameters
	
	//set input source
	
	//set input parameters
	
	//set capture mode
	
	//init I2S
	    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true; // Auto clear the legacy data in the DMA buffer
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(CURR_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_MCK_IO,
            .bclk = I2S_BCK_IO,
            .ws = I2S_WS_IO,
            .dout = I2S_DO_IO,
            .din = I2S_DI_IO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    std_cfg.clk_cfg.mclk_multiple = CURR_MCLK_MULTIPLE;

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));
    
   //Audio compression codec init
   
   
}

void EnableSound()
{
	NAU881x_Set_Mono_Mute(&hNAU8814,1);
}
void DissableSound()
{
	NAU881x_Set_Mono_Mute(&hNAU8814,0);
}

void CaptureSound()
{
   	
}

void SoundCaptureEnd()
{
	
}

void PlaySound()
{
	bool lastframe=false;
    size_t bytes_write = 0;
    size_t bytes;
    
    uint8_t *data_ptr=NULL;   
    pAudioBuffers buffer=GetABuffer_for_play(&lastframe);
	data_ptr=buffer->rawbuffer;
	bytes=buffer->rbufferCount;
	size_t bt=bytes;
	if (I2S_buffer_empty){
		I2S_buffer_empty=false;
		ESP_ERROR_CHECK(i2s_channel_disable(tx_handle));
        ESP_ERROR_CHECK(i2s_channel_preload_data(tx_handle, buffer->rawbuffer, bytes, &bytes_write));
        size_t bt=ABUFFER_SIZE-bytes_write;
        if (bt>0) data_ptr += bytes_write;  // Move forward the data pointer
        ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
	   }
	 while (bt>0){
		 
	 }  
	 A_PlayEnd();
	   
	 
}

void SoundPlayEnd()
{
	
}

void RecordSound()
{
	bool lastframe=false;
	pAudioBuffers buffer=GetABuffer_for_save(&lastframe);
	SaveAudioBuffer(buffer->encodedbuffer,buffer->enbufferCount);
	A_SaveEnd();
}

void EncodeSound()
{
	bool lastframe=false;
	uint32_t cnt;
	pAudioBuffers buffer=GetABuffer_for_convert(&lastframe);
}

void DecodeSound()
{
	bool lastframe=false;
	uint32_t cnt;
	pAudioBuffers buffer=GetABuffer_for_Rconvert(&lastframe);
	
}

void InSreamSound()
{
	bool lastframe=false;
	uint32_t cnt;
	pAudioBuffers buffer=GetABuffer_for_Receive(&lastframe);
	InSreamSoundBuffer(buffer->encodedbuffer,buffer->enbufferSize,&cnt);
	buffer->enbufferCount=cnt;
	A_ReceiveEnd();
}

void InconeSoundEnd()
{
	
}



void OutSreamSound()
{
	bool lastframe=false;
	uint32_t cnt;
	pAudioBuffers buffer=GetBuffer_for_send(&lastframe);
	
}

void OutGoingSoundEnd()
{
	
}


void SetMICGain(int Gain)
{
	
}

void SetSNDGain(int Gain)
{
	
}