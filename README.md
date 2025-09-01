# Battery Monitor Project

**Author:** Richard Simpson  
**License:** Creative Commons Attribution (CC-BY)  

This project is an **easy-to-use dual battery monitor** designed for boats, RVs, off‑grid systems, or any setup where keeping track of your batteries is critical.

It measures voltage, current, temperature, and keeps track of how much energy remains. It also speaks **NMEA2000**, so you can see battery status right on your marine electronics.

---

## 🚀 What It Does
- Monitors **two independent batteries** (12V or 24V each)
- Tracks **State of Charge (SoC)** — how full your battery is
- Tracks **State of Health (SoH)** — how much capacity remains compared to new
- Learns your battery’s **true usable capacity** over time
- Detects when a battery is **resting** or **fully charged**
- Sends all data to your **NMEA2000 network** (chartplotters, MFDs, etc.)
- Stores last known values in EEPROM so it **remembers across restarts**

---

## 🛠️ Hardware Supported
- **INA226** current/voltage sensors with external shunts
- **DS18B20** temperature sensors
- Works with ESP32 (built‑in CAN controller)

---

## 📡 NMEA2000 Data Sent
- **PGN 127508 – Battery Status** → Voltage, Current, Temperature, SoC
- **PGN 127506 – DC Detailed Status** → Voltage, Current, SoC, SoH
- **PGN 127513 – Battery Configuration** → Chemistry, Capacity, Nominal V, Peukert Exponent, Charge Efficiency

---

## ⚙️ Setup & Configuration
All setup is done in **`Config.h`**. There you can:
- Select battery system voltage (12V / 24V)
- Choose battery chemistry (Flooded, AGM, Gel, or LiFePO₄)
- Enter nominal capacity (Ah)
- Set Peukert exponent and charge efficiency
- Configure shunt resistor values (Ω, max current)
- Adjust calibration values for voltage/current/temp
- Define full charge detection (voltage + tail current)
- Set rest detection thresholds
- Adjust fault limits (voltage, current, temperature)

Once flashed, the system runs automatically — no menu diving needed.

---

## 🖥️ Debug Output
If you uncomment `#define DEBUG_OUTPUT` in **`Config.h`**, the monitor will also print:
- Raw, calibrated, and smoothed measurements
- SoC %, SoH %, remaining Ah/Wh
- Resting and full status flags

This is useful for setup, testing, or troubleshooting.

---

## 💾 Data Storage
- Uses EEPROM with wear leveling (extends flash life)
- Stores SoC, SoH, and learned capacity
- Auto‑resumes from last saved state, or uses OCV table if new

---

## 📚 Required Arduino Libraries
- [NMEA2000](https://github.com/ttlappalainen/NMEA2000)
- [NMEA2000_esp32](https://github.com/ttlappalainen/NMEA2000_esp32)
- [N2kMessages](https://github.com/ttlappalainen/NMEA2000/tree/master/N2kMessages)
- [INA226](https://github.com/RobTillaart/INA226)
- [OneWire](https://github.com/PaulStoffregen/OneWire)
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)
- [RunningAverage](https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningAverage)
- EEPROM (built into ESP32 Arduino core)

---

## 📂 File Structure
- **BatteryMonitor.ino** → Entry point (setup + loop)
- **Config.h** → All user configuration
- **Globals.h / Globals.cpp** → Shared variables
- **Sensors.h / Sensors.cpp** → Sensor reading + processing
- **Soc.h / Soc.cpp** → SoC/SoH tracking + EEPROM persistence
- **Nmea.h / Nmea.cpp** → NMEA2000 interface
- **README.md** → Project overview (this file)
- **CHANGELOG.md** → Version history
- **LICENSE.md** → License details

---

## 📜 License
Created by **Richard Simpson**  
Licensed under **Creative Commons (CC BY 4.0)**

