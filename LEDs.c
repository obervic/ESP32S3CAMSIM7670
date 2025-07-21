/*
 * LEDs.c
 *
 *  Created on: 21 June 2025 г.
 *      Author: Victor
 *
 *
 * LED1 controlled by SIM7670 and code don't access to control it
 * LED2-LED4 controlled via PCF8574
 *
 * LED5 controlled via NAU8814
 *
 *
 *
 *
 *
 *
 * blinking LED5 signaling "stream on""
 *
 * blinking control via tick_1ms method default periods set in init.h
 *
 *
 */
 
#include "LEDs.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include "I2C_routines.h"
#include "init.h"



struct LEDtiming{
	short period;
	short percnt; 
	short onstate;
	short oncnt;
	bool blink;
	bool state;
};

struct LEDtiming LEDS[4]={{LEDPERIOD,0,LEDON,0,false,false},
                          {LEDPERIOD,0,LEDON,0,false,false},
                          {LEDPERIOD,0,LEDON,0,false,false},
                          {LEDPERIOD,0,LEDON,0,false,false}};
                          
void PCF8574SetLED(uint8_t number)
{
	
} 

void PCF8574OffLED(uint8_t number)
{
	
}     
void NAU8814SetLED()
{
	
} 

void NAU8814OffLED()
{
	
}                        

void LED_on(uint8_t number)
{
	
	LEDS[number].blink=false;
	LEDS[number].state=true;
	if (number<4){PCF8574SetLED(number);}
	  else {NAU8814SetLED();}
	
}

void LED_off(uint8_t number)
{

	LEDS[number].blink=false;
	if (number<4){PCF8574OffLED(number);}
	else {NAU8814OffLED();}
}

void LEDblink(uint8_t number)
{
	//set LED timing values
	LEDS[number]=(struct LEDtiming){LEDPERIOD,0,LEDON,0,true,true};
	//LED on
	if (number<4){PCF8574SetLED(number);}
	else {NAU8814SetLED();}
}

void LEDperiod(uint8_t number,short per,short onstate)
{   //set LED timing values
	LEDS[number] =(struct LEDtiming){per,0,onstate,0,true,true};
	if (number<4){PCF8574SetLED(number);}
	else {NAU8814SetLED();}
}

void tick_1ms()
{
	for (uint8_t i=0;i<4;i++ )
	{
	  if(LEDS[i].blink){
		if (++(LEDS[i].percnt)>=LEDS[i].period){
			if(!LEDS[i].state){
				LEDS[i].state=true;
				LEDS[i].oncnt=0;
				LEDS[i].percnt=0;
				if (i<4){PCF8574SetLED(i);}
				else {NAU8814SetLED();}
			}
		}
		else{
		if (LEDS[i].state)
		  if(++(LEDS[i].oncnt)>=LEDS[i].onstate){
			LEDS[i].state=false;  
			if (i<4){PCF8574OffLED(i);}
			else {NAU8814OffLED();}
		  }
	    }
	  }    
	}
}
