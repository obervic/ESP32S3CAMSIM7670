/*
 * GPS.h
 *
 *  Created on: 21 June 2025 г.
 *      Author: Victor
 */

#ifndef MAIN_GPS_H_
#define MAIN_GPS_H_

struct GPSValue{
	char name[16];
	float Value;
};

typedef struct GPSValue tGPSValue;
typedef struct GPSValue *pGPSValue; 


void InitGPS();
void StopGPS();
pGPSValue GetLocation();

#endif /* MAIN_GPS_H_ */
