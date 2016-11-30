#include <EEPROM.h>
#include <AnalogPHMeter.h>

AnalogPHMeter pHSensor(A0);

void setup(void) {
  Serial.begin(9600);
  pHSensor.initialize();

  Serial.println("Analog PH Meter Calibration");
  Serial.println("The value will be saved on EEPROM");
  Serial.println("Procedure:");
  Serial.println(" - Put pH on pH 7 solution");
  Serial.println(" - Wait until stable");
  Serial.println(" - Press 'm' to calibrate");
  Serial.println(" - Same process for pH 4 and/or pH 10,");
  Serial.println("   except press 'l' for pH 4, and 'h' for pH 10");
  Serial.println();
  Serial.println("*to clear calibration value press 'c'");
  delay(3000);
}

void loop(void) {
  static unsigned long t = millis();
  if (millis() - t > 1000) {
    Serial.println(pHSensor.singleReading().getpH());
    t = millis();
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'm') {
      pHSensor.calibrationMid(7.000f);
    } else if (c == 'l') {
      pHSensor.calibrationLow(4.000f);
    } else if (c == 'h') {
      pHSensor.calibrationHigh(10.000f);
    } else if (c == 'c') {
      pHSensor.calibrationClear();
    }
  }
}
