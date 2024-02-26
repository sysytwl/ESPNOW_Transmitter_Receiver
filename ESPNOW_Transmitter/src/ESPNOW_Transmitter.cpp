#include "ESPNOW_Transmitter.h"
#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}
#else
#include "USB.h"
#include "USBHIDGamepad.h"
USBHIDGamepad Gamepad;

void setup() {
  Gamepad.begin();
  USB.begin();


  WiFi.mode(WIFI_STA);
  //WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
  WiFi.channel(channel_);

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

  //esp_now_get_peer();

  pinMode(interruptPin, INPUT);
  attachInterrupt(interruptPin, handleInterrupt, RISING);

  // pinMode(0,INPUT_PULLUP);
}
 
void loop() {
  if (rawValues[0] >= 800 && rawValues[0] <= 2200){
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

    Gamepad.leftStick(map(rawValues[2], 800, 2200, 128, -128), map(rawValues[3], 800, 2200, 128, -128));
    Gamepad.rightStick(map(rawValues[0], 800, 2200, 128, -128), map(rawValues[1], 800, 2200, 128, -128));
  }

  delay(15);
}
#endif /* ARDUINO_USB_MODE */
