import paho.mqtt.client as mqtt
import json
from datetime import datetime

file_path = 'data.json'

# Conection callback
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe("v3/netzer0@ttn/devices/heltec-tcc/up")

# Message callback
def on_message(client, userdata, msg):
    # Get message data
    payload = json.loads(msg.payload.decode('utf-8'))
    decoded_data_str = payload.get('uplink_message', {}).get('decoded_payload', {}).get('decodedData', "")

    # Get timestamp
    timestamp = datetime.now().strftime('%Y-%m-%d% H:%M')
    
    # Create the new entry
    new_entry = {
        'decodedData': decoded_data_str,
        'timestamp': timestamp
    }
    
    # Try open the file, if not exist save in a buffer
    try:
        with open(file_path, 'r') as file:
            existing_data = json.load(file)
    except FileNotFoudError:
        existing_data=[]

    # Append the new data
    existing_data.append(new_entry)

    # Write the data in the file
    with open(file_path, 'w') as file:
        json.dump(existing_data, file, indent=4)
    

client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set("netzer0@ttn", "NNSXS.4HXD4XCB6UPXBUILRPSHEJJ2O5Q2FVFGJV4PPDY.KGSEVMFYHORSMFIOLXIPWNXLEPXLSAQ6H22BI4BYCIW24NUYSS3Q")
client.connect("au1.cloud.thethings.network", 1883, 60)

client.loop_forever()
