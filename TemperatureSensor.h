#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "Sensor.h"
#include <DHT.h>

class TemperatureSensor : public Sensor {
private:
    DHT dht;
    float currentTemperature;
    int dhtType;

public:
    static const int HIGH_TEMPERATURE_EVENT_ID = 3;
    static const int LOW_TEMPERATURE_EVENT_ID = 4;
    static const Event HIGH_TEMPERATURE_EVENT;
    static const Event LOW_TEMPERATURE_EVENT;

    TemperatureSensor(int pin, int type, EventHandler* eventHandler = nullptr);

    bool initialize();
    void scanTemperature();
    float getTemperature() const;
};

#endif //TEMPERATURE_SENSOR_H
