// Apps Board 2026 v0.0.3 - Jeevan Shah
// v0.0.2: Added brake release debounce to handle residual hydraulic pressure
// v0.0.3: Added SD card CSV logging (CS on D10)

#include <SPI.h>
#include <SD.h>

#define MAX_IMPLAUSIBILITY_DURATION  100
#define BRAKE_RELEASE_DEBOUNCE_MS     50
#define SD_CS_PIN                     10
#define LOG_FILENAME                  "apps_log.csv"
#define LOG_INTERVAL_MS               50   // log at 20 Hz — tune as needed

int lp1 = A0;
int lp2 = A1;
int bs  = 2;
int out = 3;

bool          sdReady        = false;
unsigned long lastLogTime    = 0;

// ── SD ────────────────────────────────────────────────────────────────────────

void initSD() {
  Serial.print("Initializing SD...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD init failed — logging disabled.");
    sdReady = false;
    return;
  }
  sdReady = true;
  Serial.println("SD OK.");

  // Write header only if the file is brand new
  if (!SD.exists(LOG_FILENAME)) {
    File f = SD.open(LOG_FILENAME, FILE_WRITE);
    if (f) {
      f.println("timestamp_ms,apps1_pct,apps2_pct,brake_raw,brake_latched,"
                "pedal_pressed,mismatch_fault,brake_throttle_fault,"
                "active_fault,implausibility");
      f.close();
    } else {
      Serial.println("Could not create log file.");
      sdReady = false;
    }
  }
}

void logToSD(unsigned long ts,
             float apps1, float apps2,
             bool brakeRaw, bool brakeLatch,
             bool pedalPressed,
             bool mismatchFault, bool brakeThrottleFault,
             bool activeFault,  bool implaus) {

  if (!sdReady) return;
  if (ts - lastLogTime < LOG_INTERVAL_MS) return;
  lastLogTime = ts;

  File f = SD.open(LOG_FILENAME, FILE_WRITE);
  if (!f) {
    Serial.println("SD write failed.");
    return;
  }

  f.print(ts);                 f.print(',');
  f.print(apps1, 2);           f.print(',');
  f.print(apps2, 2);           f.print(',');
  f.print(brakeRaw   ? 1 : 0); f.print(',');
  f.print(brakeLatch ? 1 : 0); f.print(',');
  f.print(pedalPressed      ? 1 : 0); f.print(',');
  f.print(mismatchFault     ? 1 : 0); f.print(',');
  f.print(brakeThrottleFault? 1 : 0); f.print(',');
  f.print(activeFault       ? 1 : 0); f.print(',');
  f.println(implaus          ? 1 : 0);

  f.close();
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
    case 1:  return (40.0f * voltage) - 49.0f;
    case 2:  return (40.0f * voltage) - 32.0f;
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
          rawBrake, brakeLatched,
          pedalPressed,
          appsMismatchFault, brakeThrottleFault,
          activeFault, implausibility);
}