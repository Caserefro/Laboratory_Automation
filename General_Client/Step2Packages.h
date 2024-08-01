#ifndef STEP2PACKAGES_H
#define STEP2PACKAGES_H

void Step2Package_OP_GENERAL_USE(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation) {  //Client used for request that dont add any special parameters.
  JsonPackagetoSend["Operation"] = Operation;
  String PackageBuffer = "";
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

void Step2Package_OP_ITEM_REQUEST(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int *LackingIDs) {
  JsonPackagetoSend["Operation"] = Operation;
  String PackageBuffer = "";
  JsonArray arr = JsonPackagetoSend.createNestedArray("LackingIDs");
  for (int i = 0; i < MAX_ITEMS - 1; i++) {
    if (LackingIDs[i] != 0) {
      arr.add(LackingIDs[i]);
    } else {
      break;
    }
  }
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

void UpdateBorrowedItems() {
  int LackingIDs[MAX_ITEMS];
  memset(LackingIDs, 0, sizeof(LackingIDs));
  Serial.println("FIRST OPERATION-------------------------------");
  OP_INFOITEMS_Wrapper(LackingIDs);
  Serial.println("SECOND OPERATION-------------------------------");
  if (!(LackingIDs[0] == 0 && LackingIDs[1] == 0 && LackingIDs[2] == 0 && LackingIDs[3] == 0))
    OP_ITEM_REQUEST_Wrapper(LackingIDs);
  printFirstFiveItems();
}

void Step2Package_OP_RETURNED(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int ItemID, int State) {  //Client
  JsonPackagetoSend["Operation"] = OP_RETURNED;
  JsonPackagetoSend["ItemID"] = ItemID;
  JsonPackagetoSend["State"] = State;
  String PackageBuffer = "";
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

void Step2Package_OP_AC(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int Command) {  //Client used for request that dont add any special parameters.
  JsonPackagetoSend["Operation"] = Operation;
  JsonPackagetoSend["Command"] = Command;
  String PackageBuffer = "";
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}


void Step2Package_OP_ADMIN_SETTIME(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, const tm &CurrentTime) {
  String PackageBuffer = "";
  JsonPackagetoSend["tm_sec"] = CurrentTime.tm_sec;
  JsonPackagetoSend["tm_min"] = CurrentTime.tm_min;
  JsonPackagetoSend["tm_hour"] = CurrentTime.tm_hour;
  JsonPackagetoSend["tm_mday"] = CurrentTime.tm_mday;
  JsonPackagetoSend["tm_mon"] = CurrentTime.tm_mon;
  JsonPackagetoSend["tm_year"] = CurrentTime.tm_year;
  JsonPackagetoSend["tm_wday"] = CurrentTime.tm_wday;
  JsonPackagetoSend["tm_yday"] = CurrentTime.tm_yday;
  JsonPackagetoSend["tm_isdst"] = CurrentTime.tm_isdst;
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

void Step2Package_OP_ADMIN_ADDSTUDENT(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation , const UserData &User) {
  String PackageBuffer = "";
  JsonPackagetoSend["UserID"] = User.UserID;
  JsonPackagetoSend["Username"] = User.Username;
  JsonPackagetoSend["Name"] = User.Name;
  JsonPackagetoSend["NCID"] = User.NCID;
  JsonPackagetoSend["EcPw"] = User.EcPw;
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

void Step2Package_OP_ADMIN_DELSTUDENT(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, String &Username) {
  String PackageBuffer = "";
  JsonPackagetoSend["Username"] = Username;
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

#endif