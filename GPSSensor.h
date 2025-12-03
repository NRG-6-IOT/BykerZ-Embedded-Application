#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include "Sensor.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

class GPSSensor : public Sensor {
private:
    TinyGPSPlus gps;
    HardwareSerial* gpsSerial;
    float latitude;
    float longitude;
    int rxPin;
    int txPin;
public:
    GPSSensor(int pin, int rx, int tx, EventHandler* eventHandler = nullptr);
    void scanLocation();
    float getLatitude();
    float getLongitude();
};


#endif //GPS_SENSOR_H