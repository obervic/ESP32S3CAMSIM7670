/*
 * LEDs.h
 *
 *  Created on: 21 June 2025 г.
 *      Author: Victor
 */

#ifndef MAIN_LEDS_H_
#define MAIN_LEDS_H_


#include <stdint.h>
void LED_on(uint8_t number);
void LED_off(uint8_t number);
void LEDblink(uint8_t number);
void LEDperiod(uint8_t number,short per,short onstate);
void tick_1ms();
void init_LEDs();

#endif /* MAIN_LEDS_H_ */
