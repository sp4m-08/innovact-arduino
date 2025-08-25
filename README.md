---
# AI Powered Health Asssistant for remtoe patient supervision

This project uses an ESP8266 microcontroller to gather data from various health and environmental sensors and securely publish it to **AWS IoT Core** over MQTT, which is the displayed on our live web dashboard with AI Powered Query response feature.
---

# Presentation (PPT)

- **View the Presentation:** [PPT](https://drive.google.com/file/d/14lUezMo0DRYfbWNOzxxj1X1tuztRL42T/view?usp=sharing)

---

# Web Dashboard

You can view and interact with the live application here:

- **Live Website:** [https://vitalyse-website-frontend-rss4.vercel.app/](https://vitalyse-website-frontend-rss4.vercel.app/)

---

## Project Repositories

- **Frontend:** [vitalyse-website-frontend](https://github.com/sp4m-08/vitalyse-website-frontend)
- **Backend API:** [vitalyse-website-backend](https://github.com/sp4m-08/vitalyse-website-backend)
- **Machine Learning Model:** [VitalEyes ML Model](https://github.com/AvanthikaHegde/VitalEyes)

## Features

- **Multi-Sensor Data Acquisition:** Collects data from multiple sensors simultaneously.
  - **Heart Rate & SpO2:** Measures pulse and blood oxygen saturation using a MAX30100 sensor.
  - **Body Temperature:** Measures body temperature with an analog temperature sensor (like an LM35).
  - **ECG:** Reads raw analog voltage from an ECG sensor module.
  - **Environmental Data:** Measures ambient temperature and humidity using a DHT11 sensor.
- **Secure Cloud Connectivity:** Connects securely to AWS IoT Core using TLS/SSL encryption and certificate-based authentication.
- **Real-time Data Publishing:** Publishes the collected sensor data as a JSON payload to an MQTT topic every second.
- **Robust Connection Handling:** Automatically handles WiFi and MQTT disconnections and attempts to reconnect.

---

## Hardware Requirements ⚙️

- **Microcontroller:** ESP8266 Development Board (e.g., NodeMCU, Wemos D1 Mini)
- **Sensors:**
  - **Pulse Oximeter:** MAX30100 Heart Rate & SpO2 Sensor Module
  - **Environmental:** DHT11 Temperature & Humidity Sensor
  - **Body Temperature:** Analog temperature sensor like the LM35
  - **ECG:** An analog ECG sensor module (e.g., AD8232)
- **Power Supply:** 3.3V power supply suitable for the ESP8266 and sensors.
- **Jumper Wires & Breadboard**

---

## Software & Libraries 📚

- **Arduino IDE** with the **ESP8266 Board Manager** installed.
- **Required Arduino Libraries:**
  - `MAX30100_PulseOximeter` by OXullo Intersecans
  - `DHT sensor library` by Adafruit
  - `PubSubClient` by Nick O'Leary
  - `Adafruit Unified Sensor` (dependency for the DHT library)

You can install these libraries through the Arduino IDE's Library Manager (`Sketch` \> `Include Library` \> `Manage Libraries...`).

---

## Wiring & Connections 🔌

Connect the sensors to your ESP8266 board as follows. Note that pin numbers are for a NodeMCU board; adjust them if you are using a different ESP8266 variant.

| Sensor               | Sensor Pin | ESP8266 Pin |
| -------------------- | :--------: | :---------: |
| **MAX30100 (I2C)**   |    VCC     |     3V3     |
|                      |    GND     |     GND     |
|                      |    SCL     | D1 (GPIO5)  |
|                      |    SDA     | D2 (GPIO4)  |
| **DHT11**            |    VCC     |     3V3     |
|                      |    GND     |     GND     |
|                      |    DATA    | D5 (GPIO14) |
| **Body Temp (LM35)** |    VCC     |     3V3     |
|                      |    GND     |     GND     |
|                      |    VOUT    |     A0      |
| **ECG Module**       |    VCC     |     3V3     |
|                      |    GND     |     GND     |
|                      |   OUTPUT   |     A0      |

**Note:** The Body Temperature and ECG sensors are both connected to the same analog pin (`A0`) in the provided code. To use both, you must either connect them to separate analog pins (if your board has them) or use an analog multiplexer. If you only intend to use one, simply wire that sensor to `A0`.

---

## Configuration 🔧

Before uploading the code, you must configure your WiFi and AWS IoT credentials.

### 1\. WiFi Credentials

Update the following lines with your network's SSID and password:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 2\. AWS IoT Core Setup

You need to set up a "Thing" in AWS IoT Core to get the required endpoint and certificates.

1.  **Log in** to your AWS Management Console and navigate to the **IoT Core** service.

2.  In the sidebar, go to **Manage** -\> **Things** and create a new Thing.

3.  **Create Certificates:** When creating the Thing, choose to create new certificates.

4.  **Download Credentials:** This is a crucial step. **Download all four files:**

    - Device certificate (`xxxx-certificate.pem.crt`)
    - Private key (`xxxx-private.pem.key`)
    - Public key (`xxxx-public.pem.key`)
    - Root CA (Amazon Root CA 1 is used in this code)

5.  **Activate** the certificate and **attach a policy** to it. The policy should grant permissions to connect and publish to your desired topic. A simple test policy would be:

    ```json
    {
      "Version": "2012-10-17",
      "Statement": [
        {
          "Effect": "Allow",
          "Action": ["iot:Connect", "iot:Publish"],
          "Resource": "*"
        }
      ]
    }
    ```

### 3\. Update the Code with AWS Credentials

1.  **Host Endpoint:** Find your custom endpoint URL in the AWS IoT Core settings (it looks like `xxxxxxxx-ats.iot.region.amazonaws.com`). Update the `host` variable:
    ```cpp
    const char* host = "YOUR_AWS_IOT_ENDPOINT";
    ```
2.  **MQTT Topic:** Set the topic you want to publish to:
    ```cpp
    const char* mqttTopic = "esp/data";
    ```
3.  **Certificates:** Open the downloaded certificate and key files with a text editor.
    - Copy the entire content of `xxxx-certificate.pem.crt` and paste it inside the `deviceCert` variable.
    - Copy the entire content of `xxxx-private.pem.key` and paste it inside the `privateKey` variable.
    - The `rootCA` for Amazon Root CA 1 is already included.

---

## Data Flow & Payload Format

The device follows this simple data flow:

**Sensors → ESP8266 → WiFi → AWS IoT Core (MQTT Topic)**

The data is sent as a JSON object in the following format. You can use this structure to create rules in AWS IoT to process the data (e.g., save it to a database, trigger an alarm).

**Example Payload:**

```json
{
  "HeartRate": 75,
  "Sp02": 98,
  "BodyTemp": 36.5,
  "EnvTemp": 25.0,
  "Humidity": 50,
  "ECG": 1.25
}
```
