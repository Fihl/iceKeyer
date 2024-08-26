
byte testWPM,testPercent;

COROUTINE(doingSelfTest1) {
  COROUTINE_BEGIN();
  if (doingSelfTest) {
    COROUTINE_DELAY(1000);
    Serial.println("Doing tests");
    Info="Test1"; testWPM=12; testPercent=100; COROUTINE_AWAIT(!testWPM);
    Info="Test2"; testWPM=14; testPercent=100; COROUTINE_AWAIT(!testWPM);
    Info="Test3"; testWPM=16; testPercent=100; COROUTINE_AWAIT(!testWPM);

    Info="Test1C"; testWPM=12; testPercent=130; COROUTINE_AWAIT(!testWPM);
    Info="Test2C"; testWPM=14; testPercent=130; COROUTINE_AWAIT(!testWPM);
    Info="Test3C"; testWPM=16; testPercent=130; COROUTINE_AWAIT(!testWPM);

    Info="Test1B"; testWPM=12; testPercent=60; COROUTINE_AWAIT(!testWPM);
    Info="Test2B"; testWPM=14; testPercent=60; COROUTINE_AWAIT(!testWPM);
    Info="Test3B"; testWPM=16; testPercent=60; COROUTINE_AWAIT(!testWPM);

    Info="Test4"; testWPM=12; testPercent=80; COROUTINE_AWAIT(!testWPM);
    Info="Test5"; testWPM=10; testPercent=80; COROUTINE_AWAIT(!testWPM);
    Info="Test6"; testWPM=8; testPercent=80; COROUTINE_AWAIT(!testWPM);     //Virker ikke
    //Serial.println("Done doing tests. Coroutine doingSelfTest1 stopped");
    Info="Test_Done";
    doingSelfTest = false;
  }
  COROUTINE_END();
}

COROUTINE(doingSelfTest2) {
  COROUTINE_BEGIN();
  while (doingSelfTest) {
    while (!testWPM) COROUTINE_DELAY(1000);
    static String S;
    static char ch,i;
    static int dihLen, dahLen, spaceLen;
    S = "_._. __._ ";
    dihLen = (WPM2ms(testWPM) *testPercent) / 100;
    
    dahLen = 2*WPM2ms(cwSpeed) + dihLen;
    //dahLen = 3*dihLen; //Another test
    
    spaceLen = 2*WPM2ms(12) - dihLen;
    for (i=0; i<S.length(); ch=S[i++]) {
      switch (S[i]) {
        case '_': {
                    rawKeyDownTest = true;  COROUTINE_DELAY(dahLen);
                    rawKeyDownTest = false; COROUTINE_DELAY(spaceLen);
                    break;
                  }
        case '.': {
                    rawKeyDownTest = true;  COROUTINE_DELAY(dihLen);
                    rawKeyDownTest = false; COROUTINE_DELAY(spaceLen);
                    break;
                  }
        case ' ': {
                    COROUTINE_DELAY(2 * WPM2ms(cwSpeed) );
                    break;
                  }
      }
    }
    COROUTINE_DELAY(2000);
    testWPM=0;
    //Serial.println("One test done");
  }
  //Serial.println("Coroutine doingSelfTest2 stopped too");
  COROUTINE_END();
}
