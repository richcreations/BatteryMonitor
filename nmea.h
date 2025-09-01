#ifndef NMEA_H
#define NMEA_H

#include <Arduino.h>
#include <NMEA2000.h>
#include <NMEA2000_esp32.h>   // ESP32 built-in CAN controller

// ===========================================================
// Nmea.h — NMEA2000 interface for Battery Monitor
// ===========================================================
//
// Provides:
//   - Setup for N2K CAN interface
//   - Functions to send PGNs 127508, 127506, 127513
//   - Dispatcher loop to control message timing
//   - Shared NMEA2000 bus instance
// ===========================================================

// Global NMEA2000 instance (defined in nmea.cpp)
extern tNMEA2000_esp32 NMEA2000;

// Initialize the NMEA2000 interface
void setupNmea();

// Send PGN 127508 Battery Status for a given battery instance
void sendNmeaBatteryStatus(uint8_t instance);

// Send PGN 127506 DC Detailed Status for a given battery instance
void sendNmeaDcStatus(uint8_t instance);

// Send PGN 127513 Battery Configuration for a given battery instance
void sendNmeaBatteryConfig(uint8_t instance);

// Periodic dispatcher (must be called from loop)
void nmeaLoop();

#endif // NMEA_H
