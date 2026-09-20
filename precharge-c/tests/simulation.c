// Generated with Gemini

#include <stdio.h>
#include <stdbool.h>
#include "controller.h"

static const ProtocolConfig BASE_CONFIG = {
  .ready = true,
  .bitrate = 500000,
  .bms = {
    .id = 0x6B1,
    .kind = STANDARD,
    .offset = 2,
    .width = TWO,
    .endian = BIG,
    .multiplier = 1,
    .divisor = 1,
    .cartOffset = 5
  },
  .inverter = {
    .id = 0x0A7,
    .kind = EXTENDED,
    .offset = 0,
    .width = TWO,
    .endian = LITTLE,
    .multiplier = 1,
    .divisor = 10,
    .cartOffset = 0
  },
  .minVoltage = 100,             // Lower bound set to 100 V
  .maxVoltage = 450,             // Upper bound set to 450 V
  .thresholdPercent = 90,        // 90% target threshold
  .qualifyingSamples = 3,        // Requires 3 consecutive valid samples
  .freshnessTimeoutMS = 1000,    // 1 second timeout
  .prechargeTimeoutMS = 30000    // 30 seconds max precharge window
};

/* Helper Functions */
static const char* stateToString(State state) {
  switch (state) {
    case WAITING_FOR_BMS: return "WAITING_FOR_BMS";
    case PRECHARGING:     return "PRECHARGING";
    case COMPLETE:        return "COMPLETE";
    case FAULT:           return "FAULT";
    default:              return "UNKNOWN";
  }
}

static const char* faultToString(Fault fault) {
  switch (fault) {
    case NONE:                 return "NONE";
    case STALE_BMS:            return "STALE_BMS";
    case STALE_INVERTER:       return "STALE_INVERTER";
    case CONFIGURATION:        return "CONFIGURATION";
    case MALFORMED_FRAME:      return "MALFORMED_FRAME";
    case IMPLAUSIBLE_VOLTAGE:  return "IMPLAUSIBLE_VOLTAGE";
    case TIMEOUT:              return "TIMEOUT";
    default:                   return "UNKNOWN";
  }
}

static Frame makeBMSFrame(uint16_t voltage, uint8_t cartFlag, uint8_t dlc) {
  Frame f = {
    .id = 0x6B1,
    .kind = STANDARD,
    .remote = false,
    .dlc = dlc,
    .data = {0}
  };
  f.data[2] = (uint8_t)((voltage >> 8) & 0xFF);
  f.data[3] = (uint8_t)(voltage & 0xFF);
  if (dlc > 5) {
    f.data[5] = cartFlag;
  }
  return f;
}

static Frame makeInverterFrame(uint16_t rawVoltageTenths) {
  return (Frame){
    .id = 0x0A7,
    .kind = EXTENDED,
    .remote = false,
    .dlc = 2,
    .data = { (uint8_t)(rawVoltageTenths & 0xFF), (uint8_t)(rawVoltageTenths >> 8) }
  };
}

static void logStep(uint32_t nowMS, const Controller *ctrl, const char *eventDesc) {
  Frame tx = infoFrame(ctrl);
  printf("[%05u ms] %-42s | State: %-15s | Fault: %-19s | Tx: [%02X %02X %02X %02X %02X]\n",
         nowMS, eventDesc, stateToString(ctrl->state), faultToString(ctrl->fault),
         tx.data[0], tx.data[1], tx.data[2], tx.data[3], tx.data[4]);
}

/* --- Scenario Tests --- */

static void testScenario1_PlausibilityFault_VoltageTooLow(void) {
  printf("\n=== Scenario 1: BMS Voltage Below Minimum Plausible Limit (< 100V) ===\n");
  Controller ctrl;
  uint32_t nowMS = 0;
  initController(&ctrl, BASE_CONFIG, nowMS);

  logStep(nowMS, &ctrl, "Controller Initialized");

  // Ingest BMS frame with 80V payload (minVoltage = 100V)
  nowMS += 100;
  Frame lowBMS = makeBMSFrame(80, 1, 6); // 80V, Cart=1 (False)
  ingestController(&ctrl, &lowBMS, nowMS);
  logStep(nowMS, &ctrl, "Rx BMS Frame (80V < 100V Min)");

  controllerTick(&ctrl, nowMS);
}

static void testScenario2_QualificationResetOnTransientDip(void) {
  printf("\n=== Scenario 2: Qualification Count Resets on Transient Voltage Dip ===\n");
  Controller ctrl;
  uint32_t nowMS = 0;
  initController(&ctrl, BASE_CONFIG, nowMS);

  // Send valid BMS Frame (400V)
  nowMS += 100;
  Frame bmsFrame = makeBMSFrame(400, 1, 6);
  ingestController(&ctrl, &bmsFrame, nowMS);
  logStep(nowMS, &ctrl, "Rx BMS Frame (400V)");

  // Inverter Sample 1: 365V (91.25% >= 90%) -> Count 1
  nowMS += 100;
  Frame inv1 = makeInverterFrame(3650);
  ingestController(&ctrl, &inv1, nowMS);
  logStep(nowMS, &ctrl, "Rx Inv Sample 1 (365V >= 90%) [Count: 1]");

  // Inverter Sample 2: 368V (92%) -> Count 2
  nowMS += 100;
  Frame inv2 = makeInverterFrame(3680);
  ingestController(&ctrl, &inv2, nowMS);
  logStep(nowMS, &ctrl, "Rx Inv Sample 2 (368V >= 90%) [Count: 2]");

  // Inverter Sample 3: Dip to 340V (85% < 90%) -> Count resets to 0
  nowMS += 100;
  Frame inv3 = makeInverterFrame(3400);
  ingestController(&ctrl, &inv3, nowMS);
  logStep(nowMS, &ctrl, "Rx Inv Sample 3 (340V < 90%) [Count Reset]");

  // Inverter Sample 4: 370V -> Count restarts at 1
  nowMS += 100;
  Frame inv4 = makeInverterFrame(3700);
  ingestController(&ctrl, &inv4, nowMS);
  logStep(nowMS, &ctrl, "Rx Inv Sample 4 (370V >= 90%) [Count: 1]");
}

static void testScenario3_CartFlagInstantComplete(void) {
  printf("\n=== Scenario 3: BMS Signals Cart Connection (Instant Complete) ===\n");
  Controller ctrl;
  uint32_t nowMS = 0;
  initController(&ctrl, BASE_CONFIG, nowMS);

  // Ingest BMS frame where cart byte at index 5 is 0 (True)
  nowMS += 100;
  Frame cartBms = makeBMSFrame(400, 0, 6); // cartOffset = 5, raw byte = 0 -> Cart connected
  ingestController(&ctrl, &cartBms, nowMS);
  logStep(nowMS, &ctrl, "Rx BMS Frame with Cart Connected (Flag=0)");
}

static void testScenario4_MalformedFrameTruncatedPayload(void) {
  printf("\n=== Scenario 4: Malformed Frame (Truncated Payload DLC) ===\n");
  Controller ctrl;
  uint32_t nowMS = 0;
  initController(&ctrl, BASE_CONFIG, nowMS);

  // Config expects 2 bytes voltage at offset 2 (requires dlc >= 4). Send dlc = 3.
  nowMS += 100;
  Frame shortFrame = makeBMSFrame(400, 1, 3);
  ingestController(&ctrl, &shortFrame, nowMS);
  logStep(nowMS, &ctrl, "Rx Truncated BMS Frame (DLC=3)");
}

static void testScenario5_InverterStaleTimeout(void) {
  printf("\n=== Scenario 5: Inverter Message Timeout During Precharge ===\n");
  Controller ctrl;
  uint32_t nowMS = 0;
  initController(&ctrl, BASE_CONFIG, nowMS);

  // Ingest valid BMS frame
  nowMS += 100;
  Frame bmsFrame = makeBMSFrame(400, 1, 6);
  ingestController(&ctrl, &bmsFrame, nowMS);
  logStep(nowMS, &ctrl, "Rx BMS Frame (400V) -> PRECHARGING");

  // Advance time beyond freshnessTimeoutMS (1000ms) without inverter message
  nowMS += 1200;
  controllerTick(&ctrl, nowMS);
  logStep(nowMS, &ctrl, "Tick (+1200ms without Inverter update)");
}

int main(void) {
  printf("============================================================\n");
  printf("      ADVANCED CAN PRECHARGE CONTROLLER SIMULATION SUITE    \n");
  printf("============================================================\n");

  testScenario1_PlausibilityFault_VoltageTooLow();
  testScenario2_QualificationResetOnTransientDip();
  testScenario3_CartFlagInstantComplete();
  testScenario4_MalformedFrameTruncatedPayload();
  testScenario5_InverterStaleTimeout();

  printf("\n============================================================\n");
  printf("                     SIMULATION COMPLETE                    \n");
  printf("============================================================\n");
  return 0;
}