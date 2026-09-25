#include <Wire.h>

void setup() {
  Serial.begin(115200);
  // Give Serial Monitor up to 5 seconds to connect.
  unsigned long start = millis();
  while (!Serial && millis() - start < 5000) {
    delay(10);
  }

  Wire.begin();          // Nano R4: SDA = A4, SCL = A5
  Wire.setClock(100000); // 100 kHz I2C clock
  Serial.println("Nano R4 I2C scanner ready.");
}

void loop() {
  int found = 0;
  int errors = 0;
  Serial.println("Scanning A4/SDA and A5/SCL...");

  // Normal 7-bit addresses; skip reserved addresses.
  for (byte address = 0x08; address <= 0x77; address++) {
    Wire.beginTransmission(address);
    byte result = Wire.endTransmission();

    if (result == 0) {
      Serial.print("Device found at 0x");
      if (address < 0x10) Serial.print('0');
      Serial.println(address, HEX);
      found++;
    } else if (result != 2) {
      // Code 2 means no device acknowledged this address.
      Serial.print("I2C error at 0x");
      if (address < 0x10) Serial.print('0');
      Serial.print(address, HEX);
      Serial.print("; error code = ");
      Serial.println(result);
      errors++;
    }
  }

  if (found == 0) Serial.println("NO I2C DEVICES DETECTED.");
  Serial.print("Devices found: ");
  Serial.println(found);
  Serial.print("Bus errors: ");
  Serial.println(errors);
  Serial.println();
  delay(3000);
}
