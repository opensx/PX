/*
 * PX.h
 *
 *  Created on: 08.10.2015
 *  Changed on:  
 *  Version:    1.0
 *  Copyright:  Michael Blank, Reinhard Thamm
 *
 *  This faster version uses more direct bitwise port and pin operations
 * 
 *  decoding PX selectrix signal from tracks
 *  
 *  interface hardware needed ! see www.oscale.net/SX

 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


 */

#ifndef PX_H_
#define PX_H_

#include <inttypes.h>





// defines for state machine
#define SYNC  0
#define ADDR  1
#define DATA  2

#define MAX_DATACOUNT    7    // 7 dataframes in 1 SYNC Channel
#define MAX_DATABITCOUNT 12   // 12 bits in 1 frame


#if defined(__AVR_ATmega328P__)

#define N_CHAN  112       // index for declaring "_sx[]" data array
#define SX1         2    // default input of "positive" signal
#define SX2         3    // default input of "negative" signal

#elif defined(__AVR_ATtiny4313__)  
#define N_CHAN  1        // much less memory than Atmega328  
#define SX1         4    // default input of "positive" signal for ATtiny4313
#define SX2         5    // default input of "negative" signal

#else

#error "Processor not supported"
#endif 



class PX {
public:
	PX();
	void init(void);
        void init(uint8_t, uint8_t);
    	
	void isr(void);
        void isr2(void);
#if defined(__AVR_ATmega328P__)
	uint8_t get(uint8_t);
#else
        uint8_t get(void);   // single channel
        void setChannel(uint8_t);     // set the active channel for ATtiny4313
#endif

private:
	void switchAdr(void);
	void switchData(void);

	uint8_t _toggle;
	uint8_t _zeroCount;
	uint8_t _adrCount;
	uint8_t _state;
	uint8_t _dataBitCount;    // bit counting
	uint8_t _dataFrameCount;  // frame counting

	uint8_t _data;    // 1 data uint8_t
	uint8_t _baseAdr;   // base address
	uint8_t _triggerAdr;  // SX address to trigger scope
        uint8_t _scopeFlag;   // generate scope trigger signal if != 0

	uint8_t _bit;
        uint8_t _inbit;
        uint8_t _lastInbit;
	volatile uint8_t _sx[N_CHAN];   // to store the SX data
        volatile uint8_t _activeChannel;    // in case we read only a single channel

	uint8_t _channel;   // channel from 0 to 15, B3..B0 in sync data
	// 0  0  0  1  X   1  B3  B2  1  B1  B0  1 == sync frame of 12 bits
	
        
  
	/* SX Timing
	 1   Bit             50 us
	 1   Kanal          600 us (= 12 Bit)
	 1   Grundrahmen    ca. 4,8 ms
	 1   Gesamtrahmen   ca.  80 ms (= 16 Grundrahmen)  */
};

#endif /* PX_H_ */
