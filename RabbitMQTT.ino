#include <WiFi.h>
#include <Preferences.h>

Preferences prefs;

void setup() {
  Serial.begin(115200);
  while(!Serial);

  prefs.begin("nvm-test", false);

  String ssid = prefs.getString("ssid", "");
  String password = prefs.getString("password", "");

  if(ssid == ""){
    Serial.print("Enter SSID: ");
    ssid = readUART();
    Serial.print("Enter Password: ");
    password = readUART();
    prefs.putString("ssid", ssid);
    prefs.putString("password", password);
  }

  prefs.end(); // Close access

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  
}

String readUART() {
  String input = "";
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