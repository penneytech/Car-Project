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
char direction = 'X';
signed char prevPos = 0;

bool isStarted = false;
String mySerialData;
String newSerialData;
bool newData = false;

void Start() {
  if (digitalRead(ENCODER_BUTTON) == LOW) {
    isStarted = false;
    delay(200);
  }

  lcd.setCursor(0, 1);
  lcd.print(mySerialData);
}

void runMenu() {
  encoder.tick();
  signed char pos = encoder.getPosition();
  static signed char lastPos = pos;

  int buttonState = digitalRead(ENCODER_BUTTON);
  if (buttonState == LOW) {
    menuSelection = (menuSelection + 1) % 4;
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
    direction = (direction == 'X') ? 'Y' : 'X';
    lastPos = pos;
  }

  if (menuSelection == 3 && pos != lastPos) {
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
      lcd.print("INIT " + String(menuSelection == 0 ? ">" : " ") + "X: " + String(xStartPosition) + " " + String(menuSelection == 1 ? ">" : " ") + "Y:" + String(yStartPosition));
      lcd.setCursor(0, 1);
      lcd.print(String(menuSelection == 2 ? ">DIR:" : " DIR:") + direction + "  " + String(menuSelection == 3 ? ">START" : " START"));
    }
    lastLCDChange = currentTime;
  }

  // Run Mode Logic
  if (isStarted) {
    Start();
  } else {
    runMenu();
  }

  while (mySerial.available() > 0) {
    char inChar = (char)mySerial.read();
    if (inChar == '\n') {  // assuming '\n' as the end-of-line character for your incoming messages
      mySerialData = newSerialData;
      newSerialData = "";
      newData = false;
      Serial.println(mySerialData);
    } else {
      newSerialData += inChar;
    }
  }
}
