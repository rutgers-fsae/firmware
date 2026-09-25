#include <Wire.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const uint8_t SENSOR_ADDRESS = 0x28;
// Matek's 4525DO-DS5AI001DP: differential +/-1 psi, type A output.
// Datasheet: 10%..90% of 16383 counts spans -1..+1 psi.
const float COUNTS_MIN = 0.10f * 16383.0f;
const float COUNTS_MAX = 0.90f * 16383.0f;
const float PSI_TO_PA = 6894.757f;
uint8_t consecutiveFailures = 0;
uint32_t recoveryCount = 0;
uint16_t requestedHz = 20;
const uint16_t MAX_REQUESTED_HZ = 1000;
uint32_t samplePeriodUs = 50000;
uint32_t nextSampleAt = 0;
uint32_t sampleSequence = 0;
uint32_t skippedSlots = 0;
uint32_t lastRecoveryAt = 0;
char commandBuffer[32];
uint8_t commandLength = 0;
bool commandOverflow = false;

void startI2C() {
  Wire.begin();          // Nano R4: SDA = A4, SCL = A5
  Wire.setClock(100000);
  Wire.setWireTimeout(25000);
}

// Release HIGH through pull-ups; never drive either bus line HIGH.
void pullLow(uint8_t pin) {
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
}

bool waitForClockHigh() {
  unsigned long start = micros();
  while (digitalRead(SCL) == LOW && micros() - start < 2000) {}
  return digitalRead(SCL) == HIGH;
}

void recoverI2C() {
  int beforeSDA = digitalRead(SDA);
  int beforeSCL = digitalRead(SCL);
  Wire.end();
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  delayMicroseconds(50);
  uint8_t pulses = 0;
  // NXP UM10204 bus-clear procedure: up to nine released clock pulses.
  if (waitForClockHigh() && digitalRead(SDA) == LOW) {
    while (pulses < 9 && digitalRead(SDA) == LOW) {
      pullLow(SCL);
      delayMicroseconds(10);
      pinMode(SCL, INPUT_PULLUP);
      pulses++;
      if (!waitForClockHigh()) break;
      delayMicroseconds(10);
    }
  }
  // Generate STOP only when both lines can be released.
  if (digitalRead(SCL) == HIGH && digitalRead(SDA) == HIGH) {
    pullLow(SCL);
    pullLow(SDA);
    delayMicroseconds(10);
    pinMode(SCL, INPUT_PULLUP);
    if (waitForClockHigh()) {
      delayMicroseconds(10);
      pinMode(SDA, INPUT_PULLUP);
      delayMicroseconds(10);
    }
  }
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  delayMicroseconds(50);
  Serial.print("I2C_RECOVERY attempt=");
  Serial.print(++recoveryCount);
  Serial.print(" before_SDA="); Serial.print(beforeSDA);
  Serial.print(" before_SCL="); Serial.print(beforeSCL);
  Serial.print(" released_SDA="); Serial.print(digitalRead(SDA));
  Serial.print(" released_SCL="); Serial.print(digitalRead(SCL));
  Serial.print(" clock_pulses="); Serial.println(pulses);
  startI2C();
  consecutiveFailures = 0;
  lastRecoveryAt = millis();
  delay(20);
  nextSampleAt = micros() + samplePeriodUs;
}

void handleCommand() {
  commandBuffer[commandLength] = '\0';
  if (strcmp(commandBuffer, "INFO") == 0) {
    Serial.print("HELLO,2,"); Serial.print(MAX_REQUESTED_HZ);
    Serial.print(','); Serial.println(requestedHz);
  } else if (strncmp(commandBuffer, "RATE,", 5) == 0) {
    char *end;
    long value = strtol(commandBuffer + 5, &end, 10);
    if (end != commandBuffer + 5 && *end == '\0' && value >= 1 && value <= MAX_REQUESTED_HZ) {
      requestedHz = (uint16_t)value;
      samplePeriodUs = 1000000UL / requestedHz;
      nextSampleAt = micros() + samplePeriodUs;
      Serial.print("RATE_OK,"); Serial.println(requestedHz);
    } else {
      Serial.println("RATE_ERROR,use 1 through 1000");
    }
  } else if (strcmp(commandBuffer, "r") == 0) {
    recoverI2C();
  }
}

void readCommands() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      if (!commandOverflow) handleCommand();
      else Serial.println("COMMAND_ERROR,too long");
      commandLength = 0;
      commandOverflow = false;
    } else if (commandLength < sizeof(commandBuffer) - 1) {
      commandBuffer[commandLength++] = c;
    } else {
      commandOverflow = true;
    }
  }
}

void setup() {
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 5000) delay(10);

  startI2C();
  delay(100);            // Allow sensor startup time.
  nextSampleAt = micros();
}

void loop() {
  // Commands: INFO\n, RATE,20\n, r\n. No blocking one-second delay.
  readCommands();
  if (!Serial) {
    nextSampleAt = micros() + samplePeriodUs;
    return;
  }
  uint32_t sampleTime = micros();
  if ((int32_t)(sampleTime - nextSampleAt) < 0) return;
  uint32_t lateSlots = (sampleTime - nextSampleAt) / samplePeriodUs;
  skippedSlots += lateSlots;
  nextSampleAt += (lateSlots + 1) * samplePeriodUs;
  // If USB cannot accept a complete compact line, report skipped slots later.
  if (Serial.availableForWrite() < 64) {
    skippedSlots++;
    return;
  }
  sampleSequence++;
  // Read two bytes directly: status and 14-bit raw pressure.
  // This sensor does not need a register address written first.
  uint8_t received = Wire.requestFrom(SENSOR_ADDRESS, (uint8_t)2);

  if (received != 2 || Wire.available() < 2) {
    while (Wire.available()) Wire.read();
    char packet[64];
    int length = snprintf(packet, sizeof(packet), "F,%lu,%lu,%u,%u,%lu\n",
                          (unsigned long)sampleSequence, (unsigned long)sampleTime,
                          (unsigned int)received, (unsigned int)requestedHz,
                          (unsigned long)skippedSlots);
    Serial.write((const uint8_t *)packet, length);
    if (consecutiveFailures < 255) consecutiveFailures++;
    if (consecutiveFailures >= 3 && millis() - lastRecoveryAt >= 500) recoverI2C();
  } else {
    consecutiveFailures = 0;
    uint8_t highByte = Wire.read();
    uint8_t lowByte = Wire.read();
    uint8_t status = highByte >> 6;
    uint16_t raw = ((uint16_t)(highByte & 0x3F) << 8) | lowByte;

    // D,sequence,device_micros,raw,status,requested_hz,total_skipped_slots
    char packet[64];
    int length = snprintf(packet, sizeof(packet), "D,%lu,%lu,%u,%u,%u,%lu\n",
                          (unsigned long)sampleSequence, (unsigned long)sampleTime,
                          (unsigned int)raw, (unsigned int)status,
                          (unsigned int)requestedHz, (unsigned long)skippedSlots);
    Serial.write((const uint8_t *)packet, length);
  }
}
