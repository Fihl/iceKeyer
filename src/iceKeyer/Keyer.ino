//Keyer.ino

//Read cmds from txBuf and send

int KeyDownMS;

COROUTINE(LoopKeyer) {
  COROUTINE_BEGIN();
  static int cntDIHs = 0;   //UNUSED
  while (1) {
    while (txBufOut == txBufIn) { 
      cntDIHs = 0; 
      //send text
      if (memBufIn != memBufOut) {  //Cleartext output (from playback memory)
        char ch = memBuf[memBufOut++];
        if (ch==' ') { txBuf[txBufIn++] = '7'; continue; } //word space
        byte bits = cwBits(ch);
        //Serial.print("<"); Serial.print(ch); Serial.print(bits,BIN); Serial.println(">"); 
        while (bits>1) {
          txBuf[txBufIn++] = '1' + bits % 2; // 1/2 = dih/dah
          txBuf[txBufIn++] = '.';
          bits/=2;
        }
        txBuf[txBufIn++] = '3';
        txBuf[txBufIn++] = '.';
        continue;
      } 
      COROUTINE_DELAY(dihTime); 
    }
    char tx = txBuf[txBufOut++];
    int len = dihTime;
    byte interSPACE = false;
    bool addDecode = false;
    byte L3 = 3+M_FarnsworthChar;
    byte L7 = 7+M_FarnsworthWord;
    switch (tx) {
      case '1': { addDecode = true;  len = dihTime;    break; }
      case '2': { addDecode = true;  len = dihTime*3;  break; }
      case '3': { interSPACE=1;      len = dihTime*L3; break; }
      case '7': { interSPACE=1;      len = dihTime*L7; break; }
      case '.': {                    len = dihTime;    break; }
    } 
    cntDIHs += len;
    if (false)  //debug-print
      switch (tx) {
        case '1': { Serial.print("- ");   break; }
        case '2': { Serial.print("--- "); break; }
        case '3': { Serial.print("_ ");   break; }
        case '7': { Serial.print("| ");   break; }
        case '.': { Serial.print(". ");   break; }
      }
    
    if (addDecode) {
      iStrAddCh(charDecoder, tx, sizeof(charDecoder));
      KeyDownMS=len;
    }

    if (tx=='7') { crt_print(' '); Serial.print(' '); }
    
    if (interSPACE) {
      if (charDecoder[0]) {
        //Serial.println(); Serial.println(charDecoder);
        char ch = lastDecoded = decode();
        charDecoder[0] = 0;
      Serial.print(ch);
        if (!InitialCh) {
          if (ch == 'K') { InitialCh=ch; HX711Swapped=!HX711Swapped; }
          if (ch == 'R') InitialCh=ch; 
          if (InitialCh) crt_printS("\f"Logo"\nReady\n");
        } 
        else {
          if (iceState == eMenu) menuCh = ch;
          else crt_print(ch);
        }
      }
    }
    COROUTINE_DELAY(len); //Very last, as ALL locals are killed :-) 
  }
  COROUTINE_END();
}

COROUTINE(ledOnOff) {
  COROUTINE_BEGIN();
  pinMode(LEDpin, OUTPUT);
  pinMode(RELAY_norm, OUTPUT);
  pinMode(RELAY_inv, OUTPUT);
  digitalWrite(RELAY_norm, LOW);
  digitalWrite(RELAY_inv, HIGH);
  while (1) {
    if (KeyDownMS) {
      doSnd(1);
      digitalWrite(LEDpin, HIGH);
      digitalWrite(RELAY_norm, HIGH);
      digitalWrite(RELAY_inv, LOW);

      COROUTINE_DELAY(KeyDownMS);
      digitalWrite(LEDpin, LOW);
      digitalWrite(RELAY_norm, LOW);
      digitalWrite(RELAY_inv, HIGH);
      KeyDownMS=0;
      doSnd(0);
    }
    COROUTINE_YIELD();
  }
  COROUTINE_END();
}

void txBIT(char dih_dah, bool doSpace, String comment) {
  menuDihDah = dih_dah;
  if (dih_dah<='2')
    if (memBufOut != memBufIn) {
      memBufOut = memBufIn = 0; //Stop playback on any key
      return; //Skip key
    }

  if (iceState != eNormal) return;
  //if ( (dih_dah >=3) & (memBufOut != memBufIn) ) return; //No manual keys while memory playback
  if ( memBufOut != memBufIn ) return; //No manual keys while memory playback
  
  txBuf[txBufIn++] = dih_dah;
  if (doSpace)
    txBuf[txBufIn++] = '.';
  //if (comment != "") Serial.println(comment);
}
