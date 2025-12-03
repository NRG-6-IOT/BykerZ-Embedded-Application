#include "BMPSensor.h"
#include <Arduino.h>

const Event BMPSensor::LOW_PRESSURE_EVENT = Event(BMPSensor::LOW_PRESSURE_EVENT_ID);
const Event BMPSensor::HIGH_PRESSURE_EVENT = Event(BMPSensor::HIGH_PRESSURE_EVENT_ID);

BMPSensor::BMPSensor(EventHandler *eventHandler)
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

void BMPSensor::scanPressure() {
    float currentPressure = bmp->readPressure() / 100.0F; // Convert to hPa
    if (currentPressure < 980.0F) {
        on(LOW_PRESSURE_EVENT);
    } else if (currentPressure > 1050.0F) {
        on(HIGH_PRESSURE_EVENT);
    }
}

float BMPSensor::getPressure() const {
    return bmp->readPressure() / 100.0F;
}
