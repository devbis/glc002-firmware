/********************************************************************************************************
 * @file    board_glc002p.h
 *
 * @brief   This is the header file for mini5in1 glc002p
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

/*******************************************************************************************************
*********************TLSR8258F512ET32 32 Pins: Start*****************************************************
*******************************************************************************************************/
// Option Button
#define BUTTON_OPT GPIO_PB7
#define PB7_FUNC AS_GPIO
#define PB7_OUTPUT_ENABLE 0
#define PB7_INPUT_ENABLE 1
#define PULL_WAKEUP_SRC_PB7 PM_PIN_PULLUP_10K

// Factory reset button
#define BUTTON_RESET GPIO_PC0
#define PC0_FUNC AS_GPIO
#define PC0_OUTPUT_ENABLE 0
#define PC0_INPUT_ENABLE 1
#define PULL_WAKEUP_SRC_PC0 PM_PIN_PULLUP_10K

// LED load Output Mapping
#define LED_B GPIO_PC2 //		PWM0
#define LED_G GPIO_PC3 //		PWM1
#define LED_R GPIO_PC4 //		PWM2
#define LED_C GPIO_PD2 //		PWM3
#define LED_W GPIO_PB4 //		PWM4

#define PWM_B_CHANNEL 0 // PWM0
#define PWM_B_CHANNEL_SET()            \
	do                                 \
	{                                  \
		gpio_set_func(LED_B, AS_PWM0); \
	} while (0)

#define PWM_G_CHANNEL 1 // PWM1
#define PWM_G_CHANNEL_SET()            \
	do                                 \
	{                                  \
		gpio_set_func(LED_G, AS_PWM1); \
	} while (0)

#define PWM_R_CHANNEL 2 // PWM2
#define PWM_R_CHANNEL_SET()            \
	do                                 \
	{                                  \
		gpio_set_func(LED_R, AS_PWM2); \
	} while (0)

#define PWM_C_CHANNEL 3 // PWM3
#define PWM_C_CHANNEL_SET()            \
	do                                 \
	{                                  \
		gpio_set_func(LED_C, AS_PWM3); \
	} while (0)

#define PWM_W_CHANNEL 4 // PWM4
#define PWM_W_CHANNEL_SET()            \
	do                                 \
	{                                  \
		gpio_set_func(LED_W, AS_PWM4); \
	} while (0)

#define R_LIGHT_PWM_CHANNEL PWM_R_CHANNEL
#define G_LIGHT_PWM_CHANNEL PWM_G_CHANNEL
#define B_LIGHT_PWM_CHANNEL PWM_B_CHANNEL
#define COOL_LIGHT_PWM_CHANNEL PWM_C_CHANNEL
#define WARM_LIGHT_PWM_CHANNEL PWM_W_CHANNEL

#define R_LIGHT_PWM_SET() PWM_R_CHANNEL_SET()
#define G_LIGHT_PWM_SET() PWM_G_CHANNEL_SET()
#define B_LIGHT_PWM_SET() PWM_B_CHANNEL_SET()
#define COOL_LIGHT_PWM_SET() PWM_C_CHANNEL_SET()
#define WARM_LIGHT_PWM_SET() PWM_W_CHANNEL_SET()

// On-Board LED Definitions for Mini 5in1
#define OB_LED_R GPIO_PA0
#define PA0_FUNC AS_GPIO
#define PA0_OUTPUT_ENABLE 1
#define PA0_INPUT_ENABLE 0

#define OB_LED_G GPIO_PA1
#define PA1_FUNC AS_GPIO
#define PA1_OUTPUT_ENABLE 1
#define PA1_INPUT_ENABLE 0

#define OB_LED_B GPIO_PB1
#define PB1_FUNC AS_GPIO
#define PB1_OUTPUT_ENABLE 1
#define PB1_INPUT_ENABLE 0

#define LED_STATUS_R OB_LED_R
#define LED_STATUS_G OB_LED_G
#define LED_STATUS_B OB_LED_B

// UART
#if ZBHCI_UART
#error please configure the uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
#define DEBUG_INFO_TX_PIN GPIO_PC4 // print
#endif

	enum
	{
		VK_SW1 = 0x01,
		VK_SW2 = 0x02
	};

#define KB_MAP_NORMAL   \
	{                   \
		{               \
			VK_SW1,     \
		},              \
			{           \
				VK_SW2, \
			},          \
	}

#define KB_MAP_NUM KB_MAP_NORMAL
#define KB_MAP_FN KB_MAP_NORMAL

#define KB_DRIVE_PINS \
	{                 \
		NULL          \
	}
#define KB_SCAN_PINS             \
	{                            \
		BUTTON_OPT, BUTTON_RESET \
	}

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
