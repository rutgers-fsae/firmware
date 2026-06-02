// Apps Board 2026 v0.0.4 - Jeevan Shah
// v0.0.2: Added brake release debounce to handle residual hydraulic pressure
// v0.0.3: Added SD card CSV logging (CS on D10)
// v0.0.4: Keep log file open for 10ms logging; flush every 50 rows to limit data loss on power cut

#include <SPI.h>
#include <SD.h>

#define MAX_IMPLAUSIBILITY_DURATION  100
#define BRAKE_RELEASE_DEBOUNCE_MS     50
#define SD_CS_PIN                     10
#define LOG_FILENAME                  "apps_log.csv"
#define LOG_INTERVAL_MS               10   // 100 Hz
#define FLUSH_EVERY_N_ROWS            50   // flush to card every 50 rows (~500ms)

int lp1 = A0;
int lp2 = A1;
int bs  = 2;
int out = 3;

File          logFile;
bool          sdReady        = false;
unsigned long lastLogTime    = 0;
uint8_t       rowsSinceFlush = 0;

// ── SD ────────────────────────────────────────────────────────────────────────

void initSD() {
  Serial.print("Initializing SD...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD init failed — logging disabled.");
    sdReady = false;
    return;
  }

  // Delete old file so each power cycle starts a clean log
  if (SD.exists(LOG_FILENAME)) SD.remove(LOG_FILENAME);

  logFile = SD.open(LOG_FILENAME, FILE_WRITE);
  if (!logFile) {
    Serial.println("Could not create log file.");
    sdReady = false;
    return;
  }

  logFile.println("timestamp_ms,apps1_pct,apps2_pct,brake_raw,brake_latched,"
                  "pedal_pressed,mismatch_fault,brake_throttle_fault,"
                  "active_fault,implausibility");
  logFile.flush();

  sdReady = true;
  Serial.println("SD OK — logging started.");
}

void logToSD(unsigned long ts,
             float apps1,    float apps2,
             bool brakeRaw,  bool brakeLatch,
             bool pedalPressed,
             bool mismatchFault, bool brakeThrottleFault,
             bool activeFault,   bool implaus) {

  if (!sdReady)                          return;
  if (ts - lastLogTime < LOG_INTERVAL_MS) return;
  lastLogTime = ts;

  // Build row into a stack buffer — avoids String heap fragmentation

  char apps1s[12];
  char apps2s[12];

  dtostrf(apps1, 0, 2, apps1Str);
  dtostrf(apps2, 0, 2, apps2Str);

  char row[56];
  snprintf(row, sizeof(row), "%lu,%.2f,%.2f,%d,%d,%d,%d,%d,%d,%d",
           ts, apps1s, apps2,s
           brakeRaw        ? 1 : 0,
           brakeLatch      ? 1 : 0,
           pedalPressed    ? 1 : 0,
           mismatchFault   ? 1 : 0,
           brakeThrottleFault ? 1 : 0,
           activeFault     ? 1 : 0,
           implaus         ? 1 : 0);

  logFile.println(row);
  rowsSinceFlush++;

  if (rowsSinceFlush >= FLUSH_EVERY_N_ROWS) {
    logFile.flush();
    rowsSinceFlush = 0;
  }
}

// ── Helpers ───────────────────────────────────────────────────────────────────

void set_implausibility(bool plaus) {
  digitalWrite(out, plaus ? LOW : HIGH);
}

bool read_brake_switch() {
  return digitalRead(bs);
}

float sensor_transfer_fcn(int rawVal, int sensorNum) {
  float voltage = 5.0f * rawVal / 1023.0f;
  switch (sensorNum) {
    case 1:  return (69.44f * voltage) - 182.64f;
    case 2:  return (68.49f * voltage) - 148.63f;
    default:
      Serial.println("Invalid sensor number");
      return -1.0f;
  }
}

// ── Setup ─────────────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);

  pinMode(lp1, INPUT);  digitalWrite(lp1, LOW);
  pinMode(lp2, INPUT);  digitalWrite(lp2, LOW);
  pinMode(bs,  INPUT);
  pinMode(out, OUTPUT); digitalWrite(out, LOW);

  delay(100);
  initSD();
}

// ── State ─────────────────────────────────────────────────────────────────────

int   sensor1RawVal, sensor2RawVal;
float sensor1Percentage, sensor2Percentage;

bool          implausibilityLatched    = true;
bool          prevMismatch             = false;
unsigned long implausibilityStartTimer = 0;

bool          brakeLatched             = false;
bool          prevBrakeSwitch          = false;
unsigned long brakeReleaseTimer        = 0;

// ── Loop ──────────────────────────────────────────────────────────────────────

void loop() {
  unsigned long presentTimer = millis();

  sensor1RawVal = analogRead(lp1);
  sensor2RawVal = analogRead(lp2);

  sensor1Percentage = sensor_transfer_fcn(sensor1RawVal, 1);
  sensor2Percentage = sensor_transfer_fcn(sensor2RawVal, 2);

  bool pedalPressed = (sensor1Percentage > 10.0f && sensor2Percentage > 10.0f);

  Serial.print("APPS 1: ");  Serial.println(sensor1Percentage);
  Serial.print("APPS 2: ");  Serial.println(sensor2Percentage);

  // APPS mismatch fault
  bool mismatchNow = (fabsf(sensor1Percentage - sensor2Percentage) > 10.0f);
  if (mismatchNow && !prevMismatch) {
    implausibilityStartTimer = presentTimer;
  }
  bool appsMismatchFault = mismatchNow &&
                           ((presentTimer - implausibilityStartTimer) >= MAX_IMPLAUSIBILITY_DURATION);
  prevMismatch = mismatchNow;

  // Brake switch with release debounce
  bool rawBrake = read_brake_switch();
  if (rawBrake) {
    brakeLatched      = true;
    brakeReleaseTimer = presentTimer;
  } else if ((presentTimer - brakeReleaseTimer) >= BRAKE_RELEASE_DEBOUNCE_MS) {
    brakeLatched = false;
  }
  prevBrakeSwitch = rawBrake;

  bool brakeThrottleFault = (brakeLatched && pedalPressed);
  bool activeFault        = (appsMismatchFault || brakeThrottleFault);

  if (activeFault) {
    implausibilityLatched = true;
  }
  if (implausibilityLatched && !pedalPressed && !activeFault) {
    implausibilityLatched = false;
  }

  bool implausibility = implausibilityLatched || !pedalPressed;

  Serial.println(implausibility);
  set_implausibility(implausibility);

  logToSD(presentTimer,
          sensor1Percentage, sensor2Percentage,
          rawBrake,       brakeLatched,
          pedalPressed,
          appsMismatchFault, brakeThrottleFault,
          activeFault,    implausibility);
}
