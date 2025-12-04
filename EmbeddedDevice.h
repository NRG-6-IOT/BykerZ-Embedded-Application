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

public:
    static const int LED_PIN = 33;
    static const int DHT_PIN = 26;
    static const int DHT_TYPE = 11;
    static const int GAS_PIN = 34;
    static const int KNOCK_PIN = 27;
    static const int GPS_SERIAL = 2;
    static const int GPS_RX_PIN = 16;
    static const int GPS_TX_PIN = 17;

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
};

#endif //EMBEDDED_DEVICE_H
