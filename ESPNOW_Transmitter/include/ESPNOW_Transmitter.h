#include <esp_now.h>
//#include <esp_wifi.h>
#include <WiFi.h>
#include <Arduino.h>


// Set the wifi channel (1-13)
#define channel_ 13

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t RxMacAddr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

#define interruptPin 36
#define channelAmount 8
// Arrays for keeping track of channel values
unsigned long rawValues[channelAmount];
// A counter variable for determining which channel is being read next
uint8_t pulseCounter = 0;
// A time variable to remember when the last pulse was read
volatile unsigned long microsAtLastPulse = 0;
// The minimum blanking time (microseconds) after which the frame current is considered to be finished
// Should be bigger than maxChannelValue + channelValueMaxError
unsigned blankTime = 2100;

void handleInterrupt(void) {
  // Remember the current micros() and calculate the time since the last pulseReceived()
  unsigned long previousMicros = microsAtLastPulse;
  microsAtLastPulse = micros();
  unsigned long time = microsAtLastPulse - previousMicros;

  if (time > blankTime) {
      // Blank detected: restart from channel 1 
      pulseCounter = 0;
  }else {
    // Store times between pulses as channel values
    if (pulseCounter < channelAmount) {
        rawValues[pulseCounter] = time;
        ++pulseCounter;
    }
  }
}

struct PacketData {
  //uint8_t syncByte = 0xf0;
  uint16_t A: 10;
  uint16_t E: 10;
  uint16_t R: 10;
  uint16_t T: 10;

  uint8_t ch1;
  uint8_t ch2;
  uint8_t ch3;
  uint8_t ch4;
}TxData; 

uint16_t ReMap(uint16_t value, bool reverse) { //4096 -> 1024
  if (value >= 2200) {
    value = map(value, 2200, 4095, 512, 1024);
  } else if (value <= 1800) {
    value = map(value, 1800, 0, 512, 0);
  } else {
    value = 1500;
  }

  if (reverse) {
    value = map(value, 1024, 0, 0, 1024);
  }
  //Serial.println(value);  
  return value;
}
