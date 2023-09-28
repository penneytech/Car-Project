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
int LCDRefreshRate = 200;

int menuSelection = 0;
int xStartPosition = 0;
int yStartPosition = 0;
char direction = 'X';
signed char prevPos = 0;

bool isStarted = true;
String mySerialData;
String newSerialData;
bool newData = false;

void setup() {
  Serial.begin(9600);
  encoder.begin();
  mySerial.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(ENCODER_BUTTON, INPUT_PULLUP);

  pinMode(SENSOR_1_PIN, INPUT);
  pinMode(SENSOR_2_PIN, INPUT);
  pinMode(SENSOR_3_PIN, INPUT);
  pinMode(SENSOR_4_PIN, INPUT);
}

void loop() {

    if (isStarted) {

    } else {
      runMenu();
    }


  

  // Run Mode Logic
  if (isStarted) {
    Start();
  } else {
  }

  while (mySerial.available() > 0) {
    char inChar = (char)mySerial.read();

    if (inChar == '\n') {  // assuming '\n' as the end-of-line character for your incoming messages
      mySerialData = newSerialData;
      newSerialData = "";
      newData = false;
      Serial.println("Received: " + mySerialData);
      //String message = "Hi " + String(currentTime);
      //mySerial.print(message + '\n');
    } else {
      newSerialData += inChar;
    }
  }
}
