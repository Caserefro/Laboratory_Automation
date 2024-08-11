#ifndef CLIENT_DEFINITONS_H
#define CLIENT_DEFINITONS_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <mbedtls/aes.h>
#include "mbedtls/base64.h"

String serverNameS1 = "http://192.168.100.184/S1";
String serverNameS2 = "http://192.168.100.184/S2";

const char ID[] = "0x06";  //Deprecated, could add an ID recognition part, but is a wip.


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
#define OP_ADMIN_ADDSTUDENT 42
#define OP_ADMIN_DELSTUDENT 43
#define OP_ADMIN_RECORD 44

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

struct WeatherDatadef {
  double UVidx = 0;
  double WindSpd = 0;
  String WindDir = "Su";
  double Temp = 0;
  double RainG = 0;
  double RelHum = 0;
} WeatherData;

struct StationsStatedef {
  bool Station1 = false;
  bool Station2 = false;
} StationsState;


//Prototypes
void RandomNonceGenerator(unsigned char nonce[16]);
bool encryptStringCBC(const String &plainText, const char *key, String &OutputString);
bool decryptStringCBC(const String &cipherText, const char *key, String &OutputString);
bool postData(String &serverName, String &payload, String &response);
void httpGETRequest(String &serverName, String &payload);
void Step1Package(String &PackagetoSend, String &FirstNonce);
bool Step2PackageCore(String &ReceivedPackage, JsonDocument &JsonPackagetoSend, String &FirstNonce);

void Step2Package_OP_GENERAL_USE(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation);
void Step2Package_OP_ITEM_REQUEST(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int *LackingIDs);
void Step2Package_OP_RETURNED(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int ItemID, int State);
void Step2Package_OP_AC(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int Command);
void addItem(const BorrowedItem &item);
bool removeItem(int itemID);
BorrowedItem *findItem(int itemID);


void OP_SERVER_PING_Wrapper();
void OP_OPEN_CLOSE_Wrapper();
void OP_TIME_Wrapper();
void OP_WEATHER_Wrapper();
void OP_STATIONS_STATE_Wrapper();
void OP_INFOITEMS_Wrapper(int *LackingIDs);
void OP_ITEM_REQUEST_Wrapper(int *LackingIDs);
void UpdateBorrowedItems();
void OP_RETURNED_Wrapper(int ItemID, int State);
void OP_AC_Wrapper(int Command);
//void OP_ADDITEM_Handler();


int itemCount = 0;  // Current number of items in the array Not used. 

#endif