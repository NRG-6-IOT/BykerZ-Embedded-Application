#include "EmbeddedDevice.h"
#include <Arduino.h>

EmbeddedDevice::EmbeddedDevice(
    int ledPin,
    int dhtPin,
    int dhtType,
    int gasPin,
    int knockPin,
    int gpsSerial,
    int gpsRx,
    int gpsTx,
    bool initialLedState)
    : statusLed(ledPin, initialLedState, this),
      temperature(dhtPin, dhtType, this),
      pressure(this),
      gasQuality(gasPin, this),
      knock(knockPin, this),
      gps(gpsSerial, gpsRx, gpsTx, this) {
}

bool EmbeddedDevice::initialize() {
    Serial.println("Initializing Embedded Device...");

    if (!temperature.initialize()) {
        Serial.println("Failed to initialize DHT11 sensor!");
        return false;
    }

    if (!pressure.initialize()) {
        Serial.println("Failed to initialize BMP280 sensor!");
        return false;
    }

    Serial.println("All sensors initialized successfully!");
    return true;
}

void EmbeddedDevice::setMetricCallback(void (*callback)(double, double, float, float, float, float, float, bool)) {
    metricCallback = callback;
}


void EmbeddedDevice::on(Event event) {
    if (event == TemperatureSensor::HIGH_TEMPERATURE_EVENT) {
        //statusLed.handle(Led::TURN_ON_COMMAND);
        Serial.print("HIGH TEMPERATURE DETECTED: ");
        Serial.print(temperature.getTemperature());
        Serial.println(" °C");
        shouldSendMetric = true;

    } else if (event == TemperatureSensor::LOW_TEMPERATURE_EVENT) {
        //statusLed.handle(Led::TURN_OFF_COMMAND);
        Serial.print("LOW TEMPERATURE DETECTED: ");
        Serial.print(temperature.getTemperature());
        Serial.println(" °C");
        shouldSendMetric = true;

    } else if (event == PressureSensor::LOW_PRESSURE_EVENT) {
        Serial.print("⬇LOW PRESSURE DETECTED: ");
        Serial.print(pressure.getPressure());
        Serial.println(" hPa");
        shouldSendMetric = true;

    } else if (event == PressureSensor::HIGH_PRESSURE_EVENT) {
        Serial.print("⬆HIGH PRESSURE DETECTED: ");
        Serial.print(pressure.getPressure());
        Serial.println(" hPa");
        shouldSendMetric = true;

    } else if (event == GasQualitySensor::HIGH_CO2_LEVEL_EVENT) {
        //statusLed.handle(Led::TURN_ON_COMMAND);
        Serial.print("HIGH CO2 LEVEL: ");
        Serial.print(gasQuality.getCO2Level());
        Serial.println(" ppm");
        shouldSendMetric = true;

    } else if (event == GasQualitySensor::HIGH_NH3_LEVEL_EVENT) {
        Serial.print("HIGH NH3 LEVEL: ");
        Serial.print(gasQuality.getNH3Level());
        Serial.println(" ppm");
        shouldSendMetric = true;

    } else if (event == GasQualitySensor::HIGH_BENZENE_LEVEL_EVENT) {
        Serial.print("HIGH BENZENE LEVEL: ");
        Serial.print(gasQuality.getBenzeneLevel());
        Serial.println(" ppm");
        shouldSendMetric = true;

    } else if (event == KnockSensor::KNOCK_DETECTED_EVENT) {
        //statusLed.handle(Led::TOGGLE_LED_COMMAND);
        Serial.println("KNOCK DETECTED!");
        shouldSendMetric = true;
    }

    if (shouldSendMetric && metricCallback != nullptr) {
        metricCallback(
            gps.getLatitude(),
            gps.getLongitude(),
            gasQuality.getCO2Level(),
            gasQuality.getNH3Level(),
            gasQuality.getBenzeneLevel(),
            temperature.getTemperature(),
            pressure.getPressure(),
            !knock.getKnockStatus()
        );
    }
}

void EmbeddedDevice::handle(Command command) {
    if (command == Led::TOGGLE_LED_COMMAND ||
        command == Led::TURN_ON_COMMAND ||
        command == Led::TURN_OFF_COMMAND) {
        Serial.printf("LED state: %d\n", statusLed.getState());
        }
}

void EmbeddedDevice::update() {
    temperature.scanTemperature();
    pressure.scanPressure();
    gasQuality.scanGasLevels();
    knock.scanKnock();
    gps.scanLocation();

    Serial.println("\n=== SENSOR READINGS ===");

    Serial.print("Knock Detected: ");
    Serial.println(knock.getKnockStatus() ? "YES" : "NO");

    Serial.print("Temperature: ");
    Serial.print(temperature.getTemperature());
    Serial.println(" °C");

    Serial.print("Pressure: ");
    Serial.print(pressure.getPressure());
    Serial.println(" hPa");

    Serial.print("CO2: ");
    Serial.print(gasQuality.getCO2Level());
    Serial.println(" ppm");

    Serial.print("NH3: ");
    Serial.print(gasQuality.getNH3Level());
    Serial.println(" ppm");

    Serial.print("Benzene: ");
    Serial.print(gasQuality.getBenzeneLevel());
    Serial.println(" ppm");

    Serial.print("Location: ");
    Serial.print(gps.getLatitude(), 6);
    Serial.print(", ");
    Serial.println(gps.getLongitude(), 6);

    Serial.println("=======================\n");
}

Led& EmbeddedDevice::getStatusLed() {
    return statusLed;
}

TemperatureSensor& EmbeddedDevice::getTemperatureSensor() {
    return temperature;
}

PressureSensor& EmbeddedDevice::getPressureSensor() {
    return pressure;
}

GasQualitySensor& EmbeddedDevice::getGasQualitySensor() {
    return gasQuality;
}

KnockSensor& EmbeddedDevice::getKnockSensor() {
    return knock;
}

GPSSensor& EmbeddedDevice::getGPSSensor() {
    return gps;
}