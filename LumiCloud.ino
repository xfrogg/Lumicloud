#define DEBUGxx
#define LIGHTS_OFF 0
#include <SoftwareSerial.h>
// BT communication variables
SoftwareSerial BT(10, 11); 
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND

// http://fastled.io/docs/3.1/modules.html
// https://github.com/FastLED/FastLED/wiki/Overview
#include "FastLED.h"
#define NUM_LEDS 60
#define DATA_PIN 3

// LED strip related variables
CRGB leds[NUM_LEDS];

// Misc timing
unsigned long lastIterationTime = millis();
unsigned long lastMiscActionTime = millis();
unsigned long currentTime = millis();
uint8_t speedDelay;  // color change speed
uint8_t iterationDelay = 0;  // duration of the current iteration concerning color change speed delay

// Iteration Variables
uint8_t reset;  // when changing modes, used to initialize new mode
uint8_t i;  // counter
uint8_t j=0;  // counter


// noise variables
static uint16_t x;
static uint16_t z;
uint8_t noise[NUM_LEDS];
uint16_t speed = 1;
uint16_t scale = 30;

// palette variables
CRGBPalette256 currentPalette( OceanColors_p );

// modes
char mode;
char btRead;

void setup() {
    reset = 1;
    #ifdef DEBUG
    Serial.begin(9600);
    #endif
    lightOff();
    
    // set the data rate for the SoftwareSerial port
    BT.begin(9600);
}

void loop(){
  getBTdata();
  
  if(LIGHTS_OFF){
    lightOff();
    return;
  }
  switch(mode)
  {
    default:
    case '0':
      lightOff();
    break;
    case 'a':
      Aurora();
    break;
    case 'c':
      Clouds();
    break;
    case 'o':
      Ocean();
    break;
    case 'l':
      Lightning();
    break;
    case 'p':
      Party();
    break;
    case 'r':
      Rainbow();
    break;
    case 's':
      RainbowStripes();
    break;
    case 'm':
      Lava();
    break;
  }
}

void getBTdata(){
  if (BT.available())
  {
     if(BT.available()>1){
      while(BT.available()>0){
        BT.read();
        // empty buffer
      }
      BT.println("NOK input too  long");
      return;
     }
     btRead=(BT.read());
     if(btRead!=mode){
      if(btRead!='?' && btRead!='0' && btRead!='a' && btRead!='c' && btRead!='o' && btRead!='l' && btRead!='p' && btRead!='r' && btRead!='s' && btRead!='m'){
        BT.println("NOK - wrong command " + String(btRead));
        return;
     }
    if(btRead=='?'){
      BT.println("Hello from LumiCloud");
      BT.println("0 - off");
      BT.println("a - Aurora");
      BT.println("c - Clouds");
      BT.println("o - Ocean");
      BT.println("l - Lightning");
      BT.println("p - Party");
      BT.println("r - Rainbow");
      BT.println("s - Rainbow stripes");
      BT.println("m - Magma");
      return;
     }
      reset = 1;
      mode = btRead;
      BT.println("OK");
    }
  }
}
/* ****************  Lightning mode  ****************** */
// Lightning
uint8_t lp;  // lightning position
uint8_t ls;  // lightning size
uint8_t lb;  // lightning brightness
uint8_t maxCharge = 100;
uint16_t chargeStepDelay = 1000;
uint16_t minDelayBetweenLightnings = 200;
uint16_t maxDelayBetweenLightnings = 10000;
uint8_t curCharge;
bool    charging;
uint8_t minLs = 4;
void Lightning(){
    if(reset){
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    speedDelay = 20;
    curCharge = 0;
    charging = true;
    lastIterationTime = millis();
    lastMiscActionTime = lastIterationTime;
    reset = 0;
  }
  if(curCharge<=0)
    charging = true;
  currentTime = millis();
  
  if(charging && curCharge < 100){
    if((currentTime - lastMiscActionTime) > chargeStepDelay){
      curCharge += random(1,35);
      #ifdef DEBUG
      Serial.print("step delay: ");Serial.print(chargeStepDelay);Serial.print(" cur time: ");Serial.print(currentTime);Serial.print(" last time: ");Serial.print(lastMiscActionTime);Serial.print(" curCharge: ");Serial.println(curCharge);
      #endif
      lastMiscActionTime = millis();
    }
  }
  else if(curCharge >= 100){
    charging = false;
    #ifdef DEBUG
    Serial.print("stopped chargning at curCharge: ");Serial.println(curCharge);
    #endif
  }
  uint8_t dim = random(5,15);
  for (i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(0, 0, max(leds[i].b-dim,0));
  }

  if(!charging){
    bool forceLightning = (currentTime-lastIterationTime)>=maxDelayBetweenLightnings;
    #ifdef DEBUG
    Serial.print("forceLightning: ");Serial.println(forceLightning);
    #endif
    if(forceLightning || (currentTime-lastIterationTime)>minDelayBetweenLightnings ){
      if(forceLightning || random(100)<8){
          lp = random(NUM_LEDS);
          ls = floor(random(4, NUM_LEDS/2));
          lb = random(200,256);
          #ifdef DEBUG
          Serial.print("lightning position: ");Serial.print(lp);Serial.print(" lightning size: ");Serial.print(ls);Serial.print(" lightning brightness: ");Serial.println(lb);
          #endif
          for (i = 0; i < NUM_LEDS; i++){
            if (ls >= minLs && i > (lp - floor(ls/2)) && i < (lp + floor(ls/2))){
              leds[i] = CRGB(0, 0, ceil(lb));
              #ifdef DEBUG
              Serial.print("light up led n°: ");Serial.println(i);
              #endif
            }
         }
          if(ls>curCharge) curCharge=0; else curCharge -= ls;
          lastIterationTime = millis();
          #ifdef DEBUG
          Serial.print("Lghtning!  curCharge: ");Serial.println(curCharge);
          #endif
      }
    }
  }
  FastLED.show();
  delay(speedDelay);
}
void Clouds(){
  if(reset){
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    scale = 20;
    speed = 1;
    speedDelay = 20;
    currentPalette = CloudColors_p;
    reset = 0;
  }
  fillnoise8();
  mapNoiseToLEDsUsingPalette();
  
  FastLED.show();
  delay(speedDelay);
}
/* ****************  Ocean mode  ****************** */
void Ocean(){
  if(reset){
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    scale = 30;
    speed = 1;
    speedDelay = 10;
    currentPalette = OceanColors_p;
    // noise : Initialize our coordinates to some random values
    x = random16();
    z = random16();
    reset = 0;
  }
  fillnoise8();
  mapNoiseToLEDsUsingPalette();
  
  FastLED.show();
  delay(speedDelay);
}
/* ****************  Aurora mode  ****************** */
// color related variables for aurora mode
enum TiersLed { First=1, Second=2, Third=3 };
int ledsPos[5] = {0, 0, 0, 0, NUM_LEDS-1};
int minhue = 0;
int maxhue = 100;
int currentLedColors[5] = {0, 0, 0, 0, 0};
int targetLedColors[5] = {0, 0, 0, 0, 0};
int randomHue = 0;
int currentSat = 255;
int currentBright = 255;
int brightness = currentBright;
void Aurora(){
  if(reset){
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
    // initializes the aurora
    updateTiersLedPos(First);
    updateTiersLedPos(Second);
    updateTiersLedPos(Third);
    speedDelay = 200;
    brightness = currentBright;
    for(i=0; i<5; i++){
      randomHue = random(minhue, maxhue);
      hsv2rgb_spectrum(CHSV(randomHue, currentSat, currentBright), leds[ledsPos[i]]);
      currentLedColors[i] = randomHue;
      targetLedColors[i] = random(minhue, maxhue);
    }
    reset = 0;
  }
  
  // update tiers leds and fill other leds
  for(i=0; i<5; i++){
    if(currentLedColors[i]==targetLedColors[i]){
      updateTiersLedPos(i);
      targetLedColors[i] = random(minhue, maxhue);
    }
    if(currentLedColors[i]<targetLedColors[i])
      currentLedColors[i]+=1;
    else
      currentLedColors[i]-=1;
    hsv2rgb_spectrum(CHSV(currentLedColors[i], currentSat, currentBright), leds[ledsPos[i]]);

    if(i){
      if(currentLedColors[i-1]<currentLedColors[i])
        fill_gradient(leds, ledsPos[i-1], CHSV(currentLedColors[i-1], currentSat, brightness), ledsPos[i], CHSV(currentLedColors[i], currentSat, brightness), FORWARD_HUES);
      else
        fill_gradient(leds, ledsPos[i-1], CHSV(currentLedColors[i-1], currentSat, brightness), ledsPos[i], CHSV(currentLedColors[i], currentSat, brightness), BACKWARD_HUES);
    }
  }

  currentTime = millis();
  if(lastIterationTime <= (currentTime - iterationDelay)){
    lastIterationTime = currentTime;
    iterationDelay = random(10000, 60000);
    // random speed for changing lights
    speedDelay = random(400, 800);
  }
  
  FastLED.show();
  delay(speedDelay);
}
void updateTiersLedPos(int tiers){
  switch(tiers){
    case 1:
      ledsPos[1]  = random(0+NUM_LEDS*0.1, (0+NUM_LEDS*0.1)+(NUM_LEDS*0.2));
    break;
    case 2:
      ledsPos[2]  = random(NUM_LEDS*0.35, NUM_LEDS-NUM_LEDS*0.35);
    break;
    case 3:
      ledsPos[3]  = random( (NUM_LEDS-NUM_LEDS*0.1) - (NUM_LEDS*0.2), NUM_LEDS-NUM_LEDS*0.1);
    break;
    default:
    return;
  }   
}

/***************** Party Mode ******************/
void Party(){
  if(reset){
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    scale = 30;
    speed = 5;
    speedDelay = 10;
    currentPalette = PartyColors_p;
    reset = 0;
  }
  fillnoise8();
  mapNoiseToLEDsUsingPalette();
  
  FastLED.show();
  delay(speedDelay);
}
/***************** Lava Mode ******************/
void Lava(){
    if(reset){
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    scale = 40;
    speed = 2;
    speedDelay = 10;
    currentPalette = LavaColors_p;
    reset = 0;
  }
  fillnoise8();
  mapNoiseToLEDsUsingPalette();
  
  FastLED.show();
  delay(speedDelay);
}
/***************** Rainbow Mode ******************/
void Rainbow(){
    if(reset){
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    scale = 30;
    speed = 10;
    speedDelay = 10;
    currentPalette = RainbowColors_p;
    reset = 0;
  }
  fillnoise8();
  mapNoiseToLEDsUsingPalette();
  
  FastLED.show();
  delay(speedDelay);
}
void RainbowStripes(){
    if(reset){
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    scale = 20;
    speed = 10;
    speedDelay = 20;
    currentPalette = RainbowStripeColors_p;
    reset = 0;
  }
  fillnoise8();
  mapNoiseToLEDsUsingPalette();
  
  FastLED.show();
  delay(speedDelay);
}
/* ****************  UTILS  ****************** */
void lightOff(){
  if(reset){
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
    for (j = 0; j < NUM_LEDS; j++){
      leds[j] = CRGB(0,0,0);
    }
    FastLED.show();
  }
}
/* ****************  NOISE  ****************** */
// Fill the x array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  uint8_t data;
  for(int i = 0; i < NUM_LEDS; i++) {
    int ioffset = scale * i;
    //noise[i] = inoise8(x + ioffset,z);
    data = inoise8(x + ioffset,z);

    data = qsub8(data,16);
    data = qadd8(data,scale8(data,39));

    uint8_t dataSmoothing = 0;
    if( speed < 50) {
      dataSmoothing = 200 - (speed * 4);
    }
    if( dataSmoothing ) {
      uint8_t olddata = noise[i];
      uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
      data = newdata;
    }
    noise[i] = data;
  }
  z += speed;
}
void mapNoiseToLEDsUsingPalette()
{
  for(int i = 0; i < NUM_LEDS; i++) {
    uint8_t index = noise[i];
    uint8_t bri =   255;

    CRGB color = ColorFromPalette( currentPalette, index, bri);
    leds[i] = color;
  }
}

