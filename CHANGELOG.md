# Changelog

All notable changes to this project will be documented in this file.  
This project follows a locked-release workflow: once a version is marked Public Release, its files are frozen.

---

## [1.1] - 2025-09-01
### Added
- Peukert exponent and charge efficiency parameters per battery (`Config.h`).
- State of Health (SoH) calculation, stored in globals and included in NMEA2000 PGN 127506.
- Expanded NMEA2000 PGN 127513 to include Peukert exponent, charge efficiency, and nominal voltage.
- Updated documentation in `README.md` with new configuration options.
- License information (Creative Commons BY 4.0) and attribution to Richard Simpson.

### Changed
- EEPROM slot structure updated to accommodate new parameters.
- `Globals.h/.cpp` extended to hold SoH values.

---

## [1.0] - 2025-08-31
### Added
- Initial NMEA2000 integration: PGNs 127508 (Battery Status), 127506 (DC Detailed Status), 127513 (Battery Configuration).
- CAN bus pin configuration in `Config.h`.
- New files `Nmea.h` and `Nmea.cpp` for NMEA2000 interface.
- Debug output support for NMEA data.

---

## [0.9] - 2025-08-30
### Added
- First public baseline release.
- INA226 + DS18B20 sensor integration.
- SoC tracking with OCV tables and EEPROM wear leveling.
- Rest detection, full charge detection, capacity learning.
