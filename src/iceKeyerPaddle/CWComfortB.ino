//Read DIH-DAH, and handle those

// TBD, just test


#include "Globals.h"

//Playground 
COROUTINE(LoopCWComfortB) {
  COROUTINE_BEGIN();
  static char start;
  static char next;
  static char buf[10];
  static unsigned long startTm3; 
  static unsigned long startTm7; 
  Serial.println("Start-LoopCWComfortB");
  while(LoopCWComfortB_Active) {
    restartLoop:
    startTm3 = millis() + spaceDelayB;
    startTm7 = millis() + spaceDelayB * 2;
    while ( (start = rdKeys()) == '0') {
      COROUTINE_YIELD();
      if (millis() > startTm3) { startTm3+=1000000; txBIT('3',false,"Tm3"); }
      if (millis() > startTm7) { startTm7+=1000000; txBIT('7',false,"Tm7"); }
    }
    static unsigned long startTm; startTm = millis();  
    while (millis() - startTm < shortDelayB) { 
      COROUTINE_YIELD();
      if ( (next = rdKeys()) == start) continue;
      tmDiff2s(buf, startTm, millis());
      txBIT('1',true, buf); //dih
      goto restartLoop;
    }
    
    //**** Short Timeout has passed, send dah ****
    while (rdKeys() == start) COROUTINE_YIELD();
    tmDiff2s(buf, startTm, millis());
    txBIT('2',true, buf); //dah
  }
  COROUTINE_END();
}
