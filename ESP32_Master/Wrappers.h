#ifndef WRAPPERS_H
#define WRAPPERS_H

void CreateRecord() {
  JsonDocument doc;
  // not sure you can do the wobbling in one go, so doing it like this to be sure lol.
  char dateBuffer[11];  // Buffer for the date format "DD/MM/YY"
  char timeBuffer[6];   // Buffer for the time format "MM:HH"
  // Format the time as "min:hour"
  strftime(timeBuffer, sizeof(timeBuffer), "%M:%H", &timeinfo);
  // Format the date as "Day/Month/last two digits of year"
  strftime(dateBuffer, sizeof(dateBuffer), "%d/%m/%y", &timeinfo);
  String TimeReturned = String(timeBuffer);
  String DateReturned = String(dateBuffer);
  doc["Door"] = "x103";
  doc["Time:"] = TimeReturned;
  doc["Date:"] = DateReturned;
  String Record = "";
  serializeJson(doc, Record);
  File file = LittleFS.open(RecordFile, "r");
  if (!file) {
    Serial.println("Failed to open file");
    return;
  }
  size_t fileSize = file.size();
  file.close();
  if (fileSize < 2500) {
    appendFile(LittleFS, RecordFile, (const char*)Record.c_str());
  } else {
    writeFile(LittleFS, RecordFile, (const char*)Record.c_str());
  }
  readFile(LittleFS, RecordFile);
}

void UpdateTimeinfo() {  //hw proved inconsistent in the cyd (rtos bs maybe? dunno why rly), so opting for another approach
  // Calculate elapsed time since last sync in seconds
  long TimeSinceLastSyncSec = (millis() - TimeSinceLastSync) / 1000;

  // Update seconds, minutes, hours
  timeinfo.tm_sec += TimeSinceLastSyncSec;
  if (timeinfo.tm_sec >= 60) {
    timeinfo.tm_min += timeinfo.tm_sec / 60;
    timeinfo.tm_sec %= 60;
  }
  if (timeinfo.tm_min >= 60) {
    timeinfo.tm_hour += timeinfo.tm_min / 60;
    timeinfo.tm_min %= 60;
  }
  if (timeinfo.tm_hour >= 24) {
    timeinfo.tm_mday += timeinfo.tm_hour / 24;
    timeinfo.tm_hour %= 24;
  }

  // Determine maximum days in current month
  int days_in_month = 31;  // Default to maximum for most months
  switch (timeinfo.tm_mon) {
    case 3:
    case 5:
    case 8:
    case 10:  // April, June, September, November
      days_in_month = 30;
      break;
    case 1:  // February (check for leap year)
      days_in_month = (timeinfo.tm_year % 4 == 0 && (timeinfo.tm_year % 100 != 0 || timeinfo.tm_year % 400 == 0)) ? 29 : 28;
      break;
  }

  if (timeinfo.tm_mday > days_in_month) {
    timeinfo.tm_mday -= days_in_month;
    timeinfo.tm_mon++;
    if (timeinfo.tm_mon >= 12) {
      timeinfo.tm_mon = 0;
      timeinfo.tm_year++;
    }
  }

  // Update time since last sync
  TimeSinceLastSync = millis();
}

void OP_TIME_Handler(String& Package) {
  JsonDocument JsonPackagetoSend;
  JsonPackagetoSend["tm_sec"] = timeinfo.tm_sec;
  JsonPackagetoSend["tm_min"] = timeinfo.tm_min;
  JsonPackagetoSend["tm_hour"] = timeinfo.tm_hour;
  JsonPackagetoSend["tm_mday"] = timeinfo.tm_mday;
  JsonPackagetoSend["tm_mon"] = timeinfo.tm_mon;
  JsonPackagetoSend["tm_year"] = timeinfo.tm_year;
  JsonPackagetoSend["tm_wday"] = timeinfo.tm_wday;
  JsonPackagetoSend["tm_yday"] = timeinfo.tm_yday;
  JsonPackagetoSend["tm_isdst"] = timeinfo.tm_isdst;
  serializeJson(JsonPackagetoSend, Package);
}

void OP_WEATHER_Handler(String& Package) {
  JsonDocument JsonPackagetoSend;
  JsonPackagetoSend["WindSpd"] = WeatherData.WindSpd;
  JsonPackagetoSend["WindDir"] = WeatherData.WindDir;
  JsonPackagetoSend["UVidx"] = WeatherData.UVidx;
  JsonPackagetoSend["Temp"] = WeatherData.Temp;
  JsonPackagetoSend["RainG"] = WeatherData.RainG;
  JsonPackagetoSend["RelHum"] = WeatherData.RelHum;
  serializeJson(JsonPackagetoSend, Package);
}

void OP_STATIONS_STATE_Handler(String& Package) {
  JsonDocument JsonPackagetoSend;
  JsonPackagetoSend["Station1"] = StationsState.Station1;
  JsonPackagetoSend["Station2"] = StationsState.Station2;
  serializeJson(JsonPackagetoSend, Package);
}

void OP_INFOITEMS_Handler(String& Package) {
  JsonDocument JsonPackagetoSend;
  JsonDocument doc;
  String ReadedJson = "";
  int position = 0;
  JsonArray array = JsonPackagetoSend.createNestedArray("ServerIDs");  //IDs in server memory.
  while (readJsonLittleFS(LittleFS, BorrowedItemsFile, ReadedJson, doc, position, MOVING_UP_FILE, WITH_JSON)) {
    array.add(doc["ItemID"]);
  }
  serializeJson(JsonPackagetoSend, Package);
}

void OP_ITEM_REQUEST_Handler(JsonDocument& JsonPackageReceived, String& Package) {  // will  return concatenated list of JSON, not a JsonFormatted list.
  JsonDocument doc;
  String ReadedJson = "";
  int position = 0;
  JsonDocument JsonPackagetoSend;
  int counter = 0;
  for (int i = 0; counter < JsonPackageReceived["LackingIDs"].size(); i++) {
    readJsonLittleFS(LittleFS, BorrowedItemsFile, ReadedJson, doc, position, READING_FILE, WITH_JSON);
    if (JsonPackageReceived["LackingIDs"][counter] == doc["ItemID"]) {
      Package += ReadedJson;
      i = 0;
      counter++;
    }
  }
  Serial.println("Package ITEM REQUEST");
  Serial.println(Package);
}

void OP_AC_Handler(JsonDocument& JsonPackageReceived, String& Package, int& Command) {
  static int currenttemp = 5;
  static bool OnorOff = 0;
  Command = JsonPackageReceived["Command"];
  switch (Command) {
    case OP_ACONOFF:
      OnorOff = !OnorOff;
      IRsendCode(OnorOff ? TurnONCodeAC : TurnOFFCodeAC);
      IRsendCode(OnorOff ? TurnONCodeAC : TurnOFFCodeAC);
      break;
    case OP_ACSWINGFAN:
      IRsendCode(SwingCodeAC);
      IRsendCode(SwingCodeAC);
      break;
    case OP_ACUPTEMP:
      IRsendCode(tempcodes[currenttemp]);
      IRsendCode(tempcodes[currenttemp]);
      currenttemp++;
      if (currenttemp > 11)
        currenttemp = 11;
      break;
    case OP_ACDOWNTEMP:
      IRsendCode(tempcodes[currenttemp]);
      IRsendCode(tempcodes[currenttemp]);
      currenttemp--;
      if (currenttemp < 0)
        currenttemp = 0;
      break;
  }
  Package = "OK";
}
// PENDING FOR CHECKS --------------------------------------------------------------------------------------------------------------
void OP_RETURNED_Handler(JsonDocument& JsonPackageReceived, String& Package) {
  //Do brute forece  until element is found. retrieve json, read,  modify it (date and time) then
  // read Json and copy into cpy file, but ignore the one you are changing. delete old file, and rename.
  JsonDocument doc;
  String ReadedJson = "";
  int position = 0;
  int wantedItemPosition = 0;
  bool found = 0;
  const char* BorrowedItemscpyFile = "/BorrowedItemscpy";
  while (readJsonLittleFS(LittleFS, BorrowedItemsFile, ReadedJson, doc, position, MOVING_UP_FILE, WITH_JSON) && !found) {
    if (doc["ItemID"] == JsonPackageReceived["ID"]) {
      wantedItemPosition = position;
      found = 1;
    }
  }
  // not sure you can do the wobbling in one go, so doing it like this to be sure lol.
  char dateBuffer[11];  // Buffer for the date format "DD/MM/YY"
  char timeBuffer[6];   // Buffer for the time format "MM:HH"
  // Format the time as "min:hour"
  strftime(timeBuffer, sizeof(timeBuffer), "%M:%H", &timeinfo);
  // Format the date as "Day/Month/last two digits of year"
  strftime(dateBuffer, sizeof(dateBuffer), "%d/%m/%y", &timeinfo);
  String TimeReturned = String(timeBuffer);
  String DateReturned = String(dateBuffer);

  bool Returned = JsonPackageReceived["Returned"];
  doc["TimeReturned"] = TimeReturned;
  doc["DateReturned"] = DateReturned;
  doc["Returned"] = Returned;
  position = 0;
  serializeJson(doc, Package);
  while (readJsonLittleFS(LittleFS, BorrowedItemsFile, ReadedJson, doc, position, READING_FILE, WITHOUT_JSON)) {
    if (position == wantedItemPosition) {
      appendFile(LittleFS, BorrowedItemscpyFile, (const char*)Package.c_str());
    } else {
      appendFile(LittleFS, BorrowedItemscpyFile, (const char*)ReadedJson.c_str());
    }
  }
  deleteFile(LittleFS, BorrowedItemsFile);
  renameFile(LittleFS, BorrowedItemscpyFile, BorrowedItemsFile);
  JsonDocument JsonPackagetoSend;
  JsonPackagetoSend["Returned"] = Returned;
  JsonPackagetoSend["TimeReturned"] = TimeReturned;
  JsonPackagetoSend["DateReturned"] = DateReturned;
  serializeJson(JsonPackagetoSend, Package);
}

void OP_ADDITEM_Handler(JsonDocument& JsonPackageReceived, String& Package) {  //WIP, need to figure android app first
  File file = LittleFS.open(BorrowedItemsFile, "r");
  if (!file) {
    Serial.println("Failed to open file");
    return;
  }
  size_t fileSize = file.size();
  file.close();
  if (fileSize < 3500) {
    appendFile(LittleFS, BorrowedItemsFile, (const char*)Package.c_str());
  } else {
    writeFile(LittleFS, BorrowedItemsFile, (const char*)Package.c_str());
  }
}

void OP_ADMIN_INFO_Handler(String& Package) {  //left for the end
  Package = "Wassup. this is supposed to be the info or smth. if you are reading this, you might be interested on who did this dumpsterfire of a code, or you are me. \n you can find him in discord as: caserefro";
}

void printTMStructure() {  // TESTING
  Serial.println("tm_sec (Seconds): " + String(timeinfo.tm_sec));
  Serial.println("tm_min (Minutes): " + String(timeinfo.tm_min));
  Serial.println("tm_hour (Hours): " + String(timeinfo.tm_hour));
  Serial.println("tm_mday (Day of Month): " + String(timeinfo.tm_mday));
  Serial.println("tm_mon (Month of Year): " + String(timeinfo.tm_mon));
  Serial.println("tm_year (Years since 1900): " + String(timeinfo.tm_year));
  Serial.println("tm_wday (Day of Week): " + String(timeinfo.tm_wday));
  Serial.println("tm_yday (Day of Year): " + String(timeinfo.tm_yday));
  Serial.println("tm_isdst (Daylight Saving Time Flag): " + String(timeinfo.tm_isdst));
}


void OP_ADMIN_SETTIME_Handler(JsonDocument& JsonPackageReceived, String& Package) {
  //Retrieve Json, and modify internal Struct. let gpt handle this one. too much copy. no need for reply. ACK probably is good.
  String tmBuffer = JsonPackageReceived["tm_struct"].as<String>();
  JsonDocument doc;
  deserializeJson(doc, tmBuffer);
  timeinfo.tm_sec = doc["tm_sec"];
  timeinfo.tm_min = doc["tm_min"];
  timeinfo.tm_hour = doc["tm_hour"];
  timeinfo.tm_mday = doc["tm_mday"];
  timeinfo.tm_mon = doc["tm_mon"];
  timeinfo.tm_year = doc["tm_year"];
  timeinfo.tm_wday = doc["tm_wday"];
  timeinfo.tm_yday = doc["tm_yday"];
  timeinfo.tm_isdst = doc["tm_isdst"];
  Package = "ACK";
  printTMStructure();
}

void OP_ADMIN_ALLSTUDENTS_Handler(String& Package) {
  JsonDocument doc;
  String ReadedJson = "";
  String Buffer;
  int position = 0;
  while (readJsonLittleFS(LittleFS, UsersFile, ReadedJson, doc, position, READING_FILE, WITH_JSON)) {
    Buffer = doc["Username"].as<String>();
    Package += Buffer;
    Package += ",";
  }
}

void OP_ADMIN_ADDSTUDENT_Handler(JsonDocument& JsonPackageReceived, String& Package) {
  String User = JsonPackageReceived["NewUserStruct"];
  appendFile(LittleFS, UsersFile, (const char*)User.c_str());
  readFile(LittleFS, UsersFile);
  Package = "ACK";
}

void OP_ADMIN_DELSTUDENT_Handler(JsonDocument& JsonPackageReceived, String& Package) {
  int Usertodelete = JsonPackageReceived["Username"];
  JsonDocument doc;
  String ReadedJson = "";
  int position = 0;
  int wantedUserPosition = 0;
  bool found = 0;
  while (readJsonLittleFS(LittleFS, UsersFile, ReadedJson, doc, position, MOVING_UP_FILE, WITH_JSON) && !found) {
    if (doc["Username"] == Usertodelete) {
      wantedUserPosition = position;
      found = 1;
    }
  }
  const char* UserscpyFile = "/Userscpy";
  while (readJsonLittleFS(LittleFS, UsersFile, ReadedJson, doc, position, READING_FILE, WITHOUT_JSON)) {
    if (position == wantedUserPosition) {
    } else {
      appendFile(LittleFS, UserscpyFile, (const char*)ReadedJson.c_str());
    }
  }
  deleteFile(LittleFS, UsersFile);
  renameFile(LittleFS, UserscpyFile, UsersFile);
  Package = "ACK";
}

void OP_ADMIN_RECORD_DUMP_Handler(String& Package) {
  //Dump all Records.
  JsonDocument doc;
  String ReadedJson = "";
  int position = 0;
  while (readJsonLittleFS(LittleFS, RecordFile, ReadedJson, doc, position, READING_FILE, WITHOUT_JSON)) {
    Package += ReadedJson;
  }
}

void OP_ANDROID_AUTH_Handler(JsonDocument& JsonPackageReceived, String& Package) {
  String UsertoAuth = JsonPackageReceived["Username"].as<String>();
  String PasswordOfUser = JsonPackageReceived["Password"].as<String>();
  JsonDocument doc;
  String ReadedJson = "";
  int position = 0;
  String Auth = "false";
  while (readJsonLittleFS(LittleFS, UsersFile, ReadedJson, doc, position, MOVING_UP_FILE, WITH_JSON) && !(Auth == "true")) {
    if (doc["Username"] == UsertoAuth) {
      if (doc["Password"] == PasswordOfUser) {
        Auth = "true";
      }
    }
  }
  Package += Auth;
}

void OP_ANDROID_LAYOUT_INFO_Handler(String& Package) {  //Pending
  //Dump all Records.
  JsonDocument doc;
  String ReadedJson = "";
  int position = 0;
  while (readJsonLittleFS(LittleFS, BorrowedItemsFile, ReadedJson, doc, position, READING_FILE, WITHOUT_JSON)) {
    Package += ReadedJson;
  }
}

void OP_ANDROID_LAYOUT_Handler(String& Package) {  //Pending
  //Dump all Records.
  JsonDocument doc;
  String ReadedJson = "";
  int position = 0;
  while (readJsonLittleFS(LittleFS, BorrowedItemsFile, ReadedJson, doc, position, READING_FILE, WITHOUT_JSON)) {
    Package += ReadedJson;
  }
}







#endif