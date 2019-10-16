/*
Arduino Library for the PixArt PAH8002 HR sensor 
based on precompiled library for nrf52

*/


#include "PAH8002HRmon.h"

extern "C" {float __hardfp_sqrtf(float f) {return sqrtf(f);} }

PAH8002HRmon::PAH8002HRmon()
{
  

}

void PAH8002HRmon::begin(void){
	
	
	
}

uint32_t PAH8002HRmon::getVersion(void)
{
return pah8series_version();
}