#ifndef KNOCK_SENSOR_H
#define KNOCK_SENSOR_H

#include "Sensor.h"

class KnockSensor : public Sensor {
public:
    static const int KNOCK_DETECTED_EVENT_ID = 10;
    static const Event KNOCK_DETECTED_EVENT;

    KnockSensor(int pin, EventHandler* eventHandler = nullptr);

    void scanKnock();
};

#endif //KNOCK_SENSOR_H
