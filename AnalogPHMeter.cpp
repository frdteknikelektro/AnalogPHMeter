#include <Arduino.h>
#include "AnalogPHMeter.h"

int AnalogPHMeter::readADC(int oversampling) {
  long pHADCTotal = 0;

  for (int i = 0; i < oversampling; i++) {
    pHADCTotal += analogRead(pin);
  }

  return (int)(pHADCTotal / oversampling);
}

void AnalogPHMeter::inputValue(float value) {
  sumOfDeltaValue -= deltaValueBuffer[index];
  deltaValueBuffer[index] = value - valueBefore;
  sumOfDeltaValue += deltaValueBuffer[index];
  valueBefore = value;

  if (++index == 10) index = 0;

  if (-precision < sumOfDeltaValue && sumOfDeltaValue < precision)
    stableCount++;
  else
    stableCount = 0;

  stable = (stableCount > 10);
  if (stable) stableCount = 10;
}

AnalogPHMeter::AnalogPHMeter(unsigned int pin, float defaultSlope, int defaultAdcAt7) {
  this->pin = pin;
  this->pH = 0.00f;

  this->stable = false;
  this->stableCount = 0;
  this->index = 0;
  this->valueBefore = 0.00f;
  for (size_t i = 0; i < sizeof(deltaValueBuffer) / sizeof(float); i++)
    this->deltaValueBuffer[i] = 0.00f;
  this->sumOfDeltaValue = 0.00f;
  this->deltaValue = 0.00f;
  this->precision = 0.050f;

  this->defaultSlope = defaultSlope;
  this->defaultAdcAt7 = defaultAdcAt7;
}

AnalogPHMeter &AnalogPHMeter::initialize(struct PHCalibrationValue calibrationValue) {
  this->calibrationValue = calibrationValue;
  if (this->calibrationValue.point < '0' || '2' < this->calibrationValue.point ||
      this->calibrationValue.adc[0] == 0 || this->calibrationValue.adc[1] == 0) {
    this->calibrationClear();
  }

  return *this;
}

AnalogPHMeter &AnalogPHMeter::singleReading(void) {
  pH = calibrationValue.slope * (readADC() - calibrationValue.adcOffset);
  inputValue(pH);

  return *this;
}

AnalogPHMeter &AnalogPHMeter::temperatureCompensation(float temperature) {
  this->temperature = temperature;

  return *this;
}

AnalogPHMeter &AnalogPHMeter::calibration(void) {
  return *this;
}

AnalogPHMeter &AnalogPHMeter::calibrationClear(void) {
  calibrationValue.point = '0';
  calibrationValue.value[0] = 7.000f;
  calibrationValue.adc[0] = defaultAdcAt7;
  calibrationValue.value[1] = 4.000f;
  calibrationValue.adc[1] = (int) (defaultAdcAt7 + ((calibrationValue.value[1] - calibrationValue.value[0]) / defaultSlope));
  calibrationValue.slope = defaultSlope;
  calibrationValue.adcOffset = calibrationValue.adc[0] - (int)(calibrationValue.value[0] / calibrationValue.slope);

  return *this;
}

AnalogPHMeter &AnalogPHMeter::calibrationMid(float mid) {
  calibrationValue.point = '1';
  calibrationValue.value[0] = mid;
  calibrationValue.adc[0] = readADC();
  calibrationValue.value[1] = 4.000f;
  calibrationValue.adc[1] = (int) (calibrationValue.adc[0] + ((calibrationValue.value[1] - calibrationValue.value[0]) / defaultSlope));
  calibrationValue.slope = (calibrationValue.value[1] - calibrationValue.value[0]) /
                           (calibrationValue.adc[1] - calibrationValue.adc[0]);
  calibrationValue.adcOffset = calibrationValue.adc[0] - (int)(calibrationValue.value[0] / calibrationValue.slope);

  return *this;
}

AnalogPHMeter &AnalogPHMeter::calibrationLow(float low) {
  calibrationValue.point = '2';
  calibrationValue.value[1] = low;
  calibrationValue.adc[1] = readADC();
  calibrationValue.slope = (calibrationValue.value[1] - calibrationValue.value[0]) /
                           (calibrationValue.adc[1] - calibrationValue.adc[0]);
  calibrationValue.adcOffset = calibrationValue.adc[0] - (int)(calibrationValue.value[0] / calibrationValue.slope);

  return *this;
}

AnalogPHMeter &AnalogPHMeter::calibrationHigh(float high) {
  if (calibrationValue.point == '1') {
    calibrationValue.value[1] = calibrationValue.value[0];
    calibrationValue.adc[1] = calibrationValue.adc[0];
  }
  calibrationValue.point = '2';
  calibrationValue.value[0] = high;
  calibrationValue.adc[0] = readADC();
  calibrationValue.slope = (calibrationValue.value[1] - calibrationValue.value[0]) /
                           (calibrationValue.adc[1] - calibrationValue.adc[0]);
  calibrationValue.adcOffset = calibrationValue.adc[0] - (int)(calibrationValue.value[0] / calibrationValue.slope);

  return *this;
}

AnalogPHMeter &AnalogPHMeter::factoryReset(void) {
  calibrationClear();

  return *this;
}
