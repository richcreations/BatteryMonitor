#ifndef GLOBALS_H
#define GLOBALS_H

#include <Wire.h>
#include "INA226.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RunningAverage.h>

// ========== Extern Global Variables ==========

// Raw sensor variables
extern float raw_battery1_voltage;
extern float raw_battery1_current;
extern float raw_battery1_power;
extern float raw_battery2_voltage;
extern float raw_battery2_current;
extern float raw_battery2_power;
extern float raw_battery1_temp_C;
extern float raw_battery2_temp_C;
extern float raw_battery1_temp_K;
extern float raw_battery2_temp_K;

// Calibrated sensor variables
extern float calibrated_battery1_voltage;
extern float calibrated_battery1_current;
extern float calibrated_battery1_power;
extern float calibrated_battery2_voltage;
extern float calibrated_battery2_current;
extern float calibrated_battery2_power;
extern float calibrated_battery1_temp_C;
extern float calibrated_battery2_temp_C;
extern float calibrated_battery1_temp_K;
extern float calibrated_battery2_temp_K;

// Smoothed sensor variables
extern float smooth_battery1_voltage;
extern float smooth_battery1_current;
extern float smooth_battery1_power;
extern float smooth_battery2_voltage;
extern float smooth_battery2_current;
extern float smooth_battery2_power;
extern float smooth_battery1_temp_C;
extern float smooth_battery2_temp_C;
extern float smooth_battery1_temp_K;
extern float smooth_battery2_temp_K;

// SOC / SOH / Capacity tracking
extern float soc_battery1_percent;
extern float soc_battery2_percent;
extern float soh_battery1_percent;
extern float soh_battery2_percent;
extern float battery1_remaining_Ah;
extern float battery2_remaining_Ah;
extern float battery1_remaining_Wh;
extern float battery2_remaining_Wh;
extern float battery1_learned_capacity_Ah;
extern float battery2_learned_capacity_Ah;

// Rest detection state
extern bool batt1_isResting;
extern bool batt2_isResting;
extern unsigned long batt1_restStartMs;
extern unsigned long batt2_restStartMs;
extern float batt1_lastRestVoltage;
extern float batt2_lastRestVoltage;

// Full charge detection state
extern bool batt1_isFull;
extern bool batt2_isFull;
extern unsigned long batt1_fullStartMs;
extern unsigned long batt2_fullStartMs;

// Last full markers for learning
extern float batt1_socAtLastFull;
extern float batt2_socAtLastFull;
extern float batt1_AhAtLastFull;
extern float batt2_AhAtLastFull;

// EEPROM state tracking
extern float eeprom_soc_b1;
extern float eeprom_soc_b2;
extern bool haveEepromSoc;
extern bool needSocInitFromOCV;
extern unsigned long lastEepromSaveMillis;

// INA226 instances
extern INA226 ina1;
extern INA226 ina2;

// OneWire/DallasTemperature
extern OneWire oneWire;
extern DallasTemperature sensors;
extern DeviceAddress sensor1;
extern DeviceAddress sensor2;

// RunningAverage instances
extern RunningAverage ra_batt1_voltage;
extern RunningAverage ra_batt1_current;
extern RunningAverage ra_batt1_temp_C;
extern RunningAverage ra_batt2_voltage;
extern RunningAverage ra_batt2_current;
extern RunningAverage ra_batt2_temp_C;

// Timing
extern unsigned long lastTempRequest;
extern const unsigned long tempConversionTime;
extern unsigned long lastLoopMillis;

// ===== System Voltage Helpers =====
// These report true if the respective battery is configured as 24V.
bool isBatt1_24V();
bool isBatt2_24V();

// ===== Optional battery model parameters (from Config or defaults) =====
extern float batt1_peukert_exponent;
extern float batt2_peukert_exponent;
extern float batt1_charge_efficiency;
extern float batt2_charge_efficiency;

#endif // GLOBALS_H
