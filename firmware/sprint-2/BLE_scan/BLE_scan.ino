// --- Bluetooth ---
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //Define variavel para o tempo de escaneamento
int nivelRSSI = -1000; //Define variável para o raio de detecção de dispositivo
String dispositivosAutorizados = "b4:8a:0a:ad:a6:e2"; //Mac address do dispositivo autorizado

// --- Inicializa o dispositivo ---
void setup() {
  Serial.begin(115200); //Inicializa o serial monitor do Esp32
  Serial.println("Procurando dispositivo...");
  BLEDevice::init("");// Inicializa conexão BLE do dispositivo
};

// -- Funções Auxiliares ---

// Cria classe com um para avisar/perceber os dispositivos BLE
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      String dispositivosEncontrados = advertisedDevice.getAddress().toString().c_str();
      
      // Busca dos dispositivos autorizados
      if ( dispositivosEncontrados == dispositivosAutorizados  && advertisedDevice.getRSSI() > nivelRSSI) {
        Serial.println("Identificador DETECTADO!");
        Serial.print("RSSI: ");
        Serial.println(advertisedDevice.getRSSI());
      }
      
      //Printa no serial monitor todos os dispositivos disponiveis para conexão
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.getAddress().toString().c_str());
    }
};

// --- Scan Bluetooth BLE ---
void scanBLE(){
  BLEScan* pBLEScan = BLEDevice::getScan(); //Cria um escaneamento novo
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //a verificação ativa consome mais energia, mas obtém resultados mais rapidamente
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
}

// ----- Inicializa o loop do dispositivo -------
void loop() {
  scanBLE();
  delay(2000);
}
