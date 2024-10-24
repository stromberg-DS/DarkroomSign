#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

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

Adafruit_NeoPixel pixel(PIXEL_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

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
  brightness = analogRead(POT_PIN);
  brightness = map(brightness, 0, 1024, 0, 255);

  if(!isEnterOn){
    pixel.clear();
    for(int i=0; i<COLOR_CHANGE_LED; i++){
      pixel.setPixelColor(i, 0x00AA00);
    }
  } else if(!isNoEnterOn){
    for(int i=0; i<PIXEL_COUNT; i++){
      pixel.setPixelColor(i, 0xAA0000);
    }
  } else{
    pixel.clear();
  }
  
  if(abs(lastBrightness-brightness)>POT_HYSTERSIS){
    pixel.setBrightness(brightness);
    lastBrightness = brightness;
    Serial.println("updated brightness");
  }
  Serial.println(brightness);
  pixel.show();
  delay(250);

}
