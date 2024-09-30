// iceKeyerStraight
// Like iceKeyer, just straight keyer

// Using Nano (RF)

// Using Load-Cell module
// HX711 pcb has to configured to 80 samples a second. Do move a jumper from position 10 to 80.

#define Logo "iceKeyerStraight OZ1AAB"

#include <AceRoutine.h> //COROUTINE_ https://github.com/bxparks/AceRoutine
using namespace ace_routine;

#include "Globals.h"
#include "cfUtils.h"
#include "HX711CW.h"
#include "Keys.h"

bool doingSelfTest=0;  // !!!!!!!!!!!!

String announceS = "R";
byte announceWPM = 20;

byte keyDown;
byte stateDebug;
int inpRaw;
byte cwSpeed=12;
#define cwRawVal 5 //wpm
#define cwMAX 30

void setup() {
  Serial.begin(115200);
  delay(1000);
  tone(PIN_tone, HzTone);
  
  CoroutineScheduler::setup();
}

unsigned long timeChecker = 0;
byte tick100;
void loop() {
  CoroutineScheduler::loop();
  if (timeChecker < millis() ) {
    tick100++;
    timeChecker = millis() + 100;
  }
}

bool lastKeyIsDah;
int UIpauseVal=200;
COROUTINE(UI) {
  COROUTINE_BEGIN();
  static bool lastkeyDown;
  while(1) {
    COROUTINE_DELAY(12);
    if (lastkeyDown != keyDown) UIpauseVal=100;
    if (!UIpauseVal) continue;
    lastkeyDown = keyDown;
    UIpauseVal--;
    SP("dbLevel_"+String(max(cw.level6dbA,cw.level6dbB)))
    SP("A_"+String(cw.A+cw.B))
    SP("Info_"+Info)
    SP("inp_"+String(glbInp))
    SP("inp2_"+String(glbInp2))
    SP("dih="+String(!lastKeyIsDah))
    if (cwSpeed == cwRawVal) SP("cwManual") else SP("cwSpeed="+String(cwSpeed))
    SPln()
    byte green=stateDebug*2;
    if (doingSelfTest) { 
      green=0;
      if (tick100) { tick100=0; green=4;}
    }
    SP(00+rawKeyDown*4);
    SP(10+keyDown*4);
    SP(20+green);
    SP(40+max(cw.level6dbA,cw.level6dbB));  //dbTreshold
    SP(40+min((byte)50,glbInp));
    // SP(inpRaw)
    SPln();
  }
  COROUTINE_END();
}

COROUTINE(KEYER) {
  COROUTINE_BEGIN();
  static int dihTime, dihTimeQ, oneDih;
  while (1) {
    stateDebug=0;
    while (cwSpeed == cwRawVal) {
      keyDown = rawKeyDown;
      COROUTINE_DELAY(5);
    }
    keyDown = 0;
    dihTime = WPM2ms(cwSpeed); //12 wpm => 100
    dihTimeQ = dihTime / 4;    //One quarter of full dih
    oneDih = dihTime;
    while (oneDih>0) {
      if (rawKeyDown) break;
      oneDih -= 5;
      COROUTINE_DELAY(5);
    }
    //One extra dih space passed (or key pressed)
    if (!rawKeyDown) { stateDebug++; COROUTINE_DELAY(dihTime); }  //letter spacing has to be 3 dihs
    //Now ready 
    while (!rawKeyDown) COROUTINE_DELAY(1);
    stateDebug++;
    keyDown = 1;
    //Start, time = 0
    COROUTINE_DELAY(dihTime);
    stateDebug++;
    //1 dih passed
    if (!rawKeyDown) { //Correct release of dih. 
      keyDown = 0; //rawDown at 4/4
      COROUTINE_DELAY(dihTime);
      //2 dih passed
      lastKeyIsDah = false;
      continue; //1 dih down, 1 dih space. Start over
    }
    //Try 1/4 dih time later for late release of dih
    COROUTINE_DELAY(dihTimeQ);
    if (!rawKeyDown) { //Allow for late release of dih. Tested here after 1.25 dih time 
      keyDown = 0; //rawDown at 4/4
      COROUTINE_DELAY(dihTime-dihTimeQ);
      //2 dih passed, done
      lastKeyIsDah = true;
      continue; //1.25 dih down, 0.75 dih space. Start over
    } else COROUTINE_DELAY(dihTime-dihTimeQ);
    //2 dih passed
    COROUTINE_DELAY(dihTime);
    //3 dih passed
    if (!rawKeyDown) {
      keyDown = 0;
      COROUTINE_DELAY(dihTime);
      lastKeyIsDah = true;
      continue; //dah done, start over
    }
    //Start repeat loop
    while (rawKeyDown) {
      if (stateDebug ++ == 8) stateDebug--;
      keyDown = 0; COROUTINE_DELAY(dihTime);
      if (!rawKeyDown) break;
      keyDown = 1; COROUTINE_DELAY(dihTime* (1 + 2*lastKeyIsDah) ); // dih / dah
    }
  }
  COROUTINE_END();
}

COROUTINE(DIGI_OUT) {
  COROUTINE_BEGIN();
  pinMode(LED_pin, OUTPUT);
  pinMode(LED_gnd, OUTPUT);
  digitalWrite(LED_gnd, 0);
  pinMode(RELAYOUT, OUTPUT);
  while (1) {
    digitalWrite(LED_pin, 0);
    digitalWrite(RELAYOUT,RELAYOUTinvert);
    doSnd(0);
    COROUTINE_AWAIT(keyDown);
    doSnd(1);
    digitalWrite(LED_pin, 1);
    digitalWrite(RELAYOUT,1-RELAYOUTinvert);
    COROUTINE_AWAIT(!keyDown);
  }
  COROUTINE_END();
}

COROUTINE(SPEED_READER) {
  COROUTINE_BEGIN();
  static int32_t announce;
  static int lastInp;
  pinMode(PIN_POT_GND, OUTPUT); //GND
  digitalWrite(PIN_POT_GND, 0);
  COROUTINE_DELAY(1000);
  while(1) {
    COROUTINE_DELAY(100);
    if (!++announce) 
      announceS = " " + String(cwSpeed);
    inpRaw = analogRead(PIN_POT);
    if ( abs(inpRaw-lastInp) < 10 ) inpRaw = lastInp;
    lastInp = inpRaw;
    byte newSpeed = map(inpRaw, 0, 1023, cwRawVal, cwMAX);
    if (cwSpeed != newSpeed) {
      cwSpeed = newSpeed;
      UIpauseVal=max(5,UIpauseVal); //More debug
      announce = -5; 
      if (cwSpeed == cwRawVal) { announceS = " M"; announce = 0; }
    }
  }
  COROUTINE_END();
}

//To speaker, @ 20 wpm (60 mSec)
COROUTINE(Sounder) {
  COROUTINE_BEGIN();
  COROUTINE_DELAY(100);
  static byte b1, dih = 1200 / announceWPM;
  while(1) {
    COROUTINE_DELAY(dih*4);
    if ( announceS.length() ) {
      b1 = announceS.charAt(0);
      announceS.remove(0,1);
      b1 = getcodeReversed(b1);
      while (b1 > 1) {
        doSnd(1); COROUTINE_DELAY(dih* ((b1 & 1)?3:1) ); 
        doSnd(0); COROUTINE_DELAY(dih); 
        b1 >>= 1;
      }
      if ( ! announceS.length() ) COROUTINE_DELAY(dih*3); //7 dih between words
    }
  }
  COROUTINE_END();
}
