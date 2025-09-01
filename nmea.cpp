#include "Nmea.h"
#include "Globals.h"
#include "Config.h"
#include <N2kMessages.h>

// ===========================================================
// Global NMEA2000 instance (ESP32 CAN)
// ===========================================================
tNMEA2000_esp32 NMEA2000(CAN_RX_PIN, CAN_TX_PIN);

// ===========================================================
// Timers for rate control
// ===========================================================
static unsigned long last508 = 0;
static unsigned long last506 = 0;
static unsigned long last513 = 0;

// ===========================================================
// Setup
// ===========================================================
void setupNmea() {
  NMEA2000.SetProductInformation("12345678",  // Serial code
                                 100,         // Product code
                                 "Battery Monitor",  // Model ID
                                 "1.1.0.0",         // Software version
                                 "1.0.0.0");        // Hardware version

  NMEA2000.SetDeviceInformation(1,   // Unique number
                                140, // Device function = Battery monitor
                                85,  // Device class = Electrical Generation
                                2046); // Manufacturer code (demo)

  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode);
  NMEA2000.EnableForward(false);
  NMEA2000.Open();
}

// ===========================================================
// PGN 127508 — Battery Status
// ===========================================================
void sendNmeaBatteryStatus(uint8_t instance) {
  tN2kMsg N2kMsg;

  if (instance == 0) {
    SetN2kPGN127508(N2kMsg, instance,
                    smooth_battery1_voltage,
                    smooth_battery1_current,
                    smooth_battery1_temp_K,
                    soc_battery1_percent);
  } else {
    SetN2kPGN127508(N2kMsg, instance,
                    smooth_battery2_voltage,
                    smooth_battery2_current,
                    smooth_battery2_temp_K,
                    soc_battery2_percent);
  }

  NMEA2000.SendMsg(N2kMsg);
}

// ===========================================================
// PGN 127506 — DC Detailed Status
// ===========================================================
void sendNmeaDcStatus(uint8_t instance) {
  tN2kMsg N2kMsg;

  if (instance == 0) {
    SetN2kPGN127506(N2kMsg,
                    0,                 // SID
                    instance,          // DCInstance
                    N2kDCt_Battery,    // DC Type
                    soc_battery1_percent,  // SoC
                    soh_battery1_percent,  // SoH
                    smooth_battery1_voltage,
                    smooth_battery1_current,
                    N2kDoubleNA);      // Ripple (not reported)
  } else {
    SetN2kPGN127506(N2kMsg,
                    0,
                    instance,
                    N2kDCt_Battery,
                    soc_battery2_percent,
                    soh_battery2_percent,
                    smooth_battery2_voltage,
                    smooth_battery2_current,
                    N2kDoubleNA);
  }

  NMEA2000.SendMsg(N2kMsg);
}

// ===========================================================
// PGN 127513 — Battery Configuration
// ===========================================================
void sendNmeaBatteryConfig(uint8_t instance) {
  tN2kMsg N2kMsg;

  int chem = (instance == 0) ? BATT1_CHEMISTRY : BATT2_CHEMISTRY;

  tN2kBatType batType;
  tN2kBatChem batChem;
  switch (chem) {
    case CHEM_FLA: batType = (tN2kBatType)1; batChem = (tN2kBatChem)1; break; // Flooded
    case CHEM_GEL: batType = (tN2kBatType)1; batChem = (tN2kBatChem)2; break; // Gel
    case CHEM_AGM: batType = (tN2kBatType)1; batChem = (tN2kBatChem)3; break; // AGM
    case CHEM_LFP: batType = (tN2kBatType)5; batChem = (tN2kBatChem)5; break; // LiIon
    default:       batType = (tN2kBatType)0; batChem = (tN2kBatChem)0; break; // Unknown
  }

  double capacityAh = (instance == 0) ? BATT1_CAPACITY_AH : BATT2_CAPACITY_AH;

  tN2kBatNomVolt nominalVolt = ( (instance==0) ? isBatt1_24V() : isBatt2_24V() )
                                 ? (tN2kBatNomVolt)3   // 24V
                                 : (tN2kBatNomVolt)2;  // 12V

  tN2kBatEqSupport eqSupport = (tN2kBatEqSupport)0; // No equalization

  double peukertExp = (instance == 0) ? BATT1_PEUKERT_EXP : BATT2_PEUKERT_EXP;
  double chargeEff  = (instance == 0) ? BATT1_CHARGE_EFF   : BATT2_CHARGE_EFF;

  SetN2kPGN127513(N2kMsg,
                  instance,
                  batType,
                  eqSupport,
                  nominalVolt,
                  batChem,
                  capacityAh,
                  peukertExp,
                  chargeEff,
                  N2kInt8NA);  // Temp coefficient (not used)

  NMEA2000.SendMsg(N2kMsg);
}

// ===========================================================
// Periodic dispatcher (call from loop)
// ===========================================================
void nmeaLoop() {
  unsigned long now = millis();

  // Battery Status 127508 at 1 Hz
  if (now - last508 >= 1000) {
    sendNmeaBatteryStatus(0);
    sendNmeaBatteryStatus(1);
    last508 = now;
  }

  // DC Status 127506 at 5s
  if (now - last506 >= 5000) {
    sendNmeaDcStatus(0);
    sendNmeaDcStatus(1);
    last506 = now;
  }

  // Battery Config 127513 at 60s
  if (now - last513 >= 60000) {
    sendNmeaBatteryConfig(0);
    sendNmeaBatteryConfig(1);
    last513 = now;
  }

  // Let NMEA2000 library handle bus tasks
  NMEA2000.ParseMessages();
}
