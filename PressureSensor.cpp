#include "PressureSensor.h"
#include <Arduino.h>

const Event PressureSensor::LOW_PRESSURE_EVENT = Event(PressureSensor::LOW_PRESSURE_EVENT_ID);
const Event PressureSensor::HIGH_PRESSURE_EVENT = Event(PressureSensor::HIGH_PRESSURE_EVENT_ID);

PressureSensor::PressureSensor(EventHandler* eventHandler)
    : Sensor(eventHandler), currentPressure(0.0F) {
}

bool PressureSensor::initialize() {
    if (!bmp.begin(0x76)) {
        Serial.println("Could not find BMP280 at 0x76. Trying 0x77...");
        if (!bmp.begin(0x77)) {
            Serial.println("BMP280 not found. Check wiring!");
            return false;
        }
    }

    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_NONE,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::FILTER_X4,
                    Adafruit_BMP280::STANDBY_MS_500);

    Serial.println("BMP280 initialized!");
    return true;
}

void PressureSensor::scanPressure() {
    float pressure = bmp.readPressure() / 100.0F; // Convert Pa to hPa

    if (isnan(pressure)) {
        Serial.println("Error reading pressure from BMP280!");
        return;
    }

    currentPressure = pressure;

    if (currentPressure < 980.0F) {
        on(LOW_PRESSURE_EVENT);
    } else if (currentPressure > 1050.0F) {
        on(HIGH_PRESSURE_EVENT);
    }
}

float PressureSensor::getPressure() const {
    return currentPressure;
}
