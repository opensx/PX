/*
 * SX_WDEC_4313.ino
 *
 *  Created on: 08.11.2018
 *  
 *  Revision: 1.1
 *  
 *  Author: Michael Blank
 *  
 *  Example program for the Selectrix(TM) Library
 *  to decode the track signal 
 *  a turnout is thrown or closed, depending on the bit in a
 *  specific SX channel
 *  
 *  needs special hardware, see http://opensx.net/decoder4313
 *  
 */

// this is the Selectrix track signal decode library
// inputs pins INT0, INT1 are defined in this library
#include "PX.h"   

#define THROWN 11   // pin for throwing turnout
#define CLOSED 10   // pin for closing turnout

//#define DEBUG    // serial output on TXD line

#define FIRST_ADDRESS    (84)   // address range start - end = start+7


PX sx;                // library

uint8_t myChannel;
uint8_t myBit;


uint8_t oldSxData = 0;
uint8_t startUp = 1;
long startUpDelay;

void sxisr(void) {
    // if you want to understand this, see:
    // http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1239522239   
    // sx.isr() is a member function (i.e. it's part of the PX class)
    // but attachInterrupt() expects a non-member function (i.e. a 
    // function which is static and/or not part of a class at all).
    sx.isr();
} 

void sxisr2(void) {
    sx.isr2();
} 

void setup() {

    // outputs for turnout
    pinMode(THROWN,OUTPUT);
    pinMode(CLOSED,OUTPUT);
    digitalWrite(THROWN, LOW);
    digitalWrite(CLOSED, LOW);

    // inputs for channel setting 
    pinMode(12,INPUT_PULLUP);
    pinMode(6,INPUT_PULLUP);
    pinMode(7,INPUT_PULLUP);

    // inputs for bit setting    
    pinMode(2,INPUT_PULLUP);
    pinMode(3,INPUT_PULLUP);
    pinMode(13,INPUT_PULLUP);

#ifdef DEBUG
    Serial.begin(9600);      // open the serial port
#endif
    // initialize interrupt routine and SX input pins
    sx.init();  

    // read dip switches for channel and bit setting and set active sx-channel
    updateChannel();   

    // calculate some startup delay, different for each decoder (min. 2.4 seconds)
    startUpDelay = (myChannel - FIRST_ADDRESS + 3) * 800 + myBit * 100;
    
    // RISING slope on INT0/INT1 trigger the interrupt routine sxisr (see above)
    attachInterrupt(digitalPinToInterrupt(SX1), sxisr, RISING);    //SX1/SX2 are defined in PX.h
    attachInterrupt(digitalPinToInterrupt(SX2), sxisr2, RISING);
} 

uint8_t readBit() {
    uint8_t b = 0;
    if (digitalRead(2)) b += 1;
    if (digitalRead(3)) b += 2;
    if (digitalRead(13)) b += 4;
    return b;
}

uint8_t readAddress() {
    uint8_t a = FIRST_ADDRESS;
    if (digitalRead(12)) a += 1;
    if (digitalRead(6))  a += 2;
    if (digitalRead(7))  a += 4;
    return a;
}

void updateChannel() {
  myChannel = readAddress();
  myBit = readBit();
  sx.setChannel(myChannel);
}

void loop() {
    byte d = 0;
    updateChannel();
    if (millis() < startUpDelay ) {
      // do nothing (do not set all turnouts at the same time at start)
    } else {
      d = sx.get();
      if ( (bitRead(d,myBit) != bitRead(oldSxData,myBit))
           || startUp 
         ){
        oldSxData = d;
        startUp = 0;
        if (bitRead(d,myBit)) {
          // activate THROWN line for 100msec
          digitalWrite(THROWN, HIGH);
          digitalWrite(CLOSED, LOW);
          delay(100);
          digitalWrite(THROWN, LOW);
        } else {
          // activate CLOSED line for 100msec
          digitalWrite(THROWN, LOW);
          digitalWrite(CLOSED, HIGH);
          delay(100);
          digitalWrite(CLOSED, LOW);
        }
      }
    }
#ifdef DEBUG
    Serial.print(myChannel);
    Serial.print(' ');
    Serial.print(myBit);
    Serial.print(' ');
    Serial.print(d);
#endif
    delay(200);

}
