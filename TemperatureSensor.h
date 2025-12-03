#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "Sensor.h"
#include <DHT.h>

class TemperatureSensor : public Sensor {
private:
    DHT* dht;
    float currentTemperature;

public:
    static const int HIGH_TEMPERATURE_EVENT_ID = 3;
    static const int LOW_TEMPERATURE_EVENT_ID = 4;
    static const Event HIGH_TEMPERATURE_EVENT;
    static const Event LOW_TEMPERATURE_EVENT;

    TemperatureSensor(int pin, int type, EventHandler* eventHandler = nullptr);

    void scanTemperature();
    float getTemperature();
};

#endif //TEMPERATURE_SENSOR_H