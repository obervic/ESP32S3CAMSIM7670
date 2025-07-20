/*
 * Audio.h
 *
 *  Created on: 12 июн. 2025 г.
 *      Author: Victor
 */

#ifndef MAIN_AUDIO_H_
#define MAIN_AUDIO_H_

#include "Hardware.h"

#include <stdint.h>
#include "init.h"
#include "esp_audio_types.h"
#include "esp_audio_simple_dec.h"

#ifdef __cplusplus
extern "C" {
#endif



#define CURR_RECV_BUF_SIZE    ABUFFER_SIZE
#define CURR_SAMPLE_RATE     (16000)
#define CURR_MCLK_MULTIPLE   (384) // If not using 24-bit data width, 256 should be enough
#define CURR_MCLK_FREQ_HZ    (CONFIG_SAMPLE_RATE * CONFIG_MCLK_MULTIPLE)
#define CURR_VOICE_VOLUME    (1)
#define CURR_MIC_GAIN        (1)


#define I2C_NUM         (0)
#define I2S_NUM         (0)

#define I2S_MCK_IO      A_MCLK
#define I2S_BCK_IO      A_BCLK
#define I2S_WS_IO       A_FS
#define I2S_DO_IO       AUDIO_OUT
#define I2S_DI_IO       AUDIO_IN


void initSound();
void EnableSound();
void DissableSound();
void PlaySound();
void SoundPlayEnd();
void CaptureSound();
void SoundCaptureEnd();
void InSreamSound();
void InconeSoundEnd();
void OutSreamSound();
void OutGoingSoundEnd();
void RecordSound();
void SoundRecordEnd();
void SendAudio();
void EncodeSound();
void DecodeSound();
void SetMICGain(int Gain);
void SetSNDGain(int Gain);

/**
 * @brief  Audio codec test module name
 */
#define CODEC_MODULE_NAME "[esp_audio_codec]"

/**
 * @brief  Basic audio information
 */
typedef struct {
    uint8_t     channel;          /*!< Audio channel */
    uint8_t     bits_per_sample;  /*!< Audio bits per sample */
    int         sample_rate;      /*!< Sample rate */
    const void *spec_info;        /*!< Specified information for certain audio codec */
    int         spec_info_size;   /*!< Specified information length */
} audio_info_t;

/*
 * @brief  Codec  configuration
 */
 
/**
 * @brief  Read data callback
 */
typedef int (*codec_read_cb)(uint8_t *data, int size);

/**
 * @brief  Write data callback
 */
typedef int (*codec_write_cb)(uint8_t *data, int size);
 
 
 
typedef struct codec_cfg_t{
    codec_read_cb  read;   /*!< Callback to read codec input data */
    codec_write_cb write;  /*!< Callback to write codec output data */
  } audio_codec_cfg_t;




#ifdef __cplusplus
}
#endif

#endif /* MAIN_AUDIO_H_ */
