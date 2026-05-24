//TODO: buzzer para chegada
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Button.h"
#include "Led.h"
#include "Hall.h"

#define MSG_SIZE 1024

const char* ssid = "ProjetoMInDS";
const char* password = "Doi39x-Wa!";

// JSON
JsonDocument doc;
DynamicJsonDocument doc_send(MSG_SIZE);

// Broker MQTT
const char* mqtt_server = "192.168.1.114";
const int mqtt_port = 1883;
const char* pub_topic = "grupo5/hall/chamada_andar";
const char* sub_topic = "grupo5/elevador/estado";

// Prepara o cliente MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup_wifi(){
  delay(100);
  Serial.println();
  Serial.println("Conectando-se a: " + String(ssid));

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi conectado\nEndereço de IP: " + WiFi.localIP().toString());
}

void verify_wifi(){
  
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi desconectado, reconectando...");
    setup_wifi();
  }
}

void initializeJson(){
  doc.clear();
  doc_send.clear();
}

bool serializeAndSend(const char* topic){
  char sendBuffer[MSG_SIZE];
  serializeJson(doc_send, sendBuffer);

  Serial.println("MQTT msg :");
  Serial.println(sendBuffer);

  return mqttClient.publish(topic, sendBuffer, strlen(sendBuffer));
}

// Callback: executa sempre que receber mensagem no tópico inscrito (qualquer subscribe chama este metodo)
void callback(char* topic, byte* payload, unsigned int length) {
  //Apenas mostra a mensagem recebida, sem processar. Também transforma o payload em string para facilitar a leitura.
  String message;
  Serial.print("MQTT Recv: ");
  for (int i = 0; i < length; i++) {
  message += (char)payload[i];
  }
  Serial.println(message);

  //Deserializa e processa a mensagem recebida
  doc.clear();
  deserializeJson(doc, message);
  
  if(strcmp(topic, sub_topic) == 0){
    int andar_atual = doc["andar_atual"];
    int porta_status = doc["porta_status"];
    bool elevador_chegada = doc["elevador_chegada"];
    hall.setElevadorAndar(andar_atual);
    hall.setPortaStatus(porta_status);
    hall.setElevadorChegada(elevador_chegada);
    Serial.print("Andar atual: ");
    Serial.println(andar_atual);
    Serial.print("Status da porta: ");
    Serial.println(porta_status);
    Serial.print("Chegada do elevador: ");
    Serial.println(String(elevador_chegada));
  }
}

void setup_mqtt(){
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

Hall hall;
Button buttons[TOTAL_ANDARES] = { Button(12), Button(14), Button(27), Button(26) };
Led leds[TOTAL_ANDARES + 1] = { Led(25), Led(33), Led(32), Led(15), Led(13) };

// conecta no MQTT e se inscreve para escuta de mensagens em tópico
void reconnect_mqtt(){
  while(!mqttClient.connected()){
    Serial.println("Conectando ao broker MQTT...");
    //Talvez precise mudar este "ESP32Client" para algo mais específico para nao gerar conflitos com outros trabalhos
    if(mqttClient.connect("ESP32Client")){
      Serial.println("Conectado ao broker MQTT");
      mqttClient.subscribe(sub_topic);
    } else {
      Serial.print("Falha na conexão, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void readButtons(){
  for (int i = 0; i < TOTAL_ANDARES; i++) {
    if (buttons[i].readStatus() == HIGH) {
      Serial.print("Botão pressionado no andar: ");
      Serial.println(i);
      doc_send.clear();
      doc_send["andarDestino"] = i;
      serializeAndSend(pub_topic);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  setup_mqtt();
  initializeJson();
  for (int i = 0; i < TOTAL_ANDARES; i++) {
    buttons[i].init();
    leds[i].init();
  }
  leds[0].ligar();

}

void loop() {
  verify_wifi();
  if(!mqttClient.connected()){
    reconnect_mqtt();
  }
  mqttClient.loop();

  readButtons();
  hall.atualizar(leds);

}