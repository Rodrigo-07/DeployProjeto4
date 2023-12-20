#include <PubSubClient.h>
#include "WiFiClientSecure.h"
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>

#include <Wire.h>
#include "ESPAsyncWebServer.h"
#include <LiquidCrystal_I2C.h>
 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --------------- CERTIFICADOS NECESSARIOS PARA  A CONEXÃO MAIS SEGURA ----------------

const char* CA_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" \
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" \
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n" \
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n" \
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n" \
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n" \
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n" \
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n" \
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n" \
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n" \
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n" \
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n" \
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n" \
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n" \
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n" \
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n" \
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n" \
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n" \
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n" \
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n" \
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n" \
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n" \
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n" \
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n" \
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n" \
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n" \
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n" \
"-----END CERTIFICATE-----";

// -------------------- CERTIFICADOS QUE NÃO SÃO UTILIZADOS -----------------------

// const char* ESP_CA_cert = \
// "-----BEGIN CERTIFICATE-----\n" \
// "MIIDXzCCAkcCFE1FLd7aWgobPquR0eJGvedoO9G1MA0GCSqGSIb3DQEBCwUAMEUx\n" \
// "CzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRl\n" \
// "cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMjMxMTIwMTM0NzE5WhcNMjMxMjIwMTM0\n" \
// "NzE5WjCBkjELMAkGA1UEBhMCQlIxEjAQBgNVBAgMCVNhbyBQYXVsbzESMBAGA1UE\n" \
// "BwwJU2FvLVBhdWxvMQ8wDQYDVQQKDAZJbnRlbGkxCzAJBgNVBAsMAlRJMRAwDgYD\n" \
// "VQQDDAdSb2RyaWdvMSswKQYJKoZIhvcNAQkBFhxmcmVpcmVyb2RyaWdvc2FsZXNA\n" \
// "Z21haWwuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtOO+GDfy\n" \
// "nIkB+uItRhqU9eyIhdnEnzatoEco7/QELEsKYj034EDosRjlg00Pl6CTI7oW8zEv\n" \
// "L8BJqM/A0HS28tgq+sOWEZdQS4PW3r32czZYxfD8ydee6lMbJhoC1EJ4sRm+OtgA\n" \
// "C8xY+0+Ld0jTkKu5eejWe9h0YzD/z//dqc9hkJctQEF4vCb4edl3Q79dEeJP6+zD\n" \
// "qXHDIQIUzunGvIcoYNTfAvF13Hy1Lydu5nito49WJRB2rpp16IQOlal1W9VmN5Ua\n" \
// "2HjyAR7Kvddd9KskfXf2TT6HweHaGDD0YhCHFu6YGfFfz/EpcpKITR9UnB1zEaYf\n" \
// "TipAXJ7bbsbWewIDAQABMA0GCSqGSIb3DQEBCwUAA4IBAQCMkVp+g/bYrbhuIUXc\n" \
// "RaXLiWtRkPTeR6Osb9Ml4ElzRpm3NX6O1195GRUCuDUhRMcxYNW/omnjWNLxoBec\n" \
// "OEr4PnS/Hni9QSXYljh3NMMu+fBeQvr1OxlvIMqog553floSMzgFg1snrK7khmPw\n" \
// "QY0m/AodjoCVYiSsrX7DVrsSEUoh/giz3Omn8d2Kw4Oj+4gL8th77v0W/ffK/TyB\n" \
// "V/3p8Hdeioeu8XBCJ5FbSglNbEGnhnNjK460WXdzGxpIxb8/N81ALnRP9Xaxuabh\n" \
// "XXWvzPmTRCe00psYcfwUJV3znBlkhx4gl3Na5JbdMxcejSAgsQ3hxG7X2W9QXdeS\n" \
// "NaOe\n" \
// "-----END CERTIFICATE-----";

// const char* ESP_RSA_key= \
// "-----BEGIN PRIVATE KEY-----\n" \
// "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQC0474YN/KciQH6\n" \
// "4i1GGpT17IiF2cSfNq2gRyjv9AQsSwpiPTfgQOixGOWDTQ+XoJMjuhbzMS8vwEmo\n" \
// "z8DQdLby2Cr6w5YRl1BLg9bevfZzNljF8PzJ157qUxsmGgLUQnixGb462AALzFj7\n" \
// "T4t3SNOQq7l56NZ72HRjMP/P/92pz2GQly1AQXi8Jvh52XdDv10R4k/r7MOpccMh\n" \
// "AhTO6ca8hyhg1N8C8XXcfLUvJ27meK2jj1YlEHaumnXohA6VqXVb1WY3lRrYePIB\n" \
// "Hsq91130qyR9d/ZNPofB4doYMPRiEIcW7pgZ8V/P8SlykohNH1ScHXMRph9OKkBc\n" \
// "nttuxtZ7AgMBAAECggEAMvGEwttW12D6jsaOC5VG6G+iD1hCOLi0c1Q3IkTtu++y\n" \
// "T/J/vwdowR+K28yjHIfbiI9LEViDuoYD+Kfx3INSVtgsWo0kwXWE5o+NvxNuygm/\n" \
// "PJffCZVRKxsWpUdkDpVd2jquyXG1qZz2XmiLlZ+L/c1vIfdCGITgWmMdWF3t9nl4\n" \
// "+XNQMxUMTj2UtV5UOw0ZaAF7APqEQvWmIwM4G2u7YMvUmS6hSseDlWshKMIpNE0D\n" \
// "NKgVr4OZ49PSugYQHQ9jMdrZt/yqsdT+AZ4iIpdtDU8Y3htGC3LDQTXkxCEJbmfN\n" \
// "34U8OCFKJ7RkbfaJ5tv/tl6kot/DSjiNMV7f439X6QKBgQDcTU3N+JXlp9LgWwZm\n" \
// "dy9VpdxbaZu5KV9Lpj3bbxTHWKC6d3bceMRoH1FUX42Ye6ER7Em+URQgWOn3Wo+e\n" \
// "pa8/prb6WUcRER1PVr2I/MKFr/A6bVUllDToZDv78bg1UwPHSnbMYdQW+vzMD63p\n" \
// "ZDZ84z9ESe7Sds0GWby182E5vQKBgQDSM4Mo513pHIqI9EVDdhByGGNlzq2QoZVf\n" \
// "BHaz3bD61WHW/bmvPogCMzGjsao0bB2BpQ5w32IG7C8JiOi3E0idIh+F5BHG8h+t\n" \
// "ZHyVlrpx0FuuDunZTGHQBbzA9BWb7wG0a7VzrkYAym715ZBAPbLdw65/UQY0apHJ\n" \
// "2FQ88e1olwKBgQCjmbDm/e1rgCcFP/PP0kDPrP2o0RO4/PVs+4bCV4qpw7KQ5zjY\n" \
// "38KTxHYkX1WK4HysD7pv4Ew+/Wi6rYEsgsRTU55c6kHxdbE7gyH8AiD/ILoY2aq6\n" \
// "3oYv8oftQQPYbc7HiBATFa9oZXmonRg805P3SNdqAjGeEGWPisKuqJASTQKBgFNV\n" \
// "dMQ8pbCLTijpnqQWJIL3REDSZWEXjl5zKJQ6abWn7vgK8xiU09r7bMyW3QGXc0M4\n" \
// "qrb4+KYB8Mv46TnbfUg+hPzR5HJY/1IAqpurfBMvJYmmA/PQIl8URzbADekaIjTj\n" \
// "Frbb+ojJuJ/n+P5e95CPyZhUCP+y2EWLbCoVR6fVAoGBAJRjqXgYleEECdq4HrO3\n" \
// "wxwf1xBMRz4iwFu6Ww+F+1ThUf80DVYnAWZaf5gf6g7IC39XNQ2QGZOJZguxROB4\n" \
// "gkG2XoegzgEaf9xJcHNLVXA786A/0VDxYtCrWpScVEZZtrfNcPvhfxDumfmnn8sG\n" \
// "lV25ofJMqLYFPFl6MeF2tjLB\n" \
// "-----END PRIVATE KEY-----";

// Inicialização do módulo GPS
TinyGPSPlus gps;

const char* ssid        = "Iphone12";        // Nome do WIFI
const char* password    = "rodrigo123";   // Senha do WIFI

const char* mqtt_server = "3d23a9722fe5468cbb05f9e25035c21e.s2.eu.hivemq.cloud";  // Endereço do host do HiveMQ
int port                = 8883;             // Porta do broker

// Credenciais criadas no HiveMQ
const char* mqtt_user   = "ESP32_Connect4";            
const char* mqtt_pass   = "123drigo"; 

// Topicos de publicação
String topicLat = "GPS/Latitude";
String topicLong = "GPS/Longitude"; 

// Variáveis de latitude e longitude
double lat;
double longi;

char latChar[50];
char longiChar[50];

String latStr;
String longiStr;

String dateString;

// Iniciação do client de WIFI e conexão MQTT
WiFiClientSecure client;
PubSubClient mqtt_client(client); 

AsyncWebServer server(80); // Função para criar um servidor web assincrono na porta 80

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 1);
  lcd.print("Iniciando...");

  // Inciação das portas seriais
  Serial.begin(9600);
  Serial2.begin(9600);

  WiFi.softAP(ssid, password); // Iniciando um Access Point no ESP

  IPAddress IP = WiFi.softAPIP(); // Declarar uma variável para ser o IP do AP
  Serial.print("AP IP address: "); 
  Serial.println(IP); // Printar o IP do AP

  server.on("/send-data", HTTP_GET, onDataReceived); // Declarar o servidor para receber chamadas GET

  server.begin(); // Iniciar o servidor
                      
  // Configuração dos LEDs
  pinMode(19, OUTPUT);  // LED Vermelho
  pinMode(5, OUTPUT);   // LED Verde
  pinMode(18, OUTPUT);  // LED Azul

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  // Tentativa de conexão com WIFI
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(2, 1);
    lcd.print("Conectando...");
    Serial.print(".");
    // Esperar 1 segundo para tentar a conexão novamente
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  // Set up das keys de autenticação
  client.setCACert(CA_cert);          //Root CA certificate

  // Só é necessario se for configurado no broker, no caso do HiveMQ não é ncessário
  // client.setCertificate(ESP_CA_cert); 
  // client.setPrivateKey(ESP_RSA_key); 
  
  // Configurar a conexão
  mqtt_client.setServer(mqtt_server, port);
  lcd.clear();
}

// Função de tratamento e envio das mensagens MQTT
void publishMessage()
{
  // ----------------- TENTATIVA DE ENVIAR UM JSON ---------------------
  StaticJsonDocument<200> doc;
  doc["Latitude"] = lat;
  doc["Longitude"] = longi;
  doc["Date"] = dateString;
  doc["Ativo"] = "RNP-01";


  char jsonBuffer[512];

  serializeJson(doc, jsonBuffer); // print to client

  Serial.println(jsonBuffer);

  // String latStr = String(lat);
  // String longiStr = String(longi);

  mqtt_client.publish("GPS", jsonBuffer);

  // -------------------- COVERSÃO PARA CHAR PARA A PUBLICAÇÃO MQTT --------------

  // latStr = String(lat); // Converter o valor de latitude para string
  // latStr.toCharArray(latChar, latStr.length() + 1); // Preparando a variavel para o envio

  // longiStr = String(longi); // Converter o valor de Longitude para string
  // longiStr.toCharArray(longiChar, longiStr.length() + 1); // Preparando a variavel para o envio

  // Serial.print("Publish message: ");
  
  // Envio das mensagens em formato de char[50]
  // publish(Topico, mensagem);

  // mqtt_client.publish("GPS/Latitude", latChar);
  // mqtt_client.publish("GPS/Longitude", longiChar);

  // Uma vez conectado podemos se subscrever no mesmo topico
  // mqtt_client.subscribe("GPS/Latitude");
  // mqtt_client.subscribe("GPS/Longitude");
  lcd.setCursor(2, 1);
  lcd.print("Enviando...");
  delay(1000);
  lcd.clear();
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

      // Envio da mensagem
      publishMessage();
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

void loop() {    
  // Leitura e processamento de dados do módulo GPS
  // 10 tentativas de leituras de sinais GPS
  for (int i = 0; i < 10; i++) {
    while (Serial2.available() > 0)
      if (gps.encode(Serial2.read()))
        displayInfo();

    // Verifica se a ligações do GPS estão corretas
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println(F("No GPS detected: check wiring."));
      while (true);
    }
  }

    int year = gps.date.year();
    int month = gps.date.month();
    int day = gps.date.day();

    // Print or use the date information as needed
    // Create a string to store the concatenated date
    dateString = String(year) + "-" + String(month) + "-" + String(day);

    // Print or use the date string as needed
    Serial.println("Date: " + dateString);

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

    lcd.print("GPS desconectado!");
  }

  // Loop da conexão MQTT
  MQTTconnection();

  // Fazer uma conexão nova a cada 5 segundos
  delay(5000);
  lcd.clear();
}


// Função para exibir informações de latitude e longitude
void displayInfo() {
  // Exibe informações de latitude e longitude do GPS
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {

    Serial.print("Lat: ");
    // Salvar a latitude
    lat = gps.location.lat();
    Serial.print(lat, 6);


    Serial.print(F(","));

    // Salvar a longitude
    Serial.print("Lng: ");
    longi = gps.location.lng();
    Serial.print(longi, 6);

    Serial.println();

  } else {
    Serial.print(F("INVALID"));
  }

  lcd.setCursor(1, 2);
  lcd.print(lat);
  lcd.setCursor(1, 1);
  lcd.print(longi);
}

void onDataReceived(AsyncWebServerRequest* request) { // Função para quando o servidor receber dados do cliente
  // Handling GET request
  String lat = request->arg("lat"); // Transformar o argumento lat em uma string
  String longi = request->arg("long"); // Transformar o argumento long em uma string

  Serial.println("Received data (GET):"); // Printar que recebeu os dados
  Serial.println("Latitude: " + lat); // Printar os dados de latitude recebidos
  Serial.println("Longitude: " + longi); // Printar os dados de longitude recebidos

  request->send(200, "text/plain", "Data received successfully (GET)"); // Responde que os dados foram recebidos com sucesso
}
