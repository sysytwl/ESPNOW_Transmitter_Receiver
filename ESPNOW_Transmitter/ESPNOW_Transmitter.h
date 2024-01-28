#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <Arduino.h>

// Set the wifi channel (1-13)
#define channel_ 6

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t RxMacAddr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

struct PacketData {
  int16_t lxAxis;
  int16_t lyAxis;
  int16_t rxAxis;
  int16_t ryAxis;
 
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;  
  int16_t ch5;
  int16_t ch6;
  int16_t ch7;
  int16_t ch8;  
} data;

int ReMap(int value, bool reverse) {
  if (value >= 2200) {
    value = map(value, 2200, 4095, 127, 254);
  } else if (value <= 1800) {
    value = (value == 0 ? 0 : map(value, 1800, 0, 127, 0));  
  } else {
    value = 127;
  }

  if (reverse) {
    value = 254 - value;
  }
  Serial.println(value);  
  return value;
}
