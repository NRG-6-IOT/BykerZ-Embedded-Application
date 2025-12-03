#ifndef GAS_QUALITY_SENSOR_H
#define GAS_QUALITY_SENSOR_H

#include "Sensor.h"

class GasQualitySensor : public Sensor {
private:
    float R0 = 10.0;     // Resistance in clean air (must be calibrated)
    float rLoad = 20.0;  // Load resistance in KΩ

    // Gas calibration curves (ppm = a * (Rs/R0)^b)
    typedef struct {
        float a;
        float b;
    } GasCurve;

    static const GasCurve CO2_CURVE;
    static const GasCurve NH3_CURVE;
    static const GasCurve BENZENE_CURVE;

    // Thresholds in ppm
    float co2Threshold = 1000.0;
    float nh3Threshold = 50.0;
    float benzeneThreshold = 5.0;

    float currentCO2Level;
    float currentNH3Level;
    float currentBenzeneLevel;

    float getRs(int rawADC);
    float estimatePPM(float rs, const GasCurve& curve);

public:
    static const int HIGH_CO2_LEVEL_EVENT_ID = 7;
    static const int HIGH_NH3_LEVEL_EVENT_ID = 8;
    static const int HIGH_BENZENE_LEVEL_EVENT_ID = 9;
    static const Event HIGH_CO2_LEVEL_EVENT;
    static const Event HIGH_NH3_LEVEL_EVENT;
    static const Event HIGH_BENZENE_LEVEL_EVENT;

    GasQualitySensor(int pin, EventHandler* eventHandler = nullptr);

    void scanGasLevels();
    float getCO2Level();
    float getNH3Level();
    float getBenzeneLevel();
    void setThresholds(float co2, float nh3, float benzene);
    void calibrate();
    void setR0(float r0Value);
    float getR0() const;
};

#endif //GAS_QUALITY_SENSOR_H
