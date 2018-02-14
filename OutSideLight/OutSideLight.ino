/*
Name:		OutSideLight.ino
Created:	2018-02-04 21:12:40
Author:	Ge Erghon Laurus
*/

#include "DebugInfo.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
#define LIGHT D1

const String access_point = "OutSideLight";
const String command_topic = "outside/light/left/set";
const String state_topic = "outside/light/left/state";

const String mqtt_server = "0.0.0.0";
const uint16 mqtt_port = 1883;

unsigned long endTime = 0;
// 1000 = 1 sec; 1000*60 = 60000 = 1 min
unsigned long poweredTime = 60000;

WiFiClient wifiClient;
PubSubClient client(mqtt_server.c_str(), mqtt_port, wifiClient);

void setup() {

	Serial.begin(115200);
	pinMode(LIGHT, OUTPUT);

	WiFiManager wifiManager;
	wifiManager.setTimeout(180);
	wifiManager.autoConnect(access_point.c_str());

	client.setCallback(callback);
}

void loop() {
	if (millis() > endTime && endTime != 0)
		turnOffLight();
	if (WiFi.status() == WL_CONNECTED) {
		if (connectMQTTIfNeeded()) {
			client.loop();
		}
	}

}

void callback(char* topic, byte* payload, unsigned int length) {
	String payloadStr = "";
	for (int i = 0; i < length; i++) {
		payloadStr += (char)payload[i];
	}
	String topicStr = String(topic);
	//Serial.println(payloadStr);
	if (topicStr.equals(topicStr)) {
		//Serial.println(payloadStr);
		if (payloadStr.equals("ON")) {
			digitalWrite(LIGHT, HIGH);
			client.publish(state_topic.c_str(), "ON", true);
			endTime = millis() + poweredTime;
		}
		else if (payloadStr.equals("OFF")) {
			digitalWrite(LIGHT, LOW);
			client.publish(state_topic.c_str(), "OFF", true);
		}
	}
}

bool connectMQTTIfNeeded() {
	if (!client.connected()) {
		if (strlen(MQTT_USERNAME) > 0 ? client.connect(access_point.c_str(), MQTT_USERNAME, MQTT_PASSWORD) : client.connect(access_point.c_str())) {
			//Serial.println("CONNECTED TO MQTT " + access_point);
			client.subscribe(command_topic.c_str());
		}
	}
	return client.connected();
}

void turnOffLight() {
	digitalWrite(LIGHT, LOW);
	client.publish(state_topic.c_str(), "OFF", true);
	endTime = 0;
}