#ifndef LITTLEFSFUNCTIONS_H
#define LITTLEFSFUNCTIONS_H

bool readJsonLittleFS(fs::FS &fs, const char *dirname, String &ReadedText, JsonDocument &json, int &position, int ReadMode = READING_FILE, int JsonMode = WITHOUT_JSON) {
  // could be swapped by function that actually recognizes when something is not a json, but this is fine as is now.
  File file = fs.open(dirname);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return 0;
  }
  ReadedText = "";
  String Buffer = "";
  file.seek(position);
  Buffer = file.readStringUntil('}');
  if (Buffer == NULL) {
    return 0;
  }
  Buffer += "}";
  if (ReadMode == READING_FILE) {
    ReadedText += Buffer;
    Serial.println(ReadedText);
  }
  if (JsonMode == WITH_JSON) {
    //Return Json object populated with readed Text file.
    deserializeJson(json, Buffer);
  }

  position = file.position();
  file.close();
  return 1;
}

void TotalJsoninFile(fs::FS &fs, const char *dirname) {
  //known bugs: if file contains something,  but is not a json, it will add 1, when it shouldnt. But is good enough ig.
  int counter = 0;
  String Dummy;
  JsonDocument doc;
  int position = 0;
  while (readJsonLittleFS(LittleFS, BorrowedItemsFile, Dummy, doc, position, MOVING_UP_FILE, WITHOUT_JSON)) {
    counter++;
  }

  if (dirname == "/BorrowedItems") {
    totalAmountofItemsBorrowed = counter;
    NumPagesItems = (((double)totalAmountofItemsBorrowed + 9) / 10);
    preferences.putInt("NumPagesItems", NumPagesItems);
    preferences.putInt("totalAmountofItemsBorrowed", totalAmountofItemsBorrowed);
  }
  if (dirname == "/Users") {
    totalAmountofUsers = counter;
    preferences.putInt("totalAmountofUsers", totalAmountofUsers);
  }
  if (dirname == "/Record") {
    totalAmountofRecords = counter;
    preferences.putInt("totalAmountofRecords", totalAmountofRecords);
  }
  return;
}

void UpdateIDList(fs::FS &fs, const char *dirname) {
  String Dummy;
  int position = 0;
  JsonDocument doc;
  int counter = 0;
  while (readJsonLittleFS(fs, dirname, Dummy, doc, position, MOVING_UP_FILE, WITH_JSON)) {
    borrowedItemsIDs[counter] = doc["ItemID"];
  }
}

void BiggestItemID(fs::FS &fs, const char *dirname) {
  String Dummy;
  int position = 0;
  JsonDocument doc;
  while (readJsonLittleFS(fs, dirname, Dummy, doc, position, MOVING_UP_FILE, WITH_JSON)) {
    if (doc["ItemID"] > biggestItemID) {
      biggestItemID = doc["ItemID"];
    }
    if (doc["ItemID"] < SmallestItemID) {
      SmallestItemID = doc["ItemID"];
    }
  }
  preferences.putInt("biggestItemID", biggestItemID);
  preferences.putInt("SmallestItemID", SmallestItemID);
  return;
}

void readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("- file renamed");
  } else {
    Serial.println("- rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}

//TESTING ---------------------------

void serializeUserDataStruct(const UserData &user, JsonObject &json, String &Output) {
  json["UserID"] = user.UserID;
  json["Username"] = user.Username;
  json["Name"] = user.Name;
  json["NCID"] = user.NCID;
  json["EcPw"] = user.EcPw;
  serializeJson(json, Output);
}

void serializeBorrowedItemStruct(const BorrowedItem &item, JsonDocument &json, String &Output) {
  json["ItemID"] = item.ItemID;
  json["Item"] = item.Item;
  json["Name"] = item.Name;
  json["NCID"] = item.NCID;
  json["Time"] = item.Time;
  json["Date"] = item.Date;
  json["Returned"] = item.Returned;
  json["TimeReturned"] = item.TimeReturned;
  json["DateReturned"] = item.DateReturned;
  serializeJson(json, Output);
}


void TestingSetup(fs::FS &fs) {
  JsonDocument borrowedItemIE1Json;
  JsonDocument borrowedItemIE2Json;
  JsonDocument borrowedItemIE3Json;
  String borrowedItemIE1String = "";
  String borrowedItemIE2String = "";
  String borrowedItemIE3String = "";

  serializeBorrowedItemStruct(borrowedItemIE1, borrowedItemIE1Json, borrowedItemIE1String);
  serializeBorrowedItemStruct(borrowedItemIE2, borrowedItemIE2Json, borrowedItemIE2String);
  serializeBorrowedItemStruct(borrowedItemIE3, borrowedItemIE3Json, borrowedItemIE3String);
  appendFile(LittleFS, BorrowedItemsFile, (const char *)borrowedItemIE1String.c_str());
  appendFile(LittleFS, BorrowedItemsFile, (const char *)borrowedItemIE2String.c_str());
  appendFile(LittleFS, BorrowedItemsFile, (const char *)borrowedItemIE3String.c_str());
  readFile(LittleFS, BorrowedItemsFile);
}

void SettingupPreferences() {
  totalAmountofUsers = preferences.getInt("totalAmountofUsers", 0);
  totalAmountofRecords = preferences.getInt("totalAmountofRecords", 0);
}


#endif