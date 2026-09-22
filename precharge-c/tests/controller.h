// Generated with Gemini

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>

#define CAN_ID 0x0D3
#define LOOP_PERIOD_MS 5

typedef enum {
  STANDARD,
  EXTENDED
} FrameKind;

typedef enum {
  LITTLE,
  BIG
} Endian;

typedef enum {
  ONE = 1,
  TWO = 2,
  FOUR = 4
} Width;

typedef enum {
  WAITING_FOR_BMS,
  PRECHARGING,
  COMPLETE,
  FAULT
} State;

typedef enum {
  NONE = 0,
  STALE_BMS = 1,
  STALE_INVERTER = 2,
  CONFIGURATION = 3,
  MALFORMED_FRAME = 4,
  IMPLAUSIBLE_VOLTAGE = 5,
  TIMEOUT = 6
} Fault;

typedef enum {
  OK,
  REMOTE_FRAME,
  WRONG_FRAME,
  PAYLOAD_TOO_SHORT,
  INVALID_SCALE,
  INVALID_CART_FLAG,
  OVERFLOW
} DecodeError;

typedef struct {
  uint32_t id;
  FrameKind kind;
  bool remote;
  uint8_t dlc;
  uint8_t data[8];
} Frame;

typedef struct {
  uint32_t id;
  FrameKind kind;
  uint8_t offset;
  Width width;
  Endian endian;
  uint32_t multiplier;
  uint32_t divisor;
  uint8_t cartOffset;
} VoltageSpec;

typedef struct {
  bool ready;
  uint32_t bitrate;
  VoltageSpec bms;
  VoltageSpec inverter;
  uint32_t minVoltage;
  uint32_t maxVoltage;
  uint8_t thresholdPercent;
  uint8_t qualifyingSamples;
  uint32_t freshnessTimeoutMS;
  uint32_t prechargeTimeoutMS;
} ProtocolConfig;

typedef struct {
  uint32_t value;
  uint32_t timestamp;
  bool valid;
} Reading;

typedef struct {
  ProtocolConfig config;
  State state;
  Fault fault;
  uint32_t startedMS;
  bool hasPrechargingStartedMS;
  uint32_t prechargingStartedMS;
  Reading bms;
  Reading inverter;
  uint8_t consecutiveQualifying;
} Controller;

/* Pure Business Logic API */
void initController(Controller *self, ProtocolConfig config, uint32_t nowMS);
bool checkConfig(ProtocolConfig config);
bool checkSpec(VoltageSpec spec);
void ingestController(Controller *self, const Frame *frame, uint32_t nowMS);
void controllerTick(Controller *self, uint32_t nowMS);
bool isPlausible(const Controller *self, uint32_t value);
bool matchSpec(const Frame *frame, VoltageSpec spec);
DecodeError decodeVoltage(const Frame *frame, VoltageSpec spec, uint32_t *outVal);
DecodeError checkCart(const Frame *frame, VoltageSpec spec, bool *outCart);
void controllerQualify(Controller *self, uint32_t nowMS);
void controllerLatch(Controller *self, Fault fault);
uint32_t elapsed(uint32_t now, uint32_t then);
Frame infoFrame(const Controller *self);

#endif /* CONTROLLER_H */