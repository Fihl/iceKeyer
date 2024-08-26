#pragma once

// cfUtils.h

//#include "arduino.h"
//#include "pgmspace.h"

// 20log10(x), x=0..100
const PROGMEM static byte log10x20val[] = {
  0,0,6,10,12,14,16,17,18,19,20,21,22,22,23,24,24,25,25,26,26,26,27,27,28,28,
  28,29,29,29,30,30,30,30,31,31,31,31,32,32,32,32,32,33,33,33,33,33,34,34,34,
  34,34,34,35,35,35,35,35,35,36,36,36,36,36,36,36,37,37,37,37,37,37,37,37,38,
  38,38,38,38,38,38,38,38,38,39,39,39,39,39,39,39,39,39,39,40,40,40,40,40,40 };
byte log10x20(long val) {
  byte res=0;
  while (val > 1000) { res +=18; val /= 8; }
  while (val > 100)  { res += 6; val /= 2; }
  res += pgm_read_byte(log10x20val+val);
  return res;
}

bool sinusF(int deg) {
  if (deg<180) return 1;
  return 0;
}
bool cosinusF(int deg) {
  if (deg>=270) return 1;
  if (deg>=90)  return 0;
  return 1;
}

class DelayLine {
    #define meanCnt 64 // 1 sec's time
    int16_t mean[meanCnt];
    int16_t meanPtr=0; //0..meanCnt-1
    int32_t levelDClocal;
  public:
    int levelDC;

    int removeDC(int inp) {
      levelDClocal += (inp - mean[meanPtr]);
      mean[meanPtr++] = inp;
      if (meanPtr == meanCnt) meanPtr=0;
      levelDC = levelDClocal / meanCnt;
      return inp - levelDC;
    }
};

// https://gist.github.com/Fihl/09f2c4e5104c7b409d235d60bb16f4dd
bool DebounceBit(byte Integrator, byte &state, byte val) {
  byte old = state & 0b01111111; 
  bool onoff  = state >> 7;
  if (!old) old++; //Initial
  if (Integrator > 100) Integrator = 100; //Initial
  if (val) old++; else old--;
  if (!old) { old = 1; onoff = 0; }
  if (old > Integrator) { old = Integrator; onoff = 1; }
  state = old | (onoff << 7);
  return onoff;
}
