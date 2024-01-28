#include "ESPNOW_Receiver.h"



// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  sys_status.mac = mac;
  memcpy(&data, incomingData, sizeof(data));
  lastRecvTime = millis(); 
}

void setup(){
  Serial.begin(115200);
  //WiFi.mode(WIFI_STA);

  ESP_ERROR_CHECK(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE));
  if (esp_now_init() != ESP_OK) {   // Init ESP-NOW
    Serial.println("Error initializing ESP-NOW");
  } else {
    Serial.println("Succes: Initialized ESP-NOW");
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
  // check the data speed and update status
  sys_status.DataUpdateSpeed = millis() - lastRecvTime;
  if (sys_status.DataUpdateSpeed > SIGNAL_TIMEOUT ) {
    sys_status.timeout = true;
  } else {
    sys_status.timeout = false;
  }

  // print out the data
  Serial.print("ReMsg from: ");
  printMAC(sys_status.mac);
  Serial.print("  lx:");
  Serial.print(data.lxAxis);
  Serial.print("  ly:");
  Serial.print(data.lyAxis);
  Serial.print("  rx:");
  Serial.print(data.rxAxis);
  Serial.print("  ry:");
  Serial.print(data.ryAxis);
  // Other chennels
  Serial.println();

  // Drone control & PID loop
}
