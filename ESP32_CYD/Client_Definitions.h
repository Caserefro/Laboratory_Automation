#ifndef CLIENT_DEFINITONS_H
#define CLIENT_DEFINITONS_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <mbedtls/aes.h>
#include "mbedtls/base64.h"
#include <WebServer.h>

String Step1Adr = "http://192.168.100.32:5000/S1";
String Step2Adr = "http://192.168.100.32:5000/S2";
String WeatherAdr = "http://192.168.100.32:5000/Weather";
String TimeAdr = "http://192.168.100.32:5000/Time";

//Manually assigned by admin. 
int ID_DEVICE1 = 1;
int ID_DEVICE2 = 2;
String PLACE = "X-103";
String DEVICE_TYPE1 = "CYD-SCREEN";
String DEVICE_TYPE2 = "DOORLOCK";

String ActiveNonce = "";

WebServer server(80);

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 184);
//String serverName = "http://192.168.100.184/Borrowed";
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);


//Operation Catalog for this embedded device. (for the full catalog, check docs or the server code.)
#define OP_SERVER_PING 10
#define OP_E_DEVICE_SYNC 11  // notifies server of device local ip address, and saves IDs to db. ESP32 devices only.
#define OP_TIME 12
#define OP_OPEN_CLOSE 13

#define OP_BORROWEDITEMS_INFO 20     // RETURNS item_id LIST (ESP screen only)
#define OP_BORROWEDITEMS_REQUEST 21  // Returns specific item(s) based on id(s) (ESP screen only)
#define OP_BORROWED_ITEM_CHANGE 24   // Updates date and state, of an item in borroweditems table

#define OP_AC 30
#define OP_AC_ONOFF 31
#define OP_AC_SWINGFAN 32
#define OP_AC_UPTEMP 33
#define OP_AC_DOWNTEMP 34

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
#define MAX_ITEMS 50  // Maximum number of borrowed items
BorrowedItem borrowedItems[MAX_ITEMS];

struct WeatherData {
  double UVidx = 0;
  double WindSpd = 0;
  String WindDir = "Su";
  double Temp = 0;
  double RainG = 0;
  double RelHum = 0;
} ActiveWeatherData;

//Prototypes
void RandomNonceGenerator(unsigned char nonce[16]);
bool encryptStringCBC(const String &plainText, const char *key, String &OutputString);
bool decryptStringCBC(const String &cipherText, const char *key, String &OutputString);
bool postData(String &serverName, String &payload, String &response);
void httpGETRequest(String &serverName, String &payload);
void Step1Package(String &PackagetoSend, String &FirstNonce);
bool Step2PackageCore(String &ReceivedPackage, JsonDocument &JsonPackagetoSend, String &FirstNonce);

void Step2Package_OP_GENERAL_USE(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation);
void Step2Package_OP_OPEN_CLOSE(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, bool Open);
void Step2Package_OP_E_DEVICE_SYNC(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int ID_DEVICE, String& DEVICE_TYPE);
void Step2Package_OP_BORROWEDITEMS_REQUEST(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int *LackingIDs);
void Step2Package_OP_BORROWED_ITEM_CHANGE(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int ItemID, int State);
void Step2Package_OP_AC(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int Command);
void addItem(const BorrowedItem &item);
bool removeItem(int itemID);
BorrowedItem *findItem(int itemID);


void OP_SERVER_PING_Wrapper();
void OP_E_DEVICE_SYNC_Wrapper(int ID_DEVICE, String& DEVICE_TYPE);
void OP_OPEN_CLOSE_Wrapper(bool Open);
void OP_TIME_Wrapper();
void OP_WEATHER_Wrapper();

void OP_BORROWEDITEMS_INFO_Wrapper(int *LackingIDs);
void OP_BORROWEDITEMS_REQUEST_Wrapper(int *LackingIDs);
void UpdateBorrowedItems();
void OP_BORROWED_ITEM_CHANGE_Wrapper(int ItemID, int State);
void OP_AC_Wrapper(int Command);
//void OP_ADDITEM_Handler();


int itemCount = 0;  // Current number of items in the array Not used.

#endif