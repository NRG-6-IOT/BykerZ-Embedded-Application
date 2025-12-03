#ifndef GAS_QUALITY_SENSOR_H
#define GAS_QUALITY_SENSOR_H

#include "Sensor.h"

class GasQualitySensor : public Sensor {
private:
    float R0 = 76.63; // Resistance in clean air
    float RL = 10.0; // Load resistance in KΩ

    float calculatePPM(float ratio, float slope, float intercept);
    float calculateRs(int rawValue);

public:
    static const int HIGH_CO2_LEVEL_EVENT_ID = 7;
    static const int HIGH_NH4_LEVEL_EVENT_ID = 8;
    static const int HIGH_ALCOHOL_LEVEL_EVENT_ID = 9;
    static const Event HIGH_CO2_LEVEL_EVENT;
    static const Event HIGH_NH4_LEVEL_EVENT;
    static const Event HIGH_ALCOHOL_LEVEL_EVENT;

    GasQualitySensor(int pin, EventHandler* eventHandler = nullptr);

    void scanGasLevels();
    float getCO2Level();
    float getNH4Level();
    float getAlcoholLevel();
    void calibrate();
    void setR0(float r0Value);
    float getR0() const;

};


#endif //GAS_QUALITY_SENSOR_H