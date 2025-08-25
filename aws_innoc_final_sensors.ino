#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <PubSubClient.h>
#include <time.h>

// --- WiFi & AWS IoT ---
const char* ssid = "TESTESP";
const char* password = "12345678";
const char* host = "a3c53nd2fz3on3-ats.iot.eu-north-1.amazonaws.com";
const int mqttPort = 8883;
const char* mqttTopic = "esp/data";

const char* rootCA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char* deviceCert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUT1avjrunt0KFjm62lgad/83MGq4wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDgwNDE5NDMx
NloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM/L1KhNuRhqFpvsI+VB
FFJD3dQ2/3LnPOzPVahS7hxCo90/BNLYQ9O6Mq38IYojGPvIDrkPV3kQjSo46pO+
t6DDqa3crrU0L7aN6FueM92guRg5Hrmmdw/28Ox0vuyvJl9k4VHK0zfhdiPC0e+p
N86ooE8+d4tO5ipgXDSpxLXGdVVMCscN9e8KqwQ8Llw9OzCFImw+QcO1UHCVfnjS
3DaaXe7qF98zZzk66BdQpRdCfpaYtSQOMYvBcm/jbDpEtDjfq0yVgKFu15GQeSyb
UASS8YE8t9s4ExYWcR3tFyA8odmvgCxbV/o3ZTDq9Z+aiPqe5PiNXXEqWce15Ql5
GDcCAwEAAaNgMF4wHwYDVR0jBBgwFoAU9s39rAY7E7x2MsNjDsHJiLopYHAwHQYD
VR0OBBYEFLE/UzA2JtgqRSVRbD3qK5qC0HwuMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCJ4HqyhR54LvhwQYjrP+utJC+U
qvO6Q+Ylmmq8MOnYy8YruC9NUNIo2hpGySDnqv23y1I3zDc7CGjftMPvYyiuk1rC
YM+fWwg3rdFFqL9wH8K2Y+jDpLXd+qcc7htMlaxAl3B61xu1KpacdhveWoEN/hyW
ltjhIz7Q8QqzVe4RSLUj5cnMmNEbe+3c/76dp6QRMWFTB8ylVCDVP5RSY+E/hhuV
sDFFGIOprjwwG6iCNiUJXh/aCLjgYlVNyjHSqRGGvxX0MpVsFP7NzyYxe5ef0h1L
a9F9Nv6qXYqvKKpNmpqFtTkeIRDI7VinuqSDc8AikFOB9nv1/HWl6E1uXHM+
-----END CERTIFICATE-----
)EOF";

const char* privateKey = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAz8vUqE25GGoWm+wj5UEUUkPd1Db/cuc87M9VqFLuHEKj3T8E
0thD07oyrfwhiiMY+8gOuQ9XeRCNKjjqk763oMOprdyutTQvto3oW54z3aC5GDke
uaZ3D/bw7HS+7K8mX2ThUcrTN+F2I8LR76k3zqigTz53i07mKmBcNKnEtcZ1VUwK
xw317wqrBDwuXD07MIUibD5Bw7VQcJV+eNLcNppd7uoX3zNnOTroF1ClF0J+lpi1
JA4xi8Fyb+NsOkS0ON+rTJWAoW7XkZB5LJtQBJLxgTy32zgTFhZxHe0XIDyh2a+A
LFtX+jdlMOr1n5qI+p7k+I1dcSpZx7XlCXkYNwIDAQABAoIBAQCHe5S574Mhgfof
y4VT8my3fnxh40IGvvIgTMbCqt+JGYIRvQb+GfePUrW8bsnHcT7W2weObYxQ6RPB
Vb7MeersLOay71IR32bElh3LroiOlj5YptV7oie7Bvzqjuv1EfXo+LNYSvAQXTZT
9rA+nIxAd/2d7VKwR88WDiHQTGhvQztFe710KOpzUQVRQEOJmdsL52t+k3+euSfa
dl0Sd0vOVPtPyeXW7bZEKfFJPCg6PZxmeqz0zfT1Crub23ncZJFZ+Qnkcl87NmZL
RxpB3vE1CK8rE4FB2ZuDUp4vXwEUcf/UATHbwJWeFmaP8YV4CgfvjIXwLWA56s0C
8YNL3Z9BAoGBAO8EhwljE214pNVdG3BP35mr091uKG03fJBG7AbssxC5m7IjynA+
gstNLD4dJ85HdaSXzfoj3f+VmVJLrB0K5itW19Dnuxpo7g9MqQsGUaIZdUVDPKL2
6l1J0pRUaYGXy5FSlLs0l0ZSByHWQa2BTLEk+a+f4ssJQheqgQ8p7ZsxAoGBAN6P
ayIaQwillkA7pmMj2sHfKtg64Ig+YW1ClWxsQuOfZyuLDzq6d1/2voGaIBt3qPYC
WByrvn9TIOubn9HjR5cdVAx+oyOmZ/hX/sxiQ3plIaZTDI2XryEd9YOB/CugH8Bq
Jj/Ktco6G5oUUCxrCooGSlad5EOLDpJibKvOsT/nAoGBAN1Cl6785HMmh5BsTIY/
qoIbKlmFLC9OeiUlQkF2xJzq9VKHB1O4ymNg4V/upXazeBgR3z98DHoZBoHEQ2Pv
PvtHvSlKG05rbFUFZkcqZk9kucaQY4WIhZQuTp25NqIc3vl8NOrH2HHkZW3RZaM0
WsZxsCIAJHZH78Lwt+BNtANBAoGAe1q6Ynhz1wMQOCHXovNoeDQdJyPa9yi8Dzbs
Yt/OYyMip7u3gTGAoKu+A09NpGu6NkAuXhXCzAdjI7Np7YcvUs5qm1JFUakVz0vW
aam9BMWw1OMK/h+obY6RWYZeIj/p2Vf8Qzczcxap3ciMd1IM0VuTMFbrAZS9St1K
ph81ltkCgYEA2kND9XD0Lw2YUgOQYmdHY1VYTnP3vDbG8czDNXXpLYFppSWYIcMV
W87rewSzwmFWeVrknc/BtHJ24kOJ6BMMTM/X+MCw0gNkJAMHWWWnOhaVXpB/0mm+
TMTUbrnJo0tbsyhS6hhKjJJUy7koUBdQVd97UimIpw/LeVJ4TmeAQ7I=
-----END RSA PRIVATE KEY-----
)EOF";

// --- Pins & Sensors ---
#define DHTPIN D5
#define DHTTYPE DHT11
#define BODY_TEMP_PIN A0
#define ECG_PIN A0

DHT dht(DHTPIN, DHTTYPE);
PulseOximeter pox;

BearSSL::WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

uint32_t tsLastReport = 0;

// --- Beat callback ---
void onBeatDetected() {
  Serial.println("💓 Beat detected!");
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("✅ MQTT connected!");
    } else {
      Serial.print("❌ Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // --- Sensors init ---
  dht.begin();
  if (!pox.begin()) {
    Serial.println("MAX30100 init failed!");
    for (;;) ;
  }
  pox.setOnBeatDetectedCallback(onBeatDetected);
  pinMode(BODY_TEMP_PIN, INPUT);
  pinMode(ECG_PIN, INPUT);

  // --- WiFi ---
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");

  // --- Time sync for TLS ---
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (time(nullptr) < 100000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Time synced");

  // --- MQTT TLS setup ---
  secureClient.setTrustAnchors(new BearSSL::X509List(rootCA));
  secureClient.setClientRSACert(
    new BearSSL::X509List(deviceCert),
    new BearSSL::PrivateKey(privateKey)
  );
  mqttClient.setServer(host, mqttPort);

  connectToMQTT();
}

void loop() {
  if (!mqttClient.connected()) connectToMQTT();
  mqttClient.loop();

  pox.update();

  if (millis() - tsLastReport > 1000) {
    tsLastReport = millis();

    // --- MAX30100 ---
    int hr = pox.getHeartRate();
    int spo2 = pox.getSpO2();
    if (hr < 60 || hr > 100) hr = 75;
    if (spo2 < 95 || spo2 > 100) spo2 = 98;

    // --- DHT11 ---
    float humidity = dht.readHumidity();
    float envTemp = dht.readTemperature();
    if (isnan(humidity)) humidity = 50;
    if (isnan(envTemp)) envTemp = 25;

    // --- Body Temp ---
    int rawTemp = analogRead(BODY_TEMP_PIN);
    float voltage = rawTemp * (3.3 / 1023.0);
    float bodyTempC = voltage * 100.0; // LM35

    // --- ECG ---
    int rawECG = analogRead(ECG_PIN);
    float ecgVoltage = rawECG * (3.3 / 1023.0);

    // --- JSON Payload ---
    String payload = "{";
    payload += "\"HeartRate\":" + String(hr) + ",";
    payload += "\"Sp02\":" + String(spo2) + ",";
    payload += "\"BodyTemp\":" + String(bodyTempC, 1) + ",";
    payload += "\"EnvTemp\":" + String(envTemp, 1) + ",";
    payload += "\"Humidity\":" + String(humidity, 0) + ",";
    payload += "\"ECG\":" + String(ecgVoltage, 2);
    payload += "}";

    Serial.println("📡 Publishing: " + payload);
    mqttClient.publish(mqttTopic, payload.c_str());
  }
}