#include <WiFi.h>
#include <aWOT.h>
#include "StaticFiles.h"

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";

const int LED_PIN = 2;
const bool LED_ACTIVE_LOW = false;

WiFiServer server(80);
App app;
bool ledOn = false;

void writeLed() {
  const bool high = LED_ACTIVE_LOW ? !ledOn : ledOn;
  digitalWrite(LED_PIN, high ? HIGH : LOW);
}

void readLed(Request& req, Response& res) {
  res.set("Content-Type", "text/plain; charset=utf-8");
  res.set("Cache-Control", "no-store");
  res.print(ledOn ? '1' : '0');
}

void updateLed(Request& req, Response& res) {
  if (req.left() != 1) {
    res.sendStatus(400);
    return;
  }

  char value;
  if (req.readBytes(&value, 1) != 1 || (value != '0' && value != '1')) {
    res.sendStatus(400);
    return;
  }

  ledOn = value == '1';
  writeLed();
  readLed(req, res);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  writeLed();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  Serial.print("Open http://");
  Serial.println(WiFi.localIP());

  app.get("/led", readLed);
  app.put("/led", updateLed);
  app.use(staticFiles());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client.connected()) {
    app.process(&client);
    client.stop();
  }
}
