  //  Copyright [2025] [Muhammad Sulaiman]

  //  Licensed under the Apache License, Version 2.0 (the "License");
  //  you may not use this file except in compliance with the License.
  //  You may obtain a copy of the License at

  //      http://www.apache.org/licenses/LICENSE-2.0

  //  Unless required by applicable law or agreed to in writing, software
  //  distributed under the License is distributed on an "AS IS" BASIS,
  //  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  //  See the License for the specific language governing permissions and
  //  limitations under the License.

#include <WiFi.h>
#include <Preferences.h>
#include <PubSubClient.h>

#define BOOT_BUTTON 0

char mqtt_user [20], mqtt_pass [20], ID [20], routingKey [20];
int count = 0;

Preferences prefs;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(BOOT_BUTTON, INPUT_PULLUP);  // Enable internal pull-up
  
  Serial.begin(115200);
  while(!Serial);

  prefs.begin("login-details", false);

  String ssid = prefs.getString("ssid", ""),
    wifiPass = prefs.getString("wifi password", ""),
    ip = prefs.getString("IP", ""),
    username = prefs.getString("username", ""),
    mqttPass = prefs.getString("mqtt password", ""),
    id = prefs.getString("ID", "ESP32_MQTT"),
    key = prefs.getString("routing key", "");

  if((ssid == "") || (ip == "") || (username == "") || (key == "")){
    ssid = readUART("Enter SSID: ");
    wifiPass = readUART("Enter Password: ");
    ip = readUART("Enter mqtt server IP: ");
    username = readUART("Enter mqtt server username: ");
    mqttPass = readUART("Enter mqtt server password: ");
    key = readUART("Enter Routing Key to use: ");
    id = readUART("Enter ID for this device (optional): ");

    prefs.putString("ssid", ssid);
    prefs.putString("wifi password", wifiPass);
    prefs.putString("IP", ip);
    prefs.putString("username", username);
    prefs.putString("mqtt password", mqttPass);
    prefs.putString("routing key", key);
    prefs.putString("ID", id);
  }

  prefs.end();

  Serial.print("\n\nConnecting to ");
  Serial.print(ssid);
  Serial.print(" ");

  WiFi.begin(ssid, wifiPass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    checkBootButton(0);
  }

  Serial.println("\n\nWiFi Connected\nIP address: ");
  Serial.println(WiFi.localIP());

  static char mqtt_server [16];
  ip.toCharArray(mqtt_server, sizeof(mqtt_server));
  username.toCharArray(mqtt_user, sizeof(mqtt_user));
  mqttPass.toCharArray(mqtt_pass, sizeof(mqtt_pass));
  key.toCharArray(routingKey, sizeof(routingKey));
  id.toCharArray(ID, sizeof(ID));

  Serial.print("\nSetting server at IP: ");
  Serial.println(mqtt_server);
  client.setServer(mqtt_server, 1883);
}

void loop(){
  char msg[8];

  if (!client.connected()) {
    reconnect();
  }

  sprintf(msg,"%d",count++);
  client.publish(routingKey, msg);
  Serial.print("message sent: ");
  Serial.println(msg);

  checkBootButton(5);
}

void checkBootButton(unsigned int seconds){
  int loops = seconds*5;
  while(--loops > 0){
    if(digitalRead(BOOT_BUTTON) == LOW) {
      delay(200);
      if(digitalRead(BOOT_BUTTON) == LOW){
        prefs.begin("login-details", false);
        prefs.clear();
        prefs.end();
        ESP.restart();
      }
    }
    delay(200);
  }
}

String readUART(String prompt) {
  String input = "";
  Serial.print(prompt);
  while(1){
    if (Serial.available() > 0){                         // Wait for the UART recieve buffer to get a byte
      byte inByte = Serial.read();                        // Read the byte from UART
      if (inByte == 10) {                                 // Line Feed (LF)
        Serial.println();
        break;
      }
      if (inByte == 8) {                                  // Backspace (BS)
        if (input.length() > 0) {
          input.remove(input.length() - 1);               // Remove last character
          Serial.print("\b \b");                          // Move the cursor back, print a space, and move it back again
        }
      } else {
        input += (char)inByte;                            // Append the received character to the input string
        Serial.print((char)inByte);                       // Print the character
      }
    }
  }  
  input.trim();                                           // Remove trailing whitespaces
  return input;                                           // Return the collected input
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("\nAttempting MQTT connection with username, ");
    Serial.print(mqtt_user);
    Serial.println(" ...\n");

    if (client.connect(ID, mqtt_user, mqtt_pass)) {
      Serial.println("Connected to MQTT Broker\n");
    }else{
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds...");
      checkBootButton(5);
    }
  }
}
