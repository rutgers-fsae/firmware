#include <math.h>
#include <EEPROM.h>

#define CLEAR_EEPROM false // set to true to clear any stored data during startup
#define DELTA_TIME 100 // time in milliseconds between each sample
#define DECODE true // set to true to print out stored data

uint8_t pumpDuty = 40;   // Pump duty cycle (0–100)
uint8_t fanDuty = 50;     // Fan duty cycle (0–100)

bool daq = true; 

const float R0 = 10000.0;      // 10kΩ at 25°C
const float Beta = 3950.0;     // Beta value
const float T0 = 298.15;       // 25°C in Kelvin
const float R_fixed = 10000.0; // 10kΩ divider resistor
const float tempTune = 0.25;


int adc1;
float voltage1, R_therm1, tK1, tC1;

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
  adc1 = analogRead(pin);
  voltage1 = adc1 * (5.0 / 1023.0);
  R_therm1 = R_fixed * (voltage1 / (5.0 - voltage1));
  tK1 = 1.0 / ((1.0/T0) + (1.0/Beta) * log(R_therm1/R0));
  tC1 = tK1 - 273.15 + tempTune;
  return tC1;
}

void setup() {
  pinMode(9, OUTPUT);   // Pump
  pinMode(11, OUTPUT);  // Fan
  Serial.begin(115200);
  digitalWrite(9, HIGH);
  digitalWrite(11, HIGH);
  delayMicroseconds(3000);



  if (DECODE) {
    Serial.println("\n\n========== START READ EEPROM ==========");
    int sampleCount = EEPROM.length() / (int)sizeof(uint16_t);
    for (int i = 0; i < sampleCount; i++) {
      int address = i * (int)sizeof(uint16_t);
      uint16_t encodedTemp;
      EEPROM.get(address, encodedTemp);
      float t = decodeTemp10(encodedTemp);

      // Print readable output
      Serial.print("T1: ");
      Serial.print(t);
      Serial.println(" C   ");
    }
    Serial.println("========== END READ EEPROM ==========\n\n");
    delay(500);
  }

  // clear stored data (do it after reading just to prevent data loss)
  if (CLEAR_EEPROM) {
    for (int i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0);
    }
  }
 

  // =========================
  // Timer1 → 500 Hz (Pump)
  // =========================
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1A |= (1 << COM1A1) | (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);
  // Prescaler = 64
  TCCR1B |= (1 << CS11) | (1 << CS10);
  ICR1 = 499;  // 500 Hz
  OCR1A = (ICR1 * pumpDuty) / 100;


  // =========================
  // Timer2 → 31 kHz (Fan)
  // =========================
 
  TCCR2A = 0;
  TCCR2B = 0;
  // Fast PWM (8-bit)
  TCCR2A |= (1 << COM2A1) | (1 << WGM20) | (1 << WGM21);
  // Prescaler = 1
  TCCR2B |= (1 << CS20);
  OCR2A = 0;  // Start off
}

void loop() {

  while (DECODE) {
    ;
  }

  // --- Thermistor 1 ---

  if (daq) {
    int sampleCount = EEPROM.length() / (int)sizeof(uint16_t);
    Serial.println("\n\n========== START EEPROM WRITE ==========");
    for (int i = 0; i < sampleCount; i++) {
      delay(DELTA_TIME); // write data point every DELTA_TIME ms
      tC1 = adcToTemp(A0);
      uint16_t encodedTemp = encodeTemp10(tC1);
      int address = i * (int)sizeof(uint16_t);
      EEPROM.put(address, encodedTemp);

      Serial.print("T1: ");
      Serial.print(tC1);
      Serial.println(" C   ");
    }
    daq = false; // ensure we only write data for the length of EEPROM, don't want to overwrite anything
    Serial.println("========== END EEPROM WRITE  ==========\n\n");
  }

  tC1 = adcToTemp(A0);

  // Print readable output
  Serial.print("T1: ");
  Serial.print(tC1);
  Serial.println(" C   ");

   // Apply to Timer2
  OCR2A = (fanDuty * 255) / 100;
}
