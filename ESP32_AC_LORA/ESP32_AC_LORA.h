#ifndef ESP32_AC_LORA_H
#define ESP32_AC_LORA_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <mbedtls/aes.h>
#include "mbedtls/base64.h"
#include "base64.hpp"
#include "FS.h"
#include <LittleFS.h>
#include <Preferences.h>
#include <HTTPClient.h>

// all these will go into credentials file.
long TimeSinceLastSync = 0;
WebServer server(80);
String Step1Adr = "http://192.168.100.32:5000/S1";
String Step2Adr = "http://192.168.100.32:5000/S2";
String TimeAdr = "http://192.168.100.32:5000/Time";

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 184);
//String serverName = "http://192.168.100.184/Borrowed";
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
String requestBodyS1;  // Global variable to store the request body
String requestBodyS2;

struct WeatherDatadef {
  double WindSpd = 0;
  String WindDir = "Su";
  double UVidx = 0;
  double Temp = 0;
  double RainG = 0;
  double RelHum = 0;
} WeatherData;

struct StationsStatedef {
  bool Station1 = false;
  bool Station2 = false;
} StationsState;

//Manually assigned by admin. 
int ID_DEVICE1 = 3;
int ID_DEVICE2 = 4;
String PLACE = "X-103";
String DEVICE_TYPE1 = "AC";
String DEVICE_TYPE2 = "LoRa-Receiver";

String ActiveNonce = "";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -25200;
const int daylightOffset_sec = 3600;
struct tm timeinfo;

//Operations available for the device.
#define OP_SERVER_PING 10
#define OP_E_DEVICE_SYNC 11  // notifies server of device local ip address, and saves IDs to db. ESP32 devices only.
#define OP_TIME 12
#define OP_WEATHER_UPDATE 14

#define OP_AC 30
#define OP_ACONOFF 31
#define OP_ACSWINGFAN 32
#define OP_ACUPTEMP 33
#define OP_ACDOWNTEMP 34

static TaskHandle_t Step1_Task = NULL;
static TaskHandle_t Step2_Task = NULL;

// Prototypes
void RandomNonceGenerator(unsigned char nonce[16]);
bool encryptStringCBC(const String &plainText, const char *key, String &OutputString);
bool decryptStringCBC(const String &cipherText, const char *key, String &OutputString);
void Step1Response(String &ReceivedPackage, String &PackagetoSend, String &SecondNonce);
int Step2ResponseProcessing(String &ReceivedPackage, JsonDocument &JsonPackageReceived);
void Step2ResponsePackage(String &Package, JsonDocument &JsonPackageReceived, int operation);
void Step2Package_OP_E_DEVICE_SYNC(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int ID_DEVICE, String& DEVICE_TYPE);
void TestingSetup(fs::FS &fs);

void UpdateTimeinfo();
void OP_TIME_Handler(String &Package);
void OP_WEATHER_Handler(String &Package);
void OP_STATIONS_STATE_Handler(String &Package);

void OP_AC_Handler(JsonDocument &JsonPackageReceived, String &Package, int &Command);

#include "Credentials.h"
//#include "LittleFsFunctions.h"
#include "IR_Remote.h"
#include "Wrappers.h"
#include "http_handlers.h"
#endif