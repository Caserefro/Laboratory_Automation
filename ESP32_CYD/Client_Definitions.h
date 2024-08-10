#ifndef CLIENT_DEFINITONS_H
#define CLIENT_DEFINITONS_H



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

// Continue initializing other elements if needed



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


#endif