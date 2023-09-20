#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <ArduinoMDNS.h>

char ssid[] = "PenneyTest";     
char pass[] = "password";
int status = WL_IDLE_STATUS;

WiFiUDP udp;
MDNS mdns(udp);
WiFiServer server(80);

unsigned long lastBlinkTime = 0;  // To keep track of the blink interval

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(115200);

  // Wait for serial to connect, useful for native USB
  // while (!Serial) {
  //   delay(10);
  // }

  // Connect to WiFi with visual feedback
  while (status != WL_CONNECTED) {
    for (int i = 0; i < 2; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
    delay(700);  // Delay between the quick double blink sequences.
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(8500);  // Remaining delay to complete the 10s delay.
  }

  // Signal connection success with two 500ms blinks.
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }

  Serial.println("Connected to WiFi");
  server.begin();

  mdns.begin(WiFi.localIP(), "nano");
  mdns.addServiceRecord("nano._http", 80, MDNSServiceTCP);
}

void loop() {
  mdns.run();  // Update mDNS

  // Blink routine if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    if (millis() - lastBlinkTime >= 2000) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      lastBlinkTime = millis();
    }
  }

  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');

    // Handle AJAX call for runtime
    if (request.indexOf("/getRuntime") != -1) {
      unsigned long runtime = millis() / 1000; 
      client.print(runtime);
      client.stop();
      return; 
    }

    // Handle LED controls and serve webpage
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();

    if (request.indexOf("/LED=ON") != -1) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else if (request.indexOf("/LED=OFF") != -1) {
      digitalWrite(LED_BUILTIN, LOW);
    }

    client.println("<!DOCTYPE html><html>");
    client.println("<body>");
    client.println("<h1>Arduino Nano 33 IoT Web Server</h1>");
    client.println("<p><a href=\"/LED=ON\"><button>ON</button></a>&nbsp;<a href=\"/LED=OFF\"><button>OFF</button></a></p>");
    client.println("<p id='runtime'>Fetching runtime...</p>");

    client.println("<script>");
    client.println("function fetchRuntime() {");
    client.println("  var xhttp = new XMLHttpRequest();");
    client.println("  xhttp.onreadystatechange = function() {");
    client.println("    if (this.readyState == 4 && this.status == 200) {");
    client.println("      document.getElementById('runtime').innerHTML = 'Run time: ' + this.responseText + ' seconds';");
    client.println("    }");
    client.println("  };");
    client.println("  xhttp.open('GET', '/getRuntime', true);");
    client.println("  xhttp.send();");
    client.println("  setTimeout(fetchRuntime, 1000);");  // Fetch every 1 second
    client.println("}");
    client.println("window.onload = fetchRuntime;");
    client.println("</script>");

    client.println("</body></html>");

    client.stop();
  }
}
