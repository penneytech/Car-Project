#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h> // "WebSockets library by Markus Sattler"

SoftwareSerial mySerial(D5, D6);  // RX, TX
String receivedData = "";
unsigned long lastMessageTime = 0;
unsigned long lastBlinkTime = 0;

char ssid[] = "PenneyTest";
char pass[] = "password";
char serverAddress[] = "192.168.2.87";
//char serverAddress[] = "192.168.0.195";
int port = 8080;
unsigned long previousMillis = 0;
const long interval = 50;
const long disconnectInterval = 5000;
unsigned long lastDisconnectPrint = 0;
const char* clientID = "PenneyBot3000";  // Changed to a const char array
int count = 0;                             // Declare the count variable
WiFiClient wifi;
WebSocketsClient client;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("STARTING");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  client.begin(serverAddress, port);
  client.onEvent(webSocketEvent);
  client.setAuthorization("Authorization Header");  // Add your authorization header if needed
}


void loop() {
  unsigned long currentTime = millis();

  // Wifi Stuff

  client.loop();
  unsigned long currentMillis = millis();

  if (!client.isConnected()) {
    if (currentMillis - lastDisconnectPrint >= disconnectInterval) {
      lastDisconnectPrint = currentMillis;
      Serial.println("disconnected");
      client.begin(serverAddress, port);                // Reconnect to the WebSocket server
      client.setAuthorization("Authorization Header");  // Add your authorization header if needed
      delay(1000);                                      // Wait for a second before sending the initial message
      String messageToSend = "ID:" + String(clientID);  // Send the client ID on reconnection
      client.sendTXT(messageToSend);
    }
  } else {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      //Serial.print("Sending hello ");
      // Serial.println(count);
      // String messageToSend = "hello ";
      //  messageToSend += count;
      // client.sendTXT(messageToSend); // Send the message
      // count++;
    }
  }


  // Send message every second
  // if (currentTime - lastMessageTime >= 200) {
  //   lastMessageTime = currentTime;
  //   String message = "Hello " + String(currentTime);
  //   mySerial.print(message + '\n');
  // }

  // Read incoming messages
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n') {
      Serial.println("Received: " + receivedData);
       client.sendTXT(receivedData); // Send the message
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

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_TEXT:
      Serial.print("Received a message: ");
      Serial.println((char*)payload);
      break;
    case WStype_CONNECTED:
      {
        Serial.println("WebSocket connected");
        String messageToSend = "ID:" + String(clientID);
        client.sendTXT(messageToSend);
      }
      break;
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    default:
      break;
  }
}
