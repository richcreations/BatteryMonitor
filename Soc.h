#ifndef SOC_H
#define SOC_H

// ===========================================================
// Soc.h — State of Charge / Health Tracking
// ===========================================================
//
// Provides:
//   - Initialization of SOC/SOH system
//   - EEPROM persistence (wear-leveled slots)
//   - OCV-based initialization when EEPROM unavailable
//   - Coulomb counting updates
//   - Rest & full charge detection
//   - Learned capacity adjustment
//   - State of Health (SoH) calculation and persistence
// ===========================================================

// Initialize State of Charge / Health system
// - Loads EEPROM data (learned capacity, last SOC, last SOH)
// - Initializes variables for coulomb counting
// - Falls back to OCV-based SOC if no EEPROM data
void setupSoc();

// Update State of Charge / Health
// - Computes SOC using coulomb counting
// - Applies temperature-compensated OCV lookup if needed
// - Updates remaining Ah and learned capacity
// - Updates State of Health (SoH) based on learned vs nominal capacity
// - Saves periodically to EEPROM with wear-leveling
void updateSoc();

#endif // SOC_H
