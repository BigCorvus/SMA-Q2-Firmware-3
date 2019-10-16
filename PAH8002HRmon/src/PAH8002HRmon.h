/*
Arduino Library for the PixArt PAH8002 HR sensor 
based on precompiled library for nrf52

*/
#ifndef PAH8002HRmon_h
#define PAH8002HRmon_h

#include "Arduino.h"
#include "pah8series_api_c.h"
#include "pah8series_data_c.h"


class PAH8002HRmon
{
  public:
    PAH8002HRmon();
    void begin (void);
	uint32_t getVersion(void);
	
	
 
};

#endif