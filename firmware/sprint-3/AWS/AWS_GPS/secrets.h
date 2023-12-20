#include <pgmspace.h>

#define SECRET
#define THINGNAME "ESP_32"                                      

const char WIFI_SSID[] = "ssid_wifi";                                        
const char WIFI_PASSWORD[] = "password";                                   
const char AWS_IOT_ENDPOINT[] = "AWS_IOT_ENDPOINT";        

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
// chave retirada
-----END CERTIFICATE-----
)EOF";

// Certificado do dispositivo                                       
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
// chave retirada
-----END CERTIFICATE-----
)KEY";

// Key privada do dispositivo                                          
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
// chave retirada
-----END RSA PRIVATE KEY-----
)KEY";
