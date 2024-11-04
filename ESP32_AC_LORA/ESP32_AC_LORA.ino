#include "ESP32_AC_LORA.h"

void setup() {
  Serial.begin(115200);
  pinMode(IRLEDpin, OUTPUT);
  digitalWrite(IRLEDpin, LOW);  //turn off IR LED to start
  // Configures static IP address
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  OP_E_DEVICE_SYNC_Wrapper(ID_DEVICE1, DEVICE_TYPE1);
  OP_E_DEVICE_SYNC_Wrapper(ID_DEVICE2, DEVICE_TYPE2);
  server.on("/S1", HTTP_POST, Step1Handle);
  server.on("/S2", HTTP_POST, Step2Handle);
  server.begin();
}

void loop() {
  server.handleClient();
}
