import paho.mqtt.client as mqtt
import json

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe("v3/netzer0@ttn/devices/heltec-tcc/up")

def on_message(client, userdata, msg):
    payload = json.loads(msg.payload.decode('utf-8'))
    decoded_data_str = payload.get('uplink_message', {}).get('decoded_payload', {}).get('decodedData', "")

    if decoded_data_str:
        decoded_data = json.loads(decoded_data_str)
        message_time = decoded_data.get('time', "")
        aux = message_time
        print(f"Message Time: {message_time}")
    else:
        print("Nada")

client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set("netzer0@ttn", "NNSXS.4HXD4XCB6UPXBUILRPSHEJJ2O5Q2FVFGJV4PPDY.KGSEVMFYHORSMFIOLXIPWNXLEPXLSAQ6H22BI4BYCIW24NUYSS3Q")
client.connect("au1.cloud.thethings.network", 1883, 60)

client.loop_forever()
