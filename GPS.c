/*
 * GPS.c
 *
 *  Created on: 21 June 2025 г.
 *      Author: Victor
 */
 #include <stdio.h>
 #include <stdint.h>
 #include <stdbool.h>
 #include "GPS.h"
 
#define VALUES_CNT 6
 
static tGPSValue Location[VALUES_CNT];

void InitGPS()
{
	
}

void StopGPS()
{
	
}

pGPSValue GetLocation()
{
	 return &Location[0];	
}
