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

// all these will go into credentials file.
bool AdminLoggedIn = false;
long TimeSinceLastSync = 0;
WebServer server(80);

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 184);
//String serverName = "http://192.168.100.184/Borrowed";
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
String requestBodyS1;  // Global variable to store the request body
String requestBodyS2;

struct BorrowedItem {
  int ItemID = 0;
  String Item = "";  // 18 usable + null (max amount that can be fit in a row)
  String Name = "";  // 15 usable + null
  int NCID = 0;      // 2 usable + null
  String Time = "";  // hour+date 1240+null = 12:40
  String Date = "";  // day+month+year+null
  bool Returned = 0;
  //internal
  String TimeReturned = "";  // hour+date 1240+null = 12:40
  String DateReturned = "";  // day+month+year+null
};

struct UserData {
  int UserID = 0;
  String Username = "";  // 20 usable + null (max amount that can be fit in a row)
  String Name = "";      // 40 usable + null
  int NCID = 0;          // 10 usable + null
  String EcPw = "";      // password 16 usable + null
};

struct Record {
  int RecordID = 0;
  String Name = "";  // 15 usable + null
  String Time = "";  // hour+date 1240+null = 12:40
  String Date = "";  // day+month+year+null
};

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

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -25200;
const int daylightOffset_sec = 3600;
struct tm timeinfo;

const char ID[] = "0x06";
String ActiveNonce = "";

//Operation Catalog
#define OP_SERVER_PING 10
#define OP_OPEN_CLOSE 11
#define OP_TIME 12
#define OP_STATIONS_STATE 13
#define OP_WEATHER 14

#define OP_INFOITEMS 20
#define OP_ITEM_REQUEST 21
#define OP_RETURNED 22
#define OP_ADDITEM 23

#define OP_AC 30
#define OP_ACONOFF 31
#define OP_ACSWINGFAN 32
#define OP_ACUPTEMP 33
#define OP_ACDOWNTEMP 34

#define OP_ADMIN_INFO 40
#define OP_ADMIN_SETTIME 41
#define OP_ADMIN_ALLSTUDENTS 42  // pending
#define OP_ADMIN_ADDSTUDENT 43
#define OP_ADMIN_DELSTUDENT 44
#define OP_ADMIN_RECORD_DUMP 45

//Android Related
#define OP_ANDROID_AUTH 50
#define OP_ANDROID_LAYOUT_INFO 51
#define OP_ANDROID_LAYOUT 52

Preferences preferences;
const char *BorrowedItemsFile = "/BorrowedItems";
const char *UsersFile = "/Users";
const char *RecordFile = "/Record";

#define READING_FILE 1
#define MOVING_UP_FILE 2
#define WITH_JSON 1
#define WITHOUT_JSON 2

int biggestItemID = 0;
int SmallestItemID = 2;
int NumPagesItems = 0;
int totalAmountofItemsBorrowed = 0;

int totalAmountofUsers = 0;
int totalAmountofRecords = 0;
#define FORMAT_LITTLEFS_IF_FAILED true


#define MAX_ITEMS 50  // Maximum number of borrowed items
int borrowedItemsIDs[MAX_ITEMS];

static TaskHandle_t Step1_Task = NULL;
static TaskHandle_t Step2_Task = NULL;

/*
//Example data
BorrowedItem borrowedItemIE1 = { 1, "Laptop", "John Doe", 12, "1240", "15072024", true, "1300", "15072024" };
BorrowedItem borrowedItemIE2 = { 2, "Projector", "Jane Smith", 34, "0930", "14072024", false, "", "" };
BorrowedItem borrowedItemIE3 = { 3, "Camera", "Alice Johnson", 56, "1430", "13072024", true, "1500", "13072024" };

UserData UserIE1 = { 1, "johndoe", "John Doe", 89, "password1234" };
UserData UserIE2 = { 2, "janesmith", "Jane Smith", 21, "mypassword" };
UserData UserIE3 = { 3, "alicebrown", "Alice Brown", 65, "securepass" };
*/
// Prototypes
void RandomNonceGenerator(unsigned char nonce[16]);
bool encryptStringCBC(const String &plainText, const char *key, String &OutputString);
bool decryptStringCBC(const String &cipherText, const char *key, String &OutputString);
void Step1Response(String &ReceivedPackage, String &PackagetoSend, String &SecondNonce);
int Step2ResponseProcessing(String &ReceivedPackage, JsonDocument &JsonPackageReceived);
void serializeBorrowedItemStruct(const BorrowedItem &item, JsonDocument &json, String &Output);
void serializeUserDataStruct(const UserData &user, JsonObject &json, String &Output);
bool readJsonLittleFS(fs::FS &fs, const char *dirname, String &ReadedText, JsonDocument &json, int &position);
void TotalJsoninFile(fs::FS &fs, const char *dirname);
void UpdateIDList(fs::FS &fs, const char *dirname);
void BiggestItemID(fs::FS &fs, const char *dirname);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void Step2ResponsePackage(String &Package, JsonDocument &JsonPackageReceived, int operation);
void TestingSetup(fs::FS &fs);
void SettingupPreferences();

void UpdateTimeinfo();
void OP_TIME_Handler(String &Package);
void OP_WEATHER_Handler(String &Package);
void OP_STATIONS_STATE_Handler(String &Package);

void ItemIDarrayupdater();
void OP_INFOITEMS_Handler(String &Package);
void OP_ITEM_REQUEST_Handler(JsonDocument &JsonPackageReceived, String &Package);
void OP_AC_Handler(JsonDocument &JsonPackageReceived, String &Package, int &Command);
void OP_RETURNED_Handler(JsonDocument &JsonPackageReceived, String &Package);

void OP_ADDITEM_Handler(JsonDocument &JsonPackageReceived, String &Package);
void OP_ADMIN_INFO_Handler(String &Package);
void OP_ADMIN_SETTIME_Handler(JsonDocument &JsonPackageReceived, String &Package);
void OP_ADMIN_ALLSTUDENTS_Handler(String& Package);
void OP_ADMIN_ADDSTUDENT_Handler(JsonDocument &JsonPackageReceived, String &Package);
void OP_ADMIN_DELSTUDENT_Handler(JsonDocument &JsonPackageReceived, String &Package);
void OP_ADMIN_RECORD_DUMP_Handler(String &Package);

void OP_ANDROID_AUTH_Handler(String& Package);
void OP_ANDROID_LAYOUT_INFO_Handler(String& Package);
void OP_ANDROID_LAYOUT_Handler(String& Package);
#include "Credentials.h"
#include "LittleFsFunctions.h"
#include "IR_Remote.h"
#include "Wrappers.h"
#include "http_handlers.h"
#endif