// Generated with Gemini

#include "controller.h"
#include <stddef.h>

bool checkSpec(VoltageSpec spec) {
  if (spec.kind == STANDARD && spec.id > 0x7FF) {
    return false;
  }
  if (spec.kind == EXTENDED && spec.id > 0x1FFFFFFF) {
    return false;
  }
  if ((spec.offset + (uint8_t)spec.width) > 8) {
    return false;
  }
  if (spec.divisor == 0) {
    return false;
  }
  return true;
}

bool checkConfig(ProtocolConfig config) {
  return config.ready
    && checkSpec(config.bms)
    && checkSpec(config.inverter)
    && !(config.bms.id == config.inverter.id && config.bms.kind == config.inverter.kind)
    && (config.minVoltage <= config.maxVoltage)
    && (config.maxVoltage <= 0xFFFF)
    && (config.thresholdPercent > 0)
    && (config.thresholdPercent <= 100)
    && (config.qualifyingSamples > 0)
    && (config.freshnessTimeoutMS > 0)
    && (config.prechargeTimeoutMS > 0);
}

void initController(Controller *self, ProtocolConfig config, uint32_t nowMS) {
  self->config = config;
  bool valid = checkConfig(config);
  self->state = valid ? WAITING_FOR_BMS : FAULT;
  self->fault = valid ? NONE : CONFIGURATION;
  self->startedMS = nowMS;
  self->prechargingStartedMS = 0;
  self->hasPrechargingStartedMS = false;
  self->bms.valid = false;
  self->inverter.valid = false;
  self->consecutiveQualifying = 0;
}

bool matchSpec(const Frame *frame, VoltageSpec spec) {
  return (frame->id == spec.id) && (frame->kind == spec.kind);
}

DecodeError checkCart(const Frame *frame, VoltageSpec spec, bool *outCart) {
  if (frame->remote) {
    return REMOTE_FRAME;
  }
  if (!matchSpec(frame, spec)) {
    return WRONG_FRAME;
  }
  if (!spec.cartOffset) {
    *outCart = false;
    return OK;
  }
  if (frame->dlc <= spec.cartOffset) {
    return PAYLOAD_TOO_SHORT;
  }

  uint8_t raw = frame->data[spec.cartOffset];
  if (raw == 0) {
    *outCart = true;
    return OK;
  } else if (raw == 1) {
    *outCart = false;
    return OK;
  }

  return INVALID_CART_FLAG;
}

void controllerLatch(Controller *self, Fault fault) {
  self->state = FAULT;
  self->fault = fault;
}

DecodeError decodeVoltage(const Frame *frame, VoltageSpec spec, uint32_t *outVal) {
  if (frame->remote) return REMOTE_FRAME;
  if (!matchSpec(frame, spec)) return WRONG_FRAME;
  if (frame->dlc < (spec.offset + (uint8_t)spec.width)) return PAYLOAD_TOO_SHORT;
  if (spec.divisor == 0) return INVALID_SCALE;

  uint32_t raw = 0;
  const uint8_t *p = &frame->data[spec.offset];

  switch (spec.width) {
    case ONE:
      raw = p[0];
      break;
    case TWO:
      if (spec.endian == LITTLE) {
        raw = (uint32_t)p[0] | ((uint32_t)p[1] << 8);
      } else {
        raw = ((uint32_t)p[0] << 8) | (uint32_t)p[1];
      }
      break;
    case FOUR:
      if (spec.endian == LITTLE) {
        raw = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
      } else {
        raw = ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
      }
      break;
  }

  uint64_t scaled = (uint64_t)raw * spec.multiplier;
  if (scaled > 0xFFFFFFFFULL) {
    return OVERFLOW;
  }

  *outVal = (uint32_t)(scaled / spec.divisor);
  return OK;
}

bool isPlausible(const Controller *self, uint32_t value) {
  return (value >= self->config.minVoltage && value <= self->config.maxVoltage);
}

void controllerQualify(Controller *self, uint32_t nowMS) {
  if (!self->bms.valid) {
    self->consecutiveQualifying = 0;
    return;
  }
  if (elapsed(nowMS, self->bms.timestamp) >= self->config.freshnessTimeoutMS) {
    controllerLatch(self, STALE_BMS);
    return;
  }

  uint64_t lhs = (uint64_t)self->inverter.value * 100ULL;
  uint64_t rhs = (uint64_t)self->bms.value * self->config.thresholdPercent;

  if (lhs >= rhs) {
    self->consecutiveQualifying++;
    if (self->consecutiveQualifying >= self->config.qualifyingSamples) {
      self->state = COMPLETE;
    }
  } else {
    self->consecutiveQualifying = 0;
  }
}

void ingestController(Controller *self, const Frame *frame, uint32_t nowMS) {
  if (self->state == COMPLETE || self->state == FAULT) {
    return;
  }

  if (matchSpec(frame, self->config.bms)) {
    bool cart = false;
    DecodeError cartError = checkCart(frame, self->config.bms, &cart);
    if (cartError != OK) {
      controllerLatch(self, MALFORMED_FRAME);
      return;
    }
    if (self->state == WAITING_FOR_BMS && cart) {
      self->state = COMPLETE;
      return;
    }
    uint32_t value = 0;
    if (decodeVoltage(frame, self->config.bms, &value) != OK) {
      controllerLatch(self, MALFORMED_FRAME);
      return;
    }
    self->bms.value = value;
    self->bms.timestamp = nowMS;
    self->bms.valid = true;

    if (!isPlausible(self, value)) {
      controllerLatch(self, IMPLAUSIBLE_VOLTAGE);
      return;
    }
    if (self->state == WAITING_FOR_BMS) {
      self->state = PRECHARGING;
      self->prechargingStartedMS = nowMS;
      self->hasPrechargingStartedMS = true;
    }
  } else if (matchSpec(frame, self->config.inverter)) {
    uint32_t value = 0;
    if (decodeVoltage(frame, self->config.inverter, &value) != OK) {
      controllerLatch(self, MALFORMED_FRAME);
      return;
    }
    self->inverter.value = value;
    self->inverter.timestamp = nowMS;
    self->inverter.valid = true;

    if (!isPlausible(self, value)) {
      controllerLatch(self, IMPLAUSIBLE_VOLTAGE);
      return;
    }
    controllerQualify(self, nowMS);
  }
}

uint32_t elapsed(uint32_t now, uint32_t then) {
  return now - then;
}

void controllerTick(Controller *self, uint32_t nowMS) {
  if (self->state == COMPLETE || self->state == FAULT) {
    return;
  }
  if (elapsed(nowMS, self->startedMS) >= self->config.prechargeTimeoutMS) {
    controllerLatch(self, TIMEOUT);
    return;
  }
  if (self->state == WAITING_FOR_BMS) {
    if (elapsed(nowMS, self->startedMS) >= self->config.freshnessTimeoutMS) {
      controllerLatch(self, STALE_BMS);
    }
    return;
  }
  if (!self->bms.valid || elapsed(nowMS, self->bms.timestamp) >= self->config.freshnessTimeoutMS) {
    controllerLatch(self, STALE_BMS);
    return;
  }
  if (self->inverter.valid) {
    if (elapsed(nowMS, self->inverter.timestamp) >= self->config.freshnessTimeoutMS) {
      controllerLatch(self, STALE_INVERTER);
      return;
    }
  } else if (self->hasPrechargingStartedMS && elapsed(nowMS, self->prechargingStartedMS) >= self->config.freshnessTimeoutMS) {
    controllerLatch(self, STALE_INVERTER);
    return;
  }
}

Frame infoFrame(const Controller *self) {
  Frame frame = {
    .id = CAN_ID,
    .kind = STANDARD,
    .remote = false,
    .dlc = 5,
    .data = {0}
  };
  if (self->fault != NONE) {
    frame.data[0] = (uint8_t)self->fault;
  } else {
    if (self->state == COMPLETE) {
      frame.data[0] = 7;
    } else if (self->state == PRECHARGING) {
      frame.data[0] = 8;
    } else {
      frame.data[0] = 0;
    }
  }

  if (self->bms.valid) {
    frame.data[1] = (uint8_t)((self->bms.value >> 8) & 0xFF);
    frame.data[2] = (uint8_t)(self->bms.value & 0xFF);
  }
  if (self->inverter.valid) {
    frame.data[3] = (uint8_t)((self->inverter.value >> 8) & 0xFF);
    frame.data[4] = (uint8_t)(self->inverter.value & 0xFF);
  }
  return frame;
}