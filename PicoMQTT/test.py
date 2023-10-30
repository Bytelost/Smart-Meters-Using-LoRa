import paho.mqtt.client as mqtt

# This is the Subscriber
client = mqtt.Client()

client.connect("192.168.117.247", 1883)

# subscribe to a topic
client.subscribe("test/topic")

# define a callback function for when a message is received
def on_message(client, userdata, message):
    print("Received message:", message.payload.decode())

# set the callback function
client.on_message = on_message

# start the loop to listen for incoming messages
client.loop_forever()