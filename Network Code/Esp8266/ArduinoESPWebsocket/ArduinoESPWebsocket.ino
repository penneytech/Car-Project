Chat history
New Chat
Close sidebar
Today
ESP8266 WebSocket Code Modification


Arduino WebSocket Communication
WebSocket Server with ID
WebSocket Client Non-Blocking Code
Arduino Project Assignment

User
Flex Plexico
Default (GPT-3.5)
Custom instructions details

User
Modify this to work with the esp8266

#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>

char ssid[] = "Ziltoid";
char pass[] = "omniscient";
char serverAddress[] = "192.168.2.87";
int port = 8080;

WiFiClient wifi;
WebSocketClient client = WebSocketClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;
int count = 0;

unsigned long previousMillis = 0;
const long interval = 500;
const long disconnectInterval = 5000; // New interval for printing "disconnected"
unsigned long lastDisconnectPrint = 0; // New timestamp for the last "disconnected" print
const String clientID = "uniqueClientID1";

void setup() {
  Serial.begin(9600);
  Serial.println("STARTING");
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println("starting WebSocket client");
  client.begin();
  client.beginMessage(TYPE_TEXT);
  client.print("ID:" + clientID);
  client.endMessage();
}

void loop() {
  unsigned long currentMillis = millis();

  if (client.connected()) {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      Serial.print("Sending hello ");
      Serial.println(count);
      client.beginMessage(TYPE_TEXT);
      client.print("hello ");
      client.print(count);
      client.endMessage();
      count++;
      int messageSize = client.parseMessage();
      if (messageSize > 0) {
        Serial.print("Received a message: ");
        Serial.println(client.readString());
      }
    }
  } else {
    if (currentMillis - lastDisconnectPrint >= disconnectInterval) {
      lastDisconnectPrint = currentMillis;
      Serial.println("disconnected");
    }
    // Try to reconnect every 500ms
    if (currentMillis - previousMillis >= 500) {
      previousMillis = currentMillis;
      client.stop(); // Close the previous connection
      client.begin();
      client.beginMessage(TYPE_TEXT);
      client.print("ID:" + clientID);
      client.endMessage();
    }
  }
}

ChatGPT
To modify the code to work with the ESP8266, you need to make some changes to the libraries and the WebSocket client. Here's the modified code for the ESP8266:

cpp
Copy code
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

char ssid[] = "Ziltoid";
char pass[] = "omniscient";
char serverAddress[] = "192.168.2.87";
int port = 8080;

WiFiClient wifi;
WebSocketsClient client;

unsigned long previousMillis = 0;
const long interval = 500;
const long disconnectInterval = 5000;
unsigned long lastDisconnectPrint = 0;
const String clientID = "uniqueClientID1";

void setup() {
  Serial.begin(9600);
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
  client.setAuthorization("Authorization Header"); // Add your authorization header if needed
}

void loop() {
  client.loop();
  unsigned long currentMillis = millis();

  if (!client.isConnected()) {
    if (currentMillis - lastDisconnectPrint >= disconnectInterval) {
      lastDisconnectPrint = currentMillis;
      Serial.println("disconnected");
      client.begin(serverAddress, port); // Reconnect to the WebSocket server
      client.setAuthorization("Authorization Header"); // Add your authorization header if needed
      delay(1000); // Wait for a second before sending the initial message
      client.sendTXT("ID:" + clientID);
    }
  } else {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      Serial.print("Sending hello ");
      Serial.println(count);
      client.sendTXT("hello " + String(count));
      count++;
    }
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_TEXT:
      Serial.print("Received a message: ");
      Serial.println((char *)payload);
      break;
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    default:
      break;
  }
}
Please make sure to replace "Authorization Header" with your actual authorization header if your WebSocket server requires it. This code should work with the ESP8266 and the WebSocketsClient library for WebSocket communication.






Regenerate
Send a message

ChatGPT may produce inaccurate information about people, places, or facts. ChatGPT August 3 Version

?
ChatGPT
