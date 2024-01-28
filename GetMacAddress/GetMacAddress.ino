#include "WiFi.h"
 
void setup() {
  Serial.begin(115200);
  //WiFi.mode(WIFI_MODE_STA);
}
 
void loop() {
  delay(5000);
  Serial.print("This Tx Mac is:");
  Serial.println(WiFi.macAddress());
}
