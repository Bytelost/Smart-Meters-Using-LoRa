import paho.mqtt.client as mqtt

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
  print("Connected with result code "+str(rc))
  client.subscribe("topic/humidity")
  client.subscribe("topic/temperature")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
  if msg.payload.decode() == "Terminate":
    print("Yes!")
    client.disconnect()
    
  else:
    print(msg.payload)
    
client = mqtt.Client()

try:
    client.connect("192.168.4.1",1818,60)
except Exception as e:
    print(f"Failed to connect: {str(e)}")
    exit(1)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()