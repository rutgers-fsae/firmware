/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* USER CODE BEGIN PTD */
typedef enum
{
    MUX1 = 0,
    MUX2 = 1,
    MUX3 = 2
} mux_id_t;
/* USER CODE END PTD */

/* USER CODE BEGIN PD */
#define NUM_MUXES               3U
#define MUX_CHANNELS_PER_CHIP   32U
#define MUX_DISABLE_CMD         0x80U
/* USER CODE END PD */


ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
static GPIO_TypeDef *const mux_sync_port[NUM_MUXES] =
{
    GPIOB, GPIOB, GPIOB
};

static const uint16_t mux_sync_pin[NUM_MUXES] =
{
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2
};

static const char *const mux_names[NUM_MUXES][MUX_CHANNELS_PER_CHIP] =
{
    {
        "TEMP101","TEMP102","TEMP103","TEMP104","TEMP105","TEMP106","TEMP107","TEMP108",
        "TEMP109","TEMP110","TEMP111","TEMP112","TEMP113","TEMP114","TEMP115","TEMP116",
        "TEMP117","TEMP118","TEMP201","TEMP202","TEMP203","TEMP204","TEMP205","TEMP206",
        "TEMP207","TEMP208","TEMP209","TEMP210","TEMP211","TEMP212","TEMP213","TEMP214"
    },
    {
        "TEMP215","TEMP216","TEMP217","TEMP218","TEMP301","TEMP302","TEMP303","TEMP304",
        "TEMP305","TEMP306","TEMP307","TEMP308","TEMP309","TEMP310","TEMP311","TEMP312",
        "TEMP313","TEMP314","TEMP315","TEMP316","TEMP317","TEMP318","TEMP401","TEMP402",
        "TEMP403","TEMP404","TEMP405","TEMP406","TEMP407","TEMP408","TEMP409","TEMP410"
    },
    {
        "TEMP411","TEMP412","TEMP413","TEMP414","TEMP415","TEMP416","TEMP417","TEMP418",
        "TEMP501","TEMP502","TEMP503","TEMP504","TEMP505","TEMP506","TEMP507","TEMP508",
        "TEMP509","TEMP510","TEMP511","TEMP512","TEMP513","TEMP514","TEMP515","TEMP516",
        "TEMP517","TEMP518","UNUSED27","UNUSED28","UNUSED29","UNUSED30","UNUSED31","UNUSED32"
    }
};
/* USER CODE END PV */

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_CAN_Init(void);
static void MX_TIM4_Init(void);

/* USER CODE BEGIN PFP */

extern void initialise_monitor_handles(void);

static void ADC1_SetLongSampleTime(void);
static HAL_StatusTypeDef MUX_WriteRaw(mux_id_t mux, uint8_t cmd);
static HAL_StatusTypeDef MUX_Disable(mux_id_t mux);
static void MUX_DisableAll(void);
static HAL_StatusTypeDef MUX_SelectChannel(mux_id_t mux, uint8_t channel);
static uint16_t ADC1_ReadRaw(void);
static uint16_t ADC1_ReadRawSettled(void);
static float SensorVoltageToTempC(float voltage);
static void ScanAllMuxChannels(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

static void ADC1_SetLongSampleTime(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

static HAL_StatusTypeDef MUX_WriteRaw(mux_id_t mux, uint8_t cmd)
{
    HAL_StatusTypeDef status;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(mux_sync_port[mux], mux_sync_pin[mux], GPIO_PIN_RESET);

    status = HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(mux_sync_port[mux], mux_sync_pin[mux], GPIO_PIN_SET);

    return status;
}

static HAL_StatusTypeDef MUX_Disable(mux_id_t mux)
{
    return MUX_WriteRaw(mux, MUX_DISABLE_CMD);
}

static void MUX_DisableAll(void)
{
    (void)MUX_Disable(MUX1);
    (void)MUX_Disable(MUX2);
    (void)MUX_Disable(MUX3);
}

static HAL_StatusTypeDef MUX_SelectChannel(mux_id_t mux, uint8_t channel)
{
    uint8_t i;

    if (channel >= MUX_CHANNELS_PER_CHIP)
    {
        return HAL_ERROR;
    }

    for (i = 0; i < NUM_MUXES; i++)
    {
        if (i != (uint8_t)mux)
        {
            if (MUX_Disable((mux_id_t)i) != HAL_OK)
            {
                return HAL_ERROR;
            }
        }
    }

    if (MUX_WriteRaw(mux, (uint8_t)(channel & 0x1FU)) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

static uint16_t ADC1_ReadRaw(void)
{
    uint16_t value = 0;

    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }

    value = (uint16_t)HAL_ADC_GetValue(&hadc1);

    if (HAL_ADC_Stop(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    return value;
}

static uint16_t ADC1_ReadRawSettled(void)
{
    (void)ADC1_ReadRaw();
    return ADC1_ReadRaw();
}

static float SensorVoltageToTempC(float voltage)
{
    static const float temp_table[] = {
        -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40,
         45,  50,  55,  60,  65,  70,  75, 80, 85, 90, 95,100,105,110,115,120
    };

    static const float volt_table[] = {
        2.44f,2.42f,2.40f,2.38f,2.35f,2.32f,2.27f,2.23f,2.17f,2.11f,2.05f,1.99f,1.92f,1.86f,1.80f,1.74f,1.68f,
        1.63f,1.59f,1.55f,1.51f,1.48f,1.45f,1.43f,1.40f,1.38f,1.37f,1.35f,1.34f,1.33f,1.32f,1.31f,1.30f
    };

    const uint32_t n = sizeof(temp_table) / sizeof(temp_table[0]);
    uint32_t i;

    if (voltage >= volt_table[0]) return temp_table[0];
    if (voltage <= volt_table[n - 1]) return temp_table[n - 1];

    for (i = 0; i < n - 1; i++)
    {
        if ((voltage <= volt_table[i]) && (voltage >= volt_table[i + 1]))
        {
            float v1 = volt_table[i];
            float v2 = volt_table[i + 1];
            float t1 = temp_table[i];
            float t2 = temp_table[i + 1];

            return t1 + (voltage - v1) * (t2 - t1) / (v2 - v1);
        }
    }

    return -999.0f;
}

// 1.85 V

static void ScanAllMuxChannels(void)
{
    uint8_t mux;
    uint8_t ch;

    for (mux = 0; mux < NUM_MUXES; mux++)
    {
        for (ch = 0; ch < MUX_CHANNELS_PER_CHIP; ch++)
        {
            if (MUX_SelectChannel((mux_id_t)mux, ch) != HAL_OK)
            {
                printf("MUX %u CH %u: select failed\r\n", mux + 1U, ch + 1U);
                continue;
            }

            HAL_Delay(25);

            {
                uint16_t raw = ADC1_ReadRawSettled();
                float voltage = (3.0f * (float)raw) / 4095.0f;
                float temp_c = SensorVoltageToTempC(voltage);

                printf("MUX%u CH%02u %-8s : ADC=%4u  V=%.3f  T=%.1f C\r\n",
                       mux + 1U,
                       ch + 1U,
                       mux_names[mux][ch],
                       raw,
                       voltage,
                       temp_c);
            }
        }
    }

    MUX_DisableAll();
}

// temp501
// temp 511

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  initialise_monitor_handles();

  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_CAN_Init();
  MX_TIM4_Init();

  /* USER CODE BEGIN 2 */

  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
  {
      Error_Handler();
  }

  ADC1_SetLongSampleTime();
  MUX_DisableAll();

  printf("\r\n--- Temp mux scan start ---\r\n");
  /* USER CODE END 2 */

  HAL_Delay(10);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    ScanAllMuxChannels();
    printf("--- sweep done ---\r\n\r\n");
    HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* USER CODE END 3 */
  }
  /* USER CODE END WHILE */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV16;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_RCC_EnableCSS();
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 0 */
  /* USER CODE END ADC1_Init 0 */

  /* USER CODE BEGIN ADC1_Init 1 */
  /* USER CODE END ADC1_Init 1 */

  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN ADC1_Init 2 */
  /* USER CODE END ADC1_Init 2 */
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{
  /* USER CODE BEGIN CAN_Init 0 */
  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */
  /* USER CODE END CAN_Init 1 */

  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN CAN_Init 2 */
  /* USER CODE END CAN_Init 2 */
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
  /* USER CODE BEGIN SPI1_Init 0 */
  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */
  /* USER CODE END SPI1_Init 1 */

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN SPI1_Init 2 */
  /* USER CODE END SPI1_Init 2 */
}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 0 */
  /* USER CODE END TIM4_Init 0 */

  /* USER CODE BEGIN TIM4_Init 1 */
  /* USER CODE END TIM4_Init 1 */

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN TIM4_Init 2 */
  /* USER CODE END TIM4_Init 2 */
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
  /* USER CODE BEGIN USART1_Init 0 */
  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */
  /* USER CODE END USART1_Init 1 */

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN USART1_Init 2 */
  /* USER CODE END USART1_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
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
  (void)file;
  (void)line;
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
