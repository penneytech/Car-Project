int lastSensor1State = 0;
int lastSensor2State = 0;
int lastSensor3State = 0;
int lastSensor4State = 0;

void Start() {
  unsigned long currentTime = millis();

  if (digitalRead(ENCODER_BUTTON) == LOW) {
    isStarted = false;
    delay(200);
  }

  if (currentTime - lastLCDChange >= LCDRefreshRate) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("READY");
    lcd.setCursor(0, 1);
    lcd.print(mySerialData);
    lastLCDChange = currentTime;

    int currentSensor1State = digitalRead(SENSOR_1_PIN);
    int currentSensor2State = digitalRead(SENSOR_2_PIN);
    int currentSensor3State = digitalRead(SENSOR_3_PIN);
    int currentSensor4State = digitalRead(SENSOR_4_PIN);

    // Write sensor states on second line
    lcd.setCursor(0, 1);
    lcd.print(String(currentSensor1State) + " " + String(currentSensor2State) + " " + String(currentSensor3State) + " " + String(currentSensor4State));

    if (currentSensor1State != lastSensor1State) {
      mySerial.print("Sensor 1: " + String(currentSensor1State) + " Sensor 2: " + String(currentSensor2State) + " Sensor 3: " + String(currentSensor3State) + " Sensor 4: " + String(currentSensor4State) +  '\n');
      Serial.print("Sensor 1 change: " + String(currentSensor1State));
      lastSensor1State = currentSensor1State;
    }
    if (currentSensor2State != lastSensor2State) {
      mySerial.print("Sensor 1: " + String(currentSensor1State) + " Sensor 2: " + String(currentSensor2State) + " Sensor 3: " + String(currentSensor3State) + " Sensor 4: " + String(currentSensor4State) +  '\n');
            Serial.print("Sensor 2 change: " + String(currentSensor2State));
      lastSensor2State = currentSensor2State;
    }
    if (currentSensor3State != lastSensor3State) {
      mySerial.print("Sensor 1: " + String(currentSensor1State) + " Sensor 2: " + String(currentSensor2State) + " Sensor 3: " + String(currentSensor3State) + " Sensor 4: " + String(currentSensor4State) +  '\n');
            Serial.print("Sensor 3 change: " + String(currentSensor3State));
      lastSensor3State = currentSensor3State;
    }
    if (currentSensor4State != lastSensor4State) {
      mySerial.print("Sensor 1: " + String(currentSensor1State) + " Sensor 2: " + String(currentSensor2State) + " Sensor 3: " + String(currentSensor3State) + " Sensor 4: " + String(currentSensor4State) +  '\n');
            Serial.print("Sensor 4 change: " + String(currentSensor4State));
      lastSensor4State = currentSensor4State;
    }
  }
}