#ifndef HTTP_WRAPPERS_H
#define HTTP_WRAPPERS_H
/*
void addItem(const BorrowedItem &item) {
  // Shift items up
  for (int j = MAX_ITEMS - 1; j > 0; j--) {
    borrowedItems[j] = borrowedItems[j - 1];
  }
  borrowedItems[0] = item;  // Add new item at the beginning
}
*/

void OP_SERVER_PING_Wrapper() {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_SERVER_PING);
    if (postData(serverNameS2, Package, responsebuffer)) {
      decryptStringCBC(responsebuffer, key, response);
    } else {
      response = "error posting data";
      return;
    }
  } else {
    response = "error posting data";
  }
  Serial.println(response);
  return;
}

void OP_OPEN_CLOSE_Wrapper() {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_OPEN_CLOSE);
    if (postData(serverNameS2, Package, responsebuffer)) {
      decryptStringCBC(responsebuffer, key, response);
    } else {
      response = "error posting data";
      return;
    }
  } else {
    response = "error posting data";
  }
  Serial.println(response);
  return;
}

void OP_TIME_Wrapper() {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  httpGETRequest();

  timeinfo.tm_sec = doc["tm_sec"];
  timeinfo.tm_min = doc["tm_min"];
  timeinfo.tm_hour = doc["tm_hour"];
  timeinfo.tm_mday = doc["tm_mday"];
  timeinfo.tm_mon = doc["tm_mon"];
  timeinfo.tm_year = doc["tm_year"];
  timeinfo.tm_wday = doc["tm_wday"];
  timeinfo.tm_yday = doc["tm_yday"];
  timeinfo.tm_isdst = doc["tm_isdst"];
  return;
}

void OP_WEATHER_Wrapper() {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_WEATHER);
    if (postData(serverNameS2, Package, responsebuffer)) {
      decryptStringCBC(responsebuffer, key, response);
      Serial.println(response);
      deserializeJson(doc, response);
    } else {
      response = "error posting data";
      return;
    }
  } else {
    response = "error posting data";
  }
  WeatherData.WindSpd = doc["WindSpd"];
  String WindDirBuffer = doc["WindDir"].as<String>();
  WeatherData.WindDir = WindDirBuffer;
  WeatherData.UVidx = doc["UVidx"];
  WeatherData.Temp = doc["Temp"];
  WeatherData.RainG = doc["RainG"];
  WeatherData.RelHum = doc["RelHum"];
  return;
}

void OP_STATIONS_STATE_Wrapper() {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_STATIONS_STATE);
    if (postData(serverNameS2, Package, responsebuffer)) {
      decryptStringCBC(responsebuffer, key, response);
      Serial.println(response);
      deserializeJson(doc, response);
    } else {
      response = "error posting data";
      return;
    }
  } else {
    response = "error posting data";
  }
  StationsState.Station1 = doc["Station1"];
  StationsState.Station2 = doc["Station2"];
  return;
}

void OP_INFOITEMS_Wrapper(int *LackingIDs) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_INFOITEMS);
    if (postData(serverNameS2, Package, responsebuffer)) {
      decryptStringCBC(responsebuffer, key, response);
      Serial.println(response);
      deserializeJson(doc, response);
    } else {
      response = "error posting data";
      return;
    }
  } else {
    response = "error posting data";
  }
  int outputSize = doc["ServerIDs"].size();
  int discrepancyCounter = 0;

  for (int i = 0; i < outputSize; i++) {
    bool coincidence = false;  // Reset for each new item
    for (int j = 0; j < MAX_ITEMS; j++) {
      if (borrowedItems[j].ItemID == doc["ServerIDs"][i]) {
        coincidence = true;
        break;  // Exit inner loop early if a match is found
      }
    }
    if (!coincidence) {
      LackingIDs[discrepancyCounter++] = doc["ServerIDs"][i];
    }
  }
}


void OP_ITEM_REQUEST_Wrapper(int *LackingIDs) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    JsonDocument newdoc;
    String newPackage = "";
    Step2PackageCore(response, newdoc, OldNonce);
    Step2Package_OP_ITEM_REQUEST(newdoc, newPackage, OP_ITEM_REQUEST, LackingIDs);
    Serial.println(newPackage);
    if (postData(serverNameS2, newPackage, responsebuffer)) {
      decryptStringCBC(responsebuffer, key, response);
    } else {
      response = "error posting data";
      return;
    }
  } else {
    response = "error posting data";
  }
  Serial.println("response OP_ITEM_REQUEST_Wrapper ");
  Serial.println(response);
  JsonDocument ResponseBuffer;
  String SingleStructBuffer = "";
  int startIndex = 0;
  int endIndex = response.indexOf('}');
  while (endIndex != -1) {
    String SingleStructBuffer = response.substring(startIndex, endIndex + 1);  // Adjusted to include the closing brace
    Serial.println(SingleStructBuffer);
    startIndex = endIndex + 1;
    endIndex = response.indexOf('}', startIndex);
    deserializeJson(ResponseBuffer, SingleStructBuffer);
    BorrowedItem buffer;
    buffer.ItemID = ResponseBuffer["ItemID"];
    buffer.Item = ResponseBuffer["Item"].as<String>();
    buffer.Name = ResponseBuffer["Name"].as<String>();
    buffer.NCID = ResponseBuffer["NCID"];
    buffer.Time = ResponseBuffer["Time"].as<String>();
    buffer.Date = ResponseBuffer["Date"].as<String>();
    buffer.Returned = ResponseBuffer["Returned"];
    buffer.TimeReturned = ResponseBuffer["TimeReturned"].as<String>();
    buffer.DateReturned = ResponseBuffer["DateReturned"].as<String>();
    addItem(buffer);
  }
  return;
}

void addItem(const BorrowedItem &item) {
  // Shift items up
  for (int j = MAX_ITEMS - 1; j > 0; j--) {
    borrowedItems[j] = borrowedItems[j - 1];
  }
  borrowedItems[0] = item;  // Add new item at the beginning
}

bool removeItem(int itemID) {
  for (int i = 0; i < itemCount; ++i) {
    if (borrowedItems[i].ItemID == itemID) {
      for (int j = i; j < itemCount - 1; ++j) {
        borrowedItems[j] = borrowedItems[j + 1];
      }
      --itemCount;
      return true;
    }
  }
  Serial.println("Item not found.");
  return false;
}

BorrowedItem *findItem(int itemID) {
  for (int i = 0; i < itemCount; ++i) {
    if (borrowedItems[i].ItemID == itemID) {
      return &borrowedItems[i];
    }
  }
  return nullptr;
}

void OP_RETURNED_Wrapper(int ItemID, int State) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_RETURNED(doc, Package, ItemID, State);
    if (postData(serverNameS2, Package, responsebuffer)) {
      decryptStringCBC(responsebuffer, key, response);
      JsonDocument ReceivedJson;
      deserializeJson(ReceivedJson, response);
      for (int i = 0; i < MAX_ITEMS; ++i) {
        if (borrowedItems[i].ItemID == ReceivedJson["ItemID"]) {
          borrowedItems[i].Returned = ReceivedJson["Returned"];
          borrowedItems[i].TimeReturned = ReceivedJson["TimeReturned"].as<String>();
          borrowedItems[i].DateReturned = ReceivedJson["DateReturned"].as<String>();
        }
      }
    } else {
      response = "error posting data";
      return;
    }
  } else {
    response = "error posting data";
  }
  Serial.println(response);
  return;
}

void OP_AC_Wrapper(int Command) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_AC(doc, Package, OP_AC, Command);
    if (postData(serverNameS2, Package, responsebuffer)) {
      decryptStringCBC(responsebuffer, key, response);
    } else {
      response = "error posting data";
      return;
    }
  } else {
    response = "error posting data";
  }
  Serial.println(response);  // do the
  return;
}


#endif