#pragma once

#include "Globals.h"

void crt_setup();
void crt_printS(char* s);
void crt_print(char ch);
void crt_xy(int x,int y);


#define Y 32 
#define SSDport 0x3c    // !!!!!!

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 crtCW = Adafruit_SSD1306(128, Y, &Wire, 18);   //RESET = 18

// work in line numbers.  Font height in charHeight
byte charHeight = 8; 
byte charWidth  = 6; 
byte maxX=21;
byte maxY=4;

byte top,crt_x,crt_y;
bool crtUsed;
bool DelayedClear;

void crt_setup()
{
  crt_x=99;
  crtCW.begin(SSD1306_SWITCHCAPVCC, SSDport); // Address 0x3C for 128x32, 0x3D for 128x64
  crtCW.setRotation(CRTROTATION); // Uncomment to rotate display 180 degrees
  crtCW.setTextSize(1);   // Normal 1:1 pixel scale
  crtCW.setTextColor(SSD1306_WHITE); // Draw white text
  crtCW.cp437(true);      // Use full 256 char 'Code Page 437' font
  
  DelayedClear=true; //crtCW.setCursor(0, 0);  // 0,0 / 8 / 16 / 24 are the 21 char lines
  crt_printS(Logo"\n");
  crt_printS("Please key R to start\n");
}

COROUTINE(crt_idle) {
  COROUTINE_BEGIN();
  crt_setup();
  while (1) {
    if (crtUsed) crtCW.display();
    crtUsed=false;
    COROUTINE_DELAY(100);
  }
  COROUTINE_END();
}

void crt_xy(int x,int y) {
  crt_x = x;
  crt_y = y;
  //crtCW.setCursor(crt_x*charWidth,crt_y*charHeight);
}

void crt_printS(char* s) {
  while (byte ch = *s++)
    crt_print(ch);
}

void crt_print(char ch)
{
  crtUsed = true;
  if (DelayedClear) { crt_x=0; crt_y=0; crtCW.clearDisplay(); }
  DelayedClear = false;
  if (ch=='\f') { crt_x=99; crt_y=99; }
  if (ch=='\n') crt_x=99; 
  if (crt_x>=maxX) {
    crt_x = 0;
    crt_y += 1;
    if (crt_y >= maxY) {
      crt_y = 0;
      if (ch>=' ') crtCW.clearDisplay(); else DelayedClear = true;
    }
    //crtCW.setCursor(0,crt_y*charHeight);
  }
  if (ch>=' ') {
    crtCW.setCursor(crt_x*charWidth,crt_y*charHeight);
    crtCW.fillRect(crt_x*charWidth,crt_y*charHeight, charWidth,charHeight, BLACK);
    crtCW.print(ch);
    crt_x += 1;
  }
}
