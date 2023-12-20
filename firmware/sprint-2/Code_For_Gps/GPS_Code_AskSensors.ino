#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <TinyGPSPlus.h>

// Inicialização do módulo GPS
TinyGPSPlus gps;

// Inicialização do Wi-Fi
WiFiMulti WiFiMulti;
HTTPClient ask;

// Configurações de Wi-Fi e API
const char* ssid = "Iphone12";
const char* password = "rodrigo123";
const char* apiKeyIn = "zXhxyH4uTCIQKhCHIpyvaKseKAKK6YgX";
const unsigned int writeInterval = 25000;  // Intervalo de escrita (em ms)

// Configurações da API do AskSensors
const char* host = "api.asksensors.com";
const int httpPort = 80;

// Variáveis de latitude e longitude
double lat;
double longi;

bool wifiConnected = false;

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(9600);
  Serial2.begin(9600);

  Serial.println("*****************************************************");
  Serial.println("********** Program Start: Connect ESP32 to AskSensors.");
  Serial.println("Wait for WiFi... ");

  // Conecta à rede Wi-Fi
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    wifiConnected = true;
    delay(500);
  }

  // Conectado
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Configuração dos LEDs
  pinMode(19, OUTPUT);  // LED Vermelho
  pinMode(5, OUTPUT);   // LED Verde
  pinMode(18, OUTPUT);  // LED Azul

    // Esperar de 3 segundos
  delay(3000);
}

void loop() {

    // Verifica se o Wi-Fi está conectado
  if (wifiConnected == true) {
    // Piscar LED Azul se conectado ao Wi-Fi
    digitalWrite(18, HIGH);
    delay(500);
    digitalWrite(18, LOW);
    digitalWrite(18, HIGH);
    delay(500);
    digitalWrite(18, LOW);
  } else {
    digitalWrite(18, HIGH); // LED Azul aceso se não conectado ao Wi-Fi
  }

  // Leitura e processamento de dados do módulo GPS
  for (int i = 0; i < 30; i++) {
    while (Serial2.available() > 0)
      if (gps.encode(Serial2.read()))
        displayInfo();

    // Verifica se o GPS está conectado
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println(F("No GPS detected: check wiring."));
      while (true);
    }
  }

  // Verifica se a latitude e longitude são válidas
  if (lat != 0 && longi != 0) {

    // LED vermelho piscando caso se conecte ao GPS
    Serial.println("GPS connected");
    digitalWrite(19, HIGH);
    delay(500);
    digitalWrite(19, LOW);
  } else {

    // LED vermelho aceso caso não se conecte ao GPS
    Serial.println("GPS disconnected");
    digitalWrite(19, HIGH);
  }


  // Criação e envio da URL para o AskSensors
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    digitalWrite(18, HIGH);
    return;
  } else {
    wifiConnected = true;

    // Envia dados para o AskSensors  
    String url = "http://api.asksensors.com/write/"; // URL base
    url += apiKeyIn;
    url += "?module1=";
    url += String(lat, 6);
    url += "&module2=";
    url += String(longi, 6);


    // Piscar LED azul se os dados forem enviados com sucesso
    digitalWrite(18, HIGH);
    delay(300);
    digitalWrite(18, LOW);

    Serial.print("Requesting URL: ");
    Serial.println(url);

    ask.begin(url);
    int httpCode = ask.GET();

    // Verifica se os dados foram enviados com sucesso
    if (httpCode > 0) {
        // Lê a resposta
      String payload = ask.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    } else {
      Serial.println("Error on HTTP request");
    }

    ask.end();
    Serial.println("End");
    Serial.println("*****************************************************");
  }

  client.stop();
  delay(writeInterval);
}

// Função para exibir informações de latitude e longitude
void displayInfo() {
  // Exibe informações de latitude e longitude do GPS
  Serial.print(F("Location: "));
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
    Serial.print(F("INVALID"));
  }
}
