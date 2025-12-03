#include "DHTSensor.h"
#include <Arduino.h>

const Event DHTSensor::HIGH_TEMPERATURE_EVENT = Event(HIGH_TEMPERATURE_EVENT_ID);
const Event DHTSensor::LOW_TEMPERATURE_EVENT = Event(LOW_TEMPERATURE_EVENT_ID);

DHTSensor::DHTSensor(int pin, int type, EventHandler* eventHandler)
    : Sensor(pin, eventHandler) {
    dht = new DHT(pin, type);
    dht.begin();
}

void DHTSensor::scanTemperature() {
    currentTemperature = dht->readTemperature();
    if (currentTemperature > 40.0) {
        on(HIGH_TEMPERATURE_EVENT);
    } else if (currentTemperature < 10.0) {
        on(LOW_TEMPERATURE_EVENT);
    }

float DHTSensor::getTemperature() const {
    return dht->readTemperature();
}
