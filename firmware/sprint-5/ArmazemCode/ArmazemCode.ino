// Incluir bibliotecas

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <PubSubClient.h>
#include "WiFiClientSecure.h"
#include <ArduinoJson.h>

// Certificado HiveMQ
const char* CA_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----";

const char* ssidWIFI        = "Iphone12";        // Nome do WIFI
const char* passwordWIFI    = "rodrigo123";   // Senha do WIFI

const char* mqtt_server = "3d23a9722fe5468cbb05f9e25035c21e.s2.eu.hivemq.cloud";  // Endereço do host do HiveMQ
int port                = 8883;             // Porta do broker

// Credenciais criadas no HiveMQ
const char* mqtt_user   = "ESP32_Connect4";           
const char* mqtt_pass   = "123drigo"; 

// Topicos de publicação
String topicLat = "GPS/Latitude";
String topicLong = "GPS/Longitude"; 

const char* ssid = "ESP32-Access-Point"; // Declarar nome da rede do ESP
const char* password = "111222333"; // Declarar senha da rede do ESP

String lat;
String longi;
String date;
String ativoName;
String dispositivoName;

AsyncWebServer server(80); // Função para criar um servidor web assincrono na porta 80

// Iniciação do client de WIFI e conexão MQTT
WiFiClientSecure client;
PubSubClient mqtt_client(client); 


void onDataReceived(AsyncWebServerRequest* request) { // Função para quando o servidor receber dados do cliente
  // Handling GET request
  lat = request->arg("lat"); // Transformar o argumento lat em uma string
  longi = request->arg("long"); // Transformar o argumento long em uma string
  date = request->arg("date"); // Transformar o argumento lat em uma string
  ativoName = request->arg("ativo"); // Transformar o argumento long em uma string
  dispositivoName = request->arg("dispositivo"); // Transformar o argumento long em uma string


  Serial.println("Received data (GET):"); // Printar que recebeu os dados
  Serial.println("Latitude: " + lat); // Printar os dados de latitude recebidos
  Serial.println("Longitude: " + longi); // Printar os dados de longitude recebidos
  Serial.println("Date: " + date); // Printar os dados de latitude recebidos
  Serial.println("Ativo: " + ativoName); // Printar os dados de longitude recebidos
  Serial.println("Dispositivo: " + dispositivoName); // Printar os dados de longitude recebidos

  request->send(200, "text/plain", "Data received successfully (GET)"); // Responde que os dados foram recebidos com sucesso
}

void setup() {
  Serial.begin(74880); // Iniciar o SM na frequência 115200

  WiFi.softAP(ssid, password); // Iniciando um Access Point no ESP

  IPAddress IP = WiFi.softAPIP(); // Declarar uma variável para ser o IP do AP
  Serial.print("AP IP address: "); 
  Serial.println(IP); // Printar o IP do AP

  server.on("/send-data", HTTP_GET, onDataReceived); // Declarar o servidor para receber chamadas GET

  server.begin(); // Iniciar o servidor

  Serial.print("Attempting to connect to SSID WIFI: ");
  Serial.println(ssidWIFI);
  WiFi.begin(ssidWIFI, passwordWIFI);
  // Tentativa de conexão com WIFI
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // Esperar 1 segundo para tentar a conexão novamente
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssidWIFI);

  // Set up das keys de autenticação
  client.setCACert(CA_cert);          //Root CA certificate

  // Só é necessario se for configurado no broker, no caso do HiveMQ não é ncessário
  // client.setCertificate(ESP_CA_cert); 
  // client.setPrivateKey(ESP_RSA_key); 
  
  // Configurar a conexão
  mqtt_client.setServer(mqtt_server, port);
}

void loop() {
  // server.on("/send-data", HTTP_GET, onDataReceived); // Declarar o servidor para receber chamadas GET

  MQTTconnection();
}


void publishMessage()
{
  // ----------------- TENTATIVA DE ENVIAR UM JSON ---------------------
  StaticJsonDocument<200> doc;
  doc["Latitude"] = lat;
  doc["Longitude"] = longi;
  doc["Date"] = date;
  doc["Ativo"] = ativoName;
  doc["Dispositivo"] = dispositivoName;

  char jsonBuffer[512];

  serializeJson(doc, jsonBuffer); // print to client

  Serial.println(jsonBuffer);

  // String latStr = String(lat);
  // String longiStr = String(longi);

  mqtt_client.publish("GPS", jsonBuffer);

  delay(1000);
}

// Conexão MQTT
void MQTTconnection() {
  // LOOP até o client ser reconectado
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Tentativa de conexão

    // Conexão do client com o broker
    // Primeiro paramentro é a o ID do client que está solicitando a conexão
    if (mqtt_client.connect("ESP32", mqtt_user , mqtt_pass)) {
      // Printar o estado de conxão
      Serial.print("Connected, mqtt_client state: ");
      Serial.println(mqtt_client.state());



      if(lat == 0 && longi == 0){
        Serial.println("Não há dados para enviar");
      } else {
        publishMessage();
      }
    } else { // Caso de erro
      Serial.print("failed, rc=");
      Serial.println(mqtt_client.state());

      Serial.println("WiFiClientSecure client state:");
      char lastError[100];
      client.lastError(lastError,100);  // último erro do WiFiClientSecure
      Serial.print(lastError);

      Serial.println(" try again in 5 seconds");
      // Esperar 5 para tentar a conexão novamente
      delay(5000);
    }
  }
}
