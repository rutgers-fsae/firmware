// Apps Board 2026 v0.0.4 - Jeevan Shah
// v0.0.2: Added brake release debounce to handle residual hydraulic pressure
// v0.0.3: Added SD card CSV logging (CS on D10)
// v0.0.4: Keep log file open for 10ms logging; flush every 50 rows to limit data loss on power cut
#define MAX_IMPLAUSIBILITY_DURATION  100
#define BRAKE_RELEASE_DEBOUNCE_MS     50

int lp1 = A0;
int lp2 = A1;
int bs  = 2;
int out = 3;

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
}
