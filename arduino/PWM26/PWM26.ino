/*
HOW TO USE THIS CODE FOR DAQ:
  1. To record data:
     - Set CLEAR_EEPROM to true
     - Set DECODE to false
     - Set daq to true
     - Flash the code and let it run until "END EEPROM WRITE"

  2. To read stored data:
     - Set CLEAR_EEPROM to false
     - Set DECODE to true
     - Flash the code and open Serial Monitor

  3. For normal operation:
     - Set CLEAR_EEPROM to false
     - Set DECODE to false
     - Set daq to false

TIMER ALLOCATION (ATmega328P):
  Timer 0 → D5, D6  — left alone (millis/delay)
  Timer 1 → D9      — pump PWM at 500 Hz (ICR1 mode)
  Timer 2 → D3      — fan PWM at ~31.25 kHz (OC2B, Fast PWM, TOP=OCR2A=63)
*/

#include <math.h>
#include <EEPROM.h>

#define CLEAR_EEPROM false
#define DELTA_TIME 100
#define DECODE false

#define PUMP_PIN 9
#define FAN_PIN  3    // OC2B on ATmega328P — Timer 2 hardware PWM
#define TEMP_PIN A3   // Coolant Temp 3 from schematic

uint8_t pumpDuty = 5;  // Pump duty cycle 0-100
uint8_t fanDuty  = 0;  // Fan duty cycle 0-100

bool daq = false;

// ── Thermistor constants ───────────────────────────────────────────────────
const float R0       = 10000.0;  // 10k thermistor resistance at 25 C
const float Beta     = 3950.0;   // Beta coefficient (K) — verify against your part
const float T0       = 298.15;   // 25 C in Kelvin
const float R_fixed  = 10000.0;  // R3110 pull-up resistor (ohms)
const float tempTune = 0.25;     // Fine-tune offset (C)

float tC1 = 0.0;

// For timed serial printing
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 500; // ms

// ── EEPROM encode/decode ───────────────────────────────────────────────────
uint16_t encodeTemp10(float tC) {
  if (isnan(tC) || isinf(tC)) return 0;
  tC = constrain(tC, 0.0f, 80.0f);
  return (uint16_t)lroundf(tC * 10.0f);
}

float decodeTemp10(uint16_t value) {
  if (value > 800) value = 800;
  return value / 10.0f;
}

// ── ADC → temperature ─────────────────────────────────────────────────────
float adcToTemp(uint8_t pin) {
  int adc = analogRead(pin);

  if (adc <= 0)    return -999.0;
  if (adc >= 1023) return -999.0;

  float voltage = adc * (5.0 / 1023.0);

  // Voltage divider: +5V → R_fixed → pin → NTC → GND
  float R_therm = R_fixed * (voltage / (5.0 - voltage));

  float tK = 1.0 / ((1.0 / T0) + (1.0 / Beta) * log(R_therm / R0));
  float tC = tK - 273.15 + tempTune;

  return tC;
}

// ── Fan duty cycle (0–100%) via Timer 2 OC2B hardware PWM ─────────────────
// Timer 2 is configured with TOP = OCR2A = 63 (64 steps).
// Freq = 16 MHz / (prescaler=8 × (TOP+1)=64) = 31,250 Hz
// OCR2B = 0  → 0%   (fans off)
// OCR2B = 63 → 100% (full speed)
void setFanDuty(uint8_t percent) {
  percent = constrain(percent, 0, 100);
  OCR2B = (uint8_t)((uint32_t)percent * 63 / 100);
}

// ── Setup ──────────────────────────────────────────────────────────────────
void setup() {
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(FAN_PIN,  OUTPUT);

  Serial.begin(115200);

  digitalWrite(PUMP_PIN, HIGH);
  digitalWrite(FAN_PIN,  LOW);

  delayMicroseconds(3000);

  // ── DECODE: dump EEPROM to Serial then halt ──────────────────────────────
  if (DECODE) {
    Serial.println("\n\n========== START READ EEPROM ==========");

    int sampleCount = EEPROM.length() / (int)sizeof(uint16_t);

    for (int i = 0; i < sampleCount; i++) {
      int address = i * (int)sizeof(uint16_t);

      uint16_t encodedTemp;
      EEPROM.get(address, encodedTemp);

      float t = decodeTemp10(encodedTemp);

      Serial.print("Coolant Temp 3: ");
      Serial.print(t);
      Serial.println(" C");
    }

    Serial.println("========== END READ EEPROM ==========\n\n");
    delay(500);
  }

  // Halt forever after decode
  while (DECODE) { ; }

  // ── Clear EEPROM if requested ────────────────────────────────────────────
  if (CLEAR_EEPROM) {
    Serial.println("Clearing EEPROM...");
    for (int i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0);
    }
    Serial.println("EEPROM cleared.");
  }

  // ── Timer 1: pump PWM on D9 at 500 Hz ───────────────────────────────────
  // Mode 14 (Fast PWM, TOP = ICR1), prescaler /64
  // Freq = 16MHz / (64 × (ICR1+1)) = 16MHz / (64 × 500) = 500 Hz
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1A = (1 << COM1A1) | (1 << WGM11);
  TCCR1B = (1 << WGM13)  | (1 << WGM12) | (1 << CS11) | (1 << CS10);
  ICR1   = 499;
  OCR1A  = (ICR1 * pumpDuty) / 100;

  // ── Timer 2: fan PWM on D3 (OC2B) at ~31.25 kHz ────────────────────────
  // Mode 7 (Fast PWM, TOP = OCR2A), prescaler /8
  // Freq = 16MHz / (8 × 64) = 31,250 Hz
  // COM2B1 → non-inverting output on OC2B (D3)
  // COM2A bits left clear → OC2A (D11) not driven, OCR2A free to set TOP
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << WGM22)  | (1 << CS21);
  OCR2A  = 63;  // TOP — do not use D11 as output or this will be corrupted
  OCR2B  = 0;   // Start fans off
}

// ── Loop ───────────────────────────────────────────────────────────────────
void loop() {

  // ── DAQ: write temperature samples to EEPROM ────────────────────────────
  if (daq) {
    int sampleCount = EEPROM.length() / (int)sizeof(uint16_t);

    Serial.println("\n\n========== START EEPROM WRITE ==========");

    for (int i = 0; i < sampleCount; i++) {
      unsigned long startTime = millis();

      // Simple blocking delay — hardware PWM keeps fan running unattended
      while (millis() - startTime < DELTA_TIME) { }

      tC1 = adcToTemp(TEMP_PIN);

      uint16_t encodedTemp = encodeTemp10(tC1);
      int address = i * (int)sizeof(uint16_t);
      EEPROM.put(address, encodedTemp);

      Serial.print("Coolant Temp 3: ");
      Serial.print(tC1);
      Serial.println(" C");
    }

    daq = false;
    Serial.println("========== END EEPROM WRITE ==========\n\n");
    delay(2000);
  }

  // ── Read temperature ─────────────────────────────────────────────────────
  tC1 = adcToTemp(TEMP_PIN);

  // ── Fan control ───────────────────────────────────────────────────────────
  if (tC1 < 30.0) {
    fanDuty = 0;
  } else if (tC1 > 45.0) {
    fanDuty = 100;
  } else {
    fanDuty = 60;
  }

  Serial.println("Fan Duty 0");
  setFanDuty(0);
  delay(3000);

  Serial.println("Fan Duty 25");
  setFanDuty(25);
  delay(3000);

  Serial.println("Fan Duty 50");
  setFanDuty(50);
  delay(3000);

  Serial.println("Fan Duty 75");
  setFanDuty(75);
  delay(3000);

  Serial.println("Fan Duty 100");
  setFanDuty(100);
  delay(3000);
  
  

  // ── Serial output (every 500 ms) ─────────────────────────────────────────
  if (millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis();

    Serial.print("Coolant Temp 3: ");
    Serial.print(tC1);
    Serial.print(" C, Fan Duty: ");
    Serial.print(fanDuty);
    Serial.println("%");
  }
}
