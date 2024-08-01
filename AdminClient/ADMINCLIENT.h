#ifndef ADMINCLIENT_H
#define ADMINCLIENT_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <mbedtls/aes.h>
#include "mbedtls/base64.h"

String serverNameS1 = "http://192.168.100.184/S1";
String serverNameS2 = "http://192.168.100.184/S2";


#define OP_SERVER_PING 10
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

//Prototypes
void RandomNonceGenerator(unsigned char nonce[16]);
bool encryptStringCBC(const String &plainText, const char *key, String &OutputString);
bool decryptStringCBC(const String &cipherText, const char *key, String &OutputString);
bool postData(String &serverName, String &payload, String &response);
void httpGETRequest(String &serverName, String &payload);
void Step1Package(String &PackagetoSend, String &FirstNonce);
bool Step2PackageCore(String &ReceivedPackage, JsonDocument &JsonPackagetoSend, String &FirstNonce);
void Step2Package_OP_GENERAL_USE(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation);


void OP_SERVER_PING_Wrapper();
void OP_ADMIN_INFO_Wrapper();
void OP_ADMIN_SETTIME_Wrapper(const String &CurrentTime);
void OP_ADMIN_ALLSTUDENTS_Wrapper();
void OP_ADMIN_ADDSTUDENT_Wrapper(const String &User);
void OP_ADMIN_DELSTUDENT_Wrapper(String &Username);
void OP_ADMIN_RECORD_DUMP_Wrapper();
void OP_ANDROID_AUTH_Wrapper(const String &Username, const String &Password);
void OP_ANDROID_LAYOUT_INFO_Wrapper(int *LackingIDs);
void OP_ANDROID_LAYOUT_Wrapper();

void Step2Package_OP_ADMIN_SETTIME(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, const String &CurrentTime);
void Step2Package_OP_ADMIN_ADDSTUDENT(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, const String &User);
void Step2Package_OP_ADMIN_DELSTUDENT(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, String &Username);
void Step2Package_OP_ANDROID_AUTH(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, const String &Username, const String &Password);
void Step2Package_OP_ANDROID_LAYOUT(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int *LackingElements);



#include "Credentials.h"
#include "http_functions.h"
#include "Wrappers.h"
#include "Step2Packages.h"
#endif