#pragma once

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <TimeLib.h>
#include "FS.h"
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include "RemoteDebug.h"
#include <ArduinoJson.h>
#include <RHReliableDatagram.h>
#include <RH_Serial.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <PubSubClient.h>

#include <Arduino.h>
#include "Shared.h"

#define NTP_SERVER_NAME "ntp.smartcitizen.me"
#define NTP_SERVER_PORT 80
#define MQTT_SERVER_NAME "mqtt.smartcitizen.me"
#define MQTT_SERVER_PORT 80
#define MQTT_QOS 1

#define UPDATE_URL "https://github.com/fablabbcn/smartcitizen-kit-20/raw/master/bin/ESP_firmware.bin"

struct Credentials { bool set=false; char ssid[64]="null"; char pass[64]="null"; };
struct Token { bool set=false; char token[7]="null"; };
struct ESP_Configuration {
	Credentials credentials;
	Token token;
};

class SckESP
{
	private:
		// Input/Output
		bool serialDebug = false;		// Interfere with ESP <-> SAM comunnication (use with caution)
		bool telnetDebug = false;
		void SAMbusUpdate();
		void debugOUT(String strOut);

		// SAM communication
		uint8_t netPack[NETPACK_TOTAL_SIZE];
		char netBuff[NETBUFF_SIZE];
		bool sendMessage(SAMMessage wichMessage);
		bool sendMessage(SAMMessage wichMessage, const char *content);
		bool sendMessage();
		void receiveMessage(ESPMessage wichMessage);
		bool bootedPending = false;

		// Notifications
		bool sendToken();
		bool sendCredentials();
		bool sendNetinfo();
		bool sendTime();
		bool sendStartInfo();

		// **** MQTT
		bool mqttConnect();
		bool mqttHellow();
		bool mqttPublish();
		bool mqttInfo();
		bool mqttInventory();
		bool mqttCustom();

		// Led control
		const uint8_t pinLED = 4; 	// GPIO5
		uint8_t ledValue = 0;
		Ticker blink;
		uint16_t LED_SLOW = 350;
		uint16_t LED_FAST = 100;
		void ledSet(uint8_t value);
		void ledBlink(float rate);
		void ledOff();

		// Wifi related
		char hostname[20];
		String macAddr;
		String ipAddr;
		int currentWIFIStatus;
		void tryConnection();
		void wifiOFF();

		// OTA Update
		bool OTAupdate();

		// Config
		ESP_Configuration config;
		const char *configFileName = "/config.txt";
		bool saveConfig(ESP_Configuration newConfig);
		bool saveConfig();
		bool loadConfig();

		// AP mode
		void startAP();
		void stopAP();
		void scanAP();
		int netNumber;
		void startWebServer();
		bool flashReadFile(String path);
		bool captivePortal();
		bool isIp(String str);
		String toStringIp(IPAddress ip);
		const byte DNS_PORT = 53;

		// Time
		void setNTPprovider();
		void sendNTPpacket(IPAddress &address);
		String ISOtime();
		String epoch2iso(uint32_t toConvert);
		String leadingZeros(String original, int decimalNumber);
		WiFiUDP Udp;
		byte packetBuffer[48];


	public:
		const String ESPversion = "0.3.2-" + String(__GIT_HASH__);
		const String ESPbuildDate = String(__ISO_DATE__);

		void setup();
		void update();
		void webSet();
		void webStatus();

		// External calls
		void _ledToggle();
		time_t getNtpTime();
};

// Static led control
void ledToggle();

// Static webserver handlers
void extSet();
void extStatus();

// Time
time_t ntpProvider();
