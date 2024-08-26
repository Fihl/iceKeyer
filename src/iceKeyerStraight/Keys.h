#pragma once

#include "cfUtils.h"
#include "HX711CW.h"
#include "Globals.h"

// Read the current state of the paddle

volatile bool dihPaddle;
volatile bool dahPaddle;

volatile bool dihHX711;
volatile bool dahHX711;

volatile bool rawKeyDown, rawKeyDownTest;

HX711cw paddle(HX711_dt, HX711_sck, PORTA_128);  //data, clk, porta only
DelayLine dl;
HX711CWstate cw;

//Read DIH-DAH, debounced
//Return 0=none, 1=dih, 2=dah, 3=both
byte rdKeys() {
  if (dihHX711 | dahHX711) 
    return dihHX711 + dahHX711*2;
  return dihPaddle + dahPaddle * 2;
}

COROUTINE(loopHX711) {
  COROUTINE_BEGIN();
  COROUTINE_DELAY(100);
  Info = "HX711_starting...";
  pinMode(HX711_gnd,OUTPUT); digitalWrite(HX711_gnd, 0);
  pinMode(HX711_vcc,OUTPUT); digitalWrite(HX711_vcc, 1);
  
  //Get to zero, in max 5 seconds
  static byte n;
  for (n=0; n<80*5; n++) { //"static" because of protothread
    while (!paddle.ready()) COROUTINE_DELAY(1);
    int v = abs(dl.removeDC(paddle.read(true)));
    glbInp = log10x20(abs(v)); //global
    if ( v <= 2 ) break;
  }
  
  Info = "running...";
  while (1) {
    COROUTINE_DELAY(1);
    
    if (paddle.ready()) { //80 times a second = 12.5m Sec
      int inp2 = paddle.read(true);
      glbInp2 = inp2;
      inp2 -= dl.levelDC; //Remove DC asap (static level since setup);
      bool pola = inp2>0;
      inp2 = abs(inp2);
      int inp = log10x20(inp2);   //Use S scale (6db)
      glbInp = inp;
      if (inp2>65000) { Info = "-65000_error"; continue; }
      if (inp2>15000) { Info = "-15000_error"; continue; }
      if (inp >60)    { Info = "-60_error"; continue; }
      cw.poll(pola, inp, inp2);
      
      if (HX711Swapped) {dihHX711 = cw.B; dahHX711 = cw.A;}
      else              {dihHX711 = cw.A; dahHX711 = cw.B;}
      rawKeyDown = rawKeyDownTest | (rdKeys() != 0);
    }
  }
  COROUTINE_END();
}
