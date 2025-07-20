/*
 * MemoryMgt.h
 *
 *  Created on: 9 June 2025 г.
 *      Author: Victor Oberemok
 *  obervic@gmail.com
 *  +380931630905
 */

#ifndef MAIN_MEMORYMGT_H_
#define MAIN_MEMORYMGT_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_h264_enc_single_sw.h"
#include "esp_h264_enc_single.h"
#include "esp_camera.h"

#define RESULT_OK 0

extern bool sendstream;
// defining video buffers structure type
struct tVideoBuffers{
	 esp_h264_enc_in_frame_t 	*In_buffer;
	 esp_h264_enc_out_frame_t 	*Out_buffer;
	 camera_fb_t *fb;
};

typedef struct tVideoBuffers *pVideoBuffers;

#define ModeBufferNone 		0
#define ModeBufferCapture 	1
#define NodeBufferReceive   1
#define ModeBufferConvert 	2
#define ModeBufferSave		4
#define ModeBufferSend		8
#define ModeBufferPlay      16

struct tVideoBufferStruct{
	 bool first;
	 bool lastFrame;
	 uint8_t mode;
	 portMUX_TYPE *Buffer_spinlock;
     pVideoBuffers buffers;
     
     
     
     SemaphoreHandle_t semaphore_buffer_send;
	 SemaphoreHandle_t semaphore_buffer_sent;
//	 SemaphoreHandle_t semaphore_buffer_save;
	 SemaphoreHandle_t semaphore_buffer_saved;
//	 SemaphoreHandle_t semaphore_buffer_capture;
	 SemaphoreHandle_t semaphore_buffer_captured;
//	 SemaphoreHandle_t semaphore_buffer_convert;
	 SemaphoreHandle_t semaphore_buffer_converted;

};



struct tAudioBuffers{
	uint8_t *rawbuffer;
	uint32_t rbufferSize;
	uint32_t rbufferCount;
	uint8_t *encodedbuffer;
	uint32_t enbufferSize;
	uint32_t enbufferCount;
};

typedef struct tAudioBuffers *pAudioBuffers;

struct tAudioBufferStruct{
	bool first;
	bool lastBuffer;
	uint8_t mode;
	pAudioBuffers BufersSet;
	portMUX_TYPE *Buffer_spinlock;
    SemaphoreHandle_t semaphore_buffer_sent;
    SemaphoreHandle_t semaphore_buffer_send;
	SemaphoreHandle_t semaphore_buffer_captured;
	SemaphoreHandle_t semaphore_buffer_converted;
	SemaphoreHandle_t semaphore_buffer_saveplay;
};


int initMemory(esp_h264_resolution_t res);
void FreeMemory();

void EnableStreaming();
void DisableStreaming();
void SetLastFrame();


pVideoBuffers GetBuffer_for_capture();
void CaptureEnd();
pVideoBuffers GetBuffer_for_convert(bool *lastFrame);
void ConvertEnd();
pVideoBuffers GetBuffer_for_save(bool *lastFrame);
void SaveEnd();
pVideoBuffers GetBuffer_for_send(bool *lastFrame);
void SendEnd();



pAudioBuffers GetABuffer_for_capture();
void A_CaptureEnd();
pAudioBuffers GetABuffer_for_convert(bool *lastFrame);
void A_EncodeEnd();
pAudioBuffers GetABuffer_for_Receive(bool *lastFrame);
void A_ReceiveEnd();
pAudioBuffers GetABuffer_for_Rconvert(bool *lastFrame);
void A_DecodeEnd();
pAudioBuffers GetABuffer_for_save(bool *lastFrame);
void A_SaveEnd();
pAudioBuffers GetABuffer_for_send(bool *lastFrame);
void A_sendEnd();
pAudioBuffers GetABuffer_for_play(bool *lastFrame);
void A_PlayEnd();



#endif /* MAIN_MEMORYMGT_H_ */
