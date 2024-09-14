#ifndef HTTP_HANDLERS_H
#define HTTP_HANDLERS_H

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


//SERVER FUNCTIONS ---------------------------------------------------------------------------------------------------------------------
void Step1Response(String &ReceivedPackage, String &PackagetoSend, String &SecondNonce) {  //Server
                                                                                           /* Receives Step 1 Package, Decrypyts, Separates the json, saves ID of sender, and Creates Response package (String). 
    Response Package is the EncryptedNonce1 + Nonce2 + Step 1
  */
  JsonDocument JsonPackageReceived;
  JsonDocument JsonPackagetoSend;
  String ReceivedPackageDecrypted = "";
  String EncryptedFirstNonce = "";
  String PackagetoSendBuffer = "";

  //From received Package Extract ID (confirm with queue IDs)

  decryptStringCBC(ReceivedPackage, key, ReceivedPackageDecrypted);
  Serial.println(ReceivedPackageDecrypted);
  deserializeJson(JsonPackageReceived, ReceivedPackageDecrypted);
  String FirstNonce = JsonPackageReceived["FirstNonce"];

  unsigned char SecondNonceArr[16];
  RandomNonceGenerator(SecondNonceArr);
  // converto nonce to base 64 (could be a function, but due to sizes being constant, this is faster.)
  unsigned char base64Output[25];  // Base64 encoding needs 22 chars for 16 bytes + null terminator (closest 4 multiple 24, so 25.)
  size_t outputLen = 0;
  int ret = mbedtls_base64_encode(base64Output, sizeof(base64Output), &outputLen, SecondNonceArr, 16);
  if (ret == 0) {
    base64Output[outputLen] = '\0';  // Null-terminate the string
    SecondNonce = String((char *)base64Output);
  } else {
    SecondNonce = "Error encoding base64";
    return;
  }

  encryptStringCBC(FirstNonce, key, EncryptedFirstNonce);
  JsonPackagetoSend["EncryptedFirstNonce"] = EncryptedFirstNonce;
  JsonPackagetoSend["SecondNonce"] = SecondNonce;
  serializeJson(JsonPackagetoSend, PackagetoSendBuffer);
  Serial.println(PackagetoSendBuffer);
  encryptStringCBC(PackagetoSendBuffer, key, PackagetoSend);
}

int Step2ResponseProcessing(String &ReceivedPackage, JsonDocument &JsonPackageReceived) {  //Server
                                                                                           /* Creates Package (String). 
    Creates Json object that will be used to determine the answer. */
  bool Authorized = false;
  String ReceivedPackageDecrypted = "";

  decryptStringCBC(ReceivedPackage, key, ReceivedPackageDecrypted);
  Serial.println(ReceivedPackageDecrypted);
  deserializeJson(JsonPackageReceived, ReceivedPackageDecrypted);
  String EncryptedSecondNonce = JsonPackageReceived["EncryptedSecondNonce"];
  String ReceivedSecondNonce = "";
  decryptStringCBC(EncryptedSecondNonce, key, ReceivedSecondNonce);

  if (ReceivedSecondNonce == ActiveNonce) {
    Authorized = true;
  }
  if (!Authorized) {
    return 0;
  }
  int OperationBuffer = JsonPackageReceived["Operation"];
  return OperationBuffer;
}


  void Step2ResponsePackage(String &Package, JsonDocument &JsonPackageReceived, int operation, int &AC_command) {
  JsonDocument JsonPackagetoSend;
  String PackageBuffer = "";
  switch (operation) {
    case OP_SERVER_PING:
      PackageBuffer = "PING";
      break;
    case OP_OPEN_CLOSE:
      PackageBuffer = "ACK";
      CreateRecord();
      //write record here, and communicate with door.  //pending.
      break;
    case OP_TIME:
      UpdateTimeinfo();
      OP_TIME_Handler(PackageBuffer);
      break;
    case OP_STATIONS_STATE:
      OP_STATIONS_STATE_Handler(PackageBuffer);
      break;
    case OP_WEATHER:
      OP_WEATHER_Handler(PackageBuffer);
      break;

    case OP_INFOITEMS:
      OP_INFOITEMS_Handler(PackageBuffer);
      break;
    case OP_ITEM_REQUEST:
      OP_ITEM_REQUEST_Handler(JsonPackageReceived, PackageBuffer);  //pending
      break;
    case OP_RETURNED:
      OP_RETURNED_Handler(JsonPackageReceived, PackageBuffer);
      break;
    case OP_ADDITEM:
      //pending, this one will only interact with android, so idk yet bout it.
      OP_ADDITEM_Handler(JsonPackageReceived, PackageBuffer);
      break;

    case OP_AC:
      OP_AC_Handler(JsonPackageReceived, PackageBuffer, AC_command);
      break;

    case OP_ADMIN_INFO:
      OP_ADMIN_INFO_Handler(PackageBuffer);
      break;
    case OP_ADMIN_SETTIME:
      OP_ADMIN_SETTIME_Handler(JsonPackageReceived, PackageBuffer);
      break;
    case OP_ADMIN_ALLSTUDENTS:
      OP_ADMIN_ALLSTUDENTS_Handler(PackageBuffer);
      break;
    case OP_ADMIN_ADDSTUDENT:
      OP_ADMIN_ADDSTUDENT_Handler(JsonPackageReceived, PackageBuffer);
      break;
    case OP_ADMIN_DELSTUDENT:
      OP_ADMIN_DELSTUDENT_Handler(JsonPackageReceived, PackageBuffer);
      break;
    case OP_ADMIN_RECORD_DUMP:
      OP_ADMIN_RECORD_DUMP_Handler(PackageBuffer);
      break;

    case OP_ANDROID_AUTH:
      OP_ANDROID_AUTH_Handler(JsonPackageReceived, PackageBuffer);
      break;
    case OP_ANDROID_LAYOUT_INFO:
      OP_ANDROID_LAYOUT_INFO_Handler(PackageBuffer);
      break;
    case OP_ANDROID_LAYOUT:
      OP_ANDROID_LAYOUT_Handler(PackageBuffer);
      break;
  }
  encryptStringCBC(PackageBuffer, key, Package);
}

void Step1Handle() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    Serial.println("Received body: S1");
    Serial.println(body);
    String Package = "";
    Step1Response(body, Package, ActiveNonce);
    server.send(200, "plain", Package);
  } else {
    server.send(400, "plain", "No body received");
  }
}

void Step2Handle() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    Serial.println("Received body:   S2");
    Serial.println(body);
    String Package = "";
    int operation = 0;
    int AC_command = 0;
    JsonDocument JsonPackageReceived;
    operation = Step2ResponseProcessing(body, JsonPackageReceived);
    Serial.println(operation);
    if (operation == 0) {
      server.send(200, "plain", "Nonce not authorized.");
    } else {
      Step2ResponsePackage(Package, JsonPackageReceived, operation, AC_command);
      server.send(200, "plain", Package);
      if (operation == OP_AC) {
        Serial.println(AC_command);
        //function that send message trough BLE.
      }
      if (operation == OP_OPEN_CLOSE) {
        //OPEN da goddammed door.
      }
    }
  } else {
    server.send(400, "plain", "No body received");
  }
}

#endif