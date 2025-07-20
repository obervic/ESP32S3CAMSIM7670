/*
 * Audio.c
 *
 *  Created on: 12 июн. 2025 г.
 *      Author: Victor
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_check.h"
#include "esp_system.h"
#include "driver/i2s_std.h"
#include "NAU881x.h"
#include "Audio.h"
#include "esp_audio_enc_default.h"
#include "esp_audio_enc_reg.h"
#include "esp_audio_enc.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "MemoryMgt.h"


NAU881x_t hNAU8814;

static const char *TAG = "i2s_NAU8814";
/*
static const char err_reason[][30] = {"input param is invalid",
                                      "operation timeout"
                                     };
                                     
*/

void initSound()
{
	
	//codec powering up 
	NAU881x_Init( &hNAU8814);
	//set output source
	
	//set output parameters 
	
	//set microphone parameters
	
	//set input source
	
	//set input parameters
	
}

void EnableSound()
{
	NAU881x_Set_Mono_Mute(&hNAU8814,1);
}
void DissableSound()
{
	NAU881x_Set_Mono_Mute(&hNAU8814,0);
}

void CaptureSound()
{
	
}

void SoundCaptureEnd()
{
	
}

void PlaySound()
{
	
}

void SoundPlayEnd()
{
	
}

void RecordSound()
{
	
}

void EncodeSound()
{
	
}

void DecodeSound()
{
	
}

void InSreamSound()
{
	
}

void InconeSoundEnd()
{
	
}

void SendAudio()
{
	
}

void OutSreamSound()
{
	
}

void OutGoingSoundEnd()
{
	
}


void SetMICGain(int Gain)
{
	
}

void SetSNDGain(int Gain)
{
	
}