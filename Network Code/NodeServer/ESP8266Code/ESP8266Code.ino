#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

char ssid[] = "Ziltoid";
char pass[] = "omniscient";
char serverAddress[] = "192.168.2.87";
int port = 8080;

WiFiClient wifi;
WebSocketsClient client;

unsigned long previousMillis = 0;
const long interval = 50;
const long disconnectInterval = 5000;
unsigned long lastDisconnectPrint = 0;
const char* clientID = "uniqueClientID1"; // Changed to a const char array
int count = 0; // Declare the count variable

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

  // Send the client ID on connection
  String messageToSend = "ID:" + String(clientID);
  client.sendTXT(messageToSend);

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
      String messageToSend = "ID:" + String(clientID); // Send the client ID on reconnection
      client.sendTXT(messageToSend);
    }
  } else {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      Serial.print("Sending hello ");
      Serial.println(count);
      String messageToSend = "hello ";
      messageToSend += count;
      client.sendTXT(messageToSend); // Send the message
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
