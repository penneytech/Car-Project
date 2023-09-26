#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);  // RX, TX
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD address, 16 column and 2 rows
unsigned long lastLogTime = 0;
unsigned long lastLEDChange = 0;
bool ledState = LOW;
String receivedData = "";

void setup() {
  Serial.begin(9600);
  Serial.println("Starting Arduino!");
  mySerial.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.print("Starting Arduino!");
  pinMode(13, OUTPUT);
}

void loop() {
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n') {
      Serial.println("Received: " + receivedData);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Received:");
      lcd.setCursor(0, 1);
      lcd.print(receivedData);
      mySerial.println("Message received by Arduino");
      receivedData = "";
    } else {
      receivedData += c;
    }
  }

  unsigned long currentTime = millis();

  // Blink LED without using delay
  if (currentTime - lastLEDChange >= 200) {
    lastLEDChange = currentTime;
    ledState = !ledState;
    digitalWrite(13, ledState);
  }

  if (currentTime - lastLogTime >= 1000) {
    lastLogTime = currentTime;
  }
}
