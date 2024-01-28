#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <Arduino.h>

// Set the wifi channel (1-13)
#define channel_ 6

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t TxMacAddr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// Timeout
#define SIGNAL_TIMEOUT 100 //ms
unsigned long lastRecvTime = 0;

struct SystemStatus{
    unsigned int DataUpdateSpeed;
    bool timeout;
    const uint8_t * mac;
} sys_status;

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

void printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
}