#include "Weather_Station.h"


void setup() {
  Serial.begin(115200);
  timer = timerBegin(timer_id, prescaler, true);
  timerAttachInterrupt(timer, &timer_isr, true);
  timerAlarmWrite(timer, threshold, true);
  timerAlarmEnable(timer);

  LoRainit();
  LTRinit();
}

void loop() {
  if (minutes % 5 == 0 && minutes != 0) {
    LTRRead();
    // AM2315Status(AM2315Read());
    Serial.print("Sending packet ");
    Serial.println();
    Serial.println("");
    // Send packet
    LoRa.beginPacket();
    LoRa.print("putos aaaa");
  }
}
