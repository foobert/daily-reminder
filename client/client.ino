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
  
  uint8_t b = 1;
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(D1, b);
    digitalWrite(D2, b);
    b ^= 1;
    delay(500);
    debug(".");
  }
  
  debug("Wifi connected");
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  return true;
}

long sendStatusRequest(const char* format, ...) {
  va_list arg;

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

  va_start(arg, format);
  client.printf(format, arg);
  va_end(arg);

  while (client.connected()) {
    // skip over headers
    String line = client.readStringUntil('\n');
    if (line == "\r") {
//      debug("headers received");
      break;
    }
  }

  return client.parseInt();
//  String line = client.readStringUntil('\n');
//  return line.toInt();
}

long getRemoteStatus() {
  return sendStatusRequest(
    "GET /reminder/status HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Accept: application/lolin\r\n"
    "User-Agent: lolin\r\n"
    "Connection: close\r\n\r\n",
    host);
}

long postRemoteStatus() {
  return sendStatusRequest(
    "POST /reminder/status HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Content-Type: application/lolin\r\n"
    "User-Agent: lolin\r\n"
    "Content-Length: 13\r\n"
    "Connection: close\r\n\r\n"
    "input pressed",
    host);
}

void refreshDisplay() {
  digitalWrite(D1, (currentStatus & 0x01) >> 0);
  digitalWrite(D2, (currentStatus & 0x02) >> 1);
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
    idleLoop = 0;
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
