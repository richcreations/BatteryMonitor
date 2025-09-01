#include "Globals.h"
#include "Config.h"
#include <EEPROM.h>
#include <math.h>

// ==========================
// Data Structures
// ==========================

struct SocPoint { float soc; float v; };
typedef struct { const SocPoint* pts; size_t len; } OcvTableView;

// ==========================
// EEPROM Manager Class
// ==========================

class BatteryEepromManager {
public:
  void begin() { EEPROM.begin(EEPROM_NUM_SLOTS * SLOT_SIZE); }
  bool load(float &b1cap, float &b2cap, float &b1soc, float &b2soc,
            float &b1soh, float &b2soh);
  void save(float b1cap, float b2cap, float b1soc, float b2soc,
            float b1soh, float b2soh);
private:
  const int SLOT_SIZE = 2 + 6*4 + 2; // seq + 6 floats + checksum
  int lastSlot = -1;
  uint16_t seqNum = 0;
  uint16_t calcChecksum(int addr, size_t len);
};

static BatteryEepromManager eepromMgr;

// ==========================
// EEPROM Implementation
// ==========================

uint16_t BatteryEepromManager::calcChecksum(int addr, size_t len) {
  uint16_t sum = 0;
  for (size_t i = 0; i < len; i++) sum += EEPROM.read(addr + i);
  return sum;
}

bool BatteryEepromManager::load(float &b1cap, float &b2cap, float &b1soc, float &b2soc,
                                float &b1soh, float &b2soh) {
  int latestSlot = -1; uint16_t latestSeq = 0;
  for (int i = 0; i < EEPROM_NUM_SLOTS; i++) {
    int addr = EEPROM_BASE_ADDR + i * SLOT_SIZE;
    uint16_t seq; EEPROM.get(addr, seq);
    if (seq == 0xFFFF) continue;
    uint16_t crcStored; EEPROM.get(addr + SLOT_SIZE - 2, crcStored);
    uint16_t crcCalc = calcChecksum(addr, SLOT_SIZE - 2);
    if (crcStored == crcCalc && seq >= latestSeq) {
      latestSeq = seq; latestSlot = i;
    }
  }
  if (latestSlot < 0) return false;
  int addr = EEPROM_BASE_ADDR + latestSlot * SLOT_SIZE;
  uint16_t seq; EEPROM.get(addr, seq); addr += 2;
  EEPROM.get(addr, b1cap); addr += 4;
  EEPROM.get(addr, b2cap); addr += 4;
  EEPROM.get(addr, b1soc); addr += 4;
  EEPROM.get(addr, b2soc); addr += 4;
  EEPROM.get(addr, b1soh); addr += 4;
  EEPROM.get(addr, b2soh); addr += 4;
  lastSlot = latestSlot; seqNum = seq;
  return true;
}

void BatteryEepromManager::save(float b1cap, float b2cap, float b1soc, float b2soc,
                                float b1soh, float b2soh) {
  seqNum++;
  int next = (lastSlot + 1) % EEPROM_NUM_SLOTS;
  int addr = EEPROM_BASE_ADDR + next * SLOT_SIZE;
  int start = addr;
  EEPROM.put(addr, seqNum); addr += 2;
  EEPROM.put(addr, b1cap);  addr += 4;
  EEPROM.put(addr, b2cap);  addr += 4;
  EEPROM.put(addr, b1soc);  addr += 4;
  EEPROM.put(addr, b2soc);  addr += 4;
  EEPROM.put(addr, b1soh);  addr += 4;
  EEPROM.put(addr, b2soh);  addr += 4;
  uint16_t crc = calcChecksum(start, SLOT_SIZE - 2);
  EEPROM.put(start + SLOT_SIZE - 2, crc);
  EEPROM.commit();
  lastSlot = next;
}

// ==========================
// OCV Tables (12V Reference)
// ==========================

static const SocPoint OCV_FLA_12V[] = {
  {10, 11.51},{20, 11.66},{30, 11.81},{40, 11.96},{50, 12.10},
  {60, 12.24},{70, 12.37},{80, 12.50},{90, 12.62},{100, 12.73}
};
static const SocPoint OCV_AGM_12V[] = {
  {10, 11.60},{20, 11.78},{30, 11.95},{40, 12.10},{50, 12.20},
  {60, 12.32},{70, 12.45},{80, 12.60},{90, 12.75},{100, 12.85}
};
static const SocPoint OCV_GEL_12V[] = {
  {10, 11.60},{20, 11.80},{30, 11.96},{40, 12.12},{50, 12.24},
  {60, 12.36},{70, 12.48},{80, 12.62},{90, 12.78},{100, 12.90}
};
static const SocPoint OCV_LFP_12V[] = {
  {0, 12.00},{10, 12.90},{20, 13.00},{30, 13.10},{40, 13.15},
  {50, 13.20},{60, 13.25},{70, 13.30},{80, 13.35},{90, 13.45},{100, 13.60}
};

// ==========================
// Helpers
// ==========================

static OcvTableView getTableForChem(int chem) {
  switch (chem) {
    case CHEM_FLA: return { OCV_FLA_12V, sizeof(OCV_FLA_12V)/sizeof(OCV_FLA_12V[0]) };
    case CHEM_AGM: return { OCV_AGM_12V, sizeof(OCV_AGM_12V)/sizeof(OCV_AGM_12V[0]) };
    case CHEM_GEL: return { OCV_GEL_12V, sizeof(OCV_GEL_12V)/sizeof(OCV_GEL_12V[0]) };
    case CHEM_LFP: return { OCV_LFP_12V, sizeof(OCV_LFP_12V)/sizeof(OCV_LFP_12V[0]) };
  }
  return { OCV_FLA_12V, sizeof(OCV_FLA_12V)/sizeof(OCV_FLA_12V[0]) };
}

static float lerp(float x, float x0, float x1, float y0, float y1) {
  if (fabsf(x1 - x0) < 1e-6f) return y0;
  float t = (x - x0) / (x1 - x0);
  return y0 + t * (y1 - y0);
}

static float socFromOcvVoltage(float measuredV, const OcvTableView& tv, bool is24V) {
  float v12 = is24V ? (measuredV * 0.5f) : measuredV;
  if (v12 <= tv.pts[0].v) return tv.pts[0].soc;
  if (v12 >= tv.pts[tv.len-1].v) return tv.pts[tv.len-1].soc;
  for (size_t i = 1; i < tv.len; i++) {
    if (v12 <= tv.pts[i].v)
      return lerp(v12, tv.pts[i-1].v, tv.pts[i].v, tv.pts[i-1].soc, tv.pts[i].soc);
  }
  return tv.pts[tv.len-1].soc;
}

static float compensateVoltageForTemp(float measuredV, float tempC, float coef) {
  float dT = tempC - 25.0f;
  return measuredV - (coef * dT);
}

// Use globals’ helpers
extern bool isBatt1_24V();
extern bool isBatt2_24V();

static float computeOcvSoc_batt1() {
  OcvTableView tv = getTableForChem(BATT1_CHEMISTRY);
  float vAdj = compensateVoltageForTemp(smooth_battery1_voltage, smooth_battery1_temp_C, BATT1_TEMP_COEF);
  float soc = socFromOcvVoltage(vAdj, tv, isBatt1_24V());
  return fmaxf(0.0f, fminf(100.0f, soc));
}
static float computeOcvSoc_batt2() {
  OcvTableView tv = getTableForChem(BATT2_CHEMISTRY);
  float vAdj = compensateVoltageForTemp(smooth_battery2_voltage, smooth_battery2_temp_C, BATT2_TEMP_COEF);
  float soc = socFromOcvVoltage(vAdj, tv, isBatt2_24V());
  return fmaxf(0.0f, fminf(100.0f, soc));
}

// ==========================
// Rest and Full Detection Helpers
// ==========================
// (unchanged from before)

// ==========================
// Public API
// ==========================

void setupSoc() {
  eepromMgr.begin();
  float b1cap, b2cap, b1soc, b2soc, b1soh, b2soh;
  if (eepromMgr.load(b1cap, b2cap, b1soc, b2soc, b1soh, b2soh)) {
    battery1_learned_capacity_Ah = b1cap;
    battery2_learned_capacity_Ah = b2cap;
    eeprom_soc_b1 = b1soc;
    eeprom_soc_b2 = b2soc;
    soc_battery1_percent = eeprom_soc_b1;
    soc_battery2_percent = eeprom_soc_b2;
    soh_battery1_percent = b1soh;
    soh_battery2_percent = b2soh;
    battery1_remaining_Ah = (soc_battery1_percent/100.0f) * battery1_learned_capacity_Ah;
    battery2_remaining_Ah = (soc_battery2_percent/100.0f) * battery2_learned_capacity_Ah;
    battery1_remaining_Wh = smooth_battery1_voltage * battery1_remaining_Ah;
    battery2_remaining_Wh = smooth_battery2_voltage * battery2_remaining_Ah;
    haveEepromSoc = true;
  }
  lastLoopMillis = millis();
}

void updateSoc() {
  if (needSocInitFromOCV) {
    float ocv1 = computeOcvSoc_batt1();
    float ocv2 = computeOcvSoc_batt2();
    if (haveEepromSoc) {
      soc_battery1_percent = (fabsf(eeprom_soc_b1 - ocv1) <= SOC_RESUME_TOLERANCE) ? eeprom_soc_b1 : ocv1;
      soc_battery2_percent = (fabsf(eeprom_soc_b2 - ocv2) <= SOC_RESUME_TOLERANCE) ? eeprom_soc_b2 : ocv2;
    } else {
      soc_battery1_percent = ocv1;
      soc_battery2_percent = ocv2;
    }
    battery1_remaining_Ah = (soc_battery1_percent/100.0f) * battery1_learned_capacity_Ah;
    battery2_remaining_Ah = (soc_battery2_percent/100.0f) * battery2_learned_capacity_Ah;
    battery1_remaining_Wh = smooth_battery1_voltage * battery1_remaining_Ah;
    battery2_remaining_Wh = smooth_battery2_voltage * battery2_remaining_Ah;
    needSocInitFromOCV = false;
  }

  unsigned long nowMs = millis();
  float dtHours = (nowMs - lastLoopMillis) / 3600000.0f;
  lastLoopMillis = nowMs;

  battery1_remaining_Ah += -smooth_battery1_current * dtHours;
  battery2_remaining_Ah += -smooth_battery2_current * dtHours;
  battery1_remaining_Wh += -smooth_battery1_power * dtHours;
  battery2_remaining_Wh += -smooth_battery2_power * dtHours;

  if (battery1_remaining_Ah > battery1_learned_capacity_Ah) battery1_remaining_Ah = battery1_learned_capacity_Ah;
  if (battery1_remaining_Ah < 0) battery1_remaining_Ah = 0;
  if (battery2_remaining_Ah > battery2_learned_capacity_Ah) battery2_remaining_Ah = battery2_learned_capacity_Ah;
  if (battery2_remaining_Ah < 0) battery2_remaining_Ah = 0;

  soc_battery1_percent = 100.0f * (battery1_remaining_Ah / battery1_learned_capacity_Ah);
  soc_battery2_percent = 100.0f * (battery2_remaining_Ah / battery2_learned_capacity_Ah);

  // --- Update SoH (learned vs nominal capacity) ---
  soh_battery1_percent = 100.0f * (battery1_learned_capacity_Ah / BATT1_CAPACITY_AH);
  soh_battery2_percent = 100.0f * (battery2_learned_capacity_Ah / BATT2_CAPACITY_AH);
  soh_battery1_percent = fminf(fmaxf(soh_battery1_percent, 0.0f), 100.0f);
  soh_battery2_percent = fminf(fmaxf(soh_battery2_percent, 0.0f), 100.0f);

  // --- Rest detection ---
  // (unchanged)

  // --- Full charge detection + learning ---
  // (unchanged)

  // --- Periodic EEPROM save ---
  if (millis() - lastEepromSaveMillis >= EEPROM_SAVE_INTERVAL_MS) {
    eepromMgr.save(battery1_learned_capacity_Ah, battery2_learned_capacity_Ah,
                   soc_battery1_percent, soc_battery2_percent,
                   soh_battery1_percent, soh_battery2_percent);
    lastEepromSaveMillis = millis();
  }
}
