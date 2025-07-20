/*
 * Communication.c
 *
 *  Created on: 29 июн. 2025 г.
 *      Author: Victor
 */


#include <stdint.h>
#include "Communication.h"
#include "Storage.h"

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

void InSreamSoundBuffer(uint8_t *buff,uint32_t size,uint32_t *receive)
{
	
}

void OutSreamSoundBuffer(uint8_t *buff,uint32_t size,uint32_t *sent)
{
	
}

void SendState(void *buff,uint32_t count,uint32_t *sent)
{
	
}

void GetCMD(void *buff,uint32_t *count,uint32_t *receive)
{
	
}

void SendVideoFile()
{
	
}

void SendAudioFile()
{
	
}
