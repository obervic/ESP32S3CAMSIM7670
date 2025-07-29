/*
 * MemoryMgt.c
 *
 *  Created on: 9 июн. 2025 г.
 *      Author: Victor Oberemok
 *  obervic@gmail.com
 *  +380931630905
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "esp_h264_alloc.h"
#include "MemoryMgt.h"
#include "init.h"

bool sendstream=false;
bool uncompressedSave=false;//reserved

int8_t Capture_index=0;
int8_t Convert_index=0;
int8_t Save_index=0;
int8_t Send_index=0;


int8_t SND_Capture_index=0;
int8_t SNDIN_Convert_index=0;
int8_t SNDOUT_Convert_index=0;
int8_t SND_Save_index=0;
int8_t SND_Send_index=0;
int8_t SND_Play_index=0;
int8_t SND_Receive_index=0;

static struct tVideoBufferStruct buffers_pool[3];
// = portMUX_INITIALIZER_UNLOCKED;
static struct tAudioBufferStruct income_pool[3];
static struct tAudioBufferStruct outgoing_pool[3];

typedef struct tAudioBufferStruct *pbufferpoll;

void EnableStreaming()
{
  sendstream=true;	
}
void DisableStreaming()
{
  sendstream=false;
}

void SetLastFrame()
{
	buffers_pool[Capture_index].lastFrame=true;
	income_pool[SND_Receive_index].lastBuffer=true;
	outgoing_pool[SND_Capture_index].lastBuffer=true;
}

int initBuffer(int index, esp_h264_resolution_t res)
{
	//initialize video buffers
	int width = res.width;
	int height = res.height;
     
    buffers_pool[index].buffers=(pVideoBuffers) malloc(sizeof(struct tVideoBuffers));
    buffers_pool[index].buffers->fb=(camera_fb_t*)malloc(sizeof(camera_fb_t));
	buffers_pool[index].buffers->fb->format=PIXFORMAT_YUV420;
	buffers_pool[index].buffers->fb->height= height;
	buffers_pool[index].buffers->fb->width= width;
	buffers_pool[index].buffers->fb->len=(width * height + (width * height >> 1));
	buffers_pool[index].buffers->In_buffer = (esp_h264_enc_in_frame_t *)malloc(sizeof(esp_h264_enc_in_frame_t)); 
	buffers_pool[index].buffers->In_buffer->raw_data.buffer=buffers_pool[index].buffers->fb->buf;
	buffers_pool[index].buffers->In_buffer->raw_data.len=buffers_pool[index].buffers->fb->len;
	esp_h264_enc_out_frame_t *Out_buffer= (esp_h264_enc_out_frame_t *)malloc(sizeof(esp_h264_enc_out_frame_t));
	buffers_pool[index].buffers->Out_buffer =Out_buffer;
	uint32_t len;
    Out_buffer->raw_data.buffer=
    esp_h264_aligned_calloc(16, 1, (width * height + (width * height >> 1)) / 10, &len, MALLOC_CAP_INTERNAL);
    buffers_pool[index].buffers->Out_buffer->raw_data.len = len;
    buffers_pool[index].buffers->fb->buf=Out_buffer->raw_data.buffer;
    buffers_pool[index].buffers->fb->len=len;
    buffers_pool[index].Buffer_spinlock=malloc(sizeof(portMUX_TYPE));
    portMUX_INITIALIZE(buffers_pool[index].Buffer_spinlock);
    //create semaphores for buffers set
     buffers_pool[index].semaphore_buffer_captured=xSemaphoreCreateBinary();
     buffers_pool[index].semaphore_buffer_converted=xSemaphoreCreateBinary();
     buffers_pool[index].semaphore_buffer_saved=xSemaphoreCreateBinary();
     buffers_pool[index].semaphore_buffer_send=xSemaphoreCreateBinary();
     buffers_pool[index].semaphore_buffer_sent=xSemaphoreCreateBinary();
	return RESULT_OK;
}

int InitABuffer(pbufferpoll buf,uint8_t rate)
{
	pAudioBuffers buffers=NULL;
	//alocate memory for buffers pointers
	buffers=(pAudioBuffers)malloc(sizeof(struct tAudioBuffers));
	//init audio buffers for buffers set
	buffers->rawbuffer=(uint8_t*)malloc(ABUFFER_SIZE*rate);
	buffers->rbufferSize=ABUFFER_SIZE*rate;
	buffers->rbufferCount=0;
	buffers->encodedbuffer=(uint8_t*)malloc(ABUFFER_SIZE);
	buffers->enbufferSize=ABUFFER_SIZE;
	buffers->enbufferCount=0;
	buf->BufersSet=buffers;
	//create buffer spinlock
	buf->Buffer_spinlock =malloc(sizeof(portMUX_TYPE));
    portMUX_INITIALIZE(buf->Buffer_spinlock);
	//create semaphores for buffers set
	buf->semaphore_buffer_captured=xSemaphoreCreateBinary();
	buf->semaphore_buffer_converted=xSemaphoreCreateBinary();
	buf->semaphore_buffer_saveplay=xSemaphoreCreateBinary();
	buf->semaphore_buffer_sent=xSemaphoreCreateBinary();
	return RESULT_OK;
}



int initMemory(esp_h264_resolution_t res)
{
	for (int i=0;i<3;i++)
	{
		int k=initBuffer(i,res);
		
		if (k!=RESULT_OK) return k;

		k=InitABuffer(&income_pool[i],ABUFFER_RATE);
		
		if (k!=RESULT_OK) return k;
		
		k=InitABuffer(&outgoing_pool[i],1);
		
		if (k!=RESULT_OK) return k;
	}
	return RESULT_OK;
};


void FreeMemory()
{

};

void SetLowResolution()
{
	
}

pVideoBuffers GetBuffer_for_capture()
{
	if (!(buffers_pool[Capture_index].first))//no wait if first use
	 {
       xSemaphoreTake(buffers_pool[Capture_index].semaphore_buffer_saved,portMAX_DELAY); //wait for saving
	 }
	 else{
		 buffers_pool[Capture_index].first=false;
		 buffers_pool[Capture_index].lastFrame=false;
		 };
	 
	 //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
	taskENTER_CRITICAL(buffers_pool[Capture_index].Buffer_spinlock);
	buffers_pool[Capture_index].mode|= ModeBufferCapture; 
	taskEXIT_CRITICAL(buffers_pool[Capture_index].Buffer_spinlock); 
	 
	return buffers_pool[Capture_index].buffers; //return  buffers  data pointer
};

void CaptureEnd()
{    
	//call when frame data has been captured to buffer
	
	 //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
	taskENTER_CRITICAL(buffers_pool[Capture_index].Buffer_spinlock);
	buffers_pool[Capture_index].mode&= ~ModeBufferCapture;  //remove Capture mode from buffer state
	taskEXIT_CRITICAL(buffers_pool[Capture_index].Buffer_spinlock);  
	xSemaphoreGive(buffers_pool[Capture_index].semaphore_buffer_captured); // Buffer can be used for converting and saving uncompressed video.
	//increase buffer number when capturing enable
	if(!buffers_pool[Capture_index].lastFrame)
	 if (++Capture_index==4)Capture_index=0;//change index for next use
};


pVideoBuffers GetBuffer_for_convert(bool *lastFrame)
{ //call when need currents buffers for H264 codec encoding 
	
    xSemaphoreTake(buffers_pool[Convert_index].semaphore_buffer_captured,portMAX_DELAY);//wait while frame was captured
     if (sendstream){//if device in alarm mode and video send to stream via SIM7670 or WiFi
       if (((buffers_pool[Convert_index].mode)&ModeBufferSend)!=0) SetLowResolution(); //when data not sent switch lo low resolution mode
       xSemaphoreTake(buffers_pool[Convert_index].semaphore_buffer_sent,portMAX_DELAY);//wait while frame was sent
    }
    
     //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
 	taskENTER_CRITICAL(buffers_pool[Convert_index].Buffer_spinlock);
	buffers_pool[Convert_index].mode|= ModeBufferConvert; //insert Convert mode to buffer state
	taskEXIT_CRITICAL(buffers_pool[Convert_index].Buffer_spinlock);  
	
 	return buffers_pool[Convert_index].buffers; 
};


 void ConvertEnd()
 {
	 //call when encoding has been finished
	 
	 //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
	taskENTER_CRITICAL(buffers_pool[Convert_index].Buffer_spinlock);
	buffers_pool[Convert_index].mode&= ~ModeBufferConvert;  //remove Convert mode from buffer state
	taskEXIT_CRITICAL(buffers_pool[Convert_index].Buffer_spinlock); 
	if (sendstream) 
	  xSemaphoreGive(buffers_pool[Convert_index].semaphore_buffer_send);
	xSemaphoreGive(buffers_pool[Convert_index].semaphore_buffer_converted);// Out buffer can be used for  saving and send compressed video. 
	if (++Convert_index==4)Convert_index=0;//change index for next use
 };

pVideoBuffers GetBuffer_for_save(bool *lastFrame)
{
	
	//calls when need current buffer for save frame
	 xSemaphoreTake(buffers_pool[Convert_index].semaphore_buffer_converted,portMAX_DELAY);//wait while frame was converted
	taskENTER_CRITICAL(buffers_pool[Convert_index].Buffer_spinlock);
	buffers_pool[Convert_index].mode|= ModeBufferSave; //insert Convert mode to buffer state
	taskEXIT_CRITICAL(buffers_pool[Convert_index].Buffer_spinlock);  
	
 	return buffers_pool[Convert_index].buffers; 
};


void SaveEnd()
{
	//call when frame data has been saved to tf card
	
	 //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
	taskENTER_CRITICAL(buffers_pool[Save_index].Buffer_spinlock);
	buffers_pool[Save_index].mode&= ~ModeBufferSave;  //remove Save mode from buffer state
	taskEXIT_CRITICAL(buffers_pool[Save_index].Buffer_spinlock);  
	
	xSemaphoreGive(buffers_pool[Save_index].semaphore_buffer_saved); // Buffer can be used for converting and saving uncompressed video.
	if (++Save_index==4)Save_index=0;//change index for next use
}

pVideoBuffers GetBuffer_for_send(bool *lastFrame)
{
	xSemaphoreTake(buffers_pool[Convert_index].semaphore_buffer_send,portMAX_DELAY);//wait while frame was converted
	taskENTER_CRITICAL(buffers_pool[Convert_index].Buffer_spinlock);
	buffers_pool[Convert_index].mode|= ModeBufferSend; //insert Convert mode to buffer state
	taskEXIT_CRITICAL(buffers_pool[Convert_index].Buffer_spinlock);  
	
 	return buffers_pool[Convert_index].buffers; 
};


void SendEnd()
{
	//call when frame data has been sent to stream
	taskENTER_CRITICAL(buffers_pool[Send_index].Buffer_spinlock);
	buffers_pool[Send_index].mode&= ~ModeBufferSend;  //remove Send mode from buffer state
	taskEXIT_CRITICAL(buffers_pool[Send_index].Buffer_spinlock);  
	xSemaphoreGive(buffers_pool[Send_index].semaphore_buffer_sent); // Buffer can be used for converting  video.
	if (++Send_index==4)Send_index=0;//change index for next use
}

pAudioBuffers GetABuffer_for_capture()
{
	if (outgoing_pool[SND_Capture_index].first){
		xSemaphoreTake(outgoing_pool[SND_Capture_index].semaphore_buffer_converted,portMAX_DELAY);
	} else{
	    	outgoing_pool[SND_Capture_index].first=false;
	    	outgoing_pool[SND_Capture_index].lastBuffer=false;
		 }
	taskENTER_CRITICAL(outgoing_pool[SND_Capture_index].Buffer_spinlock);
	outgoing_pool[SND_Capture_index].mode|= ModeBufferCapture; 
	taskEXIT_CRITICAL(outgoing_pool[SND_Capture_index].Buffer_spinlock); 
	return outgoing_pool[SND_Capture_index].BufersSet;
};


void A_CaptureEnd()
{
	//call when frame data has been captured to buffer
	
	 //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
	taskENTER_CRITICAL(outgoing_pool[SND_Capture_index].Buffer_spinlock);
	outgoing_pool[SND_Capture_index].mode&= ~ModeBufferCapture;  //remove Capture mode from buffer state
	taskEXIT_CRITICAL(outgoing_pool[SND_Capture_index].Buffer_spinlock);  
	xSemaphoreGive(outgoing_pool[SND_Capture_index].semaphore_buffer_captured); // Buffer can be used for converting and saving uncompressed video.
	//increase buffer number when capturing enable
	if(!outgoing_pool[SND_Capture_index].lastBuffer)//when  not last buffer
	 if (++SND_Capture_index==4)SND_Capture_index=0;//change index for next use
}

pAudioBuffers GetABuffer_for_convert(bool *lastFrame)
{
	xSemaphoreTake(outgoing_pool[SNDOUT_Convert_index].semaphore_buffer_saveplay,portMAX_DELAY);
	if (sendstream){//if device in alarm mode and video send to stream via SIM7670 or WiFi
	   xSemaphoreTake(outgoing_pool[SNDOUT_Convert_index].semaphore_buffer_sent,portMAX_DELAY);}
	 taskENTER_CRITICAL(outgoing_pool[SNDOUT_Convert_index].Buffer_spinlock);
	 outgoing_pool[SNDOUT_Convert_index].mode|= ModeBufferConvert; //insert Convert mode to buffer state 
	 taskEXIT_CRITICAL(outgoing_pool[SNDOUT_Convert_index].Buffer_spinlock); 
	return outgoing_pool[SNDOUT_Convert_index].BufersSet;
}

void A_EncodeEnd()
{
	 //call when encoding has been finished
	 
	 //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
	taskENTER_CRITICAL(outgoing_pool[SNDOUT_Convert_index].Buffer_spinlock);
	outgoing_pool[SNDOUT_Convert_index].mode&= ~ModeBufferConvert;  //remove Convert mode from buffer state
	taskEXIT_CRITICAL(outgoing_pool[SNDOUT_Convert_index].Buffer_spinlock); 
	if (sendstream) 
	  xSemaphoreGive(outgoing_pool[SNDOUT_Convert_index].semaphore_buffer_send);
	xSemaphoreGive(outgoing_pool[SNDOUT_Convert_index].semaphore_buffer_converted);// Out buffer can be used for  saving and send compressed video. 
	if (++SNDOUT_Convert_index==4)SNDOUT_Convert_index=0;//change index for next use
}

pAudioBuffers GetABuffer_for_Receive(bool *lastFrame)
{
	if (income_pool[SND_Receive_index].first){
		xSemaphoreTake(income_pool[SND_Receive_index].semaphore_buffer_converted,portMAX_DELAY);
	} else{
		   income_pool[SND_Receive_index].first=false;
		   income_pool[SND_Receive_index].lastBuffer=false;
		   }
	taskENTER_CRITICAL(income_pool[SND_Receive_index].Buffer_spinlock);
	income_pool[SND_Receive_index].mode|= NodeBufferReceive; 
	taskEXIT_CRITICAL(income_pool[SND_Receive_index].Buffer_spinlock); 
	return income_pool[SND_Receive_index].BufersSet;
}

void A_ReceiveEnd()
{
		
	 //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
	taskENTER_CRITICAL(income_pool[SND_Receive_index].Buffer_spinlock);
	income_pool[SND_Receive_index].mode&= ~NodeBufferReceive;  //remove Capture mode from buffer state
	taskEXIT_CRITICAL(income_pool[SND_Receive_index].Buffer_spinlock);  
	xSemaphoreGive(income_pool[SND_Receive_index].semaphore_buffer_captured); // Buffer can be used for converting and saving uncompressed video.
	//increase buffer number when capturing enable
	if(!income_pool[SND_Receive_index].lastBuffer)//when  not last buffer
	 if (++SND_Receive_index==4)SND_Receive_index=0;//change index for next use
}


pAudioBuffers GetABuffer_for_Rconvert(bool *lastFrame)
{
	xSemaphoreTake(income_pool[SNDIN_Convert_index].semaphore_buffer_saveplay,portMAX_DELAY);
	 taskENTER_CRITICAL(income_pool[SNDIN_Convert_index].Buffer_spinlock);
	 income_pool[SNDIN_Convert_index].mode|= ModeBufferConvert; //insert Convert mode to buffer state 
	 taskEXIT_CRITICAL(income_pool[SNDIN_Convert_index].Buffer_spinlock); 
	return income_pool[SNDIN_Convert_index].BufersSet;
};

void A_DecodeEnd()
{
	 //call when encoding has been finished
	 
	 //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
	taskENTER_CRITICAL(income_pool[SNDIN_Convert_index].Buffer_spinlock);
	income_pool[SNDIN_Convert_index].mode&= ~ModeBufferConvert;  //remove Convert mode from buffer state
	taskEXIT_CRITICAL(income_pool[SNDIN_Convert_index].Buffer_spinlock); 

	xSemaphoreGive(income_pool[SNDIN_Convert_index].semaphore_buffer_converted);// Out buffer can be used for  saving and send compressed video. 
	if (++SNDIN_Convert_index==4)SNDIN_Convert_index=0;//change index for next use
}

pAudioBuffers GetABuffer_for_save(bool *lastFrame)
{
	
	//calls when need current buffer for save frame
	 xSemaphoreTake(outgoing_pool[SND_Save_index].semaphore_buffer_converted,portMAX_DELAY);//wait while frame was converted
	taskENTER_CRITICAL(outgoing_pool[SND_Save_index].Buffer_spinlock);
	outgoing_pool[SND_Save_index].mode|= ModeBufferSave; //insert Convert mode to buffer state
	taskEXIT_CRITICAL(outgoing_pool[SND_Save_index].Buffer_spinlock);  
	
 	return outgoing_pool[SND_Send_index].BufersSet; 
};

void A_SaveEnd()
{
	//call when frame data has been saved to tf card
	
	 //next operator must in critical section  for avoid change result outside
	 //for minimize idle time used spinlock for current buffer
	taskENTER_CRITICAL(outgoing_pool[SND_Save_index].Buffer_spinlock);
	outgoing_pool[SND_Save_index].mode&= ~ModeBufferSave;  //remove Save mode from buffer state
	taskEXIT_CRITICAL(outgoing_pool[SND_Save_index].Buffer_spinlock);  
	
	xSemaphoreGive(outgoing_pool[SND_Save_index].semaphore_buffer_saveplay); // Buffer can be used for converting and saving uncompressed video.
	if (++SND_Save_index==4)SND_Save_index=0;//change index for next use
}


pAudioBuffers GetABuffer_for_send(bool *lastFrame)
{
	xSemaphoreTake(outgoing_pool[SND_Send_index].semaphore_buffer_send,portMAX_DELAY);//wait while frame was converted
	taskENTER_CRITICAL(outgoing_pool[SND_Send_index].Buffer_spinlock);
	outgoing_pool[SND_Send_index].mode|= ModeBufferSend; //insert send mode to buffer state
	taskEXIT_CRITICAL(outgoing_pool[SND_Send_index].Buffer_spinlock);  
	
 	return outgoing_pool[SND_Send_index].BufersSet; 
};

void A_sendEnd()
{
	//call when frame data has been sent to stream
	taskENTER_CRITICAL(buffers_pool[Send_index].Buffer_spinlock);
	buffers_pool[Send_index].mode&= ~ModeBufferSend;  //remove Send mode from buffer state
	taskEXIT_CRITICAL(buffers_pool[Send_index].Buffer_spinlock);  
	xSemaphoreGive(buffers_pool[Send_index].semaphore_buffer_sent); // Buffer can be used for converting  video.
	if (++Send_index==4)Send_index=0;//change index for next use
}

pAudioBuffers GetABuffer_for_play(bool *lastFrame)
{
	xSemaphoreTake(income_pool[SND_Play_index].semaphore_buffer_send,portMAX_DELAY);//wait while frame was converted
	taskENTER_CRITICAL(income_pool[SND_Play_index].Buffer_spinlock);
	income_pool[SND_Play_index].mode|= ModeBufferPlay; //insert Convert mode to buffer state
	taskEXIT_CRITICAL(income_pool[SND_Play_index].Buffer_spinlock);  
	
 	return income_pool[SND_Play_index].BufersSet; 
}

void A_PlayEnd()
{
	//call when frame data has been played 
	taskENTER_CRITICAL(income_pool[SND_Play_index].Buffer_spinlock);
	income_pool[SND_Play_index].mode&= ~ModeBufferPlay;  //remove Send mode from buffer state
	taskEXIT_CRITICAL(income_pool[SND_Play_index].Buffer_spinlock);  
	xSemaphoreGive(income_pool[SND_Play_index].semaphore_buffer_sent); // Buffer can be used for converting  video.
	if (++SND_Play_index==4)SND_Play_index=0;//change index for next use
}