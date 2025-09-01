#include "Globals.h"
#include "Config.h"
#include <EEPROM.h>

// Forward declarations
float applyCalibration(float raw, float rawLow, float calLow, float rawHigh, float calHigh);

// =======================
// Setup sensors
// =======================
void setupSensors() {
#ifdef DEBUG_OUTPUT
  Serial.begin(115200);
  Serial.println("Debug output enabled");
#endif

  Wire.begin(I2C_SDA, I2C_SCL);

  if (!ina1.begin()) {
#ifdef DEBUG_OUTPUT
    Serial.println("INA226 #1 not connected!");
#endif
  }
  if (!ina2.begin()) {
#ifdef DEBUG_OUTPUT
    Serial.println("INA226 #2 not connected!");
#endif
  }

  int err1 = ina1.setMaxCurrentShunt(SHUNT1_MAX_AMPS, SHUNT1_OHMS);
#ifdef DEBUG_OUTPUT
  if (err1 != INA226_ERR_NONE) {
    Serial.print("INA226 #1 calibration error: ");
    Serial.println(err1);
  }
#endif

  int err2 = ina2.setMaxCurrentShunt(SHUNT2_MAX_AMPS, SHUNT2_OHMS);
#ifdef DEBUG_OUTPUT
  if (err2 != INA226_ERR_NONE) {
    Serial.print("INA226 #2 calibration error: ");
    Serial.println(err2);
  }
#endif

  sensors.begin();
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  lastTempRequest = millis();

  ra_batt1_voltage.clear();
  ra_batt1_current.clear();
  ra_batt1_temp_C.clear();
  ra_batt2_voltage.clear();
  ra_batt2_current.clear();
  ra_batt2_temp_C.clear();
}

// =======================
// Read sensors + update globals
// =======================
void readSensors() {
  // ----- INA226 -----
  raw_battery1_voltage = ina1.getBusVoltage();
  raw_battery1_current = ina1.getCurrent();
  raw_battery2_voltage = ina2.getBusVoltage();
  raw_battery2_current = ina2.getCurrent();
  raw_battery1_power   = raw_battery1_voltage * raw_battery1_current;
  raw_battery2_power   = raw_battery2_voltage * raw_battery2_current;

  // ----- DS18B20 non-blocking -----
  unsigned long now = millis();
  if (now - lastTempRequest >= tempConversionTime) {
    raw_battery1_temp_C = sensors.getTempC(sensor1);
    raw_battery2_temp_C = sensors.getTempC(sensor2);
    raw_battery1_temp_K = raw_battery1_temp_C + 273.15f;
    raw_battery2_temp_K = raw_battery2_temp_C + 273.15f;

    sensors.requestTemperatures();
    lastTempRequest = now;

    if (raw_battery1_temp_C == DEVICE_DISCONNECTED_C) ra_batt1_temp_C.clear();
    if (raw_battery2_temp_C == DEVICE_DISCONNECTED_C) ra_batt2_temp_C.clear();
  }

  // ----- Calibration -----
  calibrated_battery1_temp_C = raw_battery1_temp_C + BATT1_TEMP_OFFSET;
  calibrated_battery2_temp_C = raw_battery2_temp_C + BATT2_TEMP_OFFSET;
  calibrated_battery1_temp_K = calibrated_battery1_temp_C + 273.15f;
  calibrated_battery2_temp_K = calibrated_battery2_temp_C + 273.15f;

  calibrated_battery1_voltage = applyCalibration(raw_battery1_voltage, BATT1_V_RAW_LOW, BATT1_V_CAL_LOW, BATT1_V_RAW_HIGH, BATT1_V_CAL_HIGH);
  calibrated_battery1_current = applyCalibration(raw_battery1_current, BATT1_I_RAW_LOW, BATT1_I_CAL_LOW, BATT1_I_RAW_HIGH, BATT1_I_CAL_HIGH);
  calibrated_battery2_voltage = applyCalibration(raw_battery2_voltage, BATT2_V_RAW_LOW, BATT2_V_CAL_LOW, BATT2_V_RAW_HIGH, BATT2_V_CAL_HIGH);
  calibrated_battery2_current = applyCalibration(raw_battery2_current, BATT2_I_RAW_LOW, BATT2_I_CAL_LOW, BATT2_I_RAW_HIGH, BATT2_I_CAL_HIGH);
  calibrated_battery1_power   = calibrated_battery1_voltage * calibrated_battery1_current;
  calibrated_battery2_power   = calibrated_battery2_voltage * calibrated_battery2_current;

  // ----- RunningAverage smoothing -----
  ra_batt1_voltage.addValue(calibrated_battery1_voltage);
  ra_batt1_current.addValue(calibrated_battery1_current);
  if (calibrated_battery1_temp_C != DEVICE_DISCONNECTED_C) ra_batt1_temp_C.addValue(calibrated_battery1_temp_C);
  ra_batt2_voltage.addValue(calibrated_battery2_voltage);
  ra_batt2_current.addValue(calibrated_battery2_current);
  if (calibrated_battery2_temp_C != DEVICE_DISCONNECTED_C) ra_batt2_temp_C.addValue(calibrated_battery2_temp_C);

  smooth_battery1_voltage = ra_batt1_voltage.getAverage();
  smooth_battery1_current = ra_batt1_current.getAverage();
  smooth_battery1_temp_C  = ra_batt1_temp_C.getAverage();

  smooth_battery2_voltage = ra_batt2_voltage.getAverage();
  smooth_battery2_current = ra_batt2_current.getAverage();
  smooth_battery2_temp_C  = ra_batt2_temp_C.getAverage();

  smooth_battery1_power = smooth_battery1_voltage * smooth_battery1_current;
  smooth_battery2_power = smooth_battery2_voltage * smooth_battery2_current;
  smooth_battery1_temp_K = smooth_battery1_temp_C + 273.15f;
  smooth_battery2_temp_K = smooth_battery2_temp_C + 273.15f;

  // ----- Energy integration -----
  unsigned long nowMs = millis();
  float dtHours = (nowMs - lastLoopMillis) / 3600000.0f;
  lastLoopMillis = nowMs;
  battery1_remaining_Wh += -smooth_battery1_power * dtHours;
  battery2_remaining_Wh += -smooth_battery2_power * dtHours;
}

// =======================
// Simple calibration function
// =======================
float applyCalibration(float raw, float rawLow, float calLow, float rawHigh, float calHigh) {
  float slope = (calHigh - calLow) / (rawHigh - rawLow);
  float offset = calLow - slope * rawLow;
  return slope * raw + offset;
}

// =======================
// Debug printing
// =======================
void debugPrint() {
#ifdef DEBUG_OUTPUT
  // -------- RAW values --------
  Serial.print("B1 raw: "); Serial.print(raw_battery1_voltage); Serial.print(" V, ");
  Serial.print(raw_battery1_current); Serial.print(" A, ");
  Serial.print(raw_battery1_power);   Serial.print(" W, ");
  Serial.print(raw_battery1_temp_C);  Serial.print(" C, ");
  Serial.print(raw_battery1_temp_K);  Serial.println(" K");

  Serial.print("B2 raw: "); Serial.print(raw_battery2_voltage); Serial.print(" V, ");
  Serial.print(raw_battery2_current); Serial.print(" A, ");
  Serial.print(raw_battery2_power);   Serial.print(" W, ");
  Serial.print(raw_battery2_temp_C);  Serial.print(" C, ");
  Serial.print(raw_battery2_temp_K);  Serial.println(" K");

  // -------- CALIBRATED values --------
  Serial.print("B1 cal: "); Serial.print(calibrated_battery1_voltage); Serial.print(" V, ");
  Serial.print(calibrated_battery1_current); Serial.print(" A, ");
  Serial.print(calibrated_battery1_power);   Serial.print(" W, ");
  Serial.print(calibrated_battery1_temp_C);  Serial.print(" C, ");
  Serial.print(calibrated_battery1_temp_K);  Serial.println(" K");

  Serial.print("B2 cal: "); Serial.print(calibrated_battery2_voltage); Serial.print(" V, ");
  Serial.print(calibrated_battery2_current); Serial.print(" A, ");
  Serial.print(calibrated_battery2_power);   Serial.print(" W, ");
  Serial.print(calibrated_battery2_temp_C);  Serial.print(" C, ");
  Serial.print(calibrated_battery2_temp_K);  Serial.println(" K");

  // -------- SMOOTHED values --------
  Serial.print("B1 smooth: "); Serial.print(smooth_battery1_voltage); Serial.print(" V, ");
  Serial.print(smooth_battery1_current); Serial.print(" A, ");
  Serial.print(smooth_battery1_power);   Serial.print(" W, ");
  Serial.print(smooth_battery1_temp_C);  Serial.print(" C, ");
  Serial.print(smooth_battery1_temp_K);  Serial.println(" K");

  Serial.print("B2 smooth: "); Serial.print(smooth_battery2_voltage); Serial.print(" V, ");
  Serial.print(smooth_battery2_current); Serial.print(" A, ");
  Serial.print(smooth_battery2_power);   Serial.print(" W, ");
  Serial.print(smooth_battery2_temp_C);  Serial.print(" C, ");
  Serial.print(smooth_battery2_temp_K);  Serial.println(" K");

  // -------- SOC & Capacity --------
  Serial.print("SOC1: "); Serial.print(soc_battery1_percent); Serial.print("%, ");
  Serial.print("SOH1: "); Serial.print(soh_battery1_percent); Serial.print("%, ");
  Serial.print("Rem1: "); Serial.print(battery1_remaining_Ah); Serial.print(" Ah, ");
  Serial.print(battery1_remaining_Wh); Serial.println(" Wh");

  Serial.print("SOC2: "); Serial.print(soc_battery2_percent); Serial.print("%, ");
  Serial.print("SOH2: "); Serial.print(soh_battery2_percent); Serial.print("%, ");
  Serial.print("Rem2: "); Serial.print(battery2_remaining_Ah); Serial.print(" Ah, ");
  Serial.print(battery2_remaining_Wh); Serial.println(" Wh");

  // -------- Status flags --------
  Serial.print("B1 Rest: "); Serial.print(batt1_isResting ? "YES" : "NO");
  Serial.print(", Full: "); Serial.println(batt1_isFull ? "YES" : "NO");

  Serial.print("B2 Rest: "); Serial.print(batt2_isResting ? "YES" : "NO");
  Serial.print(", Full: "); Serial.println(batt2_isFull ? "YES" : "NO");

  Serial.println();
#endif
}
