#include "BMPSensor.h"
#include <Arduino.h>

BMPSensor::BMPSensor(EventHandler *eventHandler) :
    Sensor(eventHandler) {
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

float BMPSensor::getPressure() const {
    return bmp->readPressure() / 100.0F;
}
