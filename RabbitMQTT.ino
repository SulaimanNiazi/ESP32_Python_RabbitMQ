  //  Copyright [2025] [Muhammad Sulaiman]

  //  Licensed under the Apache License, Version 2.0 (the "License");
  //  you may not use this file except in compliance with the License.
  //  You may obtain a copy of the License at

  //  http://www.apache.org/licenses/LICENSE-2.0

  //  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
  //  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  //  See the License for the specific language governing permissions and limitations under the License.

#include <WiFi.h>
#include <Preferences.h>
#include <PubSubClient.h>

#define BOOT_BUTTON 0
#define MAX_LENGTH 256

char mqtt_user [50], mqtt_pass [50], ID [20], routingKey [20], buffer [MAX_LENGTH];
unsigned int count = 0, delay_s = 5;
bool manual = false;

String input;
Preferences prefs;
WiFiClient espClient;
PubSubClient client(espClient);

void setup(){
  pinMode(BOOT_BUTTON, INPUT_PULLUP);  // Enable internal pull-up
  
  Serial.begin(115200);
  while(!Serial);

  prefs.begin("login-details", false);

  String ssid = prefs.getString("ssid", ""),
    wifiPass = prefs.getString("wifi password", ""),
    server = prefs.getString("server", ""),
    username = prefs.getString("username", ""),
    mqttPass = prefs.getString("mqtt password", ""),
    id = prefs.getString("ID", "ESP32"),
    key = prefs.getString("routing key", "");

  if((ssid == "") || (server == "") || (username == "") || (key == "")){
    ssid = readUART("Enter SSID: ");
    wifiPass = readUART("Enter Password: ");
    server = readUART("Enter mqtt server IP/Domain: ");
    username = readUART("Enter mqtt server username: ");
    mqttPass = readUART("Enter mqtt server password: ");
    key = readUART("Enter Routing Key (Topic) to use: ");
    id = readUART("Enter ID for this device (optional): ");

    prefs.putString("ssid", ssid);
    prefs.putString("wifi password", wifiPass);
    prefs.putString("server", server);
    prefs.putString("username", username);
    prefs.putString("mqtt password", mqttPass);
    prefs.putString("routing key", key);
    prefs.putString("ID", id);
  }

  prefs.end();

  Serial.print("\r\n\r\nConnecting to " + ssid + " ");

  WiFi.begin(ssid, wifiPass);

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    checkBootButton(1);
  }

  Serial.print("\r\n\r\nWiFi Connected\r\nIP address: ");
  Serial.print(WiFi.localIP());
  Serial.println("\r");

  static char mqtt_server [50];
  server.toCharArray(mqtt_server, sizeof(mqtt_server));
  username.toCharArray(mqtt_user, sizeof(mqtt_user));
  mqttPass.toCharArray(mqtt_pass, sizeof(mqtt_pass));
  key.toCharArray(routingKey, sizeof(routingKey));
  id.toCharArray(ID, sizeof(ID));

  Serial.println("\nSetting server at, " + server + ":1883 \r");
  client.setServer(mqtt_server, 1883);
  
  readUART("Do you want to enter manual mode for this run? (Y/N(default)): ");
  if((input[0] == 'y')||(input[0] == 'Y')){
    manual = true;
    delay_s = 1;
  }
}

void loop(){
  if (!client.connected()) {
    reconnect();
  }

  if(manual){
    readUART("Input message: ");
    input.toCharArray(buffer, sizeof(buffer));
  } else {
    sprintf(buffer,"%d",count++);
  }
  client.publish(routingKey, buffer);
  Serial.printf("Message sent: %s\r\n", buffer);

  checkBootButton(delay_s);
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

String readUART(String prompt){
  input = "";
  Serial.print(prompt);
  while(1){
    if(Serial.available() > 0){                           // Wait for the UART recieve buffer to get a byte
      byte inByte = Serial.read();                        // Read the byte from UART
      if((inByte == 0x0d)||(inByte == 10)){               // Line Feed (LF)
        Serial.println("\r");
        break;
      }
      if(inByte == 0x08){                                 // Backspace (BS)
        if(input.length() > 0){
          input.remove(input.length() - 1);               // Remove last character
          Serial.print("\b \b");                          // Move the cursor back, print a space, and move it back again
        }
      }
      else if(input.length() < MAX_LENGTH){
        input += (char)inByte;                            // Append the received character to the input string
        Serial.print((char)inByte);                       // Print the character
      }
    }
  }  
  input.trim();                                           // Remove trailing whitespaces
  return input;                                           // Return the collected input
}

void reconnect(){
  // Loop until we're reconnected
  while(!client.connected()){
    Serial.printf("\nAttempting MQTT connection with username, %s ...\r\n\n", mqtt_user);

    if(client.connect(ID, mqtt_user, mqtt_pass)){
      Serial.println("Connected to MQTT Broker\r\n");
    }else{
      Serial.printf("Failed, rc = %d, trying again in 5 seconds...\r\n", client.state());
      checkBootButton(5);
    }
  }
}
