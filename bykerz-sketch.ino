#include "EmbeddedDevice.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

EmbeddedDevice device;

// Device configuration
const char* deviceId = "bykerz-iot-001";
const int vehicleId = 1;

// WiFi configuration
const char* ssid = "LUNAREJA";
const char* password = "Gatita2020*";

// Edge Service configuration
const char* edgeServiceUrl = "http://192.168.18.6:5000/api/v1/metrics";
const char* jwtToken = "eyJhbGciOiJIUzM4NCJ9.eyJzdWIiOiJhbHhSYVRpIiwiaWF0IjoxNzYzMTg3MzgyLCJleHAiOjE3NjM3OTIxODJ9._1a68a3YaJx3AGIH8Af7UDQBPfgFVqvj6BO0j1vrlSImoQlmFkKds8tLHK2nEoNN";

void connectWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
}

void sendMetric(double latitude, double longitude, float co2, float nh3, float benzene,
                float temp, float pressure, bool impact) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Reconnecting...");
        connectWiFi();
        return;
    }

    HTTPClient http;
    http.setTimeout(5000);
    if (!http.begin(edgeServiceUrl)) {
        Serial.println("Failed to begin HTTP connection");
        return;
    }
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(jwtToken));

    StaticJsonDocument<512> doc;
    doc["device_id"] = deviceId;
    doc["vehicle_id"] = vehicleId;
    doc["latitude"] = latitude;
    doc["longitude"] = longitude;
    doc["CO2Ppm"] = co2;
    doc["NH3Ppm"] = nh3;
    doc["BenzenePpm"] = benzene;
    doc["temperatureCelsius"] = temp;
    doc["pressureHpa"] = pressure;
    doc["impactDetected"] = impact;

    String jsonString;
    serializeJson(doc, jsonString);

    Serial.println("\nSending metric to server...");
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("Response code: ");
        Serial.println(httpResponseCode);
        Serial.print("Response: ");
        Serial.println(response);
    } else {
        Serial.print("Error sending metric: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void setup() {
    Serial.begin(115200);
    delay(3000);

    Serial.println("Bykerz IoT Device Starting");

    // Connect to WiFi
    connectWiFi();

    // Initialize device
    if (!device.initialize()) {
        Serial.println("Device initialization failed!");
        while (1) {
            delay(1000);
        }
    }

    Serial.println("Warming up components (60 seconds)...");
    delay(60000);

    Serial.println("Device ready!");
}

void loop() {
    device.update();

    static bool lastKnockState = false;
    static unsigned long lastKnockTime = 0;
    const unsigned long DEBOUNCE_DELAY = 2000; // 2 seconds debounce

    bool knockDetected = (digitalRead(EmbeddedDevice::KNOCK_PIN) == HIGH);

    // Only process knock if: 1) state changed, 2) enough time passed
    if (knockDetected && !lastKnockState &&
        (millis() - lastKnockTime > DEBOUNCE_DELAY)) {

        lastKnockTime = millis();

        Serial.println("\n🔔 KNOCK DETECTED! Sending metrics...");

        float temp = device.getTemperatureSensor().getTemperature();
        float pressure = device.getPressureSensor().getPressure();
        float co2 = device.getGasQualitySensor().getCO2Level();
        float nh3 = device.getGasQualitySensor().getNH3Level();
        float benzene = device.getGasQualitySensor().getBenzeneLevel();
        double lat = device.getGPSSensor().getLatitude();
        double lon = device.getGPSSensor().getLongitude();

        Serial.println("\nCurrent Readings:");
        Serial.print("  Temperature: "); Serial.print(temp); Serial.println(" °C");
        Serial.print("  Pressure: "); Serial.print(pressure); Serial.println(" hPa");
        Serial.print("  CO2: "); Serial.print(co2); Serial.println(" ppm");
        Serial.print("  NH3: "); Serial.print(nh3); Serial.println(" ppm");
        Serial.print("  Benzene: "); Serial.print(benzene); Serial.println(" ppm");

        if (device.getGPSSensor().isLocationValid()) {
            Serial.print("  Location: ");
            Serial.print(lat, 6);
            Serial.print(", ");
            Serial.println(lon, 6);
        } else {
            Serial.println("  GPS: No fix (acquiring satellites...)");
        }

        sendMetric(lat, lon, co2, nh3, benzene, temp, pressure, true);
    }

    lastKnockState = knockDetected;
    delay(100);
}