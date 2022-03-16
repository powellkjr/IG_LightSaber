/*
 * HC-SR04 example sketch
 *
 * https://create.arduino.cc/projecthub/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-036380
 *
 * by Isaac100
 */

// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>


//  __ 8 _ 7 _ 6 _ 5 __
// |                   |
//  \                  |
//  /                  |
// |__   _   _   _   __|
//     1   2   3   4

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN_BLADE   0 // On Trinket or Gemma, suggest changing this to 1
#define PIN_AUX   1
#define PIN_ROT_S 2

#define PIN_ROT_A 3
#define PIN_ROT_B 4

#define default_brightness 85 // Brightness modes other than throb use
// How many NeoPixels are attached to the Arduino?

#define mBLADE_EXTENDED 2
#define mBLADE_RETRACTED 3

int incomingByte = 0; // for incoming Serial data
unsigned long currentTime;
uint32_t nextTime;
uint32_t longHoldTimer;
uint32_t shortHoldTimer;
int iCounter = 0;
bool bPinDebug = false;
int pushCount = 0;

bool bIsClockwise;
bool bIsCounterClockwise;
bool bIsHeld;
bool bIsLongHold;
bool bIsShortHold;
bool bIsShortPush;

byte mBladeMode;

int encoderPosCount =500;
int state_rotA = 0;
int state_rotB = 0;
int state_push = 0;
int state_rotA_prev = 0;
int state_rotB_prev = 0;
int state_push_prev = 0;

int iLongHoldMin=2000;
int iShortHoldMin=200;

int iExtendDelay=75;

struct RGBA {
   byte r;
   byte g;
   byte b;
   float a;
   
};

RGBA cBladeColor = {255,0,0,.1};
#define LED_MIN 0
#define LED_MAX 100

#define NUMPIXELS 6// Popular NeoPixel ring size
//#define NUMPIXELS 9 // Popular NeoPixel ring size
#define NUM_LEDS 6

Adafruit_NeoPixel NEO_BLADE(NUMPIXELS, PIN_BLADE, NEO_GRB + NEO_KHZ800);


int iColorStep = 65536/18;



void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

pinMode(PIN_AUX,OUTPUT);


  NEO_BLADE.begin();
  NEO_BLADE.setBrightness(default_brightness); // Set to standard brightness
  nextTime = millis();
 
  

pinMode(PIN_ROT_S,INPUT_PULLUP);
pinMode(PIN_ROT_A,INPUT_PULLUP);
pinMode(PIN_ROT_B,INPUT_PULLUP);
SetPixelToRGBA(0,cBladeColor);
mBladeMode=mBLADE_RETRACTED;

}







void readInputs()
{


  state_rotA = digitalRead(PIN_ROT_A);
  state_rotB = digitalRead(PIN_ROT_B);
  state_push = !digitalRead(PIN_ROT_S);
  
  
    
   
    
    
    
}

void updateStates()
 {
  if(state_rotA != state_rotA_prev && !state_rotA)
  {
    if(state_rotB != state_rotA)
    {
//      encoderPosCount+=bIsClockWise;
      encoderPosCount+=iColorStep;
      bIsClockwise=true;
            //iNextSleep = t + iSetSleep;
    }
    else
    {
//      encoderPosCount-=!bIsClockWise;
      encoderPosCount-=iColorStep;
      bIsCounterClockwise=true;
            //iNextSleep = t + iSetSleep;
    }
      delay(50);
  }
    if (state_push != state_push_prev && state_push)
    {
      pushCount++;
      longHoldTimer=millis() + iLongHoldMin;
      shortHoldTimer=millis() + iShortHoldMin;
      //iNextSleep = t + iSetSleep;
    }

    if (state_push == state_push_prev && state_push)
    {
      bIsHeld=true;
      if(millis()>shortHoldTimer){
        bIsShortHold=true;
      }
      if(millis()>longHoldTimer){
        bIsShortHold=false;
        shortHoldTimer=UINT32_MAX;
        bIsLongHold=true;
      }
    }

    if(bIsShortHold && !state_push)
    {
      bIsShortPush=true;
    }

    if (state_push != state_push_prev && !state_push)
    {
      bIsHeld=false;
      bIsShortHold=false;
      bIsLongHold=false;
      longHoldTimer=UINT32_MAX  ;
      shortHoldTimer=UINT32_MAX ;
    }

    

    

    
   /*
       t = millis();
    bNextState = (t > nextTime);
    if(t > iNextSleep)
      Going_To_Sleep();
   mode_prev = mode; 
   mode = pushCount % 5;
*/
    
    
  state_rotA_prev= state_rotA;
  state_rotB_prev = state_rotB;
  state_push_prev = state_push;
 }

 void writeOutputs(){
  //NEO_BLADE.clear();
//  if(bIsHeld){
//    NEO_BLADE.setPixelColor(1,0,255,255,0);
//  }
//  else{
//  NEO_BLADE.setPixelColor(1,255,255,0);
//  }
//
//  if(bIsShortHold){
//    NEO_BLADE.setPixelColor(2,0,255,255,0);
//  }
//  else{
//  NEO_BLADE.setPixelColor(2,255,255,0);
//  }
////
//  if(bIsLongHold){
//    NEO_BLADE.setPixelColor(3,0,255,255,0);
//  }
//  else{
//  NEO_BLADE.setPixelColor(3,255,255,0);
//  }
//
//  if(state_push){
//    NEO_BLADE.setPixelColor(0,0,255,0,0);
//  }
//  else{
//  NEO_BLADE.setPixelColor(0,255,0,0);
//  }



//NEO_BLADE.setPixelColor((encoderPosCount%4)+1,255,0,0);

    uint32_t oldColor=NEO_BLADE.ColorHSV(encoderPosCount,255,255);
    uint8_t r, g, b;

    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);
    cBladeColor={r,g,b,cBladeColor.a};
    
  

  
  if(bIsLongHold){
    if (mBladeMode==mBLADE_RETRACTED){
      showBladeExtend(cBladeColor);
    }
    else
    {
      showBladeRetract(cBladeColor);
    }
    bIsLongHold=false;
    longHoldTimer=UINT32_MAX;
  }
  if(bIsCounterClockwise && mBladeMode==mBLADE_EXTENDED){
    showBladeExtend(cBladeColor);
    bIsCounterClockwise=false;
  }
  
  if(bIsClockwise && mBladeMode==mBLADE_EXTENDED){
    showBladeExtend(cBladeColor);
    bIsClockwise=false;
  }

  if(bIsShortPush && mBladeMode==mBLADE_EXTENDED){
    showBladeBlock();
    bIsShortPush = false;
    shortHoldTimer=UINT32_MAX;
  }
  SetPixelToRGBA(0,cBladeColor);
  NEO_BLADE.show();
 }


void showBladeExtend(RGBA inColor)
{
  NEO_BLADE.clear();
  for(int i = 0; i < NUMPIXELS; i++)
  {
    SetPixelToRGBA(i,inColor);
    NEO_BLADE.show();
    delay(iExtendDelay);
  }
  mBladeMode=mBLADE_EXTENDED;
}



void showBladeRetract(RGBA inColor)
{
  
  for(int i = 0; i < NUMPIXELS; i++)
  {
    SetPixelToRGBA(NUMPIXELS-i,{0,0,0,0});
    NEO_BLADE.show();
    delay(iExtendDelay);
  }
  mBladeMode=mBLADE_RETRACTED;
  SetPixelToRGBA(0,cBladeColor);
}

void showBladeBlock()
{
  int iBlockAt = random(NUMPIXELS);

    SetPixelToRGBA(iBlockAt,{255,255,255,1});
    NEO_BLADE.show();
    delay(iExtendDelay);
    SetPixelToRGBA(iBlockAt,cBladeColor);
    NEO_BLADE.show();
//    delay(iExtendDelay);
//    SetPixelToRGBA(iBlockAt,{255,255,255,1});
//    NEO_BLADE.show();
//    delay(iExtendDelay);
//    SetPixelToRGBA(iBlockAt,cBladeColor);
//    NEO_BLADE.show();
  
  
}
void SetPixelToRGBA(byte inNode, RGBA inColor)
{
NEO_BLADE.setPixelColor(inNode, NEO_BLADE.Color(round(inColor.r*inColor.a), round(inColor.g*inColor.a) , round(inColor.b*inColor.a)));    
}
void loop() {

 readInputs();
 updateStates();
 writeOutputs();
}
