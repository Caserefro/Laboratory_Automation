#ifndef IR_REMOTE_H
#define IR_REMOTE_H

#define IRLEDpin 17          //the arduino pin connected to IR LED to ground. HIGH=LED ON \
                             // library defined constants, used for aproaching real time timers.
uint16_t kCoolixTick = 276;  // Approximately 10.5 cycles at 38kHz
uint16_t kCoolixBitMarkTicks = 2;
uint16_t kCoolixBitMark = kCoolixBitMarkTicks * kCoolixTick;  // 552us ESP32
uint16_t kCoolixOneSpaceTicks = 6;
uint16_t kCoolixOneSpace = kCoolixOneSpaceTicks * kCoolixTick;  // 1656us ESP32
uint16_t kCoolixZeroSpaceTicks = 2;
uint16_t kCoolixZeroSpace = kCoolixZeroSpaceTicks * kCoolixTick;  // 552us ESP32
uint16_t kCoolixHdrMarkTicks = 17;
uint16_t kCoolixHdrMark = kCoolixHdrMarkTicks * kCoolixTick;  // 4692us ESP32
uint16_t kCoolixHdrSpaceTicks = 16;
uint16_t kCoolixHdrSpace = kCoolixHdrSpaceTicks * kCoolixTick;  // 4416us  ESP32
uint16_t kCoolixMinGapTicks = kCoolixHdrMarkTicks + kCoolixZeroSpaceTicks;
uint16_t kCoolixMinGap = kCoolixMinGapTicks * kCoolixTick;  // 5244us  ESP32

//Experimentally defined constants. Override if microcontroller is a nrf52840.
/*
uint16_t kCoolixHdrMark = 4550;
uint16_t kCoolixHdrSpace = 4446;
uint16_t kCoolixBitMark = 552;
uint16_t kCoolixOneSpace = 1656;
uint16_t kCoolixZeroSpace = 552;
uint16_t kCoolixMinGap = 5244;
*/
uint16_t kCoolixFootermark = 552;  // dont erase this one.

void IRcarrier(unsigned int IRtimemicroseconds) {
  for (int i = 0; i < (IRtimemicroseconds / 26); i++) {
    digitalWrite(IRLEDpin, HIGH);  //turn on the IR LED
    //NOTE: digitalWrite takes about 3.5us to execute, so we need to factor that into the timing. (this is for arduino uno)
    delayMicroseconds(13);        //delay for 13us (9us + digitalWrite), half the carrier frequnecy
    digitalWrite(IRLEDpin, LOW);  //turn off the IR LED
    delayMicroseconds(13);        //delay for 13us (9us + digitalWrite), half the carrier frequnecy
  }
}

void IRsendCode(unsigned long long code) {
  Serial.println(code);
  IRcarrier(kCoolixHdrMark);           // header mark
  delayMicroseconds(kCoolixHdrSpace);  // header space
  const uint16_t nbits = 48;
  //unsigned long long
  for (uint64_t mask = 1ULL << (nbits - 1); mask; mask >>= 1) {
    IRcarrier(552);
    if (code & mask) {  // Send a 1
      delayMicroseconds(kCoolixOneSpace);
    } else {  // Send a 0
      delayMicroseconds(kCoolixZeroSpace);
    }
  }
  IRcarrier(kCoolixFootermark);  // footermark
  delayMicroseconds(kCoolixMinGap);
}
//from 17C to 28C in that order.
unsigned long long tempcodes[12]{
  196044317001975, 196044317006055, 196044317014215, 196044317010135, 196044317026455, 196044317030535, 196044317022375, 196044317018295, 196044317050935, 196044317055015, 196044317038695, 196044317034615
};
#define TurnONCodeAC 196044317010135
#define TurnOFFCodeAC 196045854531615
#define SwingCodeAC 199333543323990

#endif