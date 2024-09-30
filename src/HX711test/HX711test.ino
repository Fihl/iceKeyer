
#include "cfUtils.h"
#include "Globals.h"
#include "HX711CW.h"

#define HX711_dt  3 //2 for iceKeyerStraight
#define HX711_sck 2 //3 for iceKeyerStraight
//#define HX711_gnd 5 //for iceKeyerStraight as in article
//#define HX711_vcc 2 //for iceKeyerStraight as in article

HX711cw paddle(HX711_dt, HX711_sck, PORTA_128);  //data, clk, porta only (PORT_128_A/PORT_32_B/PORT_64_A)
DelayLine dl;
HX711CWstate cw;

void setup() {
  Serial.begin(115200);
  delay(1000);
  SPln("Start")
  
  //pinMode(HX711_gnd,OUTPUT); digitalWrite(HX711_gnd, 0);
  //pinMode(HX711_vcc,OUTPUT); digitalWrite(HX711_vcc, 1);

  SPln("HX711 starting...");
  for (byte n=0; n<80*5; n++) {    //Get to zero, in max 5 seconds
    int m = dl.removeDC(paddle.read(true));
    SPln(m);
    if (abs(m) <= 2) break;
  }
  SPln("HX711 running...");
}

byte calcDC=50;
void loop() {  
  if (paddle.ready()) { //80 times a second
    int inp2=paddle.read();
    if (calcDC) {
      if ( abs(dl.removeDC(inp2)<=2) ) calcDC--; //Get to zero
    }
    inp2 = inp2 - dl.levelDC; //Remove DC asap (static level since setup);
    bool pola = inp2>0;
    inp2 = abs(inp2);
    byte inp = log10x20(inp2);    //Use S scale (6db)
    cw.poll(pola, inp, inp2);
    static int inp2Max;
    inp2Max = max(inp2,inp2Max); //Nice to know
    
    SP("dbLevelA_"+String(cw.level6dbA))
    SP("dbLevelB_"+String(cw.level6dbB))
    SP("A")
    SP("B")
    SP("inp_"+String(inp))
    SP("inp2Max_"+String(inp2Max))
    SP("inp2_"+String(inp2))
    SPln()
    
    SP(cw.level6dbA); 
    SP(cw.level6dbB); 
    SP(-15+cw.A*4);         //A level
    SP(-10+cw.B*4);         //B level
    SPln(inp);
  }

}
