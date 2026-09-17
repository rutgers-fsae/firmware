#include "stm32f1xx_hal.h"
#include "bootloader_config.h"
#include "bootloader_protocol.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t magic;
    uint32_t image_size;
    uint32_t image_crc32;
    uint32_t reserved;
} bl_metadata_t;

typedef enum { RX_IDLE, RX_WAITING_CRC, RX_DATA } rx_state_t;

static CAN_HandleTypeDef hcan;
static rx_state_t rx_state;
static uint32_t image_size;
static uint32_t expected_crc;
static uint32_t bytes_written;
static uint16_t expected_sequence;
static uint8_t pending_byte;
static bool has_pending_byte;

static void SystemClock_Config(void);
static void CAN_Init(void);
static void CAN_Filter_Init(void);
static void process_frame(const CAN_RxHeaderTypeDef *header, const uint8_t data[8]);
static void process_command(const uint8_t data[8], uint8_t length);
static void process_data(const uint8_t data[8], uint8_t length);
static void send_status(bl_status_t status, uint8_t command, uint16_t sequence, uint32_t detail);
static bool erase_application(uint32_t size);
static bool write_bytes(const uint8_t *data, uint8_t length);
static bool finish_pending_byte(void);
static uint32_t crc32_flash(uint32_t address, uint32_t length);
static bool metadata_write(void);
static bool application_is_valid(void);
static void jump_to_application(void);

/* Minimal C runtime function required by compiler-generated structure clears. */
void *memset(void *destination, int value, unsigned int length) {
    uint8_t *bytes = (uint8_t *)destination;
    for (unsigned int i = 0U; i < length; ++i) bytes[i] = (uint8_t)value;
    return destination;
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    CAN_Init();
    CAN_Filter_Init();
    if (HAL_CAN_Start(&hcan) != HAL_OK) {
        while (1) { }
    }

    rx_state = RX_IDLE;
    const uint32_t deadline = HAL_GetTick() + BL_UPDATE_WINDOW_MS;

    while (1) {
        CAN_RxHeaderTypeDef header;
        uint8_t data[8];
        if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) != 0U &&
            HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &header, data) == HAL_OK) {
            process_frame(&header, data);
        }

        if (rx_state == RX_IDLE && application_is_valid() &&
            (int32_t)(HAL_GetTick() - deadline) >= 0) {
            jump_to_application();
        }
    }
}

static void process_frame(const CAN_RxHeaderTypeDef *header, const uint8_t data[8]) {
    if (header->IDE != CAN_ID_STD || header->RTR != CAN_RTR_DATA) return;
    if (header->StdId == BL_CAN_DISCOVERY_ID) {
        send_status(BL_STATUS_READY, BL_CMD_INFO, 0U,
                    ((uint32_t)BL_PROTOCOL_VERSION << 24) | (uint32_t)BL_MAX_IMAGE_SIZE);
    } else if (header->StdId == BL_CAN_COMMAND_BASE_ID + BL_NODE_ID) {
        process_command(data, (uint8_t)header->DLC);
    } else if (header->StdId == BL_CAN_DATA_BASE_ID + BL_NODE_ID) {
        process_data(data, (uint8_t)header->DLC);
    }
}

static void process_command(const uint8_t data[8], uint8_t length) {
    if (length == 0U) return;
    const uint8_t command = data[0];
    uint32_t value = 0U;
    if (length >= 5U) {
        value = (uint32_t)data[1] | ((uint32_t)data[2] << 8) |
                ((uint32_t)data[3] << 16) | ((uint32_t)data[4] << 24);
    }

    switch (command) {
    case BL_CMD_INFO:
        send_status(BL_STATUS_READY, command, expected_sequence, BL_MAX_IMAGE_SIZE);
        break;
    case BL_CMD_BEGIN:
        if (length != 5U || value == 0U || value > BL_MAX_IMAGE_SIZE) {
            send_status(BL_STATUS_BAD_LENGTH, command, 0U, BL_MAX_IMAGE_SIZE);
            break;
        }
        image_size = value;
        expected_crc = 0U;
        bytes_written = 0U;
        expected_sequence = 0U;
        has_pending_byte = false;
        if (!erase_application(image_size)) {
            send_status(BL_STATUS_FLASH_ERROR, command, 0U, HAL_FLASH_GetError());
            break;
        }
        rx_state = RX_WAITING_CRC;
        send_status(BL_STATUS_ACK, command, 0U, image_size);
        break;
    case BL_CMD_SET_CRC:
        if (length != 5U || rx_state != RX_WAITING_CRC) {
            send_status(BL_STATUS_BAD_STATE, command, expected_sequence, 0U);
            break;
        }
        expected_crc = value;
        rx_state = RX_DATA;
        send_status(BL_STATUS_ACK, command, 0U, expected_crc);
        break;
    case BL_CMD_END:
        if (rx_state != RX_DATA || bytes_written != image_size || !finish_pending_byte()) {
            send_status(BL_STATUS_BAD_STATE, command, expected_sequence, bytes_written);
            break;
        }
        if (crc32_flash(BL_APPLICATION_ADDRESS, image_size) != expected_crc) {
            rx_state = RX_IDLE;
            send_status(BL_STATUS_BAD_IMAGE, command, expected_sequence, bytes_written);
            break;
        }
        if (!metadata_write()) {
            send_status(BL_STATUS_FLASH_ERROR, command, expected_sequence, HAL_FLASH_GetError());
            break;
        }
        rx_state = RX_IDLE;
        send_status(BL_STATUS_COMPLETE, command, expected_sequence, expected_crc);
        break;
    case BL_CMD_ABORT:
        rx_state = RX_IDLE;
        send_status(BL_STATUS_ACK, command, expected_sequence, bytes_written);
        break;
    case BL_CMD_START_APPLICATION:
        if (!application_is_valid()) send_status(BL_STATUS_BAD_IMAGE, command, 0U, 0U);
        else { send_status(BL_STATUS_ACK, command, 0U, 0U); HAL_Delay(10U); jump_to_application(); }
        break;
    case BL_CMD_RESET:
        send_status(BL_STATUS_ACK, command, 0U, 0U);
        HAL_Delay(10U);
        NVIC_SystemReset();
        break;
    default:
        send_status(BL_STATUS_BAD_COMMAND, command, expected_sequence, 0U);
        break;
    }
}

static void process_data(const uint8_t data[8], uint8_t length) {
    if (rx_state != RX_DATA || length < 3U) {
        send_status(BL_STATUS_BAD_STATE, 0U, expected_sequence, bytes_written);
        return;
    }
    const uint16_t sequence = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
    if (expected_sequence > 0U && sequence == (uint16_t)(expected_sequence - 1U)) {
        send_status(BL_STATUS_ACK, 0U, sequence, bytes_written);
        return;
    }
    if (sequence != expected_sequence) {
        send_status(BL_STATUS_BAD_SEQUENCE, 0U, expected_sequence, sequence);
        return;
    }
    uint8_t count = (uint8_t)(length - 2U);
    if (bytes_written + count > image_size) count = (uint8_t)(image_size - bytes_written);
    if (!write_bytes(&data[2], count)) {
        send_status(BL_STATUS_FLASH_ERROR, 0U, expected_sequence, HAL_FLASH_GetError());
        return;
    }
    bytes_written += count;
    ++expected_sequence;
    send_status(BL_STATUS_ACK, 0U, sequence, bytes_written);
}

static bool write_bytes(const uint8_t *data, uint8_t length) {
    HAL_FLASH_Unlock();
    for (uint8_t i = 0U; i < length; ++i) {
        if (!has_pending_byte) { pending_byte = data[i]; has_pending_byte = true; }
        else {
            uint16_t halfword = (uint16_t)pending_byte | ((uint16_t)data[i] << 8);
            uint32_t address = BL_APPLICATION_ADDRESS + bytes_written + i - 1U;
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, halfword) != HAL_OK) {
                HAL_FLASH_Lock(); return false;
            }
            has_pending_byte = false;
        }
    }
    HAL_FLASH_Lock();
    return true;
}

static bool finish_pending_byte(void) {
    if (!has_pending_byte) return true;
    HAL_FLASH_Unlock();
    HAL_StatusTypeDef result = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
        BL_APPLICATION_ADDRESS + bytes_written - 1U, (uint16_t)pending_byte | 0xFF00U);
    HAL_FLASH_Lock();
    has_pending_byte = false;
    return result == HAL_OK;
}

static bool erase_application(uint32_t size) {
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t page_error = 0U;
    uint32_t pages = (size + BL_FLASH_PAGE_SIZE - 1U) / BL_FLASH_PAGE_SIZE;
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = BL_APPLICATION_ADDRESS;
    erase.NbPages = pages;
    HAL_FLASH_Unlock();
    HAL_StatusTypeDef result = HAL_FLASHEx_Erase(&erase, &page_error);
    if (result == HAL_OK) {
        erase.PageAddress = BL_METADATA_ADDRESS;
        erase.NbPages = 1U;
        result = HAL_FLASHEx_Erase(&erase, &page_error);
    }
    HAL_FLASH_Lock();
    return result == HAL_OK;
}

static uint32_t crc32_flash(uint32_t address, uint32_t length) {
    uint32_t crc = 0xFFFFFFFFUL;
    const uint8_t *bytes = (const uint8_t *)(uintptr_t)address;
    for (uint32_t i = 0; i < length; ++i) {
        crc ^= bytes[i];
        for (uint8_t bit = 0; bit < 8U; ++bit)
            crc = (crc >> 1) ^ (0xEDB88320UL & (0UL - (crc & 1UL)));
    }
    return crc ^ 0xFFFFFFFFUL;
}

static bool metadata_write(void) {
    const bl_metadata_t metadata = {BL_METADATA_MAGIC, image_size, expected_crc, 0xFFFFFFFFUL};
    const uint16_t *words = (const uint16_t *)&metadata;
    HAL_FLASH_Unlock();
    for (uint32_t i = 0U; i < sizeof(metadata) / 2U; ++i) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, BL_METADATA_ADDRESS + i * 2U, words[i]) != HAL_OK) {
            HAL_FLASH_Lock(); return false;
        }
    }
    HAL_FLASH_Lock();
    return true;
}

static bool application_is_valid(void) {
    const bl_metadata_t *metadata = (const bl_metadata_t *)BL_METADATA_ADDRESS;
    const uint32_t stack = *(const uint32_t *)BL_APPLICATION_ADDRESS;
    const uint32_t reset = *(const uint32_t *)(BL_APPLICATION_ADDRESS + 4U);
    return metadata->magic == BL_METADATA_MAGIC && metadata->image_size > 0U &&
           metadata->image_size <= BL_MAX_IMAGE_SIZE &&
           stack >= 0x20000000UL && stack <= 0x20005000UL &&
           reset >= BL_APPLICATION_ADDRESS && reset < BL_METADATA_ADDRESS &&
           crc32_flash(BL_APPLICATION_ADDRESS, metadata->image_size) == metadata->image_crc32;
}

static void jump_to_application(void) {
    uint32_t stack = *(const uint32_t *)BL_APPLICATION_ADDRESS;
    uint32_t reset = *(const uint32_t *)(BL_APPLICATION_ADDRESS + 4U);
    void (*application_reset)(void) = (void (*)(void))(uintptr_t)reset;
    __disable_irq();
    HAL_CAN_Stop(&hcan);
    HAL_CAN_DeInit(&hcan);
    HAL_RCC_DeInit();
    HAL_DeInit();

    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL = 0U;
    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk | SCB_ICSR_PENDSVCLR_Msk;
    for (uint32_t i = 0U; i < 8U; ++i) {
        NVIC->ICER[i] = 0xFFFFFFFFUL;
        NVIC->ICPR[i] = 0xFFFFFFFFUL;
    }

    SCB->VTOR = BL_APPLICATION_ADDRESS;
    __DSB();
    __ISB();
    __set_MSP(stack);
    __enable_irq();
    application_reset();
    while (1) { }
}

static void send_status(bl_status_t status, uint8_t command, uint16_t sequence, uint32_t detail) {
    CAN_TxHeaderTypeDef header = {0};
    uint8_t data[8] = {status, command, (uint8_t)sequence, (uint8_t)(sequence >> 8), 0, 0, 0, 0};
    uint32_t mailbox;
    data[4] = (uint8_t)detail;
    data[5] = (uint8_t)(detail >> 8);
    data[6] = (uint8_t)(detail >> 16);
    data[7] = (uint8_t)(detail >> 24);
    header.StdId = BL_CAN_RESPONSE_BASE_ID + BL_NODE_ID;
    header.IDE = CAN_ID_STD; header.RTR = CAN_RTR_DATA; header.DLC = 8U;
    (void)HAL_CAN_AddTxMessage(&hcan, &header, data, &mailbox);
}

static void CAN_Filter_Init(void) {
    CAN_FilterTypeDef filter = {0};
    filter.FilterBank = 0U; filter.FilterMode = CAN_FILTERMODE_IDLIST;
    filter.FilterScale = CAN_FILTERSCALE_16BIT; filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter.FilterActivation = ENABLE; filter.SlaveStartFilterBank = 14U;
    filter.FilterIdHigh = BL_CAN_DISCOVERY_ID << 5;
    filter.FilterIdLow = (BL_CAN_COMMAND_BASE_ID + BL_NODE_ID) << 5;
    filter.FilterMaskIdHigh = (BL_CAN_DATA_BASE_ID + BL_NODE_ID) << 5;
    filter.FilterMaskIdLow = (BL_CAN_COMMAND_BASE_ID + BL_NODE_ID) << 5;
    if (HAL_CAN_ConfigFilter(&hcan, &filter) != HAL_OK) while (1) { }
}

static void CAN_Init(void) {
    hcan.Instance = CAN1;
#if BL_CAN_BITRATE == 500000UL
    hcan.Init.Prescaler = 4U;
#elif BL_CAN_BITRATE == 250000UL
    hcan.Init.Prescaler = 8U;
#elif BL_CAN_BITRATE == 125000UL
    hcan.Init.Prescaler = 16U;
#else
#error "Supported BL_CAN_BITRATE values: 125000, 250000, 500000"
#endif
    hcan.Init.Mode = CAN_MODE_NORMAL; hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan.Init.TimeSeg1 = CAN_BS1_13TQ; hcan.Init.TimeSeg2 = CAN_BS2_4TQ;
    hcan.Init.TimeTriggeredMode = DISABLE; hcan.Init.AutoBusOff = ENABLE;
    hcan.Init.AutoWakeUp = DISABLE; hcan.Init.AutoRetransmission = ENABLE;
    hcan.Init.ReceiveFifoLocked = DISABLE; hcan.Init.TransmitFifoPriority = ENABLE;
    if (HAL_CAN_Init(&hcan) != HAL_OK) while (1) { }
}

static void SystemClock_Config(void) {
    RCC_OscInitTypeDef osc = {0}; RCC_ClkInitTypeDef clock = {0};
#if BL_CLOCK_SOURCE == BL_CLOCK_SOURCE_HSE
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE; osc.HSEState = RCC_HSE_ON;
    osc.HSEPredivValue = RCC_HSE_PREDIV_DIV1; osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSE; osc.PLL.PLLMUL = RCC_PLL_MUL6;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) while (1) { }
    clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clock.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock.APB1CLKDivider = RCC_HCLK_DIV2; clock.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_2) != HAL_OK) while (1) { }
#else
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI; osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2; osc.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) while (1) { }
    clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clock.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock.APB1CLKDivider = RCC_HCLK_DIV1; clock.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_1) != HAL_OK) while (1) { }
#endif
}

void HAL_MspInit(void) { __HAL_RCC_AFIO_CLK_ENABLE(); __HAL_RCC_PWR_CLK_ENABLE(); __HAL_AFIO_REMAP_SWJ_NOJTAG(); }
void HAL_CAN_MspInit(CAN_HandleTypeDef *handle) {
    if (handle->Instance != CAN1) return;
    GPIO_InitTypeDef gpio = {0}; __HAL_RCC_CAN1_CLK_ENABLE(); __HAL_RCC_GPIOA_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_11; gpio.Mode = GPIO_MODE_INPUT; gpio.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOA, &gpio);
    gpio.Pin = GPIO_PIN_12; gpio.Mode = GPIO_MODE_AF_PP; gpio.Speed = GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(GPIOA, &gpio);
}
void HAL_CAN_MspDeInit(CAN_HandleTypeDef *handle) {
    if (handle->Instance == CAN1) { __HAL_RCC_CAN1_CLK_DISABLE(); HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12); }
}
void SysTick_Handler(void) { HAL_IncTick(); }
void __libc_init_array(void) { }
