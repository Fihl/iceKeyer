#pragma once

#include "cfUtils.h"

/*
    GitHub: https://github.com/Fihl
*/

#define PORTA_128 0
#define PORTB_32 1
#define PORTA_64 2
//  CLOCK      CHANNEL      GAIN 
//  ----------------------------
//   25           A         128 
//   26           B          32 
//   27           A          64 

class HX711cw {
  public:
    HX711cw(uint8_t data, uint8_t clock, uint8_t channel = PORTA_128) {
      _data=data; 
      _clock=clock;
      _channel=channel;
      pinMode(_data, INPUT);
      pinMode(_clock, OUTPUT);
      digitalWrite(_clock, LOW);
      read(true); //Get started
    }

    bool ready() {
      return (!digitalRead(_data));
    }
    
    // https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf
    int read(bool busyWait) {
      while (busyWait && !ready()) ; //maybe busywait
      if (ready()) {
        _reading = 0;
        for (byte i = 0; i < 16; i++) {             //Read MSB, 16 bits
          clockpulse();
          _reading *= 2;
          if (digitalRead(_data)) _reading |= 1; 
        }
        //Total left to read = 8 + _channel + 1 pulses
        for (byte i = 0; i < 8; i++) clockpulse();  //Read/skip LSBs
        for (byte i = 0; i < _channel; i++) clockpulse(); //channel selector
        clockpulse(); //Final pulse
      }
      return _reading;
    }
        
  private:
    int _reading = 0;
    uint8_t _data, _clock, _channel;
    
    void clockpulse() {
      //noInterrupts();
      volatile static int xx=123;
      digitalWrite(_clock, 1);
      xx *= 7; //Waste time >0.1 uSec
      digitalWrite(_clock, 0);
      //interrupts();
    }
    
};

class HX711CWstate {
  public:
    #define LIMIT 12  //6..12 db
    int level6dbA=MINPADDLE;
    int level6dbB=MINPADDLE;
    byte A,Astate;
    byte B,Bstate;

    void poll(bool isA, int inp, int inp2) {
      if (isA) level6dbA = max(level6dbA,inp-LIMIT);
      else     level6dbB = max(level6dbB,inp-LIMIT);

      static bool first=0;
      if (!first & level6dbA>30) {
        first=1;
        SP(inp) SP(inp2) SPln(level6dbA)
      }

      #define DBOUNCE 2 // 2 = cost 25mSec for key activation
      if ( isA & inp > level6dbA) {
        A = DebounceBit(DBOUNCE, Astate, 1); 
      } else A = DebounceBit(DBOUNCE, Astate, 0);
      
      if (!isA & inp > level6dbB) {
        B = DebounceBit(DBOUNCE, Bstate, 1); 
      } else B = DebounceBit(DBOUNCE, Bstate, 0);
    }
};
