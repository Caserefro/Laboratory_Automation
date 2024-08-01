#include "General_Client.h"

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  SettingupPreferences();
  delay(300);
  OP_TIME_Wrapper();
  OP_WEATHER_Wrapper();
  OP_STATIONS_STATE_Wrapper();
}

void loop() {
}
