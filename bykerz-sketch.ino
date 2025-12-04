#include "EmbeddedDevice.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

EmbeddedDevice device;
Preferences prefs;
String apiToken = "";

// Device configuration
String deviceId = "bykerz-iot-001";
const int vehicleId = 1;

// WiFi configuration
const char* ssid = "LUNAREJA";
const char* password = "Gatita2020*";

// Edge Service configuration
const char* edgeServiceUrl = "http://192.168.18.7:5000/api/v1/metrics";
const char* validationUrl = "http://192.168.18.7:5000/api/v1/devices/authentication/validate";
const char* authenticationUrl = "http://192.168.18.7:5000/api/v1/devices/authentication/register";
void saveApiKey(const String& apiKey) {
    prefs.begin("auth", false); // namespace: auth
    prefs.putString("apiKey", apiKey);
    prefs.end();
}
String loadApiKey() {
    prefs.begin("auth", true);
    String key = prefs.getString("apiKey", "");
    prefs.end();
    return key; // empty string if not stored yet
}

void connectWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
}

bool registerDevice() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return false;
    }

    HTTPClient http;
    http.setTimeout(5000);

    if (!http.begin(authenticationUrl)) {
        Serial.println("Failed to connect to authentication server");
        return false;
    }

    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> doc;
    doc["deviceId"] = deviceId;
    doc["vehicleId"] = vehicleId;

    String jsonString;
    serializeJson(doc, jsonString);

    Serial.println("Registering device...");
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode == 200) {
        Serial.println("Device registered successfully!");
        String response = http.getString();

        StaticJsonDocument<512> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, response);

        if (error) {
            Serial.print("JSON parsing failed: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        apiToken = responseDoc["token"].as<String>();
        
        if (apiToken.isEmpty()) {
            Serial.println("Token not found in validation response");
            http.end();
            return false;
        }

        saveApiKey(apiToken);

        Serial.println("Token retrieved successfully!");
        Serial.print("API Token: ");
        Serial.println(apiToken);

        http.end();
    } else {
        Serial.print("Registration failed with code: ");
        Serial.println(httpResponseCode);
        Serial.println(http.getString());
        http.end();
        return false;
    }
}

bool getTokenFromValidation() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return false;
    }

    HTTPClient http;
    http.setTimeout(5000);

    if (!http.begin(validationUrl)) {
        Serial.println("Failed to connect to validation server");
        return false;
    }

    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<128> doc;
    doc["deviceId"] = deviceId;

    String jsonString;
    serializeJson(doc, jsonString);

    Serial.println("Getting token from validation...");
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode == 200) {
        String response = http.getString();
        
        StaticJsonDocument<512> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, response);
        
        if (error) {
            Serial.print("JSON parsing failed: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        apiToken = responseDoc["token"].as<String>();
        
        if (apiToken.isEmpty()) {
            Serial.println("Token not found in validation response");
            http.end();
            return false;
        }

        saveApiKey(apiToken);
        
        Serial.println("Token retrieved successfully!");
        Serial.print("API Token: ");
        Serial.println(apiToken);
        
        http.end();
        return true;
    } else {
        Serial.print("Token retrieval failed with code: ");
        Serial.println(httpResponseCode);
        http.end();
        return false;
    }
}

bool authenticateDevice() {
    // Try to load existing API key
    apiToken = loadApiKey();

    if (!apiToken.isEmpty()) {
        Serial.println("Found stored API key, validating...");
        if (getTokenFromValidation()) {
            return true;
        }
        Serial.println("Stored API key is invalid");
    }

    // No valid key found, register device
    Serial.println("No valid API key found, registering device...");
    return registerDevice();
}

void sendMetric(double latitude, double longitude, float co2, float nh3, float benzene,
                float temp, float pressure, bool impact) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Reconnecting...");
        connectWiFi();
        return;
    }

    if (apiToken.isEmpty()) {
        Serial.println("No API token available. Cannot send metric.");
        return;
    }

    HTTPClient http;
    http.setTimeout(5000);
    if (!http.begin(edgeServiceUrl)) {
        Serial.println("Failed to begin HTTP connection");
        return;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + apiToken);

    StaticJsonDocument<512> doc;
    doc["vehicleId"] = vehicleId;
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
    delay(20000);
}

void setup() {
    Serial.begin(115200);
    delay(3000);

    Serial.println("Bykerz IoT Device Starting");

    // Connect to WiFi
    connectWiFi();

    // Authenticate device
    if (!authenticateDevice()) {
        Serial.println("Device authentication failed!");
        while (1) {
            delay(5000);
            Serial.println("Retrying authentication...");
            if (authenticateDevice()) {
                break;
            }
        }
    }

    // Initialize device
    if (!device.initialize()) {
        Serial.println("Device initialization failed!");
        while (1) {
            delay(1000);
        }
    }
    device.setMetricCallback(sendMetric);

    Serial.println("Warming up components (2 minutes)...");
    delay(120000);

    Serial.println("Device ready!");
}

void loop() {
    device.update();
    delay(2000);
}