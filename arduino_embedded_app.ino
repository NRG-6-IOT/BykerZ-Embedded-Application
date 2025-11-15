#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Definiciones
#define MQ135_PIN 34  // GPIO34 - Analog pin
#define LED_PIN 33 // GPIO33 - LED pin
#define DHT_PIN 25 // GPIO25 - DHT pin
#define DHT_TYPE DHT11 // DHT sensor type (11/22)

// Configuración WiFi
const char* ssid = "LUNAREJA";
const char* password = "Gatito2020*";

// Configuración Edge Service
const char* edgeServiceUrl = "http://192.168.18.6:5000/api/v1/metrics";
const char* jwtToken = "eyJhbGciOiJIUzM4NCJ9.eyJzdWIiOiJtZWNoYW5pYyIsImlhdCI6MTc2MzE4MTM4NCwiZXhwIjoxNzYzNzg2MTg0fQ.FgxuzjFkMk2myIAGci1e1KMOYqGYtL-l0-oOrjBuJwCq8eVGsQRhnVskYcYd67qq"; // Tu token JWT

// Configuración del dispositivo
const char* deviceId = "bykerz-iot-001";
const int vehicleId = 1;

// Calibration values - adjust based on your environment
const float R0 = 76.63;    // Resistance in clean air (calibrate this)
const float RL = 10.0;     // Load resistance in KΩ

// Gas curve parameters for MQ-135
typedef struct {
  float ppm;
  float slope;
  float intercept;
} GasCurve;

// Common gases detected by MQ-135
GasCurve CO_curve = {10.0, -0.421, 1.177};
GasCurve Alcohol_curve = {10.0, -0.422, 1.055};
GasCurve CO2_curve = {400.0, -0.382, 1.132};
GasCurve Toluen_curve = {10.0, -0.412, 1.431};
GasCurve NH4_curve = {10.0, -0.411, 1.786};
GasCurve Aceton_curve = {10.0, -0.396, 1.513};

// Configure DHT
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  
  // Conectar a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");

  // Configure DHT
  dht.begin();
  Serial.println("DHT Started!");

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_PIN, OUTPUT);
  
  // Configure ADC
  analogReadResolution(12);  // Set to 12-bit resolution (0-4095)
  
  // Allow sensor to warm up
  Serial.println("MQ-135 Warming Up...");
  delay(60000);  // Wait 60 seconds for sensor to stabilize
  
  Serial.println("MQ-135 Sensor Ready!");
  Serial.println("=================================");
}

void sendMetric(float CO2, float NH3, float benzene, float temp, float humidity, float pressure, bool impact) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(edgeServiceUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(jwtToken));

    // Crear JSON
    StaticJsonDocument<512> doc;
    doc["device_id"] = deviceId;
    doc["vehicle_id"] = vehicleId;
    doc["latitude"] = -12.0464;
    doc["longitude"] = -77.0428;
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

void loop() {
  // Read analog value
  int sensorValue = analogRead(MQ135_PIN);
  
  // Convert to voltage (ESP32 ADC reference voltage is 3.3V)
  float voltage = sensorValue * (3.3 / 4095.0);
  
  // Calculate sensor resistance
  float RS = ((5.0 * RL) / voltage) - RL;  // RS in KΩ
  
  // Calculate RS/R0 ratio
  float ratio = RS / R0;
  
  // Calculate PPM for different gases
  float co_ppm = calculatePPM(ratio, CO_curve);
  float alcohol_ppm = calculatePPM(ratio, Alcohol_curve);
  float co2_ppm = calculatePPM(ratio, CO_curve);
  float nh4_ppm = calculatePPM(ratio, NH4_curve);
  
  // Air Quality assessment
  String airQuality = assessAirQuality(ratio);

  // Read the temperature and humidity values
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  
  // Display results
  Serial.println("==== MQ-135 Gas Concentrations Readings ====");
  Serial.print("CO: "); Serial.print(co_ppm, 2); Serial.println(" ppm");
  Serial.print("Alcohol: "); Serial.print(alcohol_ppm, 2); Serial.println(" ppm");
  Serial.print("CO2: "); Serial.print(co2_ppm, 2); Serial.println(" ppm");
  Serial.print("NH4: "); Serial.print(nh4_ppm, 2); Serial.println(" ppm");
  Serial.print("Air Quality: "); Serial.println(airQuality);
  Serial.println("==============================================");
  Serial.println("=== DHT11 Temperature And Humidity Readings ===");
  Serial.print("Temperature: "); Serial.print(temp - 20); Serial.println("°C");
  Serial.print("Humidity%: "); Serial.print(hum - 20); Serial.println("%");
  Serial.println("==============================================");
  Serial.println();

  // If any value is high, warn with the led
  if (co2_ppm > 1000) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }


  // Leer sensores (ejemplo con valores simulados)
  //float CO2 = random(400, 600);
  //float NH3 = random(10, 50);
  float benzene = random(5, 25);
  //float temp = random(20, 30);
  //float humidity = random(40, 80);
  float pressure = random(1010, 1020);
  bool impact = false;

  // Enviar métricas cada 30 segundos
  sendMetric(co2_ppm, nh4_ppm, alcohol_ppm, temp, hum, pressure, impact);
  delay(30000);
}

float calculatePPM(float ratio, GasCurve curve) {
  // Calculate PPM using the curve parameters
  // Formula: ppm = 10^((log10(ratio) - intercept) / slope)
  return pow(10, (log10(ratio) - curve.intercept) / curve.slope);
}

String assessAirQuality(float ratio) {
  if (ratio > 1.5) return "Excellent";
  else if (ratio > 1.0) return "Good";
  else if (ratio > 0.8) return "Moderate";
  else if (ratio > 0.6) return "Poor";
  else return "Very Poor";
}

// Calibration function - run this in clean air to find R0
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
