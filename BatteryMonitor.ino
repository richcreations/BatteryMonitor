#include "Config.h"
#include "Globals.h"
#include "Sensors.h"
#include "Soc.h"

#include <NMEA2000_esp32.h>   // ESP32 built-in CAN controller
#include "Nmea.h"

void setup() {
  setupSensors();  // Initialize INA226 + DS18B20
  setupSoc();      // Initialize SoC tracking (EEPROM + OCV fallback)
  setupNmea();     // Initialize NMEA2000
}

void loop() {
  readSensors();   // Read sensors, update raw/calibrated/smoothed globals
  updateSoc();     // Update SoC and remaining capacity
  nmeaLoop();      // Handle NMEA2000 messages

#ifdef DEBUG_OUTPUT
  debugPrint();    // Print debug values if enabled
#endif
}
