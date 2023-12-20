#include "WiFi.h"
#include "ESPAsyncWebServer.h"

const char* ssid = "ESP32-Access-Point"; // Declarar nome da rede
const char* password = "123456789"; // Declarar senha da rede

AsyncWebServer server(80); // Função para criar um servidor web assincrono na porta 80

void onDataReceived(AsyncWebServerRequest* request) { // Função para quando o servidor receber dados do cliente
  // Handling GET request
  String lat = request->arg("lat"); // Transformar o argumento lat em uma string
  String longi = request->arg("long"); // Transformar o argumento long em uma string

  Serial.println("Received data (GET):"); // Printar que recebeu os dados
  Serial.println("Latitude: " + lat); // Printar os dados de latitude recebidos
  Serial.println("Longitude: " + longi); // Printar os dados de longitude recebidos

  request->send(200, "text/plain", "Data received successfully (GET)"); // Responde que os dados foram recebidos com sucesso
}

void setup() {
  Serial.begin(115200); // Iniciar o SM na frequência 115200

  WiFi.softAP(ssid, password); // Iniciando um Access Point no ESP

  IPAddress IP = WiFi.softAPIP(); // Declarar uma variável para ser o IP do AP
  Serial.print("AP IP address: "); 
  Serial.println(IP); // Printar o IP do AP

  server.on("/send-data", HTTP_GET, onDataReceived); // Declarar o servidor para receber chamadas GET

  server.begin(); // Iniciar o servidor
}

void loop() {

}
