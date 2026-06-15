#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2  // DQ connected to pin 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  sensors.begin();

    if (sensors.isParasitePowerMode()) {
    Serial.println("Parasite power mode: ON");
  } else {
    Serial.println("Normal power mode (check wiring)");
  }

  // Tell the library this sensor uses parasite power
  // (it will use strong pullup during conversion)
  Serial.println("DS18B20 Parasite Mode");
}

void loop() {
  // requestTemperatures() with parasite mode needs a
  // strong pullup on the line during conversion (750ms)
  sensors.setWaitForConversion(true);   // blocking wait
  sensors.requestTemperatures();        // sends conversion command

  for (int i = 0; i < 5; i++) {
    float tempC = sensors.getTempCByIndex(i);

    if (tempC == DEVICE_DISCONNECTED_C) {
      Serial.println("Error: sensor not found");
    } else {
      Serial.print("Sensor ");
      Serial.println(i);

      Serial.print("Temp: ");
      Serial.print(tempC);
      Serial.print(" °C  /  ");
      Serial.print(DallasTemperature::toFahrenheit(tempC));
      Serial.println(" °F");
    }

    delay(1000);
  }
}
