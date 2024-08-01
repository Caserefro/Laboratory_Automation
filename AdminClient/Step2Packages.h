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


void Step2Package_OP_ADMIN_SETTIME(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, const String &CurrentTime) {
  JsonPackagetoSend["Operation"] = Operation;
  String PackageBuffer = "";
  JsonPackagetoSend["tm_struct"] = CurrentTime;
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

void Step2Package_OP_ADMIN_ADDSTUDENT(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, const String &User) {
  JsonPackagetoSend["Operation"] = Operation;
  String PackageBuffer = "";
  JsonPackagetoSend["NewUserStruct"] = User;
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

void Step2Package_OP_ADMIN_DELSTUDENT(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, String &Username) {
  JsonPackagetoSend["Operation"] = Operation;
  String PackageBuffer = "";
  JsonPackagetoSend["Username"] = Username;
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

void Step2Package_OP_ANDROID_AUTH(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, const String &Username, const String &Password) {  //Client used for request that dont add any special parameters.
  JsonPackagetoSend["Operation"] = Operation;
  JsonPackagetoSend["Username"] = Operation;
  JsonPackagetoSend["Password"] = Password;
  String PackageBuffer = "";
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}
/*
void Step2Package_OP_ANDROID_LAYOUT(JsonDocument &JsonPackagetoSend, String &PackagetoSend, int Operation, int *LackingElements) {  //Client used for request that dont add any special parameters.
  JsonPackagetoSend["Operation"] = Operation;
  JsonPackagetoSend["Username"] = Operation;
  JsonPackagetoSend["Password"] = Password;
  String PackageBuffer = "";
  serializeJson(JsonPackagetoSend, PackageBuffer);
  Serial.println("PackageBuffer");
  Serial.println(PackageBuffer);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}
*/
#endif