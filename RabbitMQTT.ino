#include <WiFi.h>

const char *ssid = "HP-hotspot";          // Change this to your WiFi SSID
const char *password = "1234567890";      // Change this to your WiFi password

void setup(){
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println("******************************************************");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(){

}
