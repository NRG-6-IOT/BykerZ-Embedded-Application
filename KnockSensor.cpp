#include "KnockSensor.h"
#include <Arduino.h>

const Event KnockSensor::KNOCK_DETECTED_EVENT = Event(KnockSensor::KNOCK_DETECTED_EVENT_ID);

KnockSensor::KnockSensor(int pin, EventHandler* eventHandler)
    : Sensor(pin, eventHandler) {
    pinMode(pin, INPUT);
}

void KnockSensor::scanKnock() {
    int state = digitalRead(pin);
    if (state == HIGH) {
        on(KNOCK_DETECTED_EVENT);
    }
}