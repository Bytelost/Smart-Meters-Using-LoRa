import paho.mqtt.client as mqtt

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # subscribe to a topic
    client.subscribe("test/topic")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

# set the callback function
client = mqtt.Client(client_id="01", clean_session=True, userdata=None, transport="tcp")
client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.117.247", 1883, 10)
client.publish("test/topic", "Hello World!")

# start the loop to listen for incoming messages
client.loop_forever()