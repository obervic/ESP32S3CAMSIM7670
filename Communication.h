/*
 * Communication.h
 *
 *  Created on: 29 июн. 2025 г.
 *      Author: Victor
 */

#ifndef MAIN_COMMUNICATION_H_
#define MAIN_COMMUNICATION_H_

#include <stdint.h>
#include "Modem.h"

void communication_init();
void StreamVideoBuffer(uint8_t *buff,uint32_t size,uint32_t *sent);
void InSreamSoundBuffer(uint8_t *buff,uint32_t size,uint32_t *receive);
void OutSreamSoundBuffer(uint8_t *buff,uint32_t size,uint32_t *sent);
void SendState(void *buff,uint32_t count,uint32_t *sent);
void SendVideoFile(void *buff,uint32_t count,uint32_t *sent);
void SendAudioFile(void *buff,uint32_t count,uint32_t *sent);
void GetCMD(void *buff,uint32_t *count,uint32_t *receive);
#endif /* MAIN_COMMUNICATION_H_ */
