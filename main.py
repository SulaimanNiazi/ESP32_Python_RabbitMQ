import paho.mqtt.client as mqtt
 
def on_connect(client, userdata, flags, rc):
    print("Connected to broker")
 
def on_message(client, userdata, message):
    print ("Message received: "  + message.payload.decode())

if __name__ == "__main__":
    ip = input("Enter Server IP: ").strip()
    port = input("Enter Server port: ").strip()
    username = input("Enter Username: ").strip()
    password = input("Enter Password: ").strip()
    channel = input("Enter Channel: ").strip()

    client = mqtt.Client()
    client.username_pw_set(username, password=password)
    client.connect(ip, int(port)) 

    client.on_connect = on_connect       #attach function to callback
    client.on_message = on_message       #attach function to callback

    client.subscribe(channel) 
    client.loop_forever()                 #start the loop