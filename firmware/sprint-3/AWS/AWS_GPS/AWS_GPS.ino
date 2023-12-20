#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <TinyGPSPlus.h>

// Inicialização do módulo GPS
TinyGPSPlus gps;

// Variáveis de latitude e longitude
double lat;
double longi;

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Conectando ao Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Configure o WiFiClientSecure para usar as credenciais do dispositivo AWS IoT
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Conecte-se ao corretor MQTT no endpoint AWS IoT que definimos anteriormente
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Crie um manipulador de mensagens
  client.setCallback(messageHandler);

  Serial.print("Conectando ao AWS IoT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Inscreva-se em um tópico
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Conectado!");
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["Latitude"] = lat;
  doc["Longitude"] = longi;
  char jsonBuffer[512];

  serializeJson(doc, jsonBuffer); // imprima para o cliente

  Serial.println(jsonBuffer);

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("recebido: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);

  // Acesse os campos na carga útil
  int Latitude = doc["Latitude"];
  int Longitude = doc["Longitude"];

  Serial.print("Latitude: ");
  Serial.println(Latitude);
  Serial.print("Longitude: ");
  Serial.println(Longitude);
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

  // Configuração dos LEDs
  pinMode(19, OUTPUT);  // LED Vermelho
  pinMode(5, OUTPUT);   // LED Verde
  pinMode(18, OUTPUT);  // LED Azul

  connectAWS();
}

void loop() {
  // Leitura e processamento de dados do módulo GPS
  for (int i = 0; i < 30; i++) {
    while (Serial2.available() > 0)
      if (gps.encode(Serial2.read()))
        displayInfo();

    // Verifica se o GPS está conectado
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println(F("GPS não detectado: verifique a conexão."));
      while (true);
    }
  }

  // Verifica se a latitude e longitude são válidas
  if (lat != 0 && longi != 0) {
    // LED vermelho piscando caso se conecte ao GPS
    Serial.println("GPS conectado");
    // digitalWrite(19, HIGH);
    // delay(500);
    // digitalWrite(19, LOW);
  } else {
    // LED vermelho aceso caso não se conecte ao GPS
    Serial.println("GPS desconectado");
    // digitalWrite(19, HIGH);
  }

  publishMessage();
  client.loop();
  delay(1000);
}

// Função para exibir informações de latitude e longitude
void displayInfo() {
  // Exibe informações de latitude e longitude do GPS
  Serial.print(F("Localização: "));
  if (gps.location.isValid()) {
    Serial.print("Lat: ");
    lat = gps.location.lat();
    Serial.print(lat, 6);

    Serial.print(F(","));

    Serial.print("Lng: ");
    longi = gps.location.lng();
    Serial.print(longi, 6);

    Serial.println();
  } else {
    Serial.print(F("INVÁLIDO"));
  }
}
