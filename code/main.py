import time
import network
import ubinascii
from machine import Pin
from umqtt.robust import MQTTClient

wifi_ssid = ''
wifi_pass = ''
mqtt_server = ''
client_id = ubinascii.hexlify(machine.unique_id())
topic_sub = b''

last_message = 0
message_interval = 5

time.sleep(3)

# this ordering doesn't make much sense
# it's just how I happened to plug in the jacks
j1 = Pin(20, Pin.OUT)
j2 = Pin(21, Pin.OUT)
j3 = Pin(19, Pin.OUT)
j4 = Pin(18, Pin.OUT)
led = Pin("LED", Pin.OUT)

wifi = network.WLAN(network.STA_IF)
wifi.active(True)
wifi.connect(wifi_ssid, wifi_pass)

# Wait for WiFi connection
while wifi.isconnected() == False:
    pass

print('Connection successful')
print(wifi.ifconfig())

def on_message(topic, msg):
    print((topic, msg))
    if msg == b'1':        
        j1.on()
        time.sleep(0.1)
        j1.off()
    if msg == b'2':
        j2.on()
        time.sleep(0.1)
        j2.off()
    if msg == b'3':
        j3.on()
        time.sleep(0.1)
        j3.off()
    if msg == b'4':
        j4.on()
        time.sleep(0.1)
        j4.off()

def connect_and_subscribe():
    global client_id, mqtt_server, topic_sub
    client = MQTTClient(client_id, mqtt_server, keepalive=30)
    client.set_callback(on_message)
    client.connect()
    client.subscribe(topic_sub, qos=1)
    print('Connected to %s MQTT broker, subscribed to %s topic' % (mqtt_server, topic_sub))
    return client

def restart_and_reconnect():
    print('Failed to connect to MQTT broker. Reconnecting...')
    time.sleep(10)
    machine.reset()

try:
    client = connect_and_subscribe()
except OSError as e:
    restart_and_reconnect()

while True:
    try:
        client.check_msg()
        # without publishing a message, the MQTT connection would die
        # so we just publish some junk to keep it alive
        if(time.time() - last_message) > message_interval:
            client.publish(topic_sub, b'0')
            last_message = time.time()
    except OSError as e:
        pass