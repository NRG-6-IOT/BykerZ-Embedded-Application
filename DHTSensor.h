#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "Sensor.h"
#include <DHT.h>

class DHTSensor : public Sensor {
private:
    DHT* dht;
    float currentTemperature;

public:
    static const int HIGH_TEMPERATURE_EVENT_ID = 3;
    static const int LOW_TEMPERATURE_EVENT_ID = 4;
    static const Event HIGH_TEMPERATURE_EVENT;
    static const Event LOW_TEMPERATURE_EVENT;

    DHTSensor(int pin, int type, EventHandler* eventHandler = nullptr);

    void scanTemperature();
    float getTemperature();
};

#endif //DHT_SENSOR_H