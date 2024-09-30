#pragma once

#include "Keys.h"
#include "Globals.h"
#include "cfUtils.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, Y, &Wire, 18);   //RESET = 18 . (also ok if none)

void doMenuSetup()
{
  display.begin(SSD1306_SWITCHCAPVCC, SSDport); // Address 0x3C for 128x32, 0x3D for 128x64
  display.setRotation(CRTROTATION); // Uncomment to rotate display 180 degrees
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
}  

void doMenuText(char* prompt, int actual)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 8);
  display.print(prompt);
  display.setTextSize(3);
  display.setCursor(64, 8);
  display.print(actual);
  display.display();
}

int rotChanged=0;
bool rotSwitch;
byte delayRotate;

void doText(char* prompt, char* msg) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(100, 18);
  display.print(prompt);
  display.setTextSize(1);
  display.setCursor(0, 2);
  display.print(msg);
  display.display();
}

void SendBuf(char* s) {
  Serial.println(s);
  while (char ch = *s++) memBuf[memBufIn++] = ch;
}

char* promptArr[] = {"M1","M2","M3"};
enum Menu {M1, M2, M3, DELAY, VOLUME, TONE, FARNSchr, FARNSword, PADDLEa, PADDLEb, MAXMENU,     M4,M5,M6, MoreMenusHere}; 
int menu;

COROUTINE(MenuLoop) {
  COROUTINE_BEGIN();
  doMenuSetup();
  while (!InitialCh) COROUTINE_YIELD(); //Await 'R' keyed

  while (1) {
    
    restartMenu:
    SPln("Restart menu")
    iceState = eNormal;
    delayRotate=100; 
    rotChanged=0;
    crtUsed=1; // ??
    
    // Wait for next menu (turn button)
    while (!rotChanged) {
      COROUTINE_YIELD();
      if (rotSwitch) {  // If button pressed, then while keep pressed do increment M_wpm
        iceState = eMenu;
        static byte w; 
        w=0;
        while (1) {
          COROUTINE_YIELD();
          if (!rotSwitch) goto restartMenu; 
          if (rotChanged>0) if (M_wpm<WPMmax) M_wpm++;
          if (rotChanged<0) if (M_wpm>6) M_wpm--;
          rotChanged = 0;
          setWPM();
          if (w != M_wpm) doMenuText("WPM\n6..30", M_wpm);
          w = M_wpm;
        }
      }
    }
    
    iceState = eMenu;
    rotChanged=0;
    menuCh = ' ';
    rotSwitch=false;
    while(1) {
      menuDihDah= ' ';
      COROUTINE_YIELD();
      if (rotSwitch) { while (rotSwitch) COROUTINE_YIELD(); goto restartMenu; }
      if (rotChanged>0) menu++;
      if (rotChanged<0) menu--;
      menu = (menu + MAXMENU) % MAXMENU;
      rotChanged=0;

      int Add=0;
      if (menuDihDah=='1') Add++;
      if (menuDihDah=='2') Add--;
      
      char* msg;
      char* prompt;
      switch (menu) {
        case M1:  msg=testMsg1;
                  prompt=promptArr[0];
                  break;
        case M2:  msg=testMsg2;
                  prompt=promptArr[1];
                  break;
        case M3:  msg=testMsg3;
                  prompt=promptArr[2];
                  break;
      }
      switch (menu) {
        case M1:
        case M2:
        case M3:        doText(prompt,msg);
                        //if (menuCh=='P') { SendBuf(msg); goto restartMenu; }
                        if (menuDihDah=='1') { SendBuf(msg); goto restartMenu; }   //dih => tx
                        //dah could then be used to change contents
                        break;

        case VOLUME:    M_Volume = min(3,max(0,M_Volume+Add));
                        doMenuText("Volumen\n0..3", M_Volume);
                        break;
        case TONE:      M_HzTone = min(800,max(500,M_HzTone+Add*10));
                        doMenuText("Tone\nHz", M_HzTone);
                        break;
        case DELAY:     M_shortDelayA = min(300LU,max(100LU,M_shortDelayA+Add*25));
                        doMenuText("Delay\nmS", M_shortDelayA);
                        break;
        case FARNSchr:  M_FarnsworthChar = min(3,max(0,M_FarnsworthChar+Add));
                        doMenuText("Farnsworth\nChar 0..3", M_FarnsworthChar);
                        break;
        case FARNSword: M_FarnsworthWord = min(7,max(0,M_FarnsworthWord+Add));
                        doMenuText("Farnsworth\nWord 0..3", M_FarnsworthWord);
                        break;
        case PADDLEa:   cw.level6dbA = min(60,max(MINPADDLE,cw.level6dbA+Add));
                        doMenuText("Paddle\nA", cw.level6dbA);
                        break;
        case PADDLEb:   cw.level6dbB = min(60,max(MINPADDLE,cw.level6dbB+Add));
                        doMenuText("Paddle\nB", cw.level6dbB);
                        break;
        default:        doMenuText("Unknown",menu);
      }
    }
  }
  COROUTINE_END();
}

COROUTINE (rotary) {
  COROUTINE_BEGIN();
  pinMode(ROTARYsw, INPUT_PULLUP);
  pinMode(ROTARYdata, INPUT_PULLUP);
  pinMode(ROTARYclk, INPUT_PULLUP);
  static bool preClk=1;
  static int dataCnt=0;
  static byte swState=0;
  while(1) {
    COROUTINE_YIELD();
    COROUTINE_DELAY(delayRotate); delayRotate=0; //Skip rotates caused by pushing the button
    if ( preClk != digitalRead(ROTARYclk) ) {
      preClk = ! preClk;
      if (!preClk) continue;
      dataCnt += digitalRead(ROTARYdata)?-1:1;
      if (dataCnt>=ROTARYsteps) { rotChanged=1; dataCnt=0; }
      if (dataCnt<=-ROTARYsteps) { rotChanged=-1; dataCnt=0; }
    }
    rotSwitch = DebounceBit(4, swState, !digitalRead(ROTARYsw));
    //if (rotSwitch) SPln("But");
  }
  COROUTINE_END();
}
