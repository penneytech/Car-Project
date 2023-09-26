#include <SoftwareSerial.h>

SoftwareSerial mySerial(D5, D6);  // RX, TX
String receivedData = "";
unsigned long lastMessageTime = 0;
unsigned long lastBlinkTime = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long currentTime = millis();

  // Send message every second
  if (currentTime - lastMessageTime >= 200) {
    lastMessageTime = currentTime;
    String message = "Hello " + String(currentTime);
    mySerial.print(message + '\n');
  }

  // Read incoming messages
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n') {
      Serial.println("Received: " + receivedData);
      receivedData = "";
    } else {
      receivedData += c;
    }
  }

  // Strobe LED twice every second (100ms per strobe)
  if (currentTime - lastBlinkTime >= 1000) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  if (currentTime - lastBlinkTime >= 1200) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (currentTime - lastBlinkTime >= 1300) {
    digitalWrite(LED_BUILTIN, LOW);
    lastBlinkTime = currentTime;
  }
}
