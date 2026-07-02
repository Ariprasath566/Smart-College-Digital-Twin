#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22

#define PIR_PIN 13
#define LDR_PIN 34
#define MQ2_PIN 35

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
}

void reconnect() {
  while (!client.connected()) {

    Serial.print("Connecting MQTT...");

    if (client.connect("CollegeESP32")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  dht.begin();

  pinMode(PIR_PIN, INPUT);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  int pir = digitalRead(PIR_PIN);

  int ldr = analogRead(LDR_PIN);

  int gas = analogRead(MQ2_PIN);

  char msg[20];

  dtostrf(temp, 1, 2, msg);
  client.publish("college/classroom/temperature", msg);

  dtostrf(hum, 1, 2, msg);
  client.publish("college/classroom/humidity", msg);

  sprintf(msg, "%d", pir);
  client.publish("college/classroom/occupancy", msg);

  sprintf(msg, "%d", ldr);
  client.publish("college/classroom/light", msg);

  sprintf(msg, "%d", gas);
  client.publish("college/lab/gas", msg);

  Serial.println("Published Sensor Data");

  delay(3000);
}
