/*
 Copyright (c) 2016 Arduino.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

 //

#include "wiring_analog.h"
#include "wiring_digital.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_saadc.h"
#include "nrf_pwm.h"

static int _readResolution = 10;
static int _writeResolution = 10;
static int _reference=NRF_SAADC_REFERENCE_VDD4;

void analogReadResolution(int res) {
	_readResolution = res;
	
	if(res == 8) nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_8BIT); 
	else if(res == 10) nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_10BIT);   
	else nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_12BIT); 
}

void analogWriteResolution(int res) {
	_writeResolution = res;
}

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to) {
	if (from == to)
		return value;
	if (from > to)
		return value >> (from-to);
	else
		return value << (to-from);
}

void analogReference( eAnalogReference ulMode )
{
  // for nrf52832 only two reference are provided   
  switch(ulMode)
  {
    case AR_DEFAULT:
      default:
      
	  _reference=NRF_SAADC_REFERENCE_VDD4;
	  break;
    
	case AR_INTERNAL:
	  _reference=NRF_SAADC_REFERENCE_INTERNAL;
	  break;

    case AR_EXTERNAL:
	  _reference=NRF_SAADC_REFERENCE_VDD4;
	  break;
  }
  
}


uint32_t analogRead( uint32_t ulPin )
{
  uint32_t valueRead = 0;
  //configure buffer to store result
  nrf_saadc_buffer_init((nrf_saadc_value_t *)&valueRead, 1/*read just one word?*/);
  //configure ADC channel
  nrf_saadc_channel_config_t channel0={NRF_SAADC_RESISTOR_DISABLED,
									   NRF_SAADC_RESISTOR_DISABLED,
									   NRF_SAADC_GAIN1,
									   _reference,
									   NRF_SAADC_ACQTIME_3US,
									   NRF_SAADC_MODE_SINGLE_ENDED,
									   g_APinDescription[ulPin].ulADCChannelNumber,
									   g_APinDescription[ulPin].ulADCChannelNumber //pin negative ignored in single ended mode
									   };
  nrf_saadc_channel_init((uint8_t) 0, &channel0);
  
  //enable ADC
  nrf_saadc_enable();
  
  //start conversion
  nrf_saadc_task_trigger(NRF_SAADC_TASK_START);
  //wait for a complete conversion
  while(nrf_saadc_busy_check());

  //never stop adc, else reconfigure it each time
  //disable channel
  nrf_saadc_disable();
  
  return valueRead;
  
}

//in NRF52 pwm works on all pins
void analogWrite(uint32_t ulPin, uint32_t ulValue) {
	//configure pwm channel	
	nrf_pwm_configure(NRF_PWM0, NRF_PWM_CLK_16MHz, NRF_PWM_MODE_UP, (uint16_t) ulValue);
	//assign pin to pwm channel (only first channel is used)
	uint32_t pin[NRF_PWM_CHANNEL_COUNT]={ulPin, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};
	nrf_pwm_pins_set(NRF_PWM0, pin);
	//enable pwm channel
	nrf_pwm_enable(NRF_PWM0);
}

#ifdef __cplusplus
}
#endif