//Read DIH-DAH, and handle those

#include "Globals.h"

// Real CWComfort codeing logic
COROUTINE(LoopCWComfortA) {
  COROUTINE_BEGIN();
  static char start;
  static char next;
  static char buf[10];
  static unsigned long startTm3; 
  static unsigned long startTm7; 
  static unsigned long startTm; 
  static unsigned long startTmTriple; 
  Serial.println("Start-LoopCWComfortA");
  while(LoopCWComfortA_Active) {
    restartLoop:
    startTm3 = millis() + M_shortDelayA;
    startTm7 = millis() + M_shortDelayA * 2;
    while ( (start = rdKeys()) == '0') {
      COROUTINE_YIELD();
      if (millis() > startTm3) { startTm3+=1000000; txBIT('3',false,"Tm3"); }
      if (millis() > startTm7) { startTm7+=1000000; txBIT('7',false,"Tm7"); }
    }
    startTm = millis();
    while (millis() - startTm < M_shortDelayA) { 
      COROUTINE_YIELD();
      if ( (next = rdKeys()) == start) continue; //Do not "return" on longer press
      tmDiff2s(buf, startTm, millis());
      txBIT(start,true, buf);
      if (next == '0') goto restartLoop;
      // txBIT(start=='1'?'2':'1',true, "Other"); //Both was pressed
      while (rdKeys()!='0') COROUTINE_YIELD();
      goto restartLoop;
    }
    
    //**** Short Timeout has passed, so send two elements ****
    txBIT(start,true, "");
    txBIT(start,true, ""); //Double
    startTmTriple = millis() + (M_shortDelayA*3) / 2; // 1.5
    while (rdKeys() == start) {
      COROUTINE_YIELD();
      if (TRIPLE) {
        if (millis() > startTmTriple) { 
          startTmTriple+=1000000; 
          txBIT(start,false,"TRIPLE");  //under construction, needs timing! Against my own principle
        } //Triple
      }
    }
    tmDiff2s(buf, startTm, millis());
    //Serial.println(buf);
  }
  COROUTINE_END();
}
