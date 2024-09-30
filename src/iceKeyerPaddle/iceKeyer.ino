// CW Using cootie single paddle https://www.youtube.com/watch?v=5LsRpn-1bmA
//   or Load-Cell modules

// Using Lolin S2 Mini

#define Logo "iceKeyer  OZ1AAB"   //max 20 tegn

#include <AceRoutine.h> //COROUTINE_ https://github.com/bxparks/AceRoutine
using namespace ace_routine;

#include "Globals.h"
#include "cfUtils.h"
#include "crtCW.h"
#include "Menu.h"
#include "HX711CW.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(Logo);
  LoopCWComfortA_Active = true;
  LoopCWComfortB_Active = false;

  setWPM();
  doSnd(1);
  delay(300);
  doSnd(0);
  
  CoroutineScheduler::setup();
}

unsigned long timeChecker = 0;
byte maxDiff = 0;
void loop() {
  CoroutineScheduler::loop();
  unsigned long time = millis();
  byte diff = time - timeChecker;
  if (diff < 100 & maxDiff < diff) {
    maxDiff = diff;
    Serial.println("****** Max Thread delay: "+String(diff) );
  }
  timeChecker = time;    
}
