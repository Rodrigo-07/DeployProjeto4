// Incluindo bibliotecas

#include "FS.h" // Versão: 2.1 - Bilbioteca que vem junta com o core do esp32
#include "SD.h" // Versão: 1.2.4 by Arduino, SparkFun
#include <SPI.h> // Versão: 2.1 - Bilbioteca que vem junta com o core do esp32
#include <TinyGPSPlus.h> // Versão: 1.0.3 by Mikal Hart
#include <WiFi.h> // Versão: 2.1 - Bilbioteca que vem junta com o core do esp32
#include <HTTPClient.h> //  Versão: 2.2.0 by Adrian McEwen 
#include <ArduinoJson.h> // Versão: 6.21.3 by Benoit Blanchon (https://github.com/amcewen/HttpClient)
#include <Wire.h> // Versão: 2.1 - Bilbioteca que vem junta com o core do esp32
#include <LiquidCrystal_I2C.h> // Versão: 1.1.2 by DFROBOT (https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library)
#include <AsyncTCP.h> // Versão: 1.1.4 by dvarrel
#include <ESPAsyncWebServer.h> // Versão: 1.2.6 by dvarrel (https://github.com/dvarrel/ESPAsyncWebSrv)

// Instanciar LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Conexão com o esp do armazem
const char* ssid = "ESP32-Access-Point"; // Declarar nome da rede para o esp se conectar
const char* password = "111222333"; // Senha da rede para o esp se conectar

const char* serverURL = "http://192.168.4.1/send-data"; // URL do servidor do ESP central para repassar os dados

// ############## DEFINIR NOME DO ATIVO ATRIBUIDO A ESSE DISPOSITIVO ##############
String ativoName = "RNP-01"; 

// ############## NOME DO DISPOSITIVO (esp32) ##############
String dispositivoName = "GEOLOC-001";

// Instanciar GPS
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

// Definir pinos dos LEDs
int ledVermelho = 33;
int ledAzul = 32;

String dateString;

bool SDcardUpdate = true;


void setup() {
  // Iniciar os pinos dos LEDs
  pinMode(ledAzul, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  // Iniciar o LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);  
  lcd.print("Iniciando");
  lcd.setCursor(0, 1);  
  lcd.print("dispositivo...");

  delay(3000);

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
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Insira um ");
    lcd.setCursor(0, 1);
    lcd.print("cartão SD!");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error ");
    lcd.setCursor(0, 1);
    lcd.print("inciando SD!");
    return;    // Se inicialização der algum erro
  } else{
    Serial.println("SUCCESS - SD card initialized.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SDcard iniciado!");
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
  lcd.print("Conectando no");
  lcd.setCursor(0, 1);
  lcd.print("AP...");

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
    lcd.print("WIFI nao");
    lcd.setCursor(0, 1);
    lcd.print("encontrado!");
    return;
  } else {
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: "); // Ao se conectar printar que foi conectado e o IP local
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectado no AP:");
    lcd.setCursor(0, 1);
    lcd.print(String(WiFi.localIP()));
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

  // Verificar se o GPS está funcionando
  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("Sem GPS connectado: check wiring."));
    while (true);
  }

    // Pegar a data do GPS
    year = gps.date.year();
    month = gps.date.month();
    day = gps.date.day();

    // Criar uma string com a data
    dateString = String(year) + "-" + String(month) + "-" + String(day);

    // Printar a data
    Serial.print(dateString);

    if (WiFi.status() == WL_CONNECTED) { // Enquanto estiver conectado a Wi-Fi do outro ESP
      Serial.println("Connection with ESP32 hotspot made!");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Conectado no");
      lcd.setCursor(0, 1);
      lcd.print("Hotspt ESP32");

      // Deixar led azul ligado
      digitalWrite(ledAzul, HIGH);

      // Verificar se o cartão SD foi atualizado
      if(SDcardUpdate == false) {
        // Atualizar o cartão SD 

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Atualizando");
        lcd.setCursor(0, 1);
        lcd.print("cartao SD...");
        
        updateSDCard();
        SDcardUpdate = true;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Cartão SD ");
        lcd.setCursor(0, 1);
        lcd.print("atualizado!");
      } else {
        // Caso já tenha sido atualizado, mandar dados para o servidor
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Cartão SD já");
        lcd.setCursor(0, 1);
        lcd.print("atualizado!");
        Serial.println("SD Card already updated!");
        sendSensorData(Latitude, Longitude, dateString, ativoName, dispositivoName); // Chamar a função sendSensorData
        delay(5000); // Aguardar 5 segundos antes de mandar dados novamente afim de prevenir erros
      }
    } else { // Enquanto não estiver conectado a Wi-Fi do outro ESP

      // Desligar led azul
      digitalWrite(ledAzul, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sem conexao");
      lcd.setCursor(0, 1);
      lcd.print("com Hotspot!");

      Serial.println("Connection with ESP32 hotspot not made!");
      Serial.println("WiFi Disconnected"); // Caso disconecte da Wi-Fi, printar o seguinte.

      // Verificar se o cartão SD foi atualizado
      logSDCard();
      SDcardUpdate = false;
    }

    // Verificar se os dados de latitude e longitude são iguais a 0 e piscar o led vermelho caso verdadeiro
    if(Latitude == 0 & Longitude == 0){
      digitalWrite(ledVermelho, HIGH);
    } else{
      digitalWrite(ledVermelho, HIGH);
      delay(500);
      digitalWrite(ledVermelho, LOW);
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lat: " + String(Latitude, 7));
    lcd.setCursor(0, 1);
    lcd.print("Longi:" + String(Longitude, 7));
    delay(5000);
}

// Função para mandar dados para o servidor
void sendSensorData(double latitudeHTTP, double longitudeHTTP, String dateHTTP, String ativoHTTP, String dispositivoHTTP) { // Função para mandar dados
  // Criar a URL para mandar os dados
  String url = String(serverURL) + "?lat=" + String(latitudeHTTP, 6) +
               "&long=" + String(longitudeHTTP, 6) + "&date=" + dateHTTP + "&ativo=" + ativoHTTP + "&dispositivo=" + dispositivoHTTP; // Adicionar os parametros para serem mandados na URL ( LAT e LONG )

// Enviar requisição HTTP
  HTTPClient http;
  http.begin(url);

  // Iniciar GET e checar o código de resposta
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode); // Printar código HTTP

    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("Dados enviados");
    // lcd.setCursor(0, 1);
    // lcd.print("para o hotspot!");
    delay(1000);
  } else {
    Serial.print("HTTP Request failed: ");
    Serial.println(http.errorToString(httpResponseCode).c_str()); // Caso falhe, printar o código de erro HTTP
  }

  http.end(); // Liberar o HTTP
}

// Escrever os dados no cartão SD
void logSDCard() {

  // Criar a JSON para ser salva no cartão SD
  dataMessage = "{";
  dataMessage += "\"Latitude\":" + String(Latitude, 6) + ",";
  dataMessage += "\"Longitude\":" + String(Longitude, 6) + ",";
  dataMessage += "\"Date\":\"" + dateString + "\",";
  dataMessage += "\"Ativo\":\"" + ativoName + "\",";
  dataMessage += "\"Dispositivo\":\"" + dispositivoName + "\"";
  dataMessage += "},\n";
  
  Serial.print("Save data: ");
  Serial.println(dataMessage);

  // Verificar se os dados de latitude e longitude são iguais a 0 e não salvar no cartão caso verdadeiro
  if(Latitude == 0 & Longitude == 0){
    Serial.println("Dados de GPS igual a 0!");
  } else {
    appendFile(SD, "/dataGPS.json", dataMessage.c_str());
  }
}

// Função para escrever os dados no cartão
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  // Abrir o arquivo para escrever
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

  // Abrir o arquivo para dar append
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    lcd.clear(); 
    Serial.println("Append failed");
    lcd.setCursor(0, 0);
    lcd.print("Dados salvos");
    lcd.setCursor(0, 1);
    lcd.print("no SD!");
  }
  file.close();
}

// Funcão ler arquivo cartão SD
void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  // Abrir o arquivo para ler
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

// Função para atualizar o cartão SD, lendo cada linha e mandando para o servidor
void updateSDCard() {
  String line;

  int totalLines = 0;
  int linesProcessed = 0;

  // Contar o número de linhas no arquivo
  File file = SD.open("/dataGPS.json");

  if(file){
    while(file.available()){
      line = file.readStringUntil('\n');
      totalLines++;
    }
    file.close();
  } else {
    Serial.println("Failed to open file");
    return;
  }

  file = SD.open("/dataGPS.json");
  // Mandar todas as linhas para o servidor
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

      // Printar o número de linhas restantes para atualizar o cartão  
      Serial.println("Linhas restantes: " + String(totalLines - linesProcessed));

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Linhas restantes");
      lcd.setCursor(0, 1);
      lcd.print(String(totalLines - linesProcessed));

      sendSensorData(latitudeData, longitudeData, dateData, ativoData, dispositivoData);

      linesProcessed++;

      delay(3000);    
      
    }
    // Limpar o arquivo do cartao SD
    Serial.println("Update made!");
    writeFile(SD, "/dataGPS.json", "");
    file.close();
  } else {
    Serial.println("Failed to open file");
  }
}

// Função para printar os dados de latitude e longitude
void displayInfo()
{
  Serial.print(F("Location: "));

  // Verificar se os dados de latitude e longitude são válidos
  if (gps.location.isValid()){

    Serial.print("Lat: ");

    // Printar os dados de latitude e longitude
    Latitude = gps.location.lat();

    Serial.print(gps.location.lat(), 6);

    Serial.print(F(","));

    Serial.print("Lng: ");

    Serial.print(gps.location.lng(), 6);

    Longitude = gps.location.lng();

    Serial.println();

  }  
  else
  {
    Serial.print(F("INVALID"));
  }

}