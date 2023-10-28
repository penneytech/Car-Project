#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);

unsigned long previousMillis = 0;
const long interval = 1000;  // 1000 milliseconds (1 second) interval for sending message

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    //Report Location (Command, X, Y, Direction)
    mySerial.println("POS,2,3,N");

    // Report Puck Location & Color (Command, X, Y, Color)
    // Color is Red, Green or Blue
   // mySerial.print("COL,4,3,Red");
    
  }
}
