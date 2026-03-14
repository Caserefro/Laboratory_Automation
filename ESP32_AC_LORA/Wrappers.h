#ifndef WRAPPERS_H
#define WRAPPERS_H

void OP_TIME_Handler(String &Package) {
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

void OP_WEATHER_Handler(String &Package) {
  JsonDocument JsonPackagetoSend;
  JsonPackagetoSend["WindSpd"] = WeatherData.WindSpd;
  JsonPackagetoSend["WindDir"] = WeatherData.WindDir;
  JsonPackagetoSend["UVidx"] = WeatherData.UVidx;
  JsonPackagetoSend["Temp"] = WeatherData.Temp;
  JsonPackagetoSend["RainG"] = WeatherData.RainG;
  JsonPackagetoSend["RelHum"] = WeatherData.RelHum;
  serializeJson(JsonPackagetoSend, Package);
}

void OP_STATIONS_STATE_Handler(String &Package) {
  JsonDocument JsonPackagetoSend;
  JsonPackagetoSend["Station1"] = StationsState.Station1;
  JsonPackagetoSend["Station2"] = StationsState.Station2;
  serializeJson(JsonPackagetoSend, Package);
}

void OP_AC_Handler(JsonDocument &JsonPackageReceived, String &Package, int &Command) {
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


void httpGETRequest(String &serverName, String &payload) {  //Not used.
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(serverName);  //HTTP
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}

bool postData(String &serverName, String &payload, String &response) {
  if (WiFi.status() == WL_CONNECTED) {  // Check WiFi connection status
    Serial.println("Error in WiFi connection");
    return 0;
  }
  HTTPClient http;
  http.begin(serverName);  // Specify the URL
  http.addHeader("Content-Type", "text/plain");
  // Send HTTP POST request
  int httpResponseCode = http.POST(payload);
  Serial.println(httpResponseCode);
  // Check the returning code
  if (httpResponseCode > 0) {
    response = http.getString();  // Get the response to the request

    Serial.println(response);  // Print request answer
    http.end();                // Free resources
    return 1;
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    http.end();  // Free resources
    return 0;
  }

  http.end();  // Free resources
  return 1;
}


void Step1Package(String &PackagetoSend, String &FirstNonce) {  //Client
  // Modifies @PackagetoSend with json architechture, with a random nonce, + ID + Step.

  JsonDocument JsonPackage;
  String PackageBuffer = "";
  String nonceStr = "";
  unsigned char FirstNonceArr[16];
  unsigned char base64Output[25];  // Base64 encoding needs 22 chars for 16 bytes + null terminator (closest 4 multiple 24, so 25.)
  RandomNonceGenerator(FirstNonceArr);
  // converto FirstNonceArr to base 64 (could be a function, but due to sizes being constant, this is faster.)
  size_t outputLen = 0;
  int ret = mbedtls_base64_encode(base64Output, sizeof(base64Output), &outputLen, FirstNonceArr, 16);
  if (ret == 0) {
    base64Output[outputLen] = '\0';  // Null-terminate the string
    nonceStr = String((char *)base64Output);
  } else {
    nonceStr = "Error encoding base64";
    return;
  }

  JsonPackage["FirstNonce"] = nonceStr;
  //  JsonPackage["ID"] = ID;
  serializeJson(JsonPackage, PackageBuffer);
  // encryptStringCBC(nonceStr, key, FirstNonce);  //test
  FirstNonce = nonceStr;
  Serial.println("PackageBuffer ");
  Serial.println(PackageBuffer);
  // Serial.println("FirstNonce ");
  // Serial.println(FirstNonce);
  encryptStringCBC(PackageBuffer, key, PackagetoSend);  // Encrypts Package for sending.
}

bool Step2PackageCore(String &ReceivedPackage, JsonDocument &JsonPackagetoSend, String &FirstNonce) {  //Client
  /* Creates Package (String). 
    Package is EncryptedNonce2 + ID + Step + Operation 
    For Borrowed Operation, options available are: 
      ELEMENTS:
        Returns:  Num Pages available + Total Elements 
        Requires: Nothing 
      PAGE:
        Returns: Structs of elements borrowed. of that of that page. (each page consists of 10 elements.each element is a struct in json format. )
        Requires: Page Number. 
      Return:
        Returns: Echo of data of the item 
        Requieres: ItemID

    For Open/Close Operation only one option is available. 
      OPENCLOSE:
        Returns: Nothing
        Requires: Nothing 
    For Admin Operation, options available are:
      LOGIN
        Returns: State of user. 
        Requires: Admin User and Password 
      SETTIME 
        Returns: Nothing
        Requires:  Admin User and Password, Struct of time tm, in json format.  
      ADDUSER
        Returns: Echo of data Sended.
        Requires:  Admin User and Password, Struct of Student to add, in json format.  
      DELUSER
        Returns: Echo of data Sended.
        Requires:  Admin User and Password, Struct of Student to delete, in json format.  
      INFO
        Returns: Info of the system, along with version.
        Requires:  Nothing
    */


  String ReceivedPackageDecrypted = "";
  String ReceivedDecryptedNonce = "";
  JsonDocument JsonPackageReceived;

  decryptStringCBC(ReceivedPackage, key, ReceivedPackageDecrypted);
  deserializeJson(JsonPackageReceived, ReceivedPackageDecrypted);
  Serial.println(ReceivedPackageDecrypted);
  String EncryptedFirstNonce = JsonPackageReceived["EncryptedFirstNonce"];
  decryptStringCBC(EncryptedFirstNonce, key, ReceivedDecryptedNonce);


  if (ReceivedDecryptedNonce == FirstNonce) {
    String EncryptedSecondNonce = "";
    String SecondNonce = JsonPackageReceived["SecondNonce"].as<String>();
    encryptStringCBC(SecondNonce, key, EncryptedSecondNonce);
    JsonPackagetoSend["EncryptedSecondNonce"] = EncryptedSecondNonce;
    // JsonPackagetoSend["ID"] = ID;
    return true;
  } else {
    return false;
  }
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



#endif