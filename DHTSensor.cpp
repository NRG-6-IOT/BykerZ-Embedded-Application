#include "DHTSensor.h"
#include <Arduino.h>

const Event DHTSensor::HIGH_TEMPERATURE_EVENT = Event(HIGH_TEMPERATURE_EVENT_ID);
const Event DHTSensor::LOW_TEMPERATURE_EVENT = Event(LOW_TEMPERATURE_EVENT_ID);

DHTSensor::DHTSensor(int pin, int type, EventHandler* eventHandler)
    : Sensor(pin, eventHandler) {
    dht = new DHT(pin, type);
}

float DHTSensor::getTemperature() const {
    return dht->readTemperature();
}
