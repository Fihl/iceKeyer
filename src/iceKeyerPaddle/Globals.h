#pragma once

// Menu
int M_Volume=3;
int M_HzTone=700;
int M_FarnsworthChar=1; //0..3
int M_FarnsworthWord=3; //0..7
int M_wpm=12;

unsigned long M_shortDelayA = 150L; 

#define CALL "oz1aab"
char testMsg1[100] = CALL;
char testMsg2[100] = "V CQ test CQ test de " CALL " " CALL " CQ test " CALL " " CALL;
char testMsg3[100] = "V CQ test CQ test de " CALL " " CALL " de " CALL " " CALL;

byte dihTime; // 12 wpm => 100;

unsigned long shortDelayB = 180L;   // TBD
unsigned long spaceDelayB = 250L;   // TBD

bool LoopCWComfortA_Active =1;
bool LoopCWComfortB_Active =0;

#define WPMmax 30
#define MINPADDLE 30

#define SP(i)    { Serial.print(i); Serial.print(" "); }
#define SP2(s,i) { Serial.print(s); Serial.print(i); Serial.print(" "); }
#define SPln(i)  Serial.println(i); 

enum tState {eNormal, eMenu} iceState;
char menuCh;
char menuDihDah;
char InitialCh=0;

bool HX711Swapped = false;
#define CRTROTATION 2

#define RELAY_norm  4
#define RELAY_inv   5

#define PIN_tone      10
#define PIN_tone_att0 11
#define PIN_tone_att1 12

#define HX711_dat 3
#define HX711_clk 2

#define ROTARYsw   38
#define ROTARYdata 39
#define ROTARYclk  40
#define ROTARYsteps 1

#define TRIPLE false  //Some further test

#define SSDport 0x3c    // 3c or 3d

// Named 1xxx in order to be known before other files/tabs in Arduino IDE (alphabetical)

#include <AceRoutine.h> //COROUTINE_

#define SP2(s,i) { Serial.print(s); Serial.print(i); Serial.print(" "); }

// PIN_tone       Beeper (or 150 ohm speaker connected through 100uF with plus to pin)
// PIN_tone_att0  1000 ohm resistor to speaker
// PIN_tone_att1  470 ohm resistor to same speaker pin. 
// Or just skip volume adjust and use 1 resistor only
// Or just use an beeper
void doSnd(bool on)
{
  static int lastSetHz = 0;
  if (lastSetHz != M_HzTone) tone(PIN_tone, lastSetHz);
  lastSetHz = M_HzTone;
  
  //M_Volume 0..3
  byte b0 = M_Volume & B00000001;
  byte b1 = M_Volume & B00000010;
  if (!on) b0=b1=0;
  if (b0) pinMode(PIN_tone_att0, OUTPUT); else pinMode(PIN_tone_att0, INPUT);
  if (b1) pinMode(PIN_tone_att1, OUTPUT); else pinMode(PIN_tone_att1, INPUT);
  digitalWrite(PIN_tone_att0,0);
  digitalWrite(PIN_tone_att1,0);
}

int WPM2ms(int wpm)
{
  return 1200 / wpm;  //12 wpm => 100
}

void setWPM() {
  dihTime = WPM2ms(M_wpm); //12 wpm => 100
}

// https://www.pjrc.com/teensy/td_libs_Encoder.html
// #define ENCODER_OPTIMIZE_INTERRUPTS
// #include "Encoder.h"   // Encoder myEnc(5, 6);
// Encoder knobMenu(PIN_menuSCL, PIN_menuSDA);

#define LEDpin LED_BUILTIN

// Iambic   http://wb9kzy.com/modeab.pdf

// PARIS:   http://www.kent-engineers.com/codespeed.htm

// txBIT( 
// 1 = "-"          dih
// 2 = "---"        dah
// 3 = "   "        between letters
// 7 = "       "    between words
// . = space

char txBuf[256];
byte txBufIn = 0;
byte txBufOut = 0;

char memBuf[256];
byte memBufIn = 0;
byte memBufOut = 0;

byte doingMenu1 = 0;
byte doingMenu2 = 0;

char charDecoder[20];
char lastDecoded = ' ';
