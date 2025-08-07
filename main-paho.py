#    Copyright [2025] [Muhammad Sulaiman]

#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at

#        http://www.apache.org/licenses/LICENSE-2.0

#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

import paho.mqtt.client as mqtt
import json
 
def on_connect(client, userdata, flags, rc):
    print("Connected to broker")
 
def on_message(client, userdata, message):
    print ("Message received: "  + message.payload.decode())

if __name__ == "__main__":
    f = open('login-details.json', 'r')
    login_details = json.load(f)
    f.close()

    ip = login_details.get("ip", "localhost").strip()
    port = login_details.get("port", 5672)
    username = login_details.get("username", "admin1").strip()
    password = login_details.get("password", "").strip()
    routingKey = login_details.get("routing key", "").strip()

    choice = input("Would you like to enter using saved values? (Y/N):").strip().upper()

    if not ip or not port or not username or not routingKey or choice == 'N':
        ready = False
        while not ready:
            ip = input("Enter MQTT broker IP: ").strip()
            port = int("0" + input("Enter MQTT broker port: ").strip())
            username = input("Enter MQTT username: ").strip()
            password = input("Enter MQTT password: ").strip()
            routingKey = input("Enter MQTT routing key: ").strip()

            if not ip:
                ip = "localhost"
            if not port:
                port = 5672

            if username and routingKey:
                ready = True
            else:
                print("\nUsername and routing key cannot be empty. Please try again.\n")

        exchange = login_details.get("exchange", "").strip()
        queue = login_details.get("queue", "").strip()

        loginDictionary = {
            "ip": ip,
            "port": port,
            "username": username,
            "password": password,
            "routing key": routingKey,
            "exchange": exchange,
            "queue": queue
        }

        with open("login-details.json", "w") as saveFile:
            json.dump(loginDictionary, saveFile, indent=2)

    print(f"\nConnecting to MQTT Broker using the following parameters...\nIP: {ip}\nPort: {port}\nUsername: {username}\nRouting Key: {routingKey}\n")

    try: 
        client = mqtt.Client()
        client.username_pw_set(username, password=password)
        client.connect(ip, port) 

        client.on_connect = on_connect       #attach function to callback
        client.on_message = on_message       #attach function to callback

        client.subscribe(routingKey) 
        client.loop_forever()                 #start the loop

    except Exception as e:
        print(f"\nAn error occurred: {e}\n")