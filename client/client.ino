#include "config.h"

#include <ESP8266WiFi.h>
#ifdef TLS
#include <WiFiClientSecure.h>
#else
#include <WiFiClient.h>
#endif

long idleLoop = 0;
long currentStatus = 0;

void debugSetup() {
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println();
  #endif
}

void debug(const char* msg) {
  #ifdef DEBUG
  Serial.println(msg);
  #endif
}

bool connectWifi() {
  wl_status_t wifiStatus = WiFi.status();
  if (wifiStatus == WL_CONNECTED) {
    return false;
  }

  debug("Connecting to wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);
  
  bool b = true;
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(D1, b ? HIGH : LOW);
    digitalWrite(D2, b ? HIGH : LOW);
    b ^= true;
    delay(500);
    debug(".");
  }
  
  debug("Wifi connected");
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  return true;
}

long getRemoteStatus() {
  #ifdef TLS
  WiFiClientSecure client;
  #else
  WiFiClient client;
  #endif
  
  if (!client.connect(host, httpsPort)) {
    debug("connection failed");
    return 2;
  }

  #ifdef TLS
  if (!client.verify(tlsFingerprint, host)) {
    debug("certificate doesn't match");
    return 2;
  }
  #endif
  
  String url = "/reminder/status";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Accept: application/lolin\r\n" +
               "User-Agent: lolin\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      debug("headers received");
      break;
    }
  }
  
  String line = client.readStringUntil('\n');
  return line.toInt();
}

long postRemoteStatus() {
  #ifdef TLS
  WiFiClientSecure client;
  #else
  WiFiClient client;
  #endif
  
  if (!client.connect(host, httpsPort)) {
    debug("connection failed");
    return 2;
  }

  #ifdef TLS
  if (!client.verify(tlsFingerprint, host)) {
    debug("certificate doesn't match");
    return 2;
  }
  #endif

  String url = "/reminder/status";
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/lolin\r\n" +
               "User-Agent: lolin\r\n" +
               "Content-Length: 13\r\n" +
               "Connection: close\r\n\r\n" +
               "input pressed");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      debug("headers received");
      break;
    }
  }
  
  String line = client.readStringUntil('\n');
  return line.toInt();
}

void refreshDisplay() {
  switch (currentStatus) {
      case 0:
        debug("status 0");
        digitalWrite(D1, LOW);
        digitalWrite(D2, LOW);
        break;
      case 1:
        debug("status 1");
        digitalWrite(D1, LOW);
        digitalWrite(D2, HIGH);
        break;
      case 2:
        debug("status 2");
        digitalWrite(D1, HIGH);
        digitalWrite(D2, LOW);
        break;
      case 3:
        debug("status 3");
        digitalWrite(D1, HIGH);
        digitalWrite(D2, HIGH);
        break;
      default:
        debug("Unknown status");
        digitalWrite(D1, HIGH);
        digitalWrite(D2, HIGH);
        break;
    }
}

void setup() {
  debugSetup();
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);

  pinMode(D7, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(D7), handleInterrupt, FALLING);
}

volatile byte inputPressed = 0;
void handleInterrupt() {
  inputPressed++;
}

void loop() {
  if (connectWifi()) {
    currentStatus = getRemoteStatus();
    refreshDisplay();
  }

  if (inputPressed > 0) {
    inputPressed = 0;
    currentStatus = postRemoteStatus();
    refreshDisplay();
    idleLoop = 0;
  }

  if (idleLoop > 300) {
    currentStatus = getRemoteStatus();
    refreshDisplay();
    idleLoop = 0;
  } else {
    idleLoop++;
  }
  delay(1000);
}
