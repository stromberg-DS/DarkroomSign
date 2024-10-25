#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include<Button.h>

const int PIXEL_COUNT = 475;

const int LED_PIN = 6;
const int ENTER_PIN = 9;
const int NO_ENTER_PIN = 8;
const int POT_PIN = A3;
const int PARTY_PIN = 5;
const int POT_HYSTERSIS = 10;

const int COLOR_CHANGE_LED = 350;   //The LED number where the optional "Do not" text starts

int brightness = 100;
int lastBrightness;
float t;
float breatheBrightness;
int baselineBrightness = 127;
bool isPartytime = false;

int pixelDelay = 50;
int pixelCycle = 0;
unsigned long pixelPrevious = 0;
int rainbowSpeed = 5;

unsigned long lastPrintTime = 0;

Adafruit_NeoPixel pixel(PIXEL_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Button partyButton(PARTY_PIN);
Button enterButton(ENTER_PIN);
Button noEnterButton(NO_ENTER_PIN);

void segmentBreathe(int startLED, int endLED, uint32_t breatheColor, float speed, int amplitude, int yOffset);
void intervalPrint();
uint32_t wheel(byte wheelPos);
void rainbow(uint8_t wait);

void setup() {
  Serial.begin(9600);
  pinMode(ENTER_PIN, INPUT_PULLUP);
  pinMode(NO_ENTER_PIN, INPUT_PULLUP);
  pinMode(PARTY_PIN, INPUT_PULLUP);
  pinMode(POT_PIN, INPUT);

  pixel.begin();
  pixel.clear();
  pixel.show();
  pixel.setBrightness(brightness);
  
  //  LED TEST
  // for(int i=0; i<PIXEL_COUNT; i++){
  //   pixel.setPixelColor(i, 0x0000AA);
  //   pixel.show();
  // }
  // delay(2000);

  // pixel.clear();
  pixel.show();
}

void loop() {
  baselineBrightness = analogRead(POT_PIN);
  int enterBrightness = map(baselineBrightness, 0, 1024, 255, 0);
  rainbowSpeed = map(baselineBrightness, 0, 1024, 35, 1);
  baselineBrightness = map(baselineBrightness, 0, 1024, 195, 60);

  if(partyButton.isClicked()){
    isPartytime = !isPartytime;
    pixel.clear();
  }

  if(enterButton.isReleased() || enterButton.isClicked()) {
    pixel.clear();
    isPartytime=false;
  }
  if(noEnterButton.isReleased() || noEnterButton.isClicked()) isPartytime=false;

  // intervalPrint();

  if(isPartytime){
    // segmentBreathe(0, PIXEL_COUNT, 0x00FFFF, 0.25, 60, baselineBrightness);

    if(millis() - pixelPrevious >=pixelDelay){
      pixelPrevious = millis();
      rainbow(1);
    }
  }else{
    if(enterButton.isPressed()){
      pixel.setBrightness(enterBrightness);
      for(int i=0; i<COLOR_CHANGE_LED; i++){
        pixel.setPixelColor(i, 0x00FF00);
      }
    } else if(noEnterButton.isPressed()){
      segmentBreathe(0, PIXEL_COUNT, 0xFF0000, 0.25, 60, baselineBrightness);

    } else{
      pixel.clear();
    }
    
    if(abs(lastBrightness-brightness)>POT_HYSTERSIS){
      pixel.setBrightness(brightness);
      lastBrightness = brightness;
      // Serial.println("updated brightness");
    }
    // Serial.println(brightness);
  }
  pixel.show();
  // delay(250);

}

void segmentBreathe(int startLED, int endLED, uint32_t breatheColor, float speed, int amplitude, int yOffset){
    t = millis()/1000.0 + 0.5;
    brightness = amplitude*sin(M_PI*t/speed)+yOffset;
    pixel.setBrightness(brightness);
    pixel.clear();
    for(int i=startLED; i<=endLED; i++){

      pixel.setPixelColor(i, breatheColor);
    }

}

void intervalPrint(){
  if(millis()-lastPrintTime > 500){
    Serial.println(baselineBrightness);
    lastPrintTime = millis();
  }
}

void rainbow(uint8_t wait){
    if(pixelDelay != wait){
        pixelDelay = wait;
    }

    for(uint16_t i=0; i<PIXEL_COUNT; i++){
        pixel.setPixelColor(i, wheel((i+pixelCycle) &255));
    }
    pixel.show();
    pixelCycle +=rainbowSpeed;
    // pixelCycle++;
    if(pixelCycle >= 256){
        pixelCycle =0;
    }
}

uint32_t wheel(byte wheelPos){
    wheelPos = 255 - wheelPos;
    if(wheelPos < 85){
        return pixel.Color(255-wheelPos*3, 0, wheelPos * 3);
    }

    if(wheelPos < 170){
        wheelPos -= 85;
        return pixel.Color(0, wheelPos*3, 255-wheelPos*3);
    }
    wheelPos -=170;
    return pixel.Color(wheelPos*3, 255-wheelPos*3, 0);
}