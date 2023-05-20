#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Informações de conexão do MQTT
const char* mqtt_server = "192.168.0.45"; // Endereço IP do seu broker MQTT
const int mqtt_port = 1883;
const char* mqtt_user = "arduino"; // Nome de usuário do MQTT
const char* mqtt_password = "123"; // Senha do MQTT
const char* mqtt_topic = "temperatura/arduino"; // Tópico do MQTT

// Informações do sensor de temperatura e LED
int sensor_pin = A0;
int led_pin = 13;

byte mac[] = { 255, 255, 255, 255, 255, 255 }; // Endereço MAC do seu módulo Ethernet

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);
  Ethernet.begin(mac);
  mqttClient.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  int valor_sensor = analogRead(sensor_pin);
  float temperatura = (valor_sensor * 0.00488) * 100.0;
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" graus C");
  if (temperatura >= 25.0) {
    digitalWrite(led_pin, HIGH);
    mqttClient.publish(mqtt_topic, "on");
  } else {
    digitalWrite(led_pin, LOW);
    mqttClient.publish(mqtt_topic, "off");
  }
  mqttClient.loop();
  delay(1000);
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Conectando ao MQTT...");
    if (mqttClient.connect("arduino_client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado ao MQTT");
      mqttClient.subscribe(mqtt_topic);
    } else {
      Serial.print("Falha na conexão com o MQTT, rc=");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}
