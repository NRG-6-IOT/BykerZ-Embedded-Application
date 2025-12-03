#include "TemperatureSensor.h"
#include <Arduino.h>

const Event TemperatureSensor::HIGH_TEMPERATURE_EVENT = Event(HIGH_TEMPERATURE_EVENT_ID);
const Event TemperatureSensor::LOW_TEMPERATURE_EVENT = Event(LOW_TEMPERATURE_EVENT_ID);

TemperatureSensor::TemperatureSensor(int pin, int type, EventHandler* eventHandler)
    : Sensor(pin, eventHandler), dht(pin, type), currentTemperature(0.0), dhtType(type) {
}

bool TemperatureSensor::initialize() {
    dht.begin();
    Serial.println("DHT sensor initialized!");
    return true;
}

void TemperatureSensor::scanTemperature() {
    currentTemperature = dht.readTemperature();

    if (currentTemperature > 40.0) {
        on(HIGH_TEMPERATURE_EVENT);
    } else if (currentTemperature < 10.0) {
        on(LOW_TEMPERATURE_EVENT);
    }
}

float TemperatureSensor::getTemperature() const {
    return currentTemperature;
}
