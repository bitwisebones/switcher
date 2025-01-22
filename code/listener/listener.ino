#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <math.h>

char ssid[] = "";
char pass[] = "";
int status = WL_IDLE_STATUS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "";
int port = 1883;
const char topic[] = "";

// this ordering doesn't make much sense,
// it's just how I happened to plug in the 
// barrel connectors to the board
const int j1_pin = 20;
const int j2_pin = 21;
const int j3_pin = 19;
const int j4_pin = 18;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(j1_pin, OUTPUT);
  pinMode(j2_pin, OUTPUT);
  pinMode(j3_pin, OUTPUT);
  pinMode(j4_pin, OUTPUT);
  
  Serial.begin(9600);

  Serial.println("Attempting to connect to WiFi network...");

  while(WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("Connected.");
  Serial.println("Attempting to connect to MQTT broker...");

  if(!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed. Error code = ");
    Serial.println(mqttClient.connectError());
    while(1);
  }

  Serial.println("Connected.");

  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  mqttClient.subscribe(topic);
}

void loop() {
  mqttClient.poll();
}

void onMqttMessage(int messageSize) {
  Serial.println("Received message.");

  if(messageSize > 2) {
    return;
  }

  digitalWrite(LED_BUILTIN, HIGH);

  static char message[2]; // buffer needs to be big enough for 1 char plus null terminator
  mqttClient.readBytes(message, messageSize);
  Serial.println(message);

  digitalWrite(LED_BUILTIN, LOW);

  if(message[0] == '1') {
    digitalWrite(j1_pin, HIGH);
    delay(100);
    digitalWrite(j1_pin, LOW);
  }
  if(message[0] == '2') {
    digitalWrite(j2_pin, HIGH);
    delay(100);
    digitalWrite(j2_pin, LOW);
  }
  if(message[0] == '3') {
    digitalWrite(j3_pin, HIGH);
    delay(100);
    digitalWrite(j3_pin, LOW);
  }
  if(message[0] == '4') {
    digitalWrite(j4_pin, HIGH);
    delay(100);
    digitalWrite(j4_pin, LOW);
  }

  message[0] = 0;
  message[1] = 0;
}
