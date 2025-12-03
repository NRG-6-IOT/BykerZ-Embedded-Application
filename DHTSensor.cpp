#include "DHTSensor.h"
#include <Arduino.h>

DHTSensor::DHTSensor(int pin, int type, EventHandler* eventHandler)
    : Sensor(pin, eventHandler) {
    dht = new DHT(pin, type);
}

float DHTSensor::getTemperature() const {
    return dht->readTemperature();
}
