#ifndef ESP32_CYD_H
#define ESP32_CYD_H

#include "AM2315.h"

AM2315 AM2315(&Wire);

#include "Adafruit_LTR390.h"

Adafruit_LTR390 ltr = Adafruit_LTR390();

// Include required libraries
#include <SPI.h>
#include <LoRa.h>
// Define the pins used by the LoRa module
const int LoRacsPin = 5;      // LoRa radio chip select
const int LoRaResetPin = 14;  // LoRa radio reset
const int LoRaintPin = 4;     // Must be a hardware interrupt pin

hw_timer_t * timer = NULL;
uint8_t timer_id = 0;
uint16_t prescaler = 80;  // Between 0 and 65 535
int threshold = 1000000;  // 64 bits value (limited to int size of 32bits)
// all since startup
int seconds;
int mins = 0;
long int hours = 0;
int days = 0; 
int months = 0; 


// LTR290  ***********************************************************************************************
void LTRinit() {
  if (!ltr.begin()) {
    Serial.println("Couldn't find LTR sensor!");
    while (1) delay(10);
  }
  Serial.println("Found LTR sensor!");

  ltr.setMode(LTR390_MODE_UVS);
  if (ltr.getMode() == LTR390_MODE_ALS) {
    Serial.println("In ALS mode");
  } else {
    Serial.println("In UVS mode");
  }

  ltr.setGain(LTR390_GAIN_18);
  Serial.print("Gain : ");
  switch (ltr.getGain()) {
    case LTR390_GAIN_1: Serial.println(1); break;
    case LTR390_GAIN_3: Serial.println(3); break;
    case LTR390_GAIN_6: Serial.println(6); break;
    case LTR390_GAIN_9: Serial.println(9); break;
    case LTR390_GAIN_18: Serial.println(18); break;
  }

  ltr.setResolution(LTR390_RESOLUTION_20BIT);
  Serial.print("Resolution : ");
  switch (ltr.getResolution()) {
    case LTR390_RESOLUTION_13BIT: Serial.println(13); break;
    case LTR390_RESOLUTION_16BIT: Serial.println(16); break;
    case LTR390_RESOLUTION_17BIT: Serial.println(17); break;
    case LTR390_RESOLUTION_18BIT: Serial.println(18); break;
    case LTR390_RESOLUTION_19BIT: Serial.println(19); break;
    case LTR390_RESOLUTION_20BIT: Serial.println(20); break;
  }

  ltr.setThresholds(100, 1000);
  ltr.configInterrupt(true, LTR390_MODE_UVS);
}

void LTRRead() {
  if (ltr.newDataAvailable()) {
    Serial.print("UV data: ");
    Serial.print(ltr.readUVS());
  }
}

// AM2315 ***************************************************************************************************

void AM2315init() {
  Wire.begin();
  while (AM2315.begin() == false) {
    Serial.print(millis());
    Serial.println("\tCould not connect to sensor.");
    delay(1000);
  }
  delay(1000);
}

int AM2315Read() {
  //  READ DATA
  Serial.print("AM2315, \t");
  return AM2315.read();
}

void AM2315Status(int status) {
  switch (status) {
    case AM2315_OK:
      Serial.print("OK,\t");
      break;
    case AM2315_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case AM2315_ERROR_CONNECT:
      Serial.print("Connect error,\t");
      break;
    case AM2315_MISSING_BYTES:
      Serial.print("Bytes error,\t");
      break;
    default:
      Serial.print("error <");
      Serial.print(status);
      Serial.print(">,\t");
      break;
  }
}

// LoRa ************************************************************************

void LoRainit(){ 
    // Setup LoRa module
  LoRa.setPins(LoRacsPin, LoRaResetPin, LoRaintPin);

  Serial.println("LoRa Receiver Test");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
}

// ISR ***********************************************************************
void IRAM_ATTR timer_isr() {  
  .//executes each second 
    // Example increment operation
    seconds = (seconds + 1) % 60;
    if (seconds == 0) {
        minutes = (minutes + 1) % 60;
        if (minutes == 0) {
            hours = (hours + 1) % 24;
            if (hours == 0) {
                days++;
            }
        }
    }
    //seconds = (seconds + 1) % 60;
    //seconds == 0 ? (minutes = (minutes + 1) % 60, minutes == 0 ? (hours = (hours + 1) % 24, hours == 0 ? days++ : 0) : 0) : 0;
}


#endif