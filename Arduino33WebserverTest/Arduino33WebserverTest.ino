#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <ArduinoMDNS.h>

char ssid[] = "PenneyTest";
char pass[] = "password";
int status = WL_IDLE_STATUS;
unsigned long lastBlinkTime = 0;  // To keep track of the blink interval

WiFiUDP udp;
MDNS mdns(udp);
WiFiServer server(80);

int clientCount = 0; // To keep track of the number of clients

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);

  // while (!Serial) {
  //   delay(10);
  // }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  server.begin();

  mdns.begin(WiFi.localIP(), "nano");
  mdns.addServiceRecord("nano._http", 80, MDNSServiceTCP);
}

void loop() {

    if (WiFi.status() == WL_CONNECTED) {
    if (millis() - lastBlinkTime >= 2000) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      lastBlinkTime = millis();
    }
  }

  mdns.run();

  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\n');
    if (request.startsWith("GET ")) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close");  // Close connection after completion
      client.println();
      client.print(getHTML());
    } else if (request == "Hello") {
      clientCount++;
      client.println("Hi!");
      Serial.println("Client connected!");
    }

    delay(1); // Give the web browser time to receive the data
    client.stop();
  }
}

String getHTML() {
  String html = "<!DOCTYPE html><html>";
  html += "<body><h1>Server</h1>";
  html += "<p>Number of clients connected: " + String(clientCount) + "</p>";
  html += "</body></html>";
  return html;
}
