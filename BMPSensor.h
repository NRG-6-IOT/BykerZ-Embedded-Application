#ifndef BMP_SENSOR_H
#define BMP_SENSOR_H

#include "Sensor.h"
#include <Adafruit_BMP280.h>

class BMPSensor : public Sensor {
private:
    Adafruit_BMP280* bmp;
public:
    static const int LOW_PRESSURE_EVENT_ID = 0;
    static const Event LOW_PRESSURE_EVENT;

    BMPSensor(EventHandler* eventHandler = nullptr);

    float getPressure() const;
};

#endif //BMP_SENSOR_H