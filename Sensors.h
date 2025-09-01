#ifndef SENSORS_H
#define SENSORS_H

// ===========================================================
// Sensors.h — Interface for all sensor-related functionality
// ===========================================================
//
// Provides:
//   - Sensor setup (INA226 + DS18B20)
//   - Periodic sensor reads (raw → calibrated → smoothed)
//   - Energy tracking (Ah + Wh integration)
//   - Fault detection (voltage, current, temperature)
//   - Debug printing of all tiers (raw, calibrated, smoothed)
//
// Globals are declared in Globals.h and defined in Globals.cpp.
// ===========================================================

// Initialize all sensors (INA226 + DS18B20)
// - Sets up I²C, configures shunts
// - Starts DS18B20 conversions
void setupSensors();

// Perform one round of sensor updates
// - Reads INA226 volt/amp
// - Updates raw_, calibrated_, smooth_ variables
// - Handles non-blocking DS18B20 temp reads
// - Integrates Ah and Wh usage
// - Evaluates fault thresholds
void readSensors();

// Print debug info (only active if DEBUG_OUTPUT defined)
// - Shows raw, calibrated, smoothed values
// - Includes SoC %, remaining Ah, remaining Wh
// - Flags any faults detected
void debugPrint();

#endif // SENSORS_H