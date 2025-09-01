/*
===========================================================
 Battery Monitor — Configuration Instructions
===========================================================

This file controls all compile-time configuration options.

-----------------------------------------------------------
CONFIGURATION STEPS
-----------------------------------------------------------

1. Debug Output
   - To enable serial debug printing, uncomment:
       #define DEBUG_OUTPUT
   - Leave it commented out for silent operation.

2. System Voltage (per battery)
   - Select ONE option per battery:
       #define BATT1_SYSTEM_VOLTAGE_12V
       // #define BATT1_SYSTEM_VOLTAGE_24V
       #define BATT2_SYSTEM_VOLTAGE_12V
       // #define BATT2_SYSTEM_VOLTAGE_24V

3. Battery Chemistry
   - Choose per battery: CHEM_FLA, CHEM_AGM, CHEM_GEL, CHEM_LFP
       #define BATT1_CHEMISTRY CHEM_FLA
       #define BATT2_CHEMISTRY CHEM_LFP

4. Battery Capacity (Ah)
   - Set the nominal new capacity of each battery:
       #define BATT1_CAPACITY_AH 100.0
       #define BATT2_CAPACITY_AH 100.0

5. Peukert Exponent (per battery)
   - Dimensionless number describing rate-capacity effect.
   - Typical: 1.05–1.3 for lead-acid, ~1.05 for LiFePO4.
       #define BATT1_PEUKERT_EXP   1.10
       #define BATT2_PEUKERT_EXP   1.05

6. Charge Efficiency (per battery)
   - Fraction of charging energy retained (0.0–1.0).
   - Used in NMEA2000 PGN 127513.
       #define BATT1_CHARGE_EFF    0.95
       #define BATT2_CHARGE_EFF    0.96

7. Capacity Learning / EEPROM Persistence
   - Adjust learning rate, slot count, and save interval:
       #define CAPACITY_LEARNING_ALPHA   0.05
       #define EEPROM_NUM_SLOTS          16
       #define EEPROM_BASE_ADDR          0
       #define EEPROM_SAVE_INTERVAL_MS   60000
       #define SOC_RESUME_TOLERANCE      10.0

8. Shunt Resistors
   - Define resistance (Ω) and max current (A).
   - Example: 200A / 50mV shunt = 0.00025 Ω
       #define SHUNT1_OHMS 0.00025
       #define SHUNT1_MAX_AMPS 200.0
       #define SHUNT2_OHMS 0.00025
       #define SHUNT2_MAX_AMPS 200.0

9. Calibration
   - Use 2-point calibration for Voltage/Current per battery.
   - Example:
       #define BATT1_V_RAW_LOW 10.0
       #define BATT1_V_CAL_LOW 10.2
       #define BATT1_V_RAW_HIGH 15.0
       #define BATT1_V_CAL_HIGH 15.1
   - Apply simple °C offsets for temp sensors.

10. Temperature Compensation
   - Coefficients in V/°C used to adjust OCV-based SoC.
       #define BATT1_TEMP_COEF -0.030
       #define BATT2_TEMP_COEF  0.0

11. Rest Detection (per battery)
   - A battery is considered "at rest" when current is below a
     threshold and voltage remains stable within a window for
     a minimum hold time.
       #define BATT1_REST_I_THRESHOLD_A  2.0
       #define BATT1_REST_V_STABILITY_MV 20
       #define BATT1_REST_HOLD_TIME_S    1800
       #define BATT2_REST_I_THRESHOLD_A  2.0
       #define BATT2_REST_V_STABILITY_MV 20
       #define BATT2_REST_HOLD_TIME_S    1800

12. Full Charge Detection (per battery)
   - A battery is considered "full" when voltage is at or above
     absorb setpoint AND current falls below a tail threshold
     for a minimum hold time. Voltages are 12V-based references
     and are doubled automatically for 24V systems.
       #define BATT1_FULL_V_ABSORB_V   14.4
       #define BATT1_FULL_I_TAIL_A     4.0
       #define BATT1_FULL_HOLD_TIME_S  900
       #define BATT2_FULL_V_ABSORB_V   13.6
       #define BATT2_FULL_I_TAIL_A     2.0
       #define BATT2_FULL_HOLD_TIME_S  600

13. Capacity Learning Guardrails
   - Learning only occurs when discharge since last full cycle
     exceeds a minimum depth, and learned capacity is clamped
     within a safe range relative to nominal.
       #define LEARN_MIN_DELTA_SOC_PCT    20.0
       #define LEARN_CAPACITY_MIN_FACTOR  0.5f
       #define LEARN_CAPACITY_MAX_FACTOR  2.0f

14. Fault Detection Thresholds
   - Defines safe operating limits. Exceeding these can set
     fault flags or trigger alarms. Voltage thresholds are
     defined per 12V reference; they are doubled automatically
     if the system is configured for 24V.
       #define BATT1_VOLT_MIN_12V   10.5
       #define BATT1_VOLT_MAX_12V   15.0
       #define BATT1_CURR_MAX_A     180.0
       #define BATT1_TEMP_MAX_C     60.0
       #define BATT2_VOLT_MIN_12V   11.0
       #define BATT2_VOLT_MAX_12V   14.6
       #define BATT2_CURR_MAX_A     150.0
       #define BATT2_TEMP_MAX_C     55.0

15. Smoothing
   - Number of samples used in RunningAverage:
       #define SMOOTHING_SAMPLES 10

16. I²C / INA226 Settings
   - Pins and addresses:
       #define I2C_SDA 16
       #define I2C_SCL 17
       #define INA226_ADDR1 0x40
       #define INA226_ADDR2 0x41

17. CAN bus (NMEA2000) Settings
   - ESP32 GPIO pins for CAN RX/TX:
       #define CAN_RX_PIN GPIO_NUM_34
       #define CAN_TX_PIN GPIO_NUM_32

18. DS18B20 Settings
   - OneWire pin and sensor ROM addresses:
       #define ONE_WIRE_BUS 4
       #define DS18B20_ADDR1 { 0x28, 0xFF, 0x1C, 0x97, 0x91, 0x16, 0x04, 0x2C }
       #define DS18B20_ADDR2 { 0x28, 0xFF, 0x8A, 0x62, 0x92, 0x16, 0x05, 0x7B }

===========================================================
*/

// ====== Configuration Defines ======

// ----- Enable debug serial output -----
// #define DEBUG_OUTPUT

// ===== Chemistry IDs =====
#define CHEM_FLA  0
#define CHEM_AGM  1
#define CHEM_GEL  2
#define CHEM_LFP  3

// ===== System Voltage =====
#define BATT1_SYSTEM_VOLTAGE_12V
// #define BATT1_SYSTEM_VOLTAGE_24V
#define BATT2_SYSTEM_VOLTAGE_12V
// #define BATT2_SYSTEM_VOLTAGE_24V

// ===== Battery Chemistry =====
#define BATT1_CHEMISTRY CHEM_FLA
#define BATT2_CHEMISTRY CHEM_LFP

// ===== Battery Capacities (Ah) =====
#define BATT1_CAPACITY_AH 100.0
#define BATT2_CAPACITY_AH 100.0

// ===== Peukert exponent =====
#define BATT1_PEUKERT_EXP   1.10
#define BATT2_PEUKERT_EXP   1.05

// ===== Charge efficiency =====
#define BATT1_CHARGE_EFF    0.95
#define BATT2_CHARGE_EFF    0.96

// ===== Capacity learning & persistence =====
#define CAPACITY_LEARNING_ALPHA   0.05
#define EEPROM_NUM_SLOTS          16
#define EEPROM_BASE_ADDR          0
#define EEPROM_SAVE_INTERVAL_MS   60000
#define SOC_RESUME_TOLERANCE      10.0

// ===== Shunt definitions =====
#define SHUNT1_OHMS 0.00025
#define SHUNT1_MAX_AMPS 200.0
#define SHUNT2_OHMS 0.00025
#define SHUNT2_MAX_AMPS 200.0

// ===== Calibration (V/A, Temp offsets) =====
#define BATT1_V_RAW_LOW 10.0
#define BATT1_V_CAL_LOW 10.2
#define BATT1_V_RAW_HIGH 15.0
#define BATT1_V_CAL_HIGH 15.1

#define BATT1_I_RAW_LOW 0.0
#define BATT1_I_CAL_LOW 0.0
#define BATT1_I_RAW_HIGH 100.0
#define BATT1_I_CAL_HIGH 100.5

#define BATT2_V_RAW_LOW 10.0
#define BATT2_V_CAL_LOW 10.1
#define BATT2_V_RAW_HIGH 15.0
#define BATT2_V_CAL_HIGH 15.0

#define BATT2_I_RAW_LOW 0.0
#define BATT2_I_CAL_LOW 0.0
#define BATT2_I_RAW_HIGH 100.0
#define BATT2_I_CAL_HIGH 99.8

// Temperature offsets (°C)
#define BATT1_TEMP_OFFSET  0.0
#define BATT2_TEMP_OFFSET  0.0

// Temp compensation coefficients (V/°C)
#define BATT1_TEMP_COEF   -0.030
#define BATT2_TEMP_COEF    0.0

// Rest detection
#define BATT1_REST_I_THRESHOLD_A      2.0
#define BATT1_REST_V_STABILITY_MV     20
#define BATT1_REST_HOLD_TIME_S        1800

#define BATT2_REST_I_THRESHOLD_A      2.0
#define BATT2_REST_V_STABILITY_MV     20
#define BATT2_REST_HOLD_TIME_S        1800

// Full charge detection
#define BATT1_FULL_V_ABSORB_V         14.4
#define BATT1_FULL_I_TAIL_A           4.0
#define BATT1_FULL_HOLD_TIME_S        900

#define BATT2_FULL_V_ABSORB_V         13.6
#define BATT2_FULL_I_TAIL_A           2.0
#define BATT2_FULL_HOLD_TIME_S        600

// Capacity learning guardrails
#define LEARN_MIN_DELTA_SOC_PCT       20.0
#define LEARN_CAPACITY_MIN_FACTOR     0.5f
#define LEARN_CAPACITY_MAX_FACTOR     2.0f

// Fault detection thresholds
#define BATT1_VOLT_MIN_12V   10.5
#define BATT1_VOLT_MAX_12V   15.0
#define BATT1_CURR_MAX_A     180.0
#define BATT1_TEMP_MAX_C     60.0

#define BATT2_VOLT_MIN_12V   11.0
#define BATT2_VOLT_MAX_12V   14.6
#define BATT2_CURR_MAX_A     150.0
#define BATT2_TEMP_MAX_C     55.0

// Running average window size
#define SMOOTHING_SAMPLES 10

// INA226 I2C pins and addresses
#define I2C_SDA 16
#define I2C_SCL 17
#define INA226_ADDR1 0x40
#define INA226_ADDR2 0x41

// CAN bus (NMEA2000) pins on SH-ESP32
#define CAN_RX_PIN GPIO_NUM_34
#define CAN_TX_PIN GPIO_NUM_32

// DS18B20 bus + addresses
#define ONE_WIRE_BUS 4
#define DS18B20_ADDR1 { 0x28, 0xFF, 0x1C, 0x97, 0x91, 0x16, 0x04, 0x2C }
#define DS18B20_ADDR2 { 0x28, 0xFF, 0x8A, 0x62, 0x92, 0x16, 0x05, 0x7B }
