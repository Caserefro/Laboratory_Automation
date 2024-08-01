#ifndef HTTP_FUNCTIONS_H
#define HTTP_FUNCTIONS_H


void RandomNonceGenerator(unsigned char nonce[16]) {
  for (int i = 0; i < 16; i++) {
    nonce[i] = (unsigned char)esp_random();
  }
}

bool encryptStringCBC(const String &plainText, const char *key, String &OutputString) {
  unsigned char IVcopy[16];
  unsigned char IV[16];
  for (int i = 0; i < 16; i++) {
    IV[i] = (unsigned char)esp_random();
  }
  memcpy(IVcopy, IV, 16);

  //  memset(IVcopy, 0x0B, sizeof(IVcopy));
  //  memset(IV, 0x0B, sizeof(IVcopy));

  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char *)key, strlen(key) * 8);

  int slices = (plainText.length() + 15) / 16;
  unsigned char plainTextBuffer[(slices * 16)];       //LOAD OF MEMORY HERE
  unsigned char EncryptedBuffer[(slices * 16) + 16];  //LOAD OF MEMORY HERE
  memcpy(plainTextBuffer, plainText.c_str(), plainText.length());

  int bytestopadd = sizeof(plainTextBuffer) - plainText.length();
  if (bytestopadd) {
    for (int i = ((slices)*16) - (bytestopadd); i < sizeof(plainTextBuffer); i++) {
      plainTextBuffer[i] = (unsigned char)bytestopadd;
    }
  }

  memset(EncryptedBuffer, 0, sizeof(EncryptedBuffer));
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, slices * 16, IVcopy, plainTextBuffer, EncryptedBuffer);
  mbedtls_aes_free(&aes);
  memmove(EncryptedBuffer + 16, EncryptedBuffer, sizeof(EncryptedBuffer) - 16);
  memcpy(EncryptedBuffer, IV, 16);


  int base64Size = ((((double)sizeof(EncryptedBuffer) * 4 / 3) + 3) / 4) * 4;
  unsigned char base64Output[base64Size + 20];
  memset(base64Output, 0, sizeof(base64Output));
  size_t outputLen = 0;

  int ret = mbedtls_base64_encode(base64Output, sizeof(base64Output), &outputLen, (const unsigned char *)EncryptedBuffer, sizeof(EncryptedBuffer));
  if (ret == 0) {
    base64Output[outputLen] = '\0';  // Null-terminate the string
    OutputString = String((char *)base64Output);
    //Serial.println(OutputString);
    return 1;
  } else {
    OutputString = "Error encoding base64";
    //Serial.println(OutputString);
    return 0;
  }
  return 1;
}

bool decryptStringCBC(const String &cipherText, const char *key, String &OutputString) {
  unsigned char IV[16];
  unsigned char DecodedTextBuffer[(cipherText.length() * 3 / 4) + 8];
  memset(DecodedTextBuffer, 0, sizeof(DecodedTextBuffer));
  size_t outputLen = 0;
  // Convert to Base64 string
  int ret = mbedtls_base64_decode(DecodedTextBuffer, sizeof(DecodedTextBuffer), &outputLen, (const unsigned char *)cipherText.c_str(), cipherText.length());
  if (ret != 0) {
    OutputString = "Error decoding base64";
    Serial.print(OutputString);
    return false;
  }

  memcpy(IV, DecodedTextBuffer, 16);
  size_t DecryptedBufferSize = ((outputLen - 15) / 16) * 16;
  unsigned char DecryptedBuffer[DecryptedBufferSize];  // outputLen + null so it can become string.

  memmove(DecodedTextBuffer, DecodedTextBuffer + 16, outputLen);

  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char *)key, strlen(key) * 8);
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, DecryptedBufferSize, IV, (const unsigned char *)DecodedTextBuffer, DecryptedBuffer);
  mbedtls_aes_free(&aes);

  int PaddingToRemove = 0;
  int counter = 1;
  int supposed = (int)DecryptedBuffer[DecryptedBufferSize - 1];
  int newlength = 0;
  for (int i = 0; i < 14; i++) {
    if (DecryptedBuffer[DecryptedBufferSize - 2 - i] == supposed) {
      counter++;
    } else {
      break;
    }
  }
  if (counter == supposed) {
    newlength = DecryptedBufferSize - counter;
    memset(DecryptedBuffer + DecryptedBufferSize - counter, 0, counter);
  }
  OutputString = "";
  for (size_t i = 0; i < newlength; ++i) {
    OutputString += (char)DecryptedBuffer[i];
  }
  return 1;
}

bool postData(String &serverName, String &payload, String &response) {
  if (WiFi.status() == WL_CONNECTED) {  // Check WiFi connection status
    HTTPClient http;
    http.begin(serverName);  // Specify the URL
    http.addHeader("Content-Type", "text/plain");
    // Send HTTP POST request
    int httpResponseCode = http.POST(payload);
    Serial.println(httpResponseCode);
    // Check the returning code
    if (httpResponseCode > 0) {
      response = http.getString();  // Get the response to the request
                                    // Serial.println(response);     // Print request answer
      http.end();                   // Free resources
      return 1;
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
      http.end();  // Free resources
      return 0;
    }

    http.end();  // Free resources
  } else {
    Serial.println("Error in WiFi connection");

    return 0;
  }
  return 1;
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
        //  Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
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



#endif