#include <WiFi.h>
#include "aWOT.h"
#include "StaticFiles.h"
#define LED_BUILTIN 2

#define WIFI_SSID "network"
#define WIFI_PASSWORD "password"

WiFiServer server(80);
Application app;

bool ledOn;

void readLed(Request &req, Response &res) {
  res.print(ledOn);
}

void updateLed(Request &req, Response &res) {
  ledOn = (req.read() != '0');
  digitalWrite(LED_BUILTIN, ledOn);
  return readLed(req, res);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  app.get("/led", &readLed);
  app.put("/led", &updateLed);
  app.route(staticFiles());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client.connected()) {
    app.process(&client);
  }
}
