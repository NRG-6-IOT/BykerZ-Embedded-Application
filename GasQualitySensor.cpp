#include "GasQualitySensor.h"
#include <Arduino.h>

const Event GasQualitySensor::HIGH_CO2_LEVEL_EVENT = Event(GasQualitySensor::HIGH_CO2_LEVEL_EVENT_ID);
const Event GasQualitySensor::HIGH_NH3_LEVEL_EVENT = Event(GasQualitySensor::HIGH_NH3_LEVEL_EVENT_ID);
const Event GasQualitySensor::HIGH_BENZENE_LEVEL_EVENT = Event(GasQualitySensor::HIGH_BENZENE_LEVEL_EVENT_ID);

const GasQualitySensor::GasCurve GasQualitySensor::CO2_CURVE = {116.6020682, -2.769034857};
const GasQualitySensor::GasCurve GasQualitySensor::NH3_CURVE = {102.2, -2.473};
const GasQualitySensor::GasCurve GasQualitySensor::BENZENE_CURVE = {44.947, -3.445};

GasQualitySensor::GasQualitySensor(int pin, EventHandler* eventHandler)
    : Sensor(pin, eventHandler), currentCO2Level(0.0), currentNH3Level(0.0), currentBenzeneLevel(0.0) {
    pinMode(pin, INPUT);
}

float GasQualitySensor::getRs(int rawADC) {
    float vOut = rawADC * (3.3 / 4095.0);  // ESP32 ADC: 12-bit, 3.3V
    return (3.3 - vOut) * rLoad / vOut;
}

float GasQualitySensor::estimatePPM(float rs, const GasCurve& curve) {
    float ratio = rs / R0;
    return curve.a * pow(ratio, curve.b);
}

void GasQualitySensor::scanGasLevels() {
    int rawADC = analogRead(pin);
    float rs = getRs(rawADC);

    currentCO2Level = estimatePPM(rs, CO2_CURVE);
    currentNH3Level = estimatePPM(rs, NH3_CURVE);
    currentBenzeneLevel = estimatePPM(rs, BENZENE_CURVE);

    if (currentCO2Level > co2Threshold) {
        on(HIGH_CO2_LEVEL_EVENT);
    }

    if (currentNH3Level > nh3Threshold) {
        on(HIGH_NH3_LEVEL_EVENT);
    }

    if (currentBenzeneLevel > benzeneThreshold) {
        on(HIGH_BENZENE_LEVEL_EVENT);
    }
}

float GasQualitySensor::getCO2Level() {
    int rawADC = analogRead(pin);
    float rs = getRs(rawADC);
    currentCO2Level = estimatePPM(rs, CO2_CURVE);
    return currentCO2Level;
}

float GasQualitySensor::getNH3Level() {
    int rawADC = analogRead(pin);
    float rs = getRs(rawADC);
    currentNH3Level = estimatePPM(rs, NH3_CURVE);
    return currentNH3Level;
}

float GasQualitySensor::getBenzeneLevel() {
    int rawADC = analogRead(pin);
    float rs = getRs(rawADC);
    currentBenzeneLevel = estimatePPM(rs, BENZENE_CURVE);
    return currentBenzeneLevel;
}

void GasQualitySensor::setThresholds(float co2, float nh3, float benzene) {
    co2Threshold = co2;
    nh3Threshold = nh3;
    benzeneThreshold = benzene;
}

void GasQualitySensor::calibrate() {
    Serial.println("Starting MQ-135 calibration... Ensure sensor is in clean air!");
    delay(30000);  // Wait 30 seconds for stabilization

    float sum = 0;
    for (int i = 0; i < 100; i++) {
        int rawADC = analogRead(pin);
        float rs = getRs(rawADC);
        sum += rs;
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
