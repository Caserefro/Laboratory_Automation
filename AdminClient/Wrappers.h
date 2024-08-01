#ifndef WRAPPERS_H
#define WRAPPERS_H

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

void OP_ADMIN_INFO_Wrapper() {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_ADMIN_INFO);
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

void OP_ADMIN_SETTIME_Wrapper(const String &CurrentTime) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_ADMIN_SETTIME(doc, Package, OP_ADMIN_SETTIME, CurrentTime);
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

void OP_ADMIN_ALLSTUDENTS_Wrapper() {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_ADMIN_ALLSTUDENTS);
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

void OP_ADMIN_ADDSTUDENT_Wrapper(const String &User) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_ADMIN_ADDSTUDENT(doc, Package, OP_ADMIN_ADDSTUDENT, User);
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

void OP_ADMIN_DELSTUDENT_Wrapper(String &Username) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_ADMIN_DELSTUDENT(doc, Package, OP_ADMIN_DELSTUDENT, Username);
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

void OP_ADMIN_RECORD_DUMP_Wrapper() {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_ADMIN_RECORD_DUMP);
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


void OP_ANDROID_AUTH_Wrapper(const String &Username, const String &Password) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_ANDROID_AUTH(doc, Package, OP_ANDROID_AUTH, Username, Password);
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

void OP_ANDROID_LAYOUT_INFO_Wrapper(int *LackingIDs) {
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_ANDROID_LAYOUT_INFO);
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
  Serial.println(response);
  return;
}


void OP_ANDROID_LAYOUT_Wrapper() {  // this one is a kotlin function, very similar to OP_ITEM_REQUEST_Wrapper
  String response = "";
  String responsebuffer = "";
  String Package = "";
  String OldNonce = "";
  JsonDocument doc;
  Step1Package(Package, OldNonce);
  if (postData(serverNameS1, Package, response)) {
    Step2PackageCore(response, doc, OldNonce);
    Step2Package_OP_GENERAL_USE(doc, Package, OP_ADMIN_ALLSTUDENTS);
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



#endif