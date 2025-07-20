/* 
* ESP32S3CAMSIM7670.h
* hardware definition file
* 
*  Victor Oberemok
*  obervic@gmail.com
*  +380931630905
*
*  
*/


#ifndef  MAIN_HARDWARE_H_
#define  MAIN_HARDWARE_H_

#include <stdint.h>
#include "driver/i2c_master.h"


extern i2c_master_bus_handle_t bus_handle;




// Boot/Alarm button
#define BOOT (0)
#define ALARM_BTN BOOT

//ON/OFF button
#define  ON_OFF_BTN (1)

//Synchro pin of I2C bus
#define SCL_PIN (2)

//Data  pin of I2C bus
#define SDA_PIN (3)

//SD-card cmd pin
#define TF_CMD (4)

//SD-card clock pin
#define TF_CLK (5)

//SD-card data pin
#define TF_D0 (6)

//GPIO7
//CAMERA DATA BIT(INPUT)
#define CAM_Y2 (7)

//GPIO8
//DATA BIT(INPUT)
#define CAM_Y3 (8)

//GPIO9
//DATA BIT(INPUT)
#define CAM_Y4 (9)

//GPIO10
//DATA BIT(INPUT)
#define CAM_Y5 (10)

//GPIO11
//DATA BIT(INPUT)
#define CAM_Y6 (11)

//GPIO12
//DATA BIT(INPUT)
#define CAM_Y7 (12)

//GPIO13
//DATA BIT(INPUT)
#define CAM_Y8 (13)

//GPIO14
//DATA BIT(INPUT)
#define CAM_Y9 (14)

//Board GPIO2
#define IO2 (15)

//Board GPIO3
#define IO3 (16)

//Digital audio data input
#define A_ADCOUT (17)
#define AUDIO_IN A_ADCOUT

//Camera clock
#define CAM_XCLK (18)

//USB data negative
#define USB_DN (19)

//USB data positive
#define USB_DP (20)

//SD-card chip select
#define TF_CD  (21)

 //Camera HREF signal
#define CAM_HREF (38)

//Master clock (output) 
#define A_MCLK (39)

//Bit clock (input/output)*
#define A_BCLK  (40)

//Frame sync (input/output)*
#define A_FS (41)

//Audio digital data out
#define A_DACIN  (42)
#define AUDIO_OUT A_DACIN

//USART0 transmit
#define TXD (43)

//USART0 receive
#define RXD (44)

//A7672G USART data transmit
#define SHD_TX (45)

//Board GPIO1
#define IO11V8 (46)

//VSYNC(INPUT)
#define CAM_VSYNC (47)



//PCLK(INPUT) Pixels clock 
#define CAM_PCLK (48)



void HardwareInit();
void I2C_Write(uint8_t I2C_address, uint8_t reg, uint8_t val);
uint8_t I2C_Read(uint8_t I2C_address, uint8_t reg);

#endif /* MAIN_HARDWARE_H_ */