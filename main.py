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

    ip = login_details.get("ip", "").strip()
    port = login_details.get("port", 0)
    username = login_details.get("username", "").strip()
    password = login_details.get("password", "").strip()
    channel = login_details.get("channel", "").strip()

    client = mqtt.Client()
    client.username_pw_set(username, password=password)
    client.connect(ip, port) 

    client.on_connect = on_connect       #attach function to callback
    client.on_message = on_message       #attach function to callback

    client.subscribe(channel) 
    client.loop_forever()                 #start the loop