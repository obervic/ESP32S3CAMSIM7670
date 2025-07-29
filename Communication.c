/*
 * Communication.c
 *
 *  Created on: 29 июн. 2025 г.
 *      Author: Victor
 */


#include <stdint.h>
#include <stdbool.h>
#include "Communication.h"
#include "MemoryMgt.h"
#include "Storage.h"
#include "ESP32-RTSPTransport.h"

bool Vfilestreamready=false;
bool Afilestreamready=false;
bool Vstreamready=false;
bool Astreamready=false;
bool Aincomestreamready=false;

void communication_init()
{
	StartModem();
}

void StreamVideoBuffer(uint8_t *buff,uint32_t size,uint32_t *sent)
{
	
}

void StreamAudioBuffer(uint8_t *buff,uint32_t size,uint32_t *sent)
{
	
}

void InStreamSoundBuffer(uint8_t *buff,uint32_t size,uint32_t *receive)
{
	if(!Aincomestreamready){
		*receive = 0;
		return;
	}
	
	
}

void OutSreamSoundBuffer(uint8_t *buff,uint32_t size,uint32_t *sent)
{
	if(!Astreamready){
		*sent = 0;
		return;
	}
}

void SendState(void *buff,uint32_t count,uint32_t *sent)
{
	
}

void GetCMD(void *buff,uint32_t *count,uint32_t *receive)
{
	//perform this operation every CMD_REFRESH_TIME seconds
	//GET querry to http server
	//server returns commands 
	
}

void SendVideoFile(void *buff,uint32_t count,uint32_t *sent)
{
	
	
	
}

void SendAudioFile(void *buff,uint32_t count,uint32_t *sent)
{
	
	
}
