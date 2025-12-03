#ifndef BMP_SENSOR_H
#define BMP_SENSOR_H

#include "Sensor.h"
#include <Adafruit_BMP280.h>

class BMPSensor : public Sensor {
private:
    Adafruit_BMP280* bmp;
    uint8_t address;
    float currentPressure;
public:
    static const int LOW_PRESSURE_EVENT_ID = 5;
    static const int HIGH_PRESSURE_EVENT_ID = 6;
    static const Event LOW_PRESSURE_EVENT;
    static const Event HIGH_PRESSURE_EVENT;

    BMPSensor(EventHandler* eventHandler = nullptr);

    void scanPressure();
    float getPressure();
};

#endif //BMP_SENSOR_H