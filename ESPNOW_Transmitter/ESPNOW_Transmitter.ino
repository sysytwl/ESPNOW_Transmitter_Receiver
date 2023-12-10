#include "ESPNOW_Transmitter.h"

int mapAndAdjustJoystickDeadBandValues(int value, bool reverse) {
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

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("Succes: Initialized ESP-NOW");
  }

  esp_now_register_send_cb(OnDataSent);
  //esp_wifi_config_espnow_rate();
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
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

  pinMode(15,INPUT_PULLUP);
  pinMode(16,INPUT_PULLUP);
  pinMode(17,INPUT_PULLUP);
  pinMode(18,INPUT_PULLUP); 
  pinMode(19,INPUT_PULLUP);
  pinMode(21,INPUT_PULLUP);
  pinMode(22,INPUT_PULLUP);
  pinMode(23,INPUT_PULLUP); 
     
}
 
void loop() 
{
  data.lxAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(32), false);
  data.lyAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(33), false);
  data.rxAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(34), false);
  data.ryAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(35), false);
  data.switch1Value   = !digitalRead(15);
  data.switch2Value   = !digitalRead(16);
  data.switch3Value   = !digitalRead(17);
  data.switch4Value   = !digitalRead(18);
  data.switch5Value   = !digitalRead(19);
  data.switch6Value   = !digitalRead(21);
  data.switch7Value   = !digitalRead(22);
  data.switch8Value   = !digitalRead(23);  
  
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &data, sizeof(data));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }    
  
  delay(50);
}



















    
