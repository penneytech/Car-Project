#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include "EncoderStepCounter.h"

#define ENCODER_PIN1 5
#define ENCODER_PIN2 4
#define ENCODER_BUTTON 7

#define SENSOR_1_PIN A0
#define SENSOR_2_PIN A1
#define SENSOR_3_PIN A2
#define SENSOR_4_PIN A3

EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2, HALF_STEP);
SoftwareSerial mySerial(10, 11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastLCDChange = 0;

int menuSelection = 0;
int xStartPosition = 0;
int yStartPosition = 0;
signed char prevPos = 0;

bool isStarted = false;

void Start() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("READY");
  if (digitalRead(ENCODER_BUTTON) == LOW) {
    isStarted = false;
    delay(200);
  }
}

void runMenu() {
  encoder.tick();
  signed char pos = encoder.getPosition();
  static signed char lastPos = pos;

  int buttonState = digitalRead(ENCODER_BUTTON);
  if (buttonState == LOW) {
    menuSelection = (menuSelection + 1) % 3;
    delay(200);
  }

  if (menuSelection == 0 && pos != lastPos) {
    xStartPosition += (pos > lastPos) ? 1 : -1;
    xStartPosition = max(xStartPosition, 0);
    lastPos = pos;
  }

  if (menuSelection == 1 && pos != lastPos) {
    yStartPosition += (pos > lastPos) ? 1 : -1;
    yStartPosition = max(yStartPosition, 0);
    lastPos = pos;
  }

  if (menuSelection == 2 && pos != lastPos) {
    isStarted = true;
    lastPos = pos;
  }
}

void setup() {
  Serial.begin(9600);
  encoder.begin();
  mySerial.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(ENCODER_BUTTON, INPUT_PULLUP);
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastLCDChange >= 200) {
    lcd.clear();
    if (isStarted) {
      lcd.setCursor(0, 0);
      lcd.print("READY");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("INIT " + String(menuSelection == 0 ? ">" : " ") + "X: " + String(xStartPosition) +
                " " + String(menuSelection == 1 ? ">" : " ") + "Y:" + String(yStartPosition));
      lcd.setCursor(0, 1);
      lcd.print(menuSelection == 2 ? ">START" : " START");
    }
    lastLCDChange = currentTime;

    
  }

  if (isStarted) {
    Start();
  } else {
    runMenu();
  }
}
