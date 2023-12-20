// Incluir bibliotecas

#include "WiFi.h"
#include <Wire.h>
#include "ESPAsyncWebServer.h"
#include <PubSubClient.h>
#include <HTTPClient.h>
#include "WiFiClientSecure.h"
#include <ArduinoJson.h>
#include <WebSocketsServer.h>

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


unsigned long tempoUltimaMensagem;

WebSocketsServer webSocket = WebSocketsServer(81); // WebSocket server on port 81

// Iniciação do client de WIFI e conexão MQTT
WiFiClientSecure client;
PubSubClient mqtt_client(client); 


void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
    } break;
    case WStype_TEXT:
      Serial.printf("[%u] Received text: %s\n", num, payload);

      // Send a response to the client
      webSocket.sendTXT(num, "Hello from ESP32 Server!");
      break;
  }
}

bool conectarWiFi()
{
    Serial.println("Tentando estabelecer uma conexao com a rede Wi-Fi...");
    const unsigned long tempoInicial = millis();
    WiFi.begin(ssidWIFI, passwordWIFI);
    do
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("Conexao Wi-Fi estabelecida com sucesso!");
            return true;
        }
        delay(1000);
    } while ((millis() - tempoInicial) < 30000L);
    Serial.println("Impossivel estabelecer uma conexao com a rede Wi-Fi!");
    return false;
}

bool verificarWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
        return true;
    Serial.println("Conexao Wi-Fi perdida.");
    WiFi.disconnect(true);
    delay(5000);
    return conectarWiFi();
}

bool conectarMQTT()
{
    Serial.println("Tentando estabelecer uma conexao com o broker MQTT...");
    const unsigned long tempoInicial = millis();
    char clientId[32];
    do
    {
        if (mqtt_client.connect("Armazem1", mqtt_user , mqtt_pass))
        {
            if (!mqtt_client.subscribe("rename_device"))
            {
                Serial.println("Erro ao se inscrever em rename_device");
            }
            Serial.print("Conectado ao broker MQTT utilizando o client id ");
            Serial.print(clientId);
            Serial.println(", e inscrito no tópico rename_device");
            return true;
        }
        delay(1000);
        if (!verificarWiFi())
            break;
    } while ((millis() - tempoInicial) < 30000L);
    Serial.println("Impossivel estabelecer uma conexao com o broker MQTT!");
    return false;
}

bool verificarMQTT()
{
    if (!verificarWiFi())
        return false;
    if (mqtt_client.connected())
        return true;
    Serial.println("Conexao com o broker perdida.");
    mqtt_client.disconnect();
    delay(5000);
    return conectarMQTT();
}

void mensagemRecebida(char *topic, byte *payload, unsigned int length)
{
    String mensagem = "";
    Serial.print("Mensagem recebida! Topic: ");
    Serial.print(topic);
    Serial.print(" - Payload (");
    Serial.print(length);
    Serial.print(" bytes): ");
    for (int i = 0; i < length; i++)
    {
        mensagem += (char)payload[i];
    }
    Serial.println(mensagem);
    // if (strcmp(topic, "relay_inteli") == 0)
    // {
    //     if (mensagem == "liga")
    //     {
    //         Serial.println("LIGA");
    //     }
    //     if (mensagem == "desliga")
    //     {
    //         Serial.println("DESLIGA");
    //     }
    // }
    // Serial.println();
}

void setup() {
  Serial.begin(74880); // Iniciar o SM na frequência 115200

  WiFi.softAP(ssid, password); // Iniciando um Access Point no ESP

  IPAddress IP = WiFi.softAPIP(); // Declarar uma variável para ser o IP do AP
  Serial.print("AP IP address: "); 
  Serial.println(IP); // Printar o IP do AP

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);



  // server.begin(); // Iniciar o servidor

  conectarWiFi();

  // Set up das keys de autenticação
  client.setCACert(CA_cert);          //Root CA certificate

  // Só é necessario se for configurado no broker, no caso do HiveMQ não é ncessário
  // client.setCertificate(ESP_CA_cert); 
  // client.setPrivateKey(ESP_RSA_key); 
  
  // Configurar a conexão
  mqtt_client.setServer(mqtt_server, port);

  mqtt_client.setCallback(mensagemRecebida);


  tempoUltimaMensagem = millis();
  randomSeed(micros());
  conectarMQTT();
  Serial.println("Iniciando o envio...");
}

String nomeTeste = "Claudio";

const char* serverURL = "http://192.168.4.1/rename-device";

void sendRenameRequest(String newName) { // Função para mandar dados

  String url = String(serverURL) + "?nomeNovo=" + newName; // Adicionar os parametros para serem mandados na URL ( LAT e LONG )

// Enviar requisição HTTP
  HTTPClient http;
  http.begin(url);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode); // Printar código HTTP

    Serial.println("Dados enviados para o ativo");

    delay(1000);
  } else {
    Serial.print("HTTP Request failed: ");
    Serial.println(http.errorToString(httpResponseCode).c_str()); // Caso falhe, printar o código de erro HTTP
  }

  http.end(); // Liberar o HTTP
}



bool publishMessage()
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

  if (lat != 0 & longi != 0){

    if(mqtt_client.publish("GPS", jsonBuffer)){
      Serial.println("Mensagem enviada com sucesso");
      return true;

      delay(1000);
    } else {
      Serial.println("Falha ao enviar mensagem");
      return false;

      delay(1000);
    }
  } else{
    Serial.println("Não há dados para enviar!");
  }

}

void loop() {
  // server.on("/send-data", HTTP_GET, onDataReceived); // Declarar o servidor para receber chamadas GET
  if (!verificarMQTT())
      return;
  mqtt_client.loop();


  const unsigned long agora = millis();
    if ((agora - tempoUltimaMensagem) >= 5000L)
    {
        tempoUltimaMensagem = agora;
        publishMessage();
        // webSocket
        // sendRenameRequest(nomeTeste);
    }

    webSocket.loop();

}