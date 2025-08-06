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
    channel = login_details.get("channel", "").strip()

    choice = input("Would you like to enter using saved values? (Y/N):").strip().upper()

    if not ip or not port or not username or not password or not channel or choice == 'N':
        ip = input("Enter MQTT broker IP: ").strip()
        port = int(input("Enter MQTT broker port: ").strip())
        username = input("Enter MQTT username: ").strip()
        password = input("Enter MQTT password: ").strip()
        channel = input("Enter MQTT channel: ").strip()

        loginDictionary = {
            "ip": ip,
            "port": port,
            "username": username,
            "password": password,
            "channel": channel
        }

        with open("login-details.json", "w") as saveFile:
            json.dump(loginDictionary, saveFile, indent=2)

    print(f"\nConnecting to MQTT broker at {ip}:{port} with user {username} on channel {channel}...\n")

    client = mqtt.Client()
    client.username_pw_set(username, password=password)
    client.connect(ip, port) 

    client.on_connect = on_connect       #attach function to callback
    client.on_message = on_message       #attach function to callback

    client.subscribe(channel) 
    client.loop_forever()                 #start the loop