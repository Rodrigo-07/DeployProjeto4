#include <iostream> // Fluxo de entrada e saída
#include <cmath>    // Funções matemáticas
#include "NMEA.h"   // Suponha um arquivo de cabeçalho para dados GPS (protocolo NMEA) (Esta biblioteca foi criada pelo criador do GPS falso)

#define LEN(arr) ((int)(sizeof(arr) / sizeof(arr)[0])) // Macro para calcular o comprimento do array

// Defina uma união para interpretar 4 bytes como um float ou vice-versa
union {
  char bytes[4];
  float valor;
} velocidadeGPS;

float latitude; // Variável para armazenar latitude
float longitude; // Variável para armazenar longitude
const double EarthRadius = 6371.0; // Constante para o raio da Terra em quilômetros

// Estrutura para armazenar latitude e longitude como uma coordenada
struct Coordinate {
    double latitude;
    double longitude;
};

NMEA gps(GPRMC); // Inicializa uma conexão de dados GPS com o tipo de sentença GPRMC

// Função para calcular a distância entre duas coordenadas usando a fórmula de Haversine
double calcularDistancia(const Coordinate& ponto1, const Coordinate& ponto2) {
    // Calcule as diferenças em latitude e longitude em radianos
    double dLat = (ponto2.latitude - ponto1.latitude) * M_PI / 180.0;
    double dLon = (ponto2.longitude - ponto1.longitude) * M_PI / 180.0;

    // Fórmula de Haversine
    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(ponto1.latitude * M_PI / 180.0) * cos(ponto2.latitude * M_PI / 180.0) *
               sin(dLon / 2.0) * sin(dLon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    double distancia = EarthRadius * c; // Calcule a distância
    return distancia;
}

// Função para verificar se uma determinada coordenada está dentro de um raio especificado de uma coordenada central
bool estaDentroDoRaio(const Coordinate& centro, const Coordinate& ponto, double raio) {
    double distancia = calcularDistancia(centro, ponto); // Calcule a distância
    return distancia <= raio; // Verifique se a distância está dentro do raio
}

void setup() {
  Serial.begin(115200); // Inicia a comunicação serial
  Serial2.begin(9600); // Inicializa o Serial2 conectado a um chip personalizado

  pinMode(15, OUTPUT); // Define o pino 15 como pino de saída
  pinMode(21, INPUT_PULLUP); // Define o pino 21 como entrada com resistor pull-up
  Serial.println("Dados recebidos do GPS Falso:"); // Imprime uma mensagem no monitor serial
}

bool foraDoRaio = false; // Sinalizador para verificar se a localização está fora do raio

void codigoGPS() {
  while (Serial2.available()) { // Aguarde dados na porta serial
    char dadosSerial = Serial2.read(); // Leia dados da porta serial do GPS
    Serial.print(dadosSerial); // Imprima os dados recebidos

    if (gps.decode(dadosSerial)) { // Verifique se a sentença GPS é válida
      if (gps.gprmc_status() == 'A') { // Verifique se o status do GPS é 'A'
        velocidadeGPS.valor = gps.gprmc_speed(KMPH); // Obtenha a velocidade do GPS em km/h
      } else {
        velocidadeGPS.valor = 0; // Defina a velocidade como 0 se o status do GPS não for 'A'
      }

      latitude  = gps.gprmc_latitude(); // Obtenha a latitude
      longitude = gps.gprmc_longitude(); // Obtenha a longitude

      // Imprima latitude e longitude no monitor serial
      Serial.println();
      Serial.println(" Latitude: ");
      Serial.println(latitude, 8);
      Serial.print("Longitude: ");
      Serial.println(longitude, 8);

      // Imprima a velocidade em km/h no monitor serial
      Serial.print("    Velocidade: ");
      Serial.print(velocidadeGPS.valor);
      Serial.println(" Km/h");

      // Converta Coordenadas Geográficas para Plano Cartesiano
      converterCoordenadasParaCartesiano(latitude, longitude);

      // Especifique a coordenada central
      Coordinate centro;
      centro.latitude = 52.93546463; // Exemplo: coordenadas da Torre Eiffel em Paris
      centro.longitude = 8.85103325;
      
      // Coordenada de entrada para verificar
      Coordinate ponto;
      ponto.latitude = latitude;
      ponto.longitude = longitude;
      
      double raio = 5.0; // Defina um raio de 5 km
      bool estaDentro = estaDentroDoRaio(centro, ponto, raio); // Verifique se o ponto está dentro do raio
      
      // Imprima se o ponto está dentro ou fora do raio
      if (estaDentro) {
        std::cout << "O ponto está dentro do raio." << std::endl;
        foraDoRaio = false;
      } else {
        std::cout << "O ponto está fora do raio." << std::endl;
        foraDoRaio = true;
      }
      return;
    }
  }
}

void loop() {
  codigoGPS();

  if (foraDoRaio == true) {  
    digitalWrite(15, HIGH); // Ative uma saída digital no pino 15
    delay(500); // Aguarde 500 milissegundos
    digitalWrite(15, LOW); // Desative a saída digital
    delay(500); // Aguarde 500 milissegundos
  } else {
    digitalWrite(15, LOW); // Mantenha a saída digital baixa
  }
}

void converterCoordenadasParaCartesiano(float latitude, float longitude) {
  float latRadiano = latitude  * (PI) / 180; // Converta latitude de graus para radianos
  float lonRadiano = longitude * (PI) / 180; // Converta longitude de graus para radianos

  int raioTerra = 6371; // Raio da Terra em km

  float posX = raioTerra * cos(latRadiano) * cos(lonRadiano); // Calcule a coordenada X
  float posY = raioTerra * cos(latRadiano) * sin(lonRadiano); // Calcule a coordenada Y

  Serial.print("        X: ");
  Serial.println(posX); // Imprima a coordenada X pro monitor serial
  Serial.print("        Y: ");
  Serial.println(posY); // Imprima a coordenada X pro monitor serial
}
