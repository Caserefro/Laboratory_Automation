#include "ESP32_MASTER.h"

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
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  if (!preferences.begin("my-app", false)) {
    Serial.println("Failed to initialize Preferences");
    return;
  }
  deleteFile(LittleFS, BorrowedItemsFile);  //Testing
  deleteFile(LittleFS, RecordFile);         //Testing
  deleteFile(LittleFS, UsersFile);          //Testing
  TestingSetup(LittleFS);                   //Testing

  TotalJsoninFile(LittleFS, BorrowedItemsFile);
  BiggestItemID(LittleFS, BorrowedItemsFile);

  SettingupPreferences();

  server.on("/S1", HTTP_POST, Step1Handle);
  server.on("/S2", HTTP_POST, Step2Handle);
  server.begin();
}

void loop() {
  server.handleClient();
}
