#include <WiFi.h>
#include <Preferences.h>
#include <PubSubClient.h>

#define BOOT_BUTTON 0

char mqtt_user[20];
char mqtt_pass[20];
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
  wifiPass = prefs.getString("password", ""),
  ip = prefs.getString("mqtt_server", ""),
  username = prefs.getString("mqtt_user", ""),
  mqttPass = prefs.getString("mqtt_pass", "");

  if((ssid == "")||(ip == "")||(username == "")){
    ssid = readUART("Enter SSID: ");
    wifiPass = readUART("Enter Password: ");
    ip = readUART("Enter mqtt server IP: ");
    username = readUART("Enter mqtt server username: ");
    mqttPass = readUART("Enter mqtt server password: ");

    prefs.putString("ssid", ssid);
    prefs.putString("password", wifiPass);
    prefs.putString("mqtt_server", ip);
    prefs.putString("mqtt_user", username);
    prefs.putString("mqtt_pass", mqttPass);
  }

  prefs.end();

  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, wifiPass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    checkBootButton(0);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  static char mqtt_server [16];
  ip.toCharArray(mqtt_server, sizeof(mqtt_server));
  username.toCharArray(mqtt_user, sizeof(mqtt_user));
  mqttPass.toCharArray(mqtt_pass, sizeof(mqtt_pass));

  Serial.print("Setting server at IP: ");
  Serial.println(mqtt_server);
  client.setServer(mqtt_server, 1883);
}

void loop(){
  char msg[8];

  if (!client.connected()) {
    reconnect();
  }

  sprintf(msg,"%d",count++);
  client.publish("mqtt_topic", msg);
  Serial.print("value:");
  Serial.println(msg);

  checkBootButton(5);
}

void checkBootButton(unsigned int seconds){
  unsigned int loops = seconds*5;
  while(loops-- > 0){
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
  return input;                                           // Return the collected input
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection with username, ");
    Serial.print(mqtt_user);
    Serial.println(" ...");

    if (client.connect("ESP32_MQTT", mqtt_user, mqtt_pass)) {
      Serial.println("Connected");
    }else{
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds...");
      checkBootButton(5);
    }
  }
}
