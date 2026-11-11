#include "../../firmware/NanoR4Airspeed/NanoR4Airspeed.ino"
#include <iostream>
void reset() {
  mode[SDA]=mode[SCL]=INPUT_PULLUP;
  forceClockLow=forceDataLow=false;
  rises=0; releaseAfter=100; ticks=0;
  Wire.begins=Wire.ends=0;
  consecutiveFailures=0;
  nextSampleAt=0; lastRecoveryAt=0;
}
int main() {
  reset(); recoverI2C();
  assert(Wire.begins==1 && Wire.ends==1 && rises==1);
  reset(); forceDataLow=true; releaseAfter=3; recoverI2C();
  assert(rises==4 && digitalRead(SDA)==HIGH);
  reset(); forceDataLow=true; recoverI2C();
  assert(rises==9 && digitalRead(SDA)==LOW);
  reset(); forceClockLow=true; recoverI2C();
  assert(rises==0 && ticks<30000); // Includes the deliberate 20 ms settling delay.
  reset(); ticks=1000000; loop(); ticks+=samplePeriodUs; loop(); assert(Wire.begins==0);
  ticks+=samplePeriodUs;
  loop(); assert(Wire.begins==1 && consecutiveFailures==0);
  assert(mode[SDA]==INPUT_PULLUP && mode[SCL]==INPUT_PULLUP);
  std::cout << "PASS: idle recovery, released SDA, stuck SDA pulse limit, stuck SCL timeout, 3-failure trigger; no driven HIGH\n";
}
