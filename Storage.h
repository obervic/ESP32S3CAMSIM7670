/*
 * Storage.h
 *
 *  Created on: 29 июн. 2025 г.
 *      Author: Victor
 */

#ifndef MAIN_STORAGE_H_
#define MAIN_STORAGE_H_

#include <stdint.h>
uint32_t SaveVideoBuffer(uint8_t *Buffer,uint32_t size);//returns saved bytes count
void SetFrameSize(uint32_t size);

uint32_t SaveAudioBuffer(uint8_t *Buffer,uint32_t size);//returns saved bytes count
int InitStorage();

#endif /* MAIN_STORAGE_H_ */
