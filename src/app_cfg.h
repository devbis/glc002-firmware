/********************************************************************************************************
 * @file    app_cfg.h
 *
 * @brief   This is the header file for app_cfg
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C"
{
#endif

/**********************************************************************
 * Version configuration
 */
#include "version_cfg.h"

/**********************************************************************
 * Product Information
 */
/* Debug mode config */
#define UART_PRINTF_MODE 0
#define USB_PRINTF_MODE 0

/* HCI interface */
#define ZBHCI_UART 0

/* RGB + CCT always */
#define COLOR_RGB_SUPPORT 1

/* BDB */
#define TOUCHLINK_SUPPORT 1
#define FIND_AND_BIND_SUPPORT 0

/* Board ID */
#define BOARD_GLC002P 0

/* Board define */
#if defined(MCU_CORE_826x)
#define BOARD BOARD_826x_DONGLE
#define CLOCK_SYS_CLOCK_HZ 32000000
#elif defined(MCU_CORE_8258)
#if (CHIP_TYPE == TLSR_8258_1M)
#define FLASH_CAP_SIZE_1M 1
#endif
#define BOARD BOARD_GLC002P
#define CLOCK_SYS_CLOCK_HZ 48000000
#elif defined(MCU_CORE_8278)
#define FLASH_CAP_SIZE_1M 1
#define BOARD BOARD_8278_DONGLE // BOARD_8278_EVK
#define CLOCK_SYS_CLOCK_HZ 48000000
#elif defined(MCU_CORE_B91)
#define FLASH_CAP_SIZE_1M 1
#define BOARD BOARD_9518_DONGLE // BOARD_9518_EVK
#define CLOCK_SYS_CLOCK_HZ 48000000
#else
#error "MCU is undefined!"
#endif

/* Board include */
#if (BOARD == BOARD_GLC002P)
#include "board_glc002p.h"
#endif

/* Voltage detect module */
/* If you want to define the VOLTAGE_DETECT_ENABLE to 1,
 * and the model of the development board is B91 evk or dongle,
 * be sure to connect GPIO_PB0 to VCC.
 */
#define VOLTAGE_DETECT_ENABLE 0

/* Watch dog module */
#define MODULE_WATCHDOG_ENABLE 0

/* UART module */
#if ZBHCI_UART
#define MODULE_UART_ENABLE 1
#endif

#if (ZBHCI_USB_PRINT || ZBHCI_USB_CDC || ZBHCI_USB_HID || ZBHCI_UART)
#define ZBHCI_EN 1
#endif

#define VOLTAGE_DETECT_ADC_PIN                                  GPIO_PC5

/**********************************************************************
 * ZCL cluster support setting
 */
#define ZCL_ON_OFF_SUPPORT 1
#define ZCL_LEVEL_CTRL_SUPPORT 1
#if (COLOR_RGB_SUPPORT)
#define ZCL_LIGHT_COLOR_CONTROL_SUPPORT 1
#endif
#define ZCL_GROUP_SUPPORT 1
#define ZCL_SCENE_SUPPORT 1
#define ZCL_OTA_SUPPORT 1
#define ZCL_GP_SUPPORT 1
#define ZCL_WWAH_SUPPORT 0
#if TOUCHLINK_SUPPORT
#define ZCL_ZLL_COMMISSIONING_SUPPORT 1
#endif

#define AF_TEST_ENABLE 0

/**********************************************************************
 * Stack configuration
 */
#include "stack_cfg.h"

	/**********************************************************************
	 * EV configuration
	 */
	typedef enum
	{
		EV_POLL_ED_DETECT,
		EV_POLL_HCI,
		EV_POLL_IDLE,
		EV_POLL_MAX,
	} ev_poll_e;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
