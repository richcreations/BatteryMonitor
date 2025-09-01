#include "Globals.h"
#include "Config.h"

// ========== Definitions of Global Variables ==========

// Raw sensor variables
float raw_battery1_voltage = 0.0;
float raw_battery1_current = 0.0;
float raw_battery1_power   = 0.0;
float raw_battery2_voltage = 0.0;
float raw_battery2_current = 0.0;
float raw_battery2_power   = 0.0;
float raw_battery1_temp_C  = 0.0;
float raw_battery2_temp_C  = 0.0;
float raw_battery1_temp_K  = 0.0;
float raw_battery2_temp_K  = 0.0;

// Calibrated sensor variables
float calibrated_battery1_voltage = 0.0;
float calibrated_battery1_current = 0.0;
float calibrated_battery1_power   = 0.0;
float calibrated_battery2_voltage = 0.0;
float calibrated_battery2_current = 0.0;
float calibrated_battery2_power   = 0.0;
float calibrated_battery1_temp_C  = 0.0;
float calibrated_battery2_temp_C  = 0.0;
float calibrated_battery1_temp_K  = 0.0;
float calibrated_battery2_temp_K  = 0.0;

// Smoothed sensor variables
float smooth_battery1_voltage = 0.0;
float smooth_battery1_current = 0.0;
float smooth_battery1_power   = 0.0;
float smooth_battery2_voltage = 0.0;
float smooth_battery2_current = 0.0;
float smooth_battery2_power   = 0.0;
float smooth_battery1_temp_C  = 0.0;
float smooth_battery2_temp_C  = 0.0;
float smooth_battery1_temp_K  = 0.0;
float smooth_battery2_temp_K  = 0.0;

// SOC / SOH / Capacity tracking
float soc_battery1_percent = 0.0;
float soc_battery2_percent = 0.0;
float soh_battery1_percent = 100.0;
float soh_battery2_percent = 100.0;
float battery1_remaining_Ah = BATT1_CAPACITY_AH;
float battery2_remaining_Ah = BATT2_CAPACITY_AH;
float battery1_remaining_Wh = 0.0;
float battery2_remaining_Wh = 0.0;
float battery1_learned_capacity_Ah = BATT1_CAPACITY_AH;
float battery2_learned_capacity_Ah = BATT2_CAPACITY_AH;

// Rest detection state
bool batt1_isResting = false;
bool batt2_isResting = false;
unsigned long batt1_restStartMs = 0;
unsigned long batt2_restStartMs = 0;
float batt1_lastRestVoltage = 0.0;
float batt2_lastRestVoltage = 0.0;

// Full charge detection state
bool batt1_isFull = false;
bool batt2_isFull = false;
unsigned long batt1_fullStartMs = 0;
unsigned long batt2_fullStartMs = 0;

// Last full markers for learning
float batt1_socAtLastFull = 100.0f;
float batt2_socAtLastFull = 100.0f;
float batt1_AhAtLastFull  = BATT1_CAPACITY_AH;
float batt2_AhAtLastFull  = BATT2_CAPACITY_AH;

// EEPROM state tracking
float eeprom_soc_b1 = 0.0;
float eeprom_soc_b2 = 0.0;
bool haveEepromSoc = false;
bool needSocInitFromOCV = true;
unsigned long lastEepromSaveMillis = 0;

// INA226 instances
INA226 ina1(INA226_ADDR1);
INA226 ina2(INA226_ADDR2);

// OneWire/DallasTemperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Hardcoded DS18B20 addresses (from Config.h)
DeviceAddress sensor1 = DS18B20_ADDR1;
DeviceAddress sensor2 = DS18B20_ADDR2;

// RunningAverage instances
RunningAverage ra_batt1_voltage(SMOOTHING_SAMPLES);
RunningAverage ra_batt1_current(SMOOTHING_SAMPLES);
RunningAverage ra_batt1_temp_C(SMOOTHING_SAMPLES);
RunningAverage ra_batt2_voltage(SMOOTHING_SAMPLES);
RunningAverage ra_batt2_current(SMOOTHING_SAMPLES);
RunningAverage ra_batt2_temp_C(SMOOTHING_SAMPLES);

// Timing
unsigned long lastTempRequest = 0;
const unsigned long tempConversionTime = 750; // ms at 12-bit resolution
unsigned long lastLoopMillis = 0;

// ===== System Voltage Helpers =====
bool isBatt1_24V() {
#ifdef BATT1_SYSTEM_VOLTAGE_24V
  return true;
#else
  return false;
#endif
}

bool isBatt2_24V() {
#ifdef BATT2_SYSTEM_VOLTAGE_24V
  return true;
#else
  return false;
#endif
}

// ===== Optional battery model parameters =====
#ifdef BATT1_PEUKERT_EXPONENT
float batt1_peukert_exponent = BATT1_PEUKERT_EXPONENT;
#else
float batt1_peukert_exponent = 1.0f;
#endif

#ifdef BATT2_PEUKERT_EXPONENT
float batt2_peukert_exponent = BATT2_PEUKERT_EXPONENT;
#else
float batt2_peukert_exponent = 1.0f;
#endif

#ifdef BATT1_CHARGE_EFFICIENCY
float batt1_charge_efficiency = BATT1_CHARGE_EFFICIENCY;
#else
float batt1_charge_efficiency = 1.0f;
#endif

#ifdef BATT2_CHARGE_EFFICIENCY
float batt2_charge_efficiency = BATT2_CHARGE_EFFICIENCY;
#else
float batt2_charge_efficiency = 1.0f;
#endif
