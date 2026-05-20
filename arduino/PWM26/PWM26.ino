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
*/

#include <math.h>
#include <EEPROM.h>

#define CLEAR_EEPROM false
#define DELTA_TIME 100
#define DECODE false

#define PUMP_PIN 9
#define FAN_PIN 3
#define TEMP_PIN A3   // Coolant Temp 3 from schematic

uint8_t pumpDuty = 5;   // Pump duty cycle 0-100
uint8_t fanDuty = 0;    // Fan duty cycle 0-100

bool daq = false;

// Thermistor constants
const float R0 = 10000.0;          // 10k thermistor at 25 C
const float Beta = 3950.0;         // Beta value
const float T0 = 298.15;           // 25 C in Kelvin
const float R_fixed = 10000.0;     // 10k divider resistor
const float tempTune = 0.25;

float tC1 = 0.0;

// For timed serial printing
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 500; // ms

uint16_t encodeTemp10(float tC) {
  if (isnan(tC) || isinf(tC)) return 0;
  tC = constrain(tC, 0.0f, 80.0f);
  return (uint16_t)lroundf(tC * 10.0f);
}

float decodeTemp10(uint16_t value) {
  if (value > 800) value = 800;
  return value / 10.0f;
}

float adcToTemp(uint8_t pin) {
  int adc = analogRead(pin);

  // avoid divide by zero issues
  if (adc <= 0) return -999.0;
  if (adc >= 1023) return -999.0;

  float voltage = adc * (5.0 / 1023.0);

  // Schematic has 10k pullup to 5V and thermistor returning to ground
  float R_therm = R_fixed * (voltage / (5.0 - voltage));

  float tK = 1.0 / ((1.0 / T0) + (1.0 / Beta) * log(R_therm / R0));
  float tC = tK - 273.15 + tempTune;

  return tC;
}

// Software PWM for D8
void softwarePWMFan(uint8_t duty) {
  static unsigned long cycleStart = 0;

  const unsigned long period = 10000; // microseconds, 100 Hz PWM

  duty = constrain(duty, 0, 100);

  if (duty == 0) {
    digitalWrite(FAN_PIN, LOW);
    return;
  }

  if (duty >= 100) {
    digitalWrite(FAN_PIN, HIGH);
    return;
  }

  unsigned long now = micros();

  if (now - cycleStart >= period) {
    cycleStart = now;
  }

  unsigned long onTime = (period * duty) / 100;

  if ((now - cycleStart) < onTime) {
    digitalWrite(FAN_PIN, HIGH);
  } else {
    digitalWrite(FAN_PIN, LOW);
  }
}

void setup() {
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  Serial.begin(115200);

  digitalWrite(PUMP_PIN, HIGH);
  digitalWrite(FAN_PIN, LOW);

  delayMicroseconds(3000);

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
  while (DECODE) {
    ;
  }

  // Clear EEPROM after reading is skipped unless CLEAR_EEPROM is true
  if (CLEAR_EEPROM) {
    Serial.println("Clearing EEPROM...");

    for (int i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0);
    }

    Serial.println("EEPROM cleared.");
  }

  // =========================
  // Timer1 -> 500 Hz Pump PWM on D9
  // =========================
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1A |= (1 << COM1A1) | (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);

  // Prescaler = 64
  TCCR1B |= (1 << CS11) | (1 << CS10);

  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A |= (1 << COM2A1) | (1 << WGM20) | (1 << WGM21); // Fast PWM
  TCCR2B |= (1 << CS21); // Prescaler = 8

  ICR1 = 499; // 500 Hz
  OCR1A = (ICR1 * pumpDuty) / 100;
}

void loop() {
  // Keep software PWM running as often as possible
  softwarePWMFan(fanDuty);

  if (daq) {
    int sampleCount = EEPROM.length() / (int)sizeof(uint16_t);

    Serial.println("\n\n========== START EEPROM WRITE ==========");

    for (int i = 0; i < sampleCount; i++) {
      unsigned long startTime = millis();

      // Keep fan PWM alive during the sample delay
      while (millis() - startTime < DELTA_TIME) {
        softwarePWMFan(fanDuty);
      }

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

  // Read Coolant Temp 3 from A2
  tC1 = adcToTemp(TEMP_PIN);

  // Serial.println(tC1);

  // Fan control based on Coolant Temp 3
  if (tC1 < 30.0) {
    fanDuty = 0;
  } else if (tC1 > 45.0) {
    fanDuty = 100;
  } else {
    fanDuty = 60;
  }

  // Print every 500 ms instead of spamming Serial
  if (millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis();

    Serial.print("Coolant Temp 3: ");
    Serial.print(tC1);
    Serial.print(" C, Fan Duty: ");
    Serial.print(fanDuty);
    Serial.println("%");
  }

  OCR2A = (0 * 255) / 100; 
}