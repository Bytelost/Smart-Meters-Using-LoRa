import paho.mqtt.client as mqtt

client = mqtt.Client()

try:
    client.connect("localhost",1818,60)
except Exception as e:
    print(f"Failed to connect: {str(e)}")
    exit(1)
    
client.publish("topic/test", "Hello world!")
client.disconnect()