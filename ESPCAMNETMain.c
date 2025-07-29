/* 
 *  Created on: 3 June 2025 г.
 *      Author: Victor Oberemok
 *  obervic@gmail.com
 *  +380931630905
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "Hardware.h"
#include "MemoryMgt.h"
#include "init.h"
#include "Video.h"
#include "Audio.h"
#include "Communication.h"
#include "Configurator.h"


static const char *TAG = "ESP32S3CAMSIM7670";

// task handles for future use, send messages 
static TaskHandle_t s_configurator_handle;
static TaskHandle_t s_vcapture_handle;
static TaskHandle_t s_vencode_handle;
static TaskHandle_t s_vsave_handle;
static TaskHandle_t s_vsend_handle;
static TaskHandle_t s_vstream_handle;
static TaskHandle_t s_acapture_handle;
static TaskHandle_t s_adecode_handle;
static TaskHandle_t s_aencode_handle;
static TaskHandle_t s_asave_handle;
static TaskHandle_t s_asend_handle;
static TaskHandle_t s_ainstream_handle;
static TaskHandle_t s_aoutnstream_handle;
static TaskHandle_t s_statestream_handle;


//defining semaphores and spinlocks for controlling critical processes
//video streaming enabling
SemaphoreHandle_t semaphore_vstream=NULL;
StaticSemaphore_t xSemaphore_vstream_Buffer;
static bool vstream_enable=false;
portMUX_TYPE vstream_spinlock=portMUX_INITIALIZER_UNLOCKED;
//audio streaming enabling
SemaphoreHandle_t semaphore_astream=NULL;
StaticSemaphore_t xSemaphore_astream_Buffer;
SemaphoreHandle_t semaphore_ainstream=NULL;
StaticSemaphore_t xSemaphore_ainstream_Buffer;
static bool astream_enable=false;
portMUX_TYPE astream_spinlock=portMUX_INITIALIZER_UNLOCKED;
//video capturing enabling
SemaphoreHandle_t semaphore_vcapture=NULL;
StaticSemaphore_t xSemaphore_vcapture_Buffer;
static bool vcapture_enable=false;
portMUX_TYPE vcapture_spinlock=portMUX_INITIALIZER_UNLOCKED;
//audio capture enabling
SemaphoreHandle_t semaphore_acapture=NULL;
StaticSemaphore_t xSemaphore_acapture_Buffer;
static bool acapture_enable=false;
portMUX_TYPE acapture_spinlock=portMUX_INITIALIZER_UNLOCKED;




void init_device()
{
	
	HardwareInit();
	initSound();
	Video_init();
	communication_init();
	//variables init
	
	//initialize semaphores before tasks start
	semaphore_vstream=xSemaphoreCreateBinaryStatic(&xSemaphore_vstream_Buffer);
	semaphore_astream=xSemaphoreCreateBinaryStatic(&xSemaphore_astream_Buffer);
	semaphore_vcapture=xSemaphoreCreateBinaryStatic(&xSemaphore_vcapture_Buffer);
	semaphore_acapture=xSemaphoreCreateBinaryStatic(&xSemaphore_acapture_Buffer);
	semaphore_ainstream=xSemaphoreCreateBinaryStatic(&xSemaphore_astream_Buffer);
}

void StartCapturing()
{
	//enable video capture
	taskENTER_CRITICAL(&vcapture_spinlock);
	 vcapture_enable=true;
	 EnableStreaming();
	 xSemaphoreGive(semaphore_vcapture);
  	taskEXIT_CRITICAL(&vcapture_spinlock); 
	//enable audio
	taskENTER_CRITICAL(&acapture_spinlock);
	 acapture_enable=true;
	 xSemaphoreGive(semaphore_acapture);
	 xSemaphoreGive(semaphore_ainstream);
	taskEXIT_CRITICAL(&acapture_spinlock); 
}

void StopCapturing()
{
	taskENTER_CRITICAL(&vcapture_spinlock);
	 DisableStreaming();
	 xSemaphoreTake(semaphore_vcapture,portMAX_DELAY);
	 vcapture_enable=false;
	taskEXIT_CRITICAL(&vcapture_spinlock); 
	taskENTER_CRITICAL(&acapture_spinlock);
	 acapture_enable=false;
	 xSemaphoreTake(semaphore_acapture,portMAX_DELAY);
     xSemaphoreTake(semaphore_ainstream,portMAX_DELAY);
	taskEXIT_CRITICAL(&acapture_spinlock);
}

void EnableStream()
{
	//enable video 
	taskENTER_CRITICAL(&vstream_spinlock);
	taskENTER_CRITICAL(&astream_spinlock);
	EnableStreaming();
	 vstream_enable=true;
	 xSemaphoreGive(semaphore_vstream);
	//enable audio
	 astream_enable=true;
	 xSemaphoreGive(semaphore_astream);
	taskEXIT_CRITICAL(&astream_spinlock); 
	taskEXIT_CRITICAL(&vstream_spinlock); 
}

void DissableStream()
{
	taskENTER_CRITICAL(&vstream_spinlock);
	taskENTER_CRITICAL(&astream_spinlock);
	 DisableStreaming();
	 vstream_enable=false;
	 astream_enable=false;
	taskEXIT_CRITICAL(&astream_spinlock); 
	taskEXIT_CRITICAL(&vstream_spinlock);	
}


static void eventfd_timer_init(void)
{
   //not released
   // in this function will init timer
}

static void video_capture_task(void *arg)
{

//this task capturing video from camera
    while (true) {
	//suspend if  capture disabled
	  xSemaphoreTake(semaphore_vcapture,portMAX_DELAY);  
	  CaptureFrame();
	  taskENTER_CRITICAL(&vcapture_spinlock);
	    //give semaphore if capture enabled no any does when disable, capturing will suspend on next frame
	    if (vcapture_enable) {
			 xSemaphoreGive(semaphore_vcapture);
			}
			
	  taskEXIT_CRITICAL(&vcapture_spinlock); 
      CaptureEnd();
    }
    vTaskDelete(NULL);
}

static void video_h264_encode_task(void *arg)
{
//this task encoding video frame using H264 codec

    while (true) {
		EncodeFrame();
	};

    vTaskDelete(NULL);
}

static void video_tfsave_task(void *arg)
{
//this task save video file to SD-card

    while (true) {
        SaveFrame();
    }
    vTaskDelete(NULL);
}

static void video_stream_task(void *arg)
{
//this task streaming video to server

    while (true) {
        StreamFrame();     
    }
    vTaskDelete(NULL);
}

static void video_file_send_task(void *arg)
{
// this task saving video file on cloud

    while (true) {
       SaveFrame();
    }
    vTaskDelete(NULL);
}

static void audio_capture_task(void *arg)
{

//captures audio via audio codec


    while (true) {
	  xSemaphoreTake(semaphore_acapture,portMAX_DELAY);  
	  CaptureSound();
	  taskENTER_CRITICAL(&acapture_spinlock);
	    //give semaphore if capture enabled no any does when disable, capturing will suspend on next frame
	    if (acapture_enable) {
			 xSemaphoreGive(semaphore_acapture);
			}
			
	  taskEXIT_CRITICAL(&acapture_spinlock); 
      SoundCaptureEnd();
    }
    vTaskDelete(NULL);
 
}

static void audio_instream_task(void *arg)
{
// receive audio stream from server
   while (true) {
	   xSemaphoreTake(semaphore_ainstream,portMAX_DELAY);
	   InSreamSound();
	   taskENTER_CRITICAL(&acapture_spinlock);
	   if (acapture_enable) {
			 xSemaphoreGive(semaphore_ainstream);
			}
       taskEXIT_CRITICAL(&acapture_spinlock);
       A_ReceiveEnd();
    }
    vTaskDelete(NULL);
           
}

static void audio_outstream_task(void *arg)
{
//streaming capturing audio

    while (true) {
         OutSreamSound();
    }
    vTaskDelete(NULL);
}

static void audio_encoding_task(void *arg)
{

//encode capturing audio
    while (true) {
        EncodeSound();
    }
    vTaskDelete(NULL);
}

static void audio_decoding_task(void *arg)
{
//decodes incoming audio

    while (true) {
         DecodeSound();
    }
    vTaskDelete(NULL);
}

static void audio_save_task(void *arg)
{
//saves audio file to SD-card

    while (true) {
         RecordSound();
    }
    vTaskDelete(NULL);
}

static void audio_send_task(void *arg)
{
//sends audio file to cloud

    while (true) {
          SendAudio();

    }
    vTaskDelete(NULL);
}

static void audio_play_task(void *arg)
{
 //plays income audio via audio codec

    while (true) {
          PlaySound();
    }
    vTaskDelete(NULL);
}

static void configurator_task(void *arg)
{
//this task will start http server via WiFi for configure device

    while (true) {
          ConfiguratorLoop();
    }
    vTaskDelete(NULL);
}

static void State_Stream_Task(void *arg)
{
//this task will start http server via WiFi for configure device

    while (true) {
          
    }
    vTaskDelete(NULL);
}

static void event_loop_task(void *arg)
{
    //not released
   // in this function will processing events queue

    while (true) {
		//comment next line when released
       vTaskSuspend(NULL);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
   // eventfd_timer_init();
    //if (initMemory()!=RESULT_OK) exit(-1);
    init_device();
    //starting all tasks. Tasks will suspend while enabling 
    xTaskCreate(video_capture_task, "video_capture_task", 4 * 1024, NULL, 5, &s_vcapture_handle);
    xTaskCreate(video_h264_encode_task, "video_h264_encode_task", 4 * 1024, NULL, 5, &s_vencode_handle);    
    xTaskCreate(video_tfsave_task, "video_tfsave_task", 4 * 1024, NULL, 5, &s_vsave_handle);    
    xTaskCreate(video_stream_task, "video_stream_task", 4 * 1024, NULL, 5, &s_vstream_handle);
    xTaskCreate(video_file_send_task, "video_file_send_task", 4 * 1024, NULL, 5, &s_vsend_handle);    
    xTaskCreate(audio_capture_task, "audio_capture_task", 4 * 1024, NULL, 5, &s_acapture_handle);    
    xTaskCreate(audio_instream_task, "audio_instream_task", 4 * 1024, NULL, 5, &s_ainstream_handle);
    xTaskCreate(audio_outstream_task, "audio_outstream_task", 4 * 1024, NULL, 5, &s_aoutnstream_handle);    
    xTaskCreate(audio_decoding_task, "audio_decoding_task", 4 * 1024, NULL, 5, &s_adecode_handle); 
    xTaskCreate(audio_encoding_task, "audio_encoding_task", 4 * 1024, NULL, 5, &s_aencode_handle);    
    xTaskCreate(audio_save_task, "audio_save_task", 4 * 1024, NULL, 5, &s_asave_handle);        
    xTaskCreate(audio_send_task, "audio_send_task", 4 * 1024, NULL, 5, &s_asend_handle);       
    xTaskCreate(configurator_task, "worker_task", 4 * 1024, NULL, tskIDLE_PRIORITY, &s_configurator_handle);
    xTaskCreate(event_loop_task, "event_loop_task", 4 * 1024, NULL, 5, NULL);
    xTaskCreate(State_Stream_Task, "State_Stream_Task", 4 * 1024, NULL, 5, &s_statestream_handle );
    StartCapturing();
    
}
