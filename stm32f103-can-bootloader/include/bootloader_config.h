#ifndef BOOTLOADER_CONFIG_H
#define BOOTLOADER_CONFIG_H

#include <stdint.h>

/* STM32F103T8U6: 64 KiB flash, 1 KiB erase pages. */
#define BL_FLASH_BASE          0x08000000UL
#define BL_APPLICATION_ADDRESS 0x08004000UL
#define BL_METADATA_ADDRESS    0x0800FC00UL
#define BL_FLASH_END           0x08010000UL
#define BL_FLASH_PAGE_SIZE     1024UL
#define BL_MAX_IMAGE_SIZE      (BL_METADATA_ADDRESS - BL_APPLICATION_ADDRESS)

/* Change per PCB (valid range: 1..7). */
#ifndef BL_NODE_ID
#define BL_NODE_ID             1U
#endif

/* HSI variant: internal 8 MHz oscillator, 36 MHz system/APB1 clock. */
#define BL_HSI_FREQUENCY_HZ    8000000UL
#ifndef BL_CAN_BITRATE
#define BL_CAN_BITRATE         500000UL
#endif

#define BL_UPDATE_WINDOW_MS    1500UL
#define BL_METADATA_MAGIC      0x52465242UL /* "RFRB" */

#endif
