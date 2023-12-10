#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t receiverMacAddress[] = {0xAC,0x67,0xB2,0x36,0x7F,0x28};  //AC:67:B2:36:7F:28

struct PacketData {
  int16_t lxAxisValue;
  int16_t lyAxisValue;
  int16_t rxAxisValue;
  int16_t ryAxisValue;
 
  int16_t switch1Value;
  int16_t switch2Value;
  int16_t switch3Value;
  int16_t switch4Value;  
  int16_t switch5Value;
  int16_t switch6Value;
  int16_t switch7Value;
  int16_t switch8Value;  
} data;