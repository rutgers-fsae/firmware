#include <math.h>

#define LOG_INTERVAL 1000UL

#define PUMP_PIN 9
#define FAN_PIN 3
#define TEMP_PIN A2

#define PUMP_FREQ 500.0
#define FAN_FREQ 25000.0

const float R0 = 10000.0;
const float Beta = 3950.0;
const float T0 = 298.15;
const float R_fixed = 10000.0;
const float tempTune = 0.25;

struct ThermistorConfig {
  bool enabled;
  int8_t pin;
  float nominalOhms, betaK, nominalKelvin, fixedOhms, offsetC;
};
const ThermistorConfig extraThermistors[] = {
  {false, -1, 0, 0, 0, 0, 0},
  {false, -1, 0, 0, 0, 0, 0},
  {false, -1, 0, 0, 0, 0, 0},
};
const uint8_t EXTRA_THERMISTOR_COUNT = sizeof(extraThermistors) / sizeof(extraThermistors[0]);
static_assert(EXTRA_THERMISTOR_COUNT == 3, "Coordinate slot count with Pi CSV schema");

bool configuredThermistor(const ThermistorConfig &c) {
  return c.enabled && c.pin >= A0 && c.pin <= A7 && c.pin != TEMP_PIN &&
         c.nominalOhms > 0 && c.betaK > 0 && c.nominalKelvin > 0 &&
         c.fixedOhms > 0 && isfinite(c.nominalOhms) && isfinite(c.betaK) &&
         isfinite(c.nominalKelvin) && isfinite(c.fixedOhms) && isfinite(c.offsetC);
}

float readExtraThermistor(const ThermistorConfig &c) {

  analogRead(c.pin);
  int adc = analogRead(c.pin);
  if (adc <= 0 || adc >= 1023) return -999.0;
  float resistance = c.fixedOhms * (float(adc) / (1023.0f - adc));
  float inverseKelvin = 1.0f / c.nominalKelvin + log(resistance / c.nominalOhms) / c.betaK;
  if (inverseKelvin <= 0 || !isfinite(inverseKelvin)) return -999.0;
  float value = 1.0f / inverseKelvin - 273.15f + c.offsetC;
  return isfinite(value) ? value : -999.0;
}

const float TEMP_LOW = 30.0;
const float TEMP_HIGH = 45.0;

uint8_t pumpDuty = 20;
uint8_t fanDuty = 0;
float tC1 = 0.0;

unsigned long lastLogTime = 0;

void setupTimer1(float freq) {
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1A = (1 << COM1A1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << CS10);
  ICR1 = (uint16_t)(F_CPU / (2.0 * freq));
  OCR1A = 0;
}

void setupTimer2(float freq) {
  TIMSK2 = 0;
  TIFR2 = 0;
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << WGM22) | (1 << CS21);
  OCR2A = (uint8_t)(F_CPU / (8.0 * freq) - 1);
  OCR2B = 0;
}

void setPumpDuty(uint8_t percent) {
  percent = constrain(percent, 0, 100);
  OCR1A = (uint16_t)((uint32_t)ICR1 * (100 - percent) / 100);
}

void setFanDuty(uint8_t percent) {
  percent = constrain(percent, 0, 100);
  OCR2B = (uint8_t)((uint16_t)OCR2A * (100 - percent) / 100);
}

void logSample(unsigned long timestamp, float temperature, uint8_t fanPercent,
               uint8_t pumpPercent) {
  Serial.print(timestamp);
  Serial.print(',');
  Serial.print(temperature, 2);
  Serial.print(',');
  Serial.print(fanPercent);
  Serial.print(',');
  Serial.print(pumpPercent);
  for (uint8_t i = 0; i < EXTRA_THERMISTOR_COUNT; ++i) {
    Serial.print(',');
    if (configuredThermistor(extraThermistors[i]))
      Serial.print(readExtraThermistor(extraThermistors[i]), 2);

  }
  Serial.println();
}

float adcToTemp(uint8_t pin) {
  analogRead(pin);
  int adc = analogRead(pin);

  if (adc <= 0 || adc >= 1023)
    return -999.0;

  float voltage = adc * (5.0f / 1023.0f);
  float R_therm = R_fixed * (voltage / (5.0f - voltage));
  float tK = 1.0f / ((1.0f / T0) + (1.0f / Beta) * log(R_therm / R0));

  return tK - 273.15f + tempTune;
}

void setup() {
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  Serial.begin(115200);

  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(PUMP_PIN, HIGH);
  delayMicroseconds(3000);
  setPumpDuty(95);
  delay(100);

  setupTimer1(PUMP_FREQ);
  setupTimer2(FAN_FREQ);

  setFanDuty(fanDuty);

  Serial.println(F("time_ms,coolant_temp_c,fan_duty_percent,pump_duty_percent,thermistor_1_c,thermistor_2_c,thermistor_3_c"));
}

void loop() {
  pumpDuty = 70;
  setPumpDuty(pumpDuty);

  tC1 = adcToTemp(TEMP_PIN);

  if (tC1 < TEMP_LOW) {
    fanDuty = 0;
  } else if (tC1 > TEMP_HIGH) {
    fanDuty = 100;
  } else {
    fanDuty = 60;
  }

  setFanDuty(fanDuty);

  unsigned long now = millis();
  if (now - lastLogTime >= LOG_INTERVAL) {
    lastLogTime = now;
    logSample(now, tC1, fanDuty, pumpDuty);
  }
}
