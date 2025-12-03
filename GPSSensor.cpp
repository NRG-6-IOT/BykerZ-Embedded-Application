#include "GPSSensor.h"
#include <Arduino.h>

GPSSensor::GPSSensor(int pin, int rx, int tx, EventHandler* eventHandler)
    : Sensor(pin, eventHandler), rxPin(rx), txPin(tx), latitude(0.0), longitude(0.0) {
    gpsSerial = new HardwareSerial(pin); // Use pin as serial number (2 for Serial2)
    gpsSerial->begin(9600, SERIAL_8N1, rxPin, txPin);
}

void GPSSensor::scanLocation() {
    while (gpsSerial->available()) {
        gps.encode(gpsSerial->read());
    }

    if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    }
}

float GPSSensor::getLatitude() {
    return latitude;
}

float GPSSensor::getLongitude() {
    return longitude;
}