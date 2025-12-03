#include "GasQualitySensor.h"
#include <Arduino.h>

const Event GasQualitySensor::HIGH_CO2_LEVEL_EVENT = Event(GasQualitySensor::HIGH_CO2_LEVEL_EVENT_ID);
const Event GasQualitySensor::HIGH_NH4_LEVEL_EVENT = Event(GasQualitySensor::HIGH_NH4_LEVEL_EVENT_ID);
const Event GasQualitySensor::HIGH_ALCOHOL_LEVEL_EVENT = Event(GasQualitySensor::HIGH_ALCOHOL_LEVEL_EVENT_ID);

typedef struct {
    float slope;
    float intercept;
} GasCurve;

static const GasCurve CO2_CURVE = {-0.42, 1.60};
static const GasCurve NH4_CURVE = {-0.38, 1.58};
static const GasCurve ALCOHOL_CURVE = {-0.45, 1.70};

GasQualitySensor::GasQualitySensor(int pin, EventHandler* eventHandler)
    : Sensor(pin, eventHandler) currentCO2Level(0.0), currentNH4Level(0.0), currentAlcoholLevel(0.0) {
    pinMode(pin, INPUT);
}

float GasQualitySensor::calculateRs(int rawValue) {
    float vout = (rawValue / 1024.0) * 5.0;
    return ((5.0 - vout) / vout) * RL;
}

float GasQualitySensor::calculatePPM(float ratio, float slope, float intercept) {
    return pow(10, ((log10(ratio) - intercept) / slope));
}

void GasQualitySensor::scanGasLevels() {
    int rawValue = analogRead(pin);
    float Rs = calculateRs(rawValue);
    float ratio = Rs / R0;

    currentCO2Level = calculatePPM(ratio, CO2_CURVE.slope, CO2_CURVE.intercept);
    currentNH4Level = calculatePPM(ratio, NH4_CURVE.slope, NH4_CURVE.intercept);
    currentAlcoholLevel = calculatePPM(ratio, ALCOHOL_CURVE.slope, ALCOHOL_CURVE.intercept);

    // Avoid repeated events since at least one condition is met
    if (currentCO2Level > 1000.0) {
        on(HIGH_CO2_LEVEL_EVENT);
    } else if (currentCO2Level > 800.0) {
        on(HIGH_CO2_LEVEL_EVENT);
    } else if (currentCO2Level > 600.0) {
        on(HIGH_CO2_LEVEL_EVENT);
    }
}

float GasQualitySensor::getCO2Level() {
    return currentCO2Level;
}

float GasQualitySensor::getNH4Level() {
    return currentNH4Level;
}

float GasQualitySensor::getAlcoholLevel() {
    return currentAlcoholLevel;
}

void GasQualitySensor::calibrate() {
    Serial.println("Starting calibration... Ensure sensor is in clean air!");
    delay(30000);  // Wait 30 seconds

    float sum = 0;
    for (int i = 0; i < 100; i++) {
        int sensorValue = analogRead(pin);
        float vout = (sensorValue / 1024.0) * 5.0;
        float Rs = ((5.0 - vout) / vout) * RL;
        sum += Rs;
        delay(100);
    }

    R0 = sum / 100.0;
    Serial.print("Calibrated R0 value: ");
    Serial.println(R0, 2);
}

void GasQualitySensor::setR0(float r0Value) {
    R0 = r0Value;
}

float GasQualitySensor::getR0() const {
    return R0;
}