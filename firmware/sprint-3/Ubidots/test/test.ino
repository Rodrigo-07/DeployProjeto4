#include <WiFi.h>
#include <Ubidots.h>

#define WIFI_SSID "Iphone12"
#define WIFI_PASSWORD "rodrigo123"
#define TOKEN "BBUS-qrZs2fn8JoMWkCNcoKdA0h23EH4szb"
#define VARIABLE_ID "65601816896d06000dd25cdf"

Ubidots ubidots(TOKEN);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Wait for the connection to Ubidots
  ubidots.setDebug(true);
  while (!ubidots.wifiConnection()) {
    Serial.println("Could not connect to Ubidots");
    delay(1000);
  }
  Serial.println("Connected to Ubidots");
}

void loop() {
  // Send a random value to Ubidots
  float value = random(0, 100);
  ubidots.add(VARIABLE_ID, value);
  ubidots.sendAll(true);
  
  Serial.println("Value sent to Ubidots: " + String(value));

  delay(5000); // Wait for 5 seconds before sending the next value
}
