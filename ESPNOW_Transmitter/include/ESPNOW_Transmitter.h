#include <esp_now.h>
//#include <esp_wifi.h>
#include <WiFi.h>
#include <Arduino.h>
#include <Preferences.h>



Preferences preferences;



#define MAX_CHANNEL 13 // change it base on ur location
uint8_t wifi_channel;
uint8_t RxMacAddr[6];
uint8_t TxMacAddr[6];
uint8_t BCMacAddr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
bool pair_status; // pair status

struct PacketData {
  uint8_t syncByte: 1;
  
  uint16_t A: 10;
  uint16_t E: 10;
  uint16_t R: 10;
  uint16_t T: 10;

  uint8_t ch1;
  uint8_t ch2;
  uint8_t ch3;
  uint8_t ch4;
}TxData;

struct TelemetryData {
  uint8_t syncByte: 1;

  uint8_t Battery_v; // int/10 -> fixed point 25.6v max
}inData;

void pair(){
  preferences.begin("pair_data", false);
  preferences.putBool("pair_status", false); //set to pair mode
  preferences.end();
  delay(5000); //since im using pin 0, avoid go into flash mode
  ESP.restart();
}

void IRAM_ATTR OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  for (int i = 0; i < 6; i++) {
    if (mac_addr[i] == TxMacAddr[i]) return; // Not from Tx when paring
  }

  memcpy(&inData, incomingData, sizeof(inData));
  switch (inData.syncByte) {
  case 0://TODO: telemetry
    break;

  case 1://pair
    memcpy(&RxMacAddr, mac_addr, 6);
    pair_status = true;
    break;
  }  
}


#define ppmPin 23
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
unsigned long lastsendtime = 0; // send frequency
void IRAM_ATTR handleInterrupt(void) {
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
