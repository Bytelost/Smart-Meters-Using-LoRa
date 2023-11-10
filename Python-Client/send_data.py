import paho.mqtt.client as mqtt
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("test/test")


client = mqtt.Client()
client.on_connect = on_connect
client.connect("192.168.4.1", 1818, 60)

while True:
    client.publish("test/test", "Hello world!")
    time.sleep(5)