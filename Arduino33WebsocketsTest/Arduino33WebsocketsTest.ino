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

