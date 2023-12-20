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

const char* ssid = "ESP32-Access-Point"; // Declarar nome da rede para o esp se conectar
const char* password = "111222333"; // Senha da rede para o esp se conectar

const char* serverURL = "http://192.168.4.1/send-data"; // URL do servidor do ESP central para repassar os dados

TinyGPSPlus gps;

// Definindo variáveis

#define SD_CS 5
String dataMessage;

float randomData;

double Latitude;
double Longitude;

int year;
int month;
int day;

String dateString;

bool SDcardUpdate = true;


void setup() {
  Serial.begin(115200); // Iniciar o MS na frequência 115200

  Serial2.begin(9600);

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
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
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

  WiFi.begin(ssid, password); // Conectar o ESP a wifi do ESP principal
  Serial.println("Connecting"); // Printar conectando

  for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) { // Tentar conectar 10 vezes
    delay(500);
    Serial.print(".");
  }

  if(WiFi.status() != WL_CONNECTED) { // Caso não consiga conectar printar que não foi possível
    Serial.println("Connection Failed!");
    return;
  } else {
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: "); // Ao se conectar printar que foi conectado e o IP local
    Serial.println(WiFi.localIP());
  }
}
void loop() {

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

      if(SDcardUpdate == false) {
        // logSDCard();
        updateSDCard();
        SDcardUpdate = true;
      } else {
        Serial.println("SD Card already updated!");
        sendSensorData(Latitude, Longitude); // Chamar a função sendSensorData
        delay(5000); // Aguardar 5 segundos antes de mandar dados novamente afim de prevenir erros
      }
    } else {
      Serial.println("Connection with ESP32 hotspot not made!");
      Serial.println("WiFi Disconnected"); // Caso disconecte da Wi-Fi, printar o seguinte.
      logSDCard();
      SDcardUpdate = false;
    }

    // randomNumbers();
    // logSDCard();

    delay(5000); //Wait for 5 seconds before writing the next data 
}

void sendSensorData(double latitudeHTTP, double longitudeHTTP) { // Função para mandar dados


  String url = String(serverURL) + "?lat=" + String(latitudeHTTP, 6) +
               "&long=" + String(longitudeHTTP, 6); // Adicionar os parametros para serem mandados na URL ( LAT e LONG )

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


// Escrever os dados no cartão SD
void logSDCard() {

  dataMessage = "{";
  dataMessage += "\"Latitude\":" + String(Latitude, 6) + ",";
  dataMessage += "\"Longitude\":" + String(Longitude, 6) + ",";
  dataMessage += "\"Date\":\"" + dateString + "\",";
  dataMessage += "\"Type\":\"RNP-01\"";
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
  }
  file.close();
}
void randomNumbers()
{
  randomData = random();
  Serial.println(randomData);
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

      // Printar ambos no monitor
      Serial.print("Latitude Data: ");
      Serial.println(latitudeData, 6);
      Serial.print("Longitude Data: ");
      Serial.println(longitudeData, 6);  


      sendSensorData(latitudeData, longitudeData);
      delay(5000);    
    }
    Serial.println("Update made!");
    writeFile(SD, "/dataGPS.json", "");
    file.close();
  } else {
    Serial.println("Failed to open file");
  }
}
