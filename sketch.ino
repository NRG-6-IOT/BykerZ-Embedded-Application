#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Definiciones
#define LED_PIN 33 // GPIO33 - LED pin
#define DHT_PIN 25 // GPIO25 - DHT pin
#define DHT_TYPE DHT11 // DHT sensor type (11/22)
#define MQ135_PIN 34  // GPIO34 - Analog pin

// Configuración del dispositivo
const char* deviceId = "bykerz-iot-001";
const int vehicleId = 1;

// Configuración presión
Adafruit_BMP280 bmp;  // I2C

// Configuración Temperatura
DHT dht(DHT_PIN, DHT_TYPE);

// Configuración GPS
TinyGPSPlus gps;
HardwareSerial GPS(2);

// Configuración noqueo
const int knockPin = 27;

// Calibración Calidad de aire
const float R0 = 76.63;    // Resistance in clean air (calibrate this)
const float RL = 10.0;     // Load resistance in KΩ

typedef struct {
  float ppm;
  float slope;
  float intercept;
} GasCurve;

GasCurve CO_curve = {10.0, -0.421, 1.177};
GasCurve Alcohol_curve = {10.0, -0.422, 1.055};
GasCurve CO2_curve = {400.0, -0.382, 1.132};
GasCurve Toluen_curve = {10.0, -0.412, 1.431};
GasCurve NH4_curve = {10.0, -0.411, 1.786};
GasCurve Aceton_curve = {10.0, -0.396, 1.513};

// Configuración WiFi
const char* ssid = "";
const char* password = "";

// Configuración Edge Service
const char* edgeServiceUrl = "http://192.168.18.6:5000/api/v1/metrics";
const char* jwtToken = "eyJhbGciOiJIUzM4NCJ9.eyJzdWIiOiJhbHhSYVRpIiwiaWF0IjoxNzYzMTg3MzgyLCJleHAiOjE3NjM3OTIxODJ9._1a68a3YaJx3AGIH8Af7UDQBPfgFVqvj6BO0j1vrlSImoQlmFkKds8tLHK2nEoNN"; // Tu token JWT



void setup() {
  Serial.begin(115200);
  delay(3000);

  // Conectar a WiFi
  Serial.println("Connecting WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");

  // Configure DHT
  dht.begin();
  Serial.println("DHT Started!");

  // Start digital pin LED_BUILTIN as an output.
  pinMode(LED_PIN, OUTPUT);

  // Configure ADC
  analogReadResolution(12);  // Set to 12-bit resolution (0-4095)

  // Conectar Presion al address correcto
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find BMP280 at 0x76. Trying 0x77...");
    if (!bmp.begin(0x77)) {
      Serial.println("BMP280 not found. Check wiring!");
      while (1);
    }
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  Serial.println("BMP280 initialized!");

  // Conectar el GPS
  GPS.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("GPS initialized!");

  // Calentar el Air quality
  Serial.println("Air quality Warming Up, 60 Seconds of wait time...");
  delay(60000);  // Wait 60 seconds for sensor to stabilize

  Serial.println("Air quality initialized!");

  // Iniciar el noqueo
  pinMode(knockPin, INPUT);
  Serial.println("Knock initialized!");
}

void sendMetric(float latitude, float longitude, float CO2, float NH3, float benzene, float temp, float humidity, float pressure, bool impact) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(edgeServiceUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(jwtToken));

    // Crear JSON
    StaticJsonDocument<512> doc;
    doc["device_id"] = deviceId;
    doc["vehicle_id"] = vehicleId;
    doc["latitude"] = latitude;
    doc["longitude"] = longitude;
    doc["CO2Ppm"] = CO2;
    doc["NH3Ppm"] = NH3;
    doc["BenzenePpm"] = benzene;
    doc["temperatureCelsius"] = temp;
    doc["humidityPercentage"] = humidity;
    doc["pressureHpa"] = pressure;
    doc["impactDetected"] = impact;

    String jsonString;
    serializeJson(doc, jsonString);

    // Enviar POST
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Código: " + String(httpResponseCode));
      Serial.println("Respuesta: " + response);
    } else {
      Serial.println("Error: " + String(httpResponseCode));
    }

    http.end();
  }
}

String assessAirQuality(float ratio) {
  if (ratio > 1.5) return "Excellent";
  else if (ratio > 1.0) return "Good";
  else if (ratio > 0.8) return "Moderate";
  else if (ratio > 0.6) return "Poor";
  else return "Very Poor";
}

float calculatePPM(float ratio, GasCurve curve) {
  // Calculate PPM using the curve parameters
  // Formula: ppm = 10^((log10(ratio) - intercept) / slope)
  return pow(10, (log10(ratio) - curve.intercept) / curve.slope);
}

void calibrateSensor() {
  Serial.println("Starting calibration... Ensure sensor is in clean air!");
  delay(30000);  // Wait 30 seconds

  float sum = 0;
  for (int i = 0; i < 100; i++) {
    int sensorValue = analogRead(MQ135_PIN);
    float voltage = sensorValue * (3.3 / 4095.0);
    float RS = ((5.0 * RL) / voltage) - RL;
    sum += RS;
    delay(100);
  }

  float R0_calibrated = sum / 100.0;
  Serial.print("Calibrated R0 value: ");
  Serial.println(R0_calibrated, 2);
}

void loop() {
  digitalWrite(LED_PIN, LOW);
  int state = digitalRead(knockPin);
  if (state = HIGH) {
    digitalWrite(LED_PIN, HIGH);
    // Obtener valores
    float temp = dht.readTemperature();
    float hum = dht.readTemperature();

    float pres = bmp.readPressure() / 100.0;

    while (GPS.available()) {
      gps.encode(GPS.read());
    }
    float lat = gps.location.lat(), 6;
    float lon = gps.location.lng(), 6;

    int sensorValue = analogRead(MQ135_PIN);
    float voltage = sensorValue * (3.3 / 4095.0); // Convert to voltage
    float RS = ((5.0 * RL) / voltage) - RL; // Calculate sensor resistance
    float ratio = RS / R0;
    float co_ppm = calculatePPM(ratio, CO_curve), 2;
    float alcohol_ppm = calculatePPM(ratio, Alcohol_curve), 2;
    float co2_ppm = calculatePPM(ratio, CO_curve), 2;
    float nh4_ppm = calculatePPM(ratio, NH4_curve), 2;



    Serial.println("Knock detected! Registering data...");
    Serial.println("----- Temperature Sensor-----");
    Serial.print("Temperature: "); Serial.print(temp); Serial.println("°C");
    Serial.print("Humidity: "); Serial.print(hum); Serial.println("%");

    Serial.println("----- Pressure Sensor-----");
    Serial.print("Temperature: "); Serial.print(bmp.readTemperature()); Serial.println(" °C");
    Serial.print("Pressure: "); Serial.print(pres); Serial.println(" hPa");


    Serial.println("----- GPS Sensor-----");
    Serial.print("Latitude: "); Serial.println(lat);
    Serial.print("Longitude: "); Serial.println(lon);

    Serial.println("----- Air Quality Sensor-----");
    int sensorValue = analogRead(MQ135_PIN);
    float voltage = sensorValue * (3.3 / 4095.0); // Convert to voltage
    float RS = ((5.0 * RL) / voltage) - RL; // Calculate sensor resistance
    float ratio = RS / R0; // Calculate RS/R0 ratio
    Serial.print("CO: "); Serial.print(co_ppm); Serial.println(" ppm");
    Serial.print("Alcohol: "); Serial.print(alcohol_ppm); Serial.println(" ppm");
    Serial.print("CO2: "); Serial.print(co2_ppm); Serial.println(" ppm");
    Serial.print("NH4: "); Serial.print(nh4_ppm); Serial.println(" ppm");
    Serial.print("Air Quality: "); Serial.println(assessAirQuality(ratio));
    Serial.println("-----------------------");

    sendMetric(lat, lon, co2_ppm, nh4_ppm, alcohol_ppm, temp, hum, pressure, impact);
    delay(10000);

  }

  delay(100);

}
