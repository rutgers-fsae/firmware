#pragma once
#include <cstdint>
#include <cassert>
constexpr int LOW=0, HIGH=1, OUTPUT=1, INPUT_PULLUP=2, SDA=18, SCL=19;
inline int mode[32] = {};
inline bool forceClockLow=false, forceDataLow=false;
inline int releaseAfter=100, rises=0;
inline unsigned long ticks=0;
inline void digitalWrite(uint8_t, int value) { assert(value == LOW); }
inline void pinMode(uint8_t pin, int value) {
  if (pin==SCL && mode[pin]==OUTPUT && value==INPUT_PULLUP) rises++;
  mode[pin]=value;
}
inline int digitalRead(uint8_t pin) {
  if (mode[pin]==OUTPUT) return LOW;
  if (pin==SCL && forceClockLow) return LOW;
  if (pin==SDA && forceDataLow && rises<releaseAfter) return LOW;
  return HIGH;
}
inline unsigned long micros() { return ticks+=100; }
inline unsigned long millis() { return ticks/1000; }
inline void delay(unsigned long n) { ticks+=n*1000; }
inline void delayMicroseconds(unsigned long n) { ticks+=n; }
struct SerialMock {
  void begin(int) {}
  operator bool() const { return true; }
  int available() { return 0; }
  int availableForWrite() { return 128; }
  int read() { return -1; }
  void write(const uint8_t*, int) {}
  template<class T> void print(T) {}
  template<class T> void print(T,int) {}
  template<class T> void println(T) {}
};
inline SerialMock Serial;
struct WireMock {
  int begins=0, ends=0;
  void begin() { begins++; }
  void end() { ends++; }
  void setClock(int) {}
  void setWireTimeout(int) {}
  int requestFrom(uint8_t,uint8_t) { return 0; }
  int available() { return 0; }
  int read() { return -1; }
};
inline WireMock Wire;
