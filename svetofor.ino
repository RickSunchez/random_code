#define GPIO2 2
#define GPIO0 0
#define TX 1

#define WIFI_SSID         "NK_RED"
#define WIFI_PASSWORD     "12345678qwe"
#define MQTT_SERVER_HOST  "dev.rightech.io"
#define MQTT_CLIENT_ID    "mqtt-svetofor"
#define MQTT_PASSWORD     "jgUNeyDw3hseXTb"

#include <ArduinoJson.h>
#include "EspMQTTClient.h"

EspMQTTClient client(
  WIFI_SSID,
  WIFI_PASSWORD,
  MQTT_SERVER_HOST,
  MQTT_CLIENT_ID,
  MQTT_PASSWORD,
  MQTT_CLIENT_ID
);

bool get_state(const String& payload) {
  DynamicJsonDocument data(1024);
  deserializeJson(data, payload);
  bool state = data["state"];

  return state;
}

void switch_state(String pin, bool state) {
  if (pin == "GPIO0") {
    GPIO0_state(true);
    GPIO2_state(false);
    TX_state(false);
  }
  if (pin == "GPIO2") {
    GPIO0_state(false);
    GPIO2_state(true);
    TX_state(false);
  } 
  if (pin == "TX") {
    GPIO0_state(false);
    GPIO2_state(false);
    TX_state(true);
  } 
}

void GPIO2_state(bool state) {
  digitalWrite(GPIO2, state);
  client.publish("/pin/g2", String(state));
}
void GPIO0_state(bool state) {
  digitalWrite(GPIO0, state);
  client.publish("/pin/g0", String(state));
}
void TX_state(bool state) {
  digitalWrite(TX, state);
  client.publish("/pin/tx", String(state));
}


void onConnectionEstablished() {
  client.subscribe("g0", [] (const String& payload) {
    switch_state("GPIO0", get_state(payload)); });

  client.subscribe("g2", [] (const String& payload) {
    switch_state("GPIO2", get_state(payload)); });
  
  client.subscribe("tx", [] (const String& payload) {
    switch_state("TX", get_state(payload)); });
}

void setup() {  
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO0, OUTPUT);
  pinMode(TX, OUTPUT);

  digitalWrite(GPIO2, LOW);
  digitalWrite(GPIO0, LOW);
  digitalWrite(TX, LOW);
}

void loop() {
  client.loop();

  if (!client.isConnected()) {
    delay(500);
    return;
  }
}
