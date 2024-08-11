#ifndef STEP2_PACKAGES_H
#define STEP2_PACKAGES_H


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


#endif