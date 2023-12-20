/*
 * SD Card Module Interfacing with ESP32 Wifi + Bluetooth Module
 * https://www.electroniclinic.com/
 */

// Incluindo bibliotecas

#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsClient.h>

// Instanciar LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "ESP32-Access-Point"; // Declarar nome da rede para o esp se conectar
const char* password = "111222333"; // Senha da rede para o esp se conectar

const char* serverURL = "http://192.168.4.1/send-data"; // URL do servidor do ESP central para repassar os dados

String ativoName = "RNP-01";

String dispositivoName = "GEOLOC-01";

TinyGPSPlus gps;

const char *serverAddress = "esp32-server-ip-address";


// Definindo variáveis

#define SD_CS 5
String dataMessage;

float randomData;

double Latitude;
double Longitude;

int year;
int month;
int day;

int ledVermelho = 33;
int ledAzul = 32;

String dateString;

bool SDcardUpdate = true;

WebSocketsClient webSocket;

void onWebSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected!");
      break;
    case WStype_CONNECTED:
      Serial.println("Connected to ESP32 Server!");
      break;
    case WStype_TEXT:
      Serial.printf("Received text: %s\n", payload);
      break;
  }
}

void setup() {
  pinMode(ledAzul, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  // server.begin(); // Iniciar o servidor

  // server.on("/send-data", HTTP_GET, onDataReceived); // Declarar o servidor para receber chamadas GET

  webSocket.begin(serverAddress, 81); // WebSocket server on port 81
  webSocket.onEvent(onWebSocketEvent);


  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);  
  lcd.print("Iniciando dispositivo");

  Serial.begin(115200); // Iniciar o MS na frequência 115200

  Serial2.begin(9600);

  delay(3000);


  lcd.setCursor(0, 0);
  lcd.print("Inciando SD...");
  delay(3000);

  // Iniciar o módulo do cartão SD
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  // Checar se tem cartão SD
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    lcd.setCursor(2, 1);
    lcd.print("Insira um cartão SD!");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    lcd.setCursor(0, 0);
    lcd.print("Error inciando SD!");
    return;    // Se inicialização der algum erro
  }
  File file = SD.open("/dataGPS.json");
  // Checar se foi possível abrir o arquivo
  if(!file) { // Se não, criar o arquivo
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/dataGPS.json", "");
  }
  else {
    Serial.println("File already exists");  
  }
  // Fechar arquivo
  file.close();

  lcd.setCursor(0, 0);
  lcd.print("Conectando no AP...");

  WiFi.begin(ssid, password); // Conectar o ESP a wifi do ESP principal
  Serial.println("Connecting"); // Printar conectando

  for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) { // Tentar conectar 10 vezes
    delay(500);
    Serial.print(".");
  }

  if(WiFi.status() != WL_CONNECTED) { // Caso não consiga conectar printar que não foi possível
    Serial.println("Connection Failed!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WIFI nao encontrado!");
    return;
  } else {
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: "); // Ao se conectar printar que foi conectado e o IP local
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectado no AP:" + String(WiFi.localIP()));
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  webSocket.loop();

  WiFi.begin(ssid, password); // Conectar o ESP a wifi do ESP principal
  Serial.println("Trying to connect to WIFI"); // Printar conectando

  for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) { // Tentar conectar 10 vezes
    delay(500);
    Serial.print(".");
  }

  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }

    year = gps.date.year();
    month = gps.date.month();
    day = gps.date.day();

    dateString = String(year) + "-" + String(month) + "-" + String(day);

    Serial.print(dateString);

    if (WiFi.status() == WL_CONNECTED) { // Enquanto estiver conectado a Wi-Fi do outro ESP
      Serial.println("Connection with ESP32 hotspot made!");

      lcd.setCursor(0, 0);
      lcd.print("Conectado no Hotspt ESP32");

      digitalWrite(ledAzul, HIGH);

      if(SDcardUpdate == false) {
        // logSDCard();
        lcd.setCursor(0, 0);
        lcd.print("Atualizando cartao SD...");

        updateSDCard();
        SDcardUpdate = true;

        lcd.setCursor(0, 0);
        lcd.print("Cartão SD atualizado!");
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Cartão SD já atualizado!");
        Serial.println("SD Card already updated!");
        // sendSensorData(Latitude, Longitude, dateString, ativoName, dispositivoName); // Chamar a função sendSensorData
        webSocket.sendTXT("Hello from ESP32 Client!");  
        delay(5000); // Aguardar 5 segundos antes de mandar dados novamente afim de prevenir erros
      }
    } else {
      digitalWrite(ledAzul, LOW);
      lcd.setCursor(0, 0);
      lcd.print("Sem conexao com ESP32");

      Serial.println("Connection with ESP32 hotspot not made!");
      Serial.println("WiFi Disconnected"); // Caso disconecte da Wi-Fi, printar o seguinte.
      logSDCard();
      SDcardUpdate = false;
    }

    if(Latitude == 0 & Longitude == 0){
      digitalWrite(ledVermelho, HIGH);
    } else{
      digitalWrite(ledVermelho, HIGH);
      delay(500);
      digitalWrite(ledVermelho, LOW);
    }
    
    delay(5000); //Wait for 5 seconds before writing the next data 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Latitude: " + String(Latitude, 4));
    lcd.setCursor(0, 1);
    lcd.print("Longitude:" + String(Longitude, 4));
    delay(4000);
}

void sendSensorData(double latitudeHTTP, double longitudeHTTP, String dateHTTP, String ativoHTTP, String dispositivoHTTP) { // Função para mandar dados

  String url = String(serverURL) + "?lat=" + String(latitudeHTTP, 6) +
               "&long=" + String(longitudeHTTP, 6) + "&date=" + dateHTTP + "&ativo=" + ativoHTTP + "&dispositivo=" + dispositivoHTTP; // Adicionar os parametros para serem mandados na URL ( LAT e LONG )

// Enviar requisição HTTP
  HTTPClient http;
  http.begin(url);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode); // Printar código HTTP

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dados enviados para o hotspot!");
    delay(1000);
  } else {
    Serial.print("HTTP Request failed: ");
    Serial.println(http.errorToString(httpResponseCode).c_str()); // Caso falhe, printar o código de erro HTTP
  }

  http.end(); // Liberar o HTTP
}


// Escrever os dados no cartão SD
void logSDCard() {

  dataMessage = "{";
  dataMessage += "\"Latitude\":" + String(Latitude, 6) + ",";
  dataMessage += "\"Longitude\":" + String(Longitude, 6) + ",";
  dataMessage += "\"Date\":\"" + dateString + "\",";
  dataMessage += "\"Ativo\":\"" + ativoName + "\",";
  dataMessage += "\"Dispositivo\":\"" + dispositivoName + "\"";
  dataMessage += "},\n";
  
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/dataGPS.json", dataMessage.c_str());
}
// Função para escrever os dados no cartão
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
// Dar append dos dados no cartão SD
void appendFile(fs::FS &fs, const char * path, const char * message) {
  lcd.clear();
  
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
    lcd.setCursor(0, 0);
    lcd.print("Dados salvos no SD!");
  }
  file.close();
}

void displayInfo()
{
  Serial.print(F("Location: "));

  if (gps.location.isValid()){

    Serial.print("Lat: ");

    Latitude = gps.location.lat();

    Serial.print(gps.location.lat(), 6);

    Serial.print(F(","));

    Serial.print("Lng: ");

    Serial.print(gps.location.lng(), 6);

    Longitude = gps.location.lng();

    Serial.println();


    // year = gps.date.year();
    // month = gps.date.month();
    // day = gps.date.day();

    // dateString = String(year) + "-" + String(month) + "-" + String(day);

    // Serial.print(dateString);

  }  
  else
  {
    Serial.print(F("INVALID"));
  }


}

// Funcão ler arquivo cartão SD
void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
}


void updateSDCard() {
  // number of lines in the file:
  // int numberOfLines = 0;
  String line;
  // File file = SD.open("/dataGPS.json");
  // if(file){
  //   while(file.available()){
  //     line = file.readStringUntil('\n');
  //     numberOfLines++;
  //   }
  //   file.close();
  // } else {
  //   Serial.println("Failed to open file");
  // }

  // Mandar todas as linhas para o servidor
  File file = SD.open("/dataGPS.json");
  if(file){
    while(file.available()){
      line = file.readStringUntil('\n');
      // Ler o JSON de Latitude e Longitude
      Serial.println(line);

      // Analisar dados JSON de todos os dados
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, line);
      if (error) {
        Serial.println("Failed to parse JSON");
        return;
      }

      // Extrair a latitude e longitude
      double latitudeData = doc["Latitude"];
      double longitudeData = doc["Longitude"];
      String dateData = doc["Date"];
      String ativoData = doc["Ativo"];
      String dispositivoData = doc["Dispositivo"];


      // Printar ambos no monitor
      Serial.print("Latitude Data: ");
      Serial.println(latitudeData, 6);
      Serial.print("Longitude Data: ");
      Serial.println(longitudeData, 6);  


      sendSensorData(latitudeData, longitudeData, dateData, ativoData, dispositivoData);
      delay(5000);    
    }
    Serial.println("Update made!");
    writeFile(SD, "/dataGPS.json", "");
    file.close();
  } else {
    Serial.println("Failed to open file");
  }
}
