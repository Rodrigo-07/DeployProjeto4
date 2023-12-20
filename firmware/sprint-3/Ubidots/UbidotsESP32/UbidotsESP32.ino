/******************************************
 *
 * Este exemplo funciona para usuários industriais e STEM.
 *
 * Desenvolvido por Jose Garcia, https://github.com/jotathebest/
 *
 ****************************************/
/****************************************
 * Incluir Bibliotecas
 ****************************************/
#include "UbidotsEsp32Mqtt.h"
#include <TinyGPSPlus.h>
TinyGPSPlus gps;
/****************************************
 * Definir Constantes
 ****************************************/
const char *UBIDOTS_TOKEN = "BBUS-AJSvBG83GyV1fFxLBBAAe5H2mTcft7";  // Insira aqui o seu TOKEN do Ubidots
const char *WIFI_SSID = "Iphone12";      // Insira aqui o SSID da sua rede Wi-Fi
const char *WIFI_PASS = "rodrigo123";      // Insira aqui a senha da sua rede Wi-Fi
const char *DEVICE_LABEL = "esp32-ativo";   // Insira aqui a etiqueta do seu dispositivo para a qual os dados serão publicados
const char *LATITUDE = "65601816896d06000dd25cdf"; // Variável que define a latitude localizada
const char *LONGITUDE = "Longitude"; // Variável que define a longitude localizada

double lat; // Criação de variável numérica de alta definição para a latitude
double longi; // Criação de variável numérica de alta definição para a longitude
const int PUBLISH_FREQUENCY = 5000; // Taxa de atualização em milissegundos
unsigned long timer; // Define a criação do timer
uint8_t analogPin = 34; // Pino usado para ler dados do ADC_CH6 do GPIO34.
Ubidots ubidots(UBIDOTS_TOKEN); // Cria a definição a ser utilizada do Ubidots_token
/****************************************
 * Funções Auxiliares
 ****************************************/
void callback(char *topic, byte *payload, unsigned int length) //cria uma função callback, que recebe e envia requisições mqtt
{
  Serial.print("Mensagem recebida ["); // Printa no monitor serial a mensagem recebida
  Serial.print(topic); // Printa o topic
  Serial.print("] "); // Printa um ']'
  for (int i = 0; i < length; i++) // O for percorre o tamanho da mensagem
  {
    Serial.print((char)payload[i]); //  Cada byte da mensagem é convertido em texto e expresso no 
  }
  Serial.println(); // Pula uma linha
}
/****************************************
 * Funções Principais
 ****************************************/
void setup() // Função que inicializa o ESP32
{
  // Coloque seu código de inicialização aqui, para ser executado uma vez:
  Serial.begin(9600); // Inicia o monitor serial
  Serial2.begin(9600); // Inicia o segundo monitor serial
  ubidots.setDebug(true);  // descomente isso para tornar as mensagens de depuração disponíveis
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS); // Conecta no UBIDOTS com a senha e o id do wifi
  ubidots.setCallback(callback); // // Seta o callback
  ubidots.setup(); // Inicializa o UBIDOTS
  ubidots.reconnect(); // Conecta o UBIDOTS
  delay(3000); // Atrasa por 3 segundos
}


void loop() // Função que funciona ciclicamente dentro da solução
{
while (Serial2.available() > 0) // Enquanto a leitura do serial2 for menor que 0...
    if (gps.encode(Serial2.read())) //... e se o GPS realizar alguma leitura
      displayInfo(); // mostra a informação 
  if (millis() > 5000 && gps.charsProcessed() < 10) // Se o tempo for maior que 5 segundos e os caracteres processados forem menos que 10
  {
    Serial.println(F("GPS não detectado: verifique a conexão.")); // Será printado no Monitor Serial uma frase
  }
  // Coloque seu código principal aqui, para ser executado repetidamente:
  if (!ubidots.connect()) // Confere se o ubidots está conectado
  {
    ubidots.reconnect(); // reconecta com o ubidots
  }

  delay(5000); // aguarda 5 segundos

  ubidots.add(LATITUDE, lat); // Insere as etiquetas das variáveis e o valor a ser enviado
  ubidots.add(LONGITUDE, longi); // Insere as etiquetas das variáveis e o valor a ser enviado
  ubidots.publish(DEVICE_LABEL); // Publica a variável indicada
  timer = millis(); // Define uma função parao timer

  Serial.print(lat, 6);  // Printa no Monitor serial a variável lat
  Serial.print(longi, 6); // Printa no Monitor serial a variável lat
  ubidots.loop(); // define isso rodando em um loop
}
void displayInfo() // Cria uma função para mostrar a informação
{
  if (gps.location.isValid()) // Checa se a posição de GPS é válida
  {
    lat = gps.location.lat(); // informa a localização latitudinal do gps e a guarda na variável lat
    longi = gps.location.lng(); // informa a localização longitudinal do gps e a guarda na variável longi
  }
  else
  {
    Serial.print(F("INVÁLIDO")); // se a informação enviada não for válida, será printado um Inválido no monitor serial
  }
}
void updateSerial() // cria uma função que dá update para a porta Serial Software
{
  delay(500); // Espera meio segundo
  while (Serial.available()) // Enquanto o serial estiver disponível
  { 
    Serial2.write(Serial.read());  // Encaminhar o que o Serial recebeu para a porta Serial Software
  }
  while (Serial2.available()) // enquanto o serial2 estiver disponível
  {
    Serial.write(Serial2.read());  // Encaminhar o que a porta Serial Software recebeu para a porta Serial
  }
}