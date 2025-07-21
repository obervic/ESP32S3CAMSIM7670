/*
 * Storage.h
 *
 *  Created on: 29 июн. 2025 г.
 *      Author: Victor
 */

#ifndef MAIN_STORAGE_H_
#define MAIN_STORAGE_H_

#include <stddef.h>
#include <stdint.h>
size_t SaveVideoBuffer(uint8_t *Buffer,size_t size);//returns saved bytes count
void SetFrameSize(size_t size);
size_t GetFrameSize();
size_t SaveAudioBuffer(uint8_t *Buffer,size_t size);//returns saved bytes count

int InitStorage();




#endif /* MAIN_STORAGE_H_ */
