#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include<Button.h>

const int PIXEL_COUNT = 36;

const int LED_PIN = 6;
const int ENTER_PIN = 9;
const int NO_ENTER_PIN = 8;
const int POT_PIN = A3;
const int PARTY_PIN = 5;
const int POT_HYSTERSIS = 10;

const int COLOR_CHANGE_LED = 10;   //The LED number where the optional "Do not" text starts

int brightness = 100;
int lastBrightness;
float t;
float breatheBrightness;
int baselineBrightness = 127;
bool isPartytime = false;

unsigned long lastPrintTime = 0;

Adafruit_NeoPixel pixel(PIXEL_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Button partyButton(PARTY_PIN);

void segmentBreathe(int startLED, int endLED, uint32_t breatheColor, float speed, int amplitude, int yOffset);
void intervalPrint();

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
  
  for(int i=0; i<PIXEL_COUNT; i++){
    pixel.setPixelColor(i, 0x0000AA);
    pixel.show();
    delay(50);
  }
  delay(2000);

  // pixel.clear();
  pixel.show();
}

void loop() {
  bool isEnterOn = digitalRead(ENTER_PIN);
  bool isNoEnterOn = digitalRead(NO_ENTER_PIN);
  baselineBrightness = analogRead(POT_PIN);
  baselineBrightness = map(baselineBrightness, 0, 1024, 60, 195);

  if(partyButton.isClicked()){
    isPartytime = !isPartytime;
  }

  intervalPrint();

  if(isPartytime){
    segmentBreathe(5, 10, 0x00FFFF, 0.25, 60, baselineBrightness);
  }else{
    if(!isEnterOn){
      segmentBreathe(0, COLOR_CHANGE_LED, 0x00FF00, 2.0, 60, baselineBrightness);
    } else if(!isNoEnterOn){
      segmentBreathe(0, PIXEL_COUNT, 0xFF0000, 0.5, 60, baselineBrightness);

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