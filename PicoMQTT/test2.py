import paho.mqtt.client as mqtt

client = mqtt.Client(client_id="02", clean_session=True, userdata=None, transport="tcp")
client.connect("192.168.117.247", 1883, 10)

client.subscribe("test/topic")
client.publish("test/topic", "Hello World!")
