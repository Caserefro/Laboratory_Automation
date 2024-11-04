#ifndef SERVER_HANDLERS_H
#define SERVER_HANDLERS_H

//SERVER FUNCTIONS FOR LOCKMODULE FUNCTIONALITY---------------------------------------------------------------------------------------------------------------------
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


void Step2ResponsePackage(String &Package, JsonDocument &JsonPackageReceived, int operation) {
  JsonDocument JsonPackagetoSend;
  String PackageBuffer = "";
  switch (operation) {  // Code left in case extra functionality is planned.
    case OP_OPEN_CLOSE:
      PackageBuffer = "ACK";
      //write record here, and communicate with door.  //pending.
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
    JsonDocument JsonPackageReceived;
    operation = Step2ResponseProcessing(body, JsonPackageReceived);
    Serial.println(operation);
    Step2ResponsePackage(Package, JsonPackageReceived, operation);
    if (operation == OP_OPEN_CLOSE) {
      server.send(200, "plain", Package);
      Serial.println("OPEN DA GODDAMMED DOOOR BOY.");
    }else {
          server.send(200, "plain", "This device is not capable of performing this operation");
    }

  } else {
    server.send(400, "plain", "No body received");
  }
}



#endif