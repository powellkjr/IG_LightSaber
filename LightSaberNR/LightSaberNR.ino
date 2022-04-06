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

//#include <Adafruit_NeoPixel.h>
#include <NRPixelStrip.h>




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



#define NUM_PIXELS 144// this is the upper limit based on the ram avalible.
#define MIN_BLADE_LEN 6 // this is the minimum pixels to show even when retracted.


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

int iExtendDelay=2;

struct RGBA {
   byte r;
   byte g;
   byte b;
   float a;
   
};
float fFullBright = .1;
RGBA cBladeArray[]={
  {255,000,000,fFullBright},
  {255,255,000,fFullBright},
  {000,255,000,fFullBright},
  {000,255,255,fFullBright},
  {000,000,255,fFullBright},
  {255,000,255,fFullBright}
};

byte iBladeLen;


RGBA cBladeColor = cBladeArray[0];
#define LED_MIN 0
#define LED_MAX 100



NRPixelStrip gPixelStrip;




void setup() {
  #ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
    if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif

  pinMode(PIN_AUX,OUTPUT);

  gPixelStrip.Init(); //Initalizes the I/O pin
  clearBlade();
  nextTime = millis();
 
  

  pinMode(PIN_ROT_S,INPUT_PULLUP);
  pinMode(PIN_ROT_A,INPUT_PULLUP);
  pinMode(PIN_ROT_B,INPUT_PULLUP);

  mBladeMode = mBLADE_RETRACTED;
  iBladeLen = MIN_BLADE_LEN;
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
      encoderPosCount++;
      bIsClockwise=true;
    }
    else
    {
      encoderPosCount--;
      bIsCounterClockwise=true;
    }
      delay(50);
  }
    if (state_push != state_push_prev && state_push)
    {
      pushCount++;
      longHoldTimer =millis() + iLongHoldMin;
      shortHoldTimer =millis() + iShortHoldMin;
      delay(50);
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
    
  state_rotA_prev= state_rotA;
  state_rotB_prev = state_rotB;
  state_push_prev = state_push;
 }

 void writeOutputs(){
  
 
    cBladeColor = cBladeArray[encoderPosCount%(sizeof(cBladeArray) / sizeof(RGBA))];
    cBladeColor.a =random(80,100)/100.0*cBladeColor.a;

  if(bIsCounterClockwise || bIsClockwise){
    bIsCounterClockwise=false;
    bIsClockwise=false;
    return;
  }
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
    return;
  }
 
  if(bIsShortPush && mBladeMode==mBLADE_EXTENDED){
    showBladeBlock(cBladeColor);
    bIsShortPush = false;
    shortHoldTimer=UINT32_MAX;
    return;
  }

    showBladeIdle(cBladeColor);
    
 }

void clearBlade(){
  cli();//Turn off interrupts
  gPixelStrip.SendPixels(0,0,0,NUM_PIXELS);
  sei();//Turn interrupts back on
  gPixelStrip.Show();
}
void showBladeExtend(RGBA inColor)
{
  clearBlade();    

for(iBladeLen = MIN_BLADE_LEN; iBladeLen < NUM_PIXELS; iBladeLen++)
  {
    cli();//Turn off interrupts
    float fBlink=inColor.a * random(50,100)/100.0;
    gPixelStrip.SendPixels(inColor.r * fBlink,inColor.g * fBlink,inColor.b * fBlink, iBladeLen);
    sei();//Turn interrupts back on
    gPixelStrip.Show();
    delay(iExtendDelay);
  }
  mBladeMode=mBLADE_EXTENDED;
  iBladeLen = NUM_PIXELS;
}


void showBladeRetract(RGBA inColor)
{
  for(iBladeLen = NUM_PIXELS; iBladeLen > MIN_BLADE_LEN;iBladeLen--)
  {
  clearBlade();
      cli();//Turn off interrupts
      float fBlink=inColor.a * random(50,100)/100.0;
      gPixelStrip.SendPixels(inColor.r * fBlink,inColor.g * fBlink,inColor.b * fBlink, iBladeLen);
      sei();//Turn interrupts back on
      gPixelStrip.Show();
    delay(iExtendDelay);
  }
  mBladeMode=mBLADE_RETRACTED;
  iBladeLen = MIN_BLADE_LEN;
  
}

void showBladeIdle(RGBA inColor)
{
  clearBlade();
  cli();//Turn off interrupts
  gPixelStrip.SendPixels(inColor.r * inColor.a, inColor.g * inColor.a, inColor.b * inColor.a, iBladeLen); 
  sei();//Turn interrupts back on
  gPixelStrip.Show();
  delay(iExtendDelay);
}

void showBladeBlock(RGBA inColor)
{
  int iBlockAt = random(MIN_BLADE_LEN,NUM_PIXELS);
  cli();//Turn off interrupts
  gPixelStrip.SendPixels(inColor.r * inColor.a,inColor.g * inColor.a,inColor.b * inColor.a,iBlockAt);
  gPixelStrip.SendPixels(255,255,255,1);
  gPixelStrip.SendPixels(inColor.r * inColor.a,inColor.g * inColor.a,inColor.b * inColor.a,NUM_PIXELS-iBlockAt);
  sei();//Turn interrupts back on
  gPixelStrip.Show();
  delay(iExtendDelay*10);
}
  
  

void loop() {

 readInputs();
 updateStates();
 writeOutputs();
}
