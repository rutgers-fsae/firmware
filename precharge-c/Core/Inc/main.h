/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f103xb.h"
#include "stm32f1xx_hal_can.h"
#include "stm32f1xx_hal_flash.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
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
  uint8_t dlc; // Originally unsigned 4 bits
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
  uint32_t timestamp; // In ms
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
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define CAN_ID 0x0D3
#define LOOP_PERIOD_MS 5
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void initCAN(void);
uint32_t packedFilterId(VoltageSpec spec);

void initController(Controller *self, ProtocolConfig config, uint32_t nowMS);
bool checkConfig(ProtocolConfig config);
bool checkSpec(VoltageSpec spec);

bool rxCAN(Frame *frame);

void ingestController(Controller *self, const Frame *frame, uint32_t nowMS);
bool isPlausible(const Controller *self, uint32_t value);
bool matchSpec(const Frame *frame, VoltageSpec spec);
DecodeError decodeVoltage(const Frame *frame, VoltageSpec spec, uint32_t *outVal);
DecodeError checkCart(const Frame *frame, VoltageSpec spec, bool *outCart);
void controllerQualify(Controller *self, uint32_t nowMS);
void controllerLatch(Controller *self, Fault fault);

void controllerTick(Controller *self, uint32_t nowMS);
uint32_t elapsed(uint32_t now, uint32_t then);

Frame infoFrame(const Controller *self);
bool txCAN(const Frame *frame);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
