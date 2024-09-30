#pragma once

#include "cfUtils.h"
#include "HX711CW.h"

// Read the current state of the paddle
volatile bool dihHX711;
volatile bool dahHX711;

HX711cw paddle(HX711_dat, HX711_clk, 0);  //data, clk
DelayLine dl;
HX711CWstate cw;

//Read DIH-DAH, debounced
//Return '0'=none, '1'=dih, '2'=dah, '3'=both (not on iceKeyer)
char rdKeys() {
  return '0' + dihHX711 + dahHX711*2;
}

COROUTINE(loopHX711) {
  COROUTINE_BEGIN();
  for (static byte n=0; n<5; n++) { //"static" because of protothread
    while ( abs(dl.removeDC(paddle.read(true))>2) ) COROUTINE_DELAY(1); //Get to zero
    COROUTINE_DELAY(100);
  }
  while (1) {
    COROUTINE_DELAY(1);
    if (paddle.ready()) { //80 times a second = 12.5m Sec
      int inp2 = paddle.read(true) - dl.levelDC; //Remove DC asap (static level since setup);
      bool pola = inp2>0;
      inp2 = abs(inp2);
      byte inp = log10x20(inp2);   //Use S scale (6db)
    
      //if (inp>60) { SP(inp) SP(inp2) SPln("-60 error") continue; }
      if (inp2>65000) { SP(inp) SPln("-65000 error") continue; }
      if (inp2>15000) { SP(inp) SPln("-15000 error") continue; }
      
      cw.poll(pola, inp, inp2);
      
      if (HX711Swapped) {dihHX711 = cw.B; dahHX711 = cw.A;}
      else              {dihHX711 = cw.A; dahHX711 = cw.B;}
    }
  }
  COROUTINE_END();
}
