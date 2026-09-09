/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static const ProtocolConfig BASE_CONFIG = {
  .ready = false, // Change to true once ready
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
  .minVoltage = 0,
  .maxVoltage = 450,
  .thresholdPercent = 90,
  .qualifyingSamples = 3,
  .freshnessTimeoutMS = 10000, // 10 sec
  .prechargeTimeoutMS = 300000 // 5 min
};

static Controller controller;
CAN_HandleTypeDef hcan;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // Set pin 7 to low

  initCAN();
  if (HAL_CAN_Start(&hcan) != HAL_OK) {
    Error_Handler();
  }

  uint32_t nowMS = HAL_GetTick();
  initController(&controller, BASE_CONFIG, nowMS);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    nowMS = HAL_GetTick();
    Frame frame;

    while (rxCAN(&frame)) {
      ingestController(&controller, &frame, nowMS);
    }
    controllerTick(&controller, nowMS);

    Frame info = infoFrame(&controller);
    txCAN(&info);

    if (controller.state == COMPLETE) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
    }
    else if (controller.state == FAULT) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    }

    HAL_Delay(LOOP_PERIOD_MS);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
uint32_t packedFilterId(VoltageSpec spec) {
  if (spec.kind == EXTENDED) {
    return ((spec.id & 0x1FFFFFFF) << 3) | (1 << 2);
  }
  else {
    return (spec.id & 0x7FF) << 21;
  }
}

void initCAN(void) {
  CAN_FilterTypeDef filter = {0};
  filter.FilterBank = 0;
  filter.FilterMode = CAN_FILTERMODE_IDLIST; // Set filter mode to List Mode
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter.FilterActivation = ENABLE;
  uint32_t bmsFilter = packedFilterId(BASE_CONFIG.bms);
  uint32_t invFilter = packedFilterId(BASE_CONFIG.inverter);
  filter.FilterIdHigh = (uint16_t)(bmsFilter >> 16); // Hold CAN ID #1
  filter.FilterIdLow = (uint16_t)(bmsFilter & 0xFFFF);
  filter.FilterMaskIdHigh = (uint16_t)(invFilter >> 16); // Hold CAN ID #2
  filter.FilterMaskIdLow = (uint16_t)(invFilter & 0xFFFF);

  HAL_CAN_ConfigFilter(&hcan, &filter);
  HAL_CAN_Start(&hcan);
}

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
  self->hasFault = !valid;
  self->fault = valid ? NONE : CONFIGURATION; // Set fault code to CONFIGURATION
  self->startedMS = nowMS;
  self->prechargingStartedMS = false;
  self->bms.valid = false;
  self->inverter.valid = false;
  self->consecutiveQualifying = 0;
}

bool rxCAN(Frame *frame) {
  if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) == 0) {
    return false;
  }

  CAN_RxHeaderTypeDef header;
  if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &header, frame->data) != HAL_OK) {
    return false;
  }

  frame->kind = (header.IDE == CAN_ID_EXT) ? EXTENDED : STANDARD;
  frame->id = (header.IDE == CAN_ID_EXT) ? header.ExtId : header.StdId;
  frame->remote = (header.RTR == CAN_RTR_REMOTE);
  frame->dlc = (uint8_t)header.DLC;
  return true;
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
  // No cart flag present but valid frame
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
  }
  else if (raw == 1) {
    *outCart = false;
    return OK;
  }

  return INVALID_CART_FLAG;
}

void controllerLatch(Controller *self, Fault fault) {
  self->state = FAULT;
  self->fault = fault;
  self->hasFault = true;
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
      } 
      else {
        raw = ((uint32_t)p[0] << 8) | (uint32_t)p[1];
      }
      break;
    case FOUR:
      if (spec.endian == LITTLE) {
        raw = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
      }
      else {
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
    return;
  }

  // Calculate 90% check
  uint64_t targetVoltage = ((uint64_t)self->bms.value * self->config.thresholdPercent) / 100ULL;

  // 3 count check
  if (self->inverter.value >= targetVoltage) {
    self->consecutiveQualifying++;
    if (self->consecutiveQualifying >= self->config.qualifyingSamples) {
      self->state = COMPLETE;
    }
  }
  else {
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

    if (!checkCart(frame, self->config.bms, &cart)) {
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
  }
  else if (matchSpec(frame, self->config.inverter)) {
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
  }
  else if (self->hasPrechargingStartedMS && elapsed(nowMS, self->prechargingStartedMS) >= self->config.freshnessTimeoutMS) {
    controllerLatch(self, STALE_INVERTER);
    return;
  }
}

bool txCAN(const Frame *frame) {
  if (frame == NULL) {
    return false;
  }

  uint32_t maxId = (frame->kind == EXTENDED) ? 0x1FFFFFFF : 0x7FF;
  if (frame->id > maxId || frame->dlc > 8) {
    return false;
  }

  CAN_TxHeaderTypeDef header = {0};
  uint32_t txMailbox = 0;

  header.DLC = frame->dlc;
  header.RTR = CAN_RTR_DATA;
  header.TransmitGlobalTime = DISABLE;

  if (frame->kind == EXTENDED) {
    header.IDE = CAN_ID_EXT;
    header.ExtId = frame->id;
  }
  else {
    header.IDE = CAN_ID_STD;
    header.StdId = frame->id;
  }

  return (HAL_CAN_AddTxMessage(&hcan, &header, (uint8_t*)frame->data, &txMailbox) == HAL_OK);
}

Frame infoFrame(const Controller *self) {
  Frame frame = {
    .id = CAN_ID,
    .kind = STANDARD,
    .remote = false,
    .dlc = 5,
    .data = {0}
  };
  if (self->hasFault) {
    frame.data[0] = (uint8_t)self->fault;
  }
  else {
    if (self->state == COMPLETE) {
      frame.data[0] = 7;
    }
    else if (self->state == PRECHARGING) {
      frame.data[0] = 8;
    }
    else {
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
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
