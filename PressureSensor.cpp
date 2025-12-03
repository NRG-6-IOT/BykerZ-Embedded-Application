#include "PressureSensor.h"
#include <Arduino.h>

const Event PressureSensor::LOW_PRESSURE_EVENT = Event(PressureSensor::LOW_PRESSURE_EVENT_ID);
const Event PressureSensor::HIGH_PRESSURE_EVENT = Event(PressureSensor::HIGH_PRESSURE_EVENT_ID);

PressureSensor::PressureSensor(EventHandler *eventHandler)
    : Sensor(eventHandler) {
    bmp = new Adafruit_BMP280();
    if (!bmp->begin(0x76)) {
        Serial.println("Could not find BMP280 at 0x76. Trying 0x77...");
        if (!bmp->begin(0x77)) {
            Serial.println("BMP280 not found. Check wiring!");
            return;
        }
    }

    bmp->setSampling(Adafruit_BMP280::MODE_NORMAL,
                        Adafruit_BMP280::SAMPLING_X2,
                        Adafruit_BMP280::SAMPLING_X16,
                        Adafruit_BMP280::FILTER_X16,
                        Adafruit_BMP280::STANDBY_MS_500);

    Serial.println("BMP280 initialized!");
}

void PressureSensor::scanPressure() {
    float currentPressure = bmp->readPressure() / 100.0F; // Convert to hPa
    if (currentPressure < 980.0F) {
        on(LOW_PRESSURE_EVENT);
    } else if (currentPressure > 1050.0F) {
        on(HIGH_PRESSURE_EVENT);
    }
}

float PressureSensor::getPressure() const {
    return bmp->readPressure() / 100.0F;
}
