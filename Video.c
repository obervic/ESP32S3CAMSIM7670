/*
 * Video.c
 *
 *  Created on: 3 June 2025 г.
 *      Author: Victor Oberemok
 *  obervic@gmail.com
 *  +380931630905
 */
 


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor.h"
#include <mbedtls/base64.h>
#include <stdint.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "MemoryMgt.h"
#include "Video.h"

#include "esp_h264_enc_single_sw.h"
#include "esp_h264_enc_single.h"
#include "Events.h"
#include "Hardware.h"
#include "Communication.h"
#include "Storage.h"

#include "esp_camera.h"

static const char *TAG = "ESP32S3CAMSIM7670_VIDEO";

esp_h264_resolution_t res={640,480};
esp_h264_enc_rc_t rc;
esp_h264_err_t ret = ESP_H264_ERR_FAIL;
esp_h264_enc_handle_t enc = NULL;

static camera_config_t camera_config = {
    .pin_pwdn = -1,//use these pins external from library
    .pin_reset = -1,//
    .pin_xclk = CAM_XCLK,
    .pin_sccb_sda = -1,//use existing I2C port must defined external from library
    .pin_sccb_scl = -1,//
    .sccb_i2c_port = 0,
    .pin_d7 = CAM_Y9,
    .pin_d6 = CAM_Y8,
    .pin_d5 = CAM_Y7,
    .pin_d4 = CAM_Y6,
    .pin_d3 = CAM_Y5,
    .pin_d2 = CAM_Y4,
    .pin_d1 = CAM_Y3,
    .pin_d0 = CAM_Y2,
    .pin_vsync = CAM_VSYNC,
    .pin_href = CAM_HREF,
    .pin_pclk = CAM_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_YUV420, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_SVGA,    //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 0, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};



esp_err_t init_camera()
{
	//uint32_t xclk_freq_hz, pixformat_t pixel_format, framesize_t frame_size, uint8_t fb_count, int sccb_sda_gpio_num, int sccb_port
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }



    return ESP_OK;
 
}

esp_h264_err_t Video_init()
{
	init_camera();
	
	initMemory(res);
	return ESP_H264_ERR_OK;
};








void CaptureFrame()
{
	
	pVideoBuffers Buffers=GetBuffer_for_capture();
	esp_camera_fb_return(Buffers->fb);
	
}


void LastStreamed()
{
	
}

void LastConverted()
{
	
}

void LastSaved()
{
	
}

void StreamFrame()
{
	bool IsLast;
	pVideoBuffers Buffers=GetBuffer_for_send(&IsLast);
	uint32_t cnt=0;
    StreamVideoBuffer(Buffers->Out_buffer->raw_data.buffer,Buffers->Out_buffer->raw_data.len,&cnt);    
	if(IsLast&&sendstream) LastStreamed();
	SendEnd();
}

void EncodeFrame()
{
	bool IsLast;
	pVideoBuffers Buffers=GetBuffer_for_convert(&IsLast);
    ret = esp_h264_enc_process(enc, Buffers->In_buffer, Buffers->Out_buffer);
    if (ret != ESP_H264_ERR_OK) {
            printf("Can't encode frame. line %d \n", __LINE__);
          //  goto _exit_;
        }
	if(IsLast) LastConverted();
	ConvertEnd();
}


void SaveFrame()
{
	bool IsLast;
	pVideoBuffers Buffers=GetBuffer_for_send(&IsLast);    
    SaveVideoBuffer(Buffers->Out_buffer->raw_data.buffer,Buffers->Out_buffer->raw_data.len);    
	if(IsLast) LastSaved();
	SendEnd();
}