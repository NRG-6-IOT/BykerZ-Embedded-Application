#include "GPSSensor.h"
#include <Arduino.h>

GPSSensor::GPSSensor(int uartNum, int rx, int tx, EventHandler* eventHandler)
    : Sensor(uartNum, eventHandler), gpsSerial(uartNum), rxPin(rx), txPin(tx),
      latitude(0.0), longitude(0.0) {
    gpsSerial.begin(9600, SERIAL_8N1, rxPin, txPin);
    Serial.println("GPS initialized!");
}

void GPSSensor::scanLocation() {
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    }
}

double GPSSensor::getLatitude() const {
    return latitude;
}

double GPSSensor::getLongitude() const {
    return longitude;
}

bool GPSSensor::isLocationValid() const {
    return gps.location.isValid();
}
