#include "ESPNOW_Transmitter.h"
#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
#else
#include "USB.h"
#include "USBHIDGamepad.h"
USBHIDGamepad Gamepad;
#endif

void setup() {
  Serial.begin();

  preferences.begin("pair_data", true);
  preferences.getBytes("RxMacAddr", RxMacAddr, 6);
  pair_status = preferences.getBool("pair_status", false);
  wifi_channel = preferences.getUChar("wifi_channel", 1);
  preferences.end();

#if !ARDUINO_USB_MODE
  Gamepad.begin();
  USB.begin();
#endif

  WiFi.mode(WIFI_STA);
  WiFi.macAddress(TxMacAddr);

  if(pair_status){
    //WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
    WiFi.channel(wifi_channel);

    //esp_wifi_start();
    //ESP_ERROR_CHECK(esp_wifi_set_channel(channel_, WIFI_SECOND_CHAN_NONE));

    if (esp_now_init() != ESP_OK) {   // Init ESP-NOW
      Serial.println("Error initializing ESP-NOW");
    } else {
      Serial.println("Succes: Initialized ESP-NOW");
    }

    //esp_wifi_config_espnow_rate();
    
    // Register peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, RxMacAddr, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
    } else {
      Serial.println("Succes: Added peer");
    } 

    TxData.syncByte = 0;
    //esp_now_get_peer();

    pinMode(ppmPin, INPUT);
    attachInterrupt(ppmPin, handleInterrupt, RISING);

    pinMode(0,INPUT_PULLUP);
    attachInterrupt(0, pair, RISING);
  } else {
    TxData.syncByte = 1;
    while(1){
      Serial.print("Pairing request on channel "  );
      Serial.println(wifi_channel);

      // set WiFi channel
      ESP_ERROR_CHECK(esp_wifi_set_channel(wifi_channel,  WIFI_SECOND_CHAN_NONE));
      if (esp_now_init() != ESP_OK) Serial.println("Error initializing ESP-NOW");

      // Add peer
      esp_now_peer_info_t peerInfo = {};
      memcpy(peerInfo.peer_addr, RxMacAddr, 6);
      peerInfo.channel = 0;  
      peerInfo.encrypt = false;
      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        ESP.restart();
      }

      esp_now_send(RxMacAddr, (uint8_t *) &TxData, sizeof(TxData));
      esp_now_register_recv_cb(OnDataRecv);//try to get paired

      delay(500);
      wifi_channel ++;//Next channel
      if (wifi_channel > MAX_CHANNEL){
        wifi_channel = 1;
      }
    }
  }
}
 
void loop() {
  if (rawValues[0] >= 800 && rawValues[0] <= 2200 && millis() - lastsendtime >= 20){// 50Hz
    lastsendtime = millis();

    // data.lxAxis    = ReMap(analogRead(32), false);
    // data.lyAxis    = ReMap(analogRead(33), false);
    // data.rxAxis    = ReMap(analogRead(34), false);
    // data.ryAxis    = ReMap(analogRead(35), false);

    TxData.A = map(rawValues[0], 800, 2200, 0, 1024);
    TxData.E = map(rawValues[1], 800, 2200, 0, 1024);
    TxData.R = map(rawValues[2], 800, 2200, 0, 1024);
    TxData.T = map(rawValues[3], 800, 2200, 0, 1024);

    TxData.ch1 = map(rawValues[4], 800, 2200, 0, 255);
    TxData.ch2 = map(rawValues[5], 800, 2200, 0, 255);
    TxData.ch3 = map(rawValues[6], 800, 2200, 0, 255);
    TxData.ch4 = map(rawValues[7], 800, 2200, 0, 255);

    // for(int i=0; i < 8; i++){
    //   Serial.print(rawValues[i]);
    //   Serial.print("    ");
    // }

    // static bool fallingedge = false;
    // if (!digitalRead(0) && !fallingedge) {
    //   fallingedge = true;
    //   _channels[4] = (_channels[4] == 1100)? 2100:1100;
    // } else if (digitalRead(0) && fallingedge) {
    //   fallingedge = false;
    // }
    
    esp_now_send(RxMacAddr, (uint8_t *) &TxData, sizeof(TxData));

#if !ARDUINO_USB_MODE
    Gamepad.leftStick(map(rawValues[2], 800, 2200, 128, -128), map(rawValues[3], 800, 2200, 128, -128));
    Gamepad.rightStick(map(rawValues[0], 800, 2200, 128, -128), map(rawValues[1], 800, 2200, 128, -128));
#endif
  } else {
    delay(1);
  }
}
