#pragma once

byte glbInp, glbInp2;
String Info = "...";

#define SP(i)    { Serial.print(i); Serial.print(" "); }
#define SP2(s,i) { Serial.print(s); Serial.print(i); Serial.print(" "); }
#define SPln(i)  Serial.println(i); 

#define HX711_gnd 5
#define HX711_dt  4
#define HX711_sck 3
#define HX711_vcc 2

#define HzTone 700
#define PIN_tone 8
#define PIN_tone_att1 9
#define PIN_tone_att2 10

#define LED_pin A5
#define LED_gnd A4

#define RELAYOUT 12
#define RELAYOUTinvert 1

#define PIN_POT_GND A1
#define PIN_POT A0
//      PIN_POT_Plus = AREF

#define MINPADDLE 30
bool HX711Swapped = false;
bool PaddlesSwapped = false;

int WPM2ms(int wpm)
{
  return 1200 / wpm;  //12 wpm => 100
}
void doSnd(bool on)
{
  if (on) pinMode(PIN_tone_att1, OUTPUT); else pinMode(PIN_tone_att1, INPUT);
  digitalWrite(PIN_tone_att1,0);
}

typedef struct {
  char ch;
  byte bits;
} morse_t;

const morse_t MorseCodes[] = {
  {'A', B101},
  {'B', B11000},
  {'C', B11010},
  {'D', B1100},
  {'E', B10},
  {'F', B10010},
  {'G', B1110},
  {'H', B10000},
  {'I', B100},
  {'J', B10111},
  {'K', B1101},
  {'L', B10100},
  {'M', B111},
  {'N', B110},
  {'O', B1111},
  {'P', B10110},
  {'Q', B11101},
  {'R', B1010},
  {'S', B1000},
  {'T', B11},
  {'U', B1001},
  {'V', B10001},
  {'W', B1011},
  {'X', B11001},
  {'Y', B11011},
  {'Z', B11100},

  {'1', B101111},
  {'2', B100111},
  {'3', B100011},
  {'4', B100001},
  {'5', B100000},
  {'6', B110000},
  {'7', B111000},
  {'8', B111100},
  {'9', B111110},
  {'0', B111111},
  {0,0}
};

byte getcode(char letter) {
  for (byte n=0; char ch = MorseCodes[n].ch; n++) {
    if (letter == ch) 
      return MorseCodes[n].bits;
  }
  return 0;
}

byte getcodeReversed(char letter) {
  char ch = getcode(letter);
  if (!ch) return 0;
  byte res = 1;
  while (ch>1) {
    res = res*2 + (ch & 1) ;
    ch >>= 1;
  }
  return res;
}
