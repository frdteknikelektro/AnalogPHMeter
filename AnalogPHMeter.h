#ifndef __ANALOG_PH_METER_H__
#define __ANALOG_PH_METER_H__

struct PHCalibrationValue {
  char point;
  float value[2];
  int adc[2];
  float slope;
  int adcOffset;
};

class AnalogPHMeter {
 private:
  struct PHCalibrationValue calibrationValue;

  unsigned int pin;
  float pH;
  float temperature;
  bool debug;

  bool stable;
  unsigned char stableCount;
  float precision;
  unsigned char index;
  float valueBefore, deltaValue, sumOfDeltaValue;
  float deltaValueBuffer[10];

  int readADC(int oversampling = 64);
  void inputValue(float value);

 public:
  AnalogPHMeter(unsigned int pin);

  AnalogPHMeter &initialize(struct PHCalibrationValue = (struct PHCalibrationValue){});

  AnalogPHMeter &singleReading(void);
  AnalogPHMeter &temperatureCompensation(float temperature);
  AnalogPHMeter &calibration(void);
  AnalogPHMeter &calibrationClear(void);
  AnalogPHMeter &calibrationMid(float mid);
  AnalogPHMeter &calibrationLow(float low);
  AnalogPHMeter &calibrationHigh(float high);
  AnalogPHMeter &factoryReset(void);

  float getpH(void) { return this->pH; };
  float getTemperature(void) { return this->temperature; };
  float getCalibrationPoint(void) { return this->calibrationValue.point; };
  struct PHCalibrationValue getCalibrationValue(void) {
    return calibrationValue;
  };

  bool ispHStable(void) { return this->stable; };
  void setpHPrecision(float precision) { this->precision = precision; };
};

#endif
