/*
 * Setting.h
 *
 *  Created on: 3 June 2025 г.
 *      Author: Victor Oberemok
 *  obervic@gmail.com
 *  +380931630905
 *      
 */

#ifndef MAIN_VIDEO_H_
#define MAIN_VIDEO_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_h264_enc_single_sw.h"

esp_h264_err_t Video_init();



void CaptureFrame();
void StreamFrame();
void EncodeFrame();
void SaveFrame();





#endif /* MAIN_VIDEO_H_ */
