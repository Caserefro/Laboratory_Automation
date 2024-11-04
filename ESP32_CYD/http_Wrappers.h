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
  if (postData(Step1Adr, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_SERVER_PING);
    if (postData(Step2Adr, Package, responsebuffer)) {
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

void OP_E_DEVICE_SYNC_Wrapper(int ID_DEVICE, String &DEVICE_TYPE) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(Step1Adr, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_E_DEVICE_SYNC(doc, Package, OP_E_DEVICE_SYNC, ID_DEVICE, DEVICE_TYPE);
    if (postData(Step2Adr, Package, responsebuffer)) {
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
  JsonDocument doc;
  httpGETRequest(TimeAdr, response);
  deserializeJson(doc, response);
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

void OP_OPEN_CLOSE_Wrapper(bool Open) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(Step1Adr, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_OPEN_CLOSE(doc, Package, OP_OPEN_CLOSE, Open);
    if (postData(Step2Adr, Package, responsebuffer)) {
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

void OP_WEATHER_Wrapper() {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  httpGETRequest(WeatherAdr, response);
  deserializeJson(doc, response);
  ActiveWeatherData.WindSpd = doc["WindSpd"];
  String WindDirBuffer = doc["WindDir"].as<String>();
  ActiveWeatherData.WindDir = WindDirBuffer;
  ActiveWeatherData.UVidx = doc["UVidx"];
  ActiveWeatherData.Temp = doc["Temp"];
  ActiveWeatherData.RainG = doc["RainG"];
  ActiveWeatherData.RelHum = doc["RelHum"];
  return;
}

void OP_BORROWEDITEMS_INFO_Wrapper(int *LackingIDs) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(Step1Adr, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_BORROWEDITEMS_INFO);
    if (postData(Step2Adr, Package, responsebuffer)) {
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

void OP_BORROWEDITEMS_REQUEST_Wrapper(int *LackingIDs) {  // Modified and cleaner version
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  DynamicJsonDocument doc(4096);  // Assuming response isn't too large; increase if needed

  // Step 1: Create and send the first package
  Step1Package(Package, OldNonce);
  if (postData(Step1Adr, Package, response)) {
    DynamicJsonDocument newdoc(4096);
    String newPackage = "";

    // Step 2: Handle response and prepare for step 2
    Step2PackageCore(response, newdoc, OldNonce);
    Step2Package_OP_BORROWEDITEMS_REQUEST(newdoc, newPackage, OP_BORROWEDITEMS_REQUEST, LackingIDs);
    Serial.println(newPackage);

    // Step 3: Post data and decrypt response
    if (postData(Step2Adr, newPackage, responsebuffer)) {
      decryptStringCBC(responsebuffer, key, response);
      Serial.println(response);

      // Deserialize the entire JSON response
      DeserializationError error = deserializeJson(doc, response);
      if (error) {
        Serial.print("JSON Deserialization failed: ");
        Serial.println(error.c_str());
        return;
      }
    } else {
      Serial.println("Error posting data for step 2");
      return;
    }
  } else {
    Serial.println("Error posting data for step 1");
    return;
  }

  // Step 4: Extract the "Items" array from the JSON response
  JsonArray itemsArray = doc["Items"].as<JsonArray>();
  if (itemsArray.isNull()) {
    Serial.println("No 'Items' array found in the response");
    return;
  }

  // Step 5: Iterate through the "Items" array and process each object
  for (JsonObject itemObj : itemsArray) {
    BorrowedItem buffer;
    buffer.ItemID = itemObj["ItemID"];
    buffer.Item = itemObj["Item"].as<String>();
    buffer.Name = itemObj["Name"].as<String>();
    buffer.NCID = itemObj["NCID"];
    buffer.Time = itemObj["Time"].as<String>();
    buffer.Date = itemObj["Date"].as<String>();
    buffer.Returned = itemObj["Returned"];
    buffer.TimeReturned = itemObj["TimeReturned"].as<String>();
    buffer.DateReturned = itemObj["DateReturned"].as<String>();

    // Add the item to your system (replace addItem with your actual function)
    addItem(buffer);
  }
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

void OP_BORROWED_ITEM_CHANGE_Wrapper(int ItemID, int State) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(Step1Adr, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_BORROWED_ITEM_CHANGE(doc, Package, ItemID, State);
    if (postData(Step2Adr, Package, responsebuffer)) {
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
  if (postData(Step1Adr, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_AC(doc, Package, OP_AC, Command);
    if (postData(Step2Adr, Package, responsebuffer)) {
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