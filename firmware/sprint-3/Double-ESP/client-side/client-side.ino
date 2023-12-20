#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "ESP32-Access-Point"; // Declarar nome da rede para o esp se conectar
const char* password = "123456789"; // Senha da rede para o esp se conectar

const char* serverURL = "http://192.168.4.1/send-data"; // URL do servidor do ESP central para repassar os dados

void setup() {
  Serial.begin(115200); // Iniciar o SM na frequência 115200

  // Connect to WiFi
  WiFi.begin(ssid, password); // Conectar o ESP a wifi do ESP principal
  Serial.println("Connecting"); // Printar conectando
  while (WiFi.status() != WL_CONNECTED) { // Função para printar . ao fim do conectando até o ESP se conectar
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: "); // Ao se conectar printar que foi conectado e o IP local
  Serial.println(WiFi.localIP());
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) { // Enquanto estiver conectado a Wi-Fi do outro ESP
    sendSensorData(); // Chamar a função sendSensorData
    delay(5000); // Aguardar 5 segundos antes de mandar dados novamente afim de prevenir erros
  } else {
    Serial.println("WiFi Disconnected"); // Caso disconecte da Wi-Fi, printar o seguinte.
    delay(1000); // Esperar 1 segundo antes de tentar mandar dados novamente.
  }
}

void sendSensorData() { // Função para mandar dados

  float fakeLatitude = random(-90.0, 90.0); // Gerar latitudes falsas para fins de teste
  float fakeLongitude = random(-180.0, 180.0); // Gerar longitudes falsas para fins de teste


  String url = String(serverURL) + "?lat=" + String(fakeLatitude) +
               "&long=" + String(fakeLongitude); // Adicionar os parametros para serem mandados na URL ( LAT e LONG )

// Enviar requisição HTTP
  HTTPClient http;
  http.begin(url);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode); // Printar código HTTP
  } else {
    Serial.print("HTTP Request failed: ");
    Serial.println(http.errorToString(httpResponseCode).c_str()); // Caso falhe, printar o código de erro HTTP
  }

  http.end(); // Liberar o HTTP
}
