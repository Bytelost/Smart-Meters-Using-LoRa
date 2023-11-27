import paho.mqtt.client as mqtt

client = mqtt.Client()

try:
    client.connect("192.168.4.1",1818,60)
except Exception as e:
    print(f"Failed to connect: {str(e)}")
    exit(1)
    
client.publish("test/broker", "Hello world!")
client.disconnect()