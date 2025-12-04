#ifndef EMBEDDED_DEVICE_H
#define EMBEDDED_DEVICE_H

#include "Device.h"
#include "Led.h"
#include "TemperatureSensor.h"
#include "PressureSensor.h"
#include "GasQualitySensor.h"
#include "KnockSensor.h"
#include "GPSSensor.h"

class EmbeddedDevice : public Device {
private:
    Led statusLed;
    TemperatureSensor temperature;
    PressureSensor pressure;
    GasQualitySensor gasQuality;
    KnockSensor knock;
    GPSSensor gps;
    void (*metricCallback)(double, double, float, float, float, float, float, bool);

public:
    static const int LED_PIN = 33; // Self explanatory
    static const int DHT_PIN = 26; //DHT "DATA" PIN
    static const int DHT_TYPE = 11; // DHT TYPE (11/22)
    static const int GAS_PIN = 34; //GAS "AO" PIN
    static const int KNOCK_PIN = 27; //KY-031 "MIDDLE" PIN
    static const int GPS_SERIAL = 2; //UART2
    static const int GPS_RX_PIN = 16; //PIN RECEIVES FROM GPS "TX"
    static const int GPS_TX_PIN = 17; //PIN TRANSMITS TO GPS "RX"

    EmbeddedDevice(
        int ledPin = LED_PIN,
        int dhtPin = DHT_PIN,
        int dhtType = DHT_TYPE,
        int gasPin = GAS_PIN,
        int knockPin = KNOCK_PIN,
        int gpsSerial = GPS_SERIAL,
        int gpsRx = GPS_RX_PIN,
        int gpsTx = GPS_TX_PIN,
        bool initialLedState = false
    );

    void on(Event event) override;
    void handle(Command command) override;
    void update();
    bool initialize();

    Led& getStatusLed();
    TemperatureSensor& getTemperatureSensor();
    PressureSensor& getPressureSensor();
    GasQualitySensor& getGasQualitySensor();
    KnockSensor& getKnockSensor();
    GPSSensor& getGPSSensor();

    void setMetricCallback(void (*callback)(double, double, float, float, float, float, float, bool));
};

#endif //EMBEDDED_DEVICE_H
