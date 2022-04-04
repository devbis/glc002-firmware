/********************************************************************************************************
 * @file    app_ui.c
 *
 * @brief   This is the source file for app_ui
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

#if (__PROJECT_TL_DIMMABLE_LIGHT__)

/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "sampleLight.h"
#include "app_ui.h"
#include "sampleLightCtrl.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */

/**********************************************************************
 * TYPEDEFS
 */

/**********************************************************************
 * GLOBAL VARIABLES
 */

/**********************************************************************
 * LOCAL FUNCTIONS
 */
void led_on(u32 pin)
{
	drv_gpio_write(pin, LED_ON);
}

void led_off(u32 pin)
{
	drv_gpio_write(pin, LED_OFF);
}

void led_init(void)
{
	led_off(LED_STATUS_R);
	led_off(LED_STATUS_G);
	led_off(LED_STATUS_B);
}

void localPermitJoinState(void)
{
	static bool assocPermit = 0;
	if (assocPermit != zb_getMacAssocPermit())
	{
		assocPermit = zb_getMacAssocPermit();
		if (assocPermit)
		{
			led_on(LED_STATUS_R);
		}
		else
		{
			led_off(LED_STATUS_R);
		}
	}
}

void buttonKeepPressed(u8 btNum)
{
	if (btNum == VK_SW1)
	{
		gLightCtx.state = APP_FACTORY_NEW_DOING;
		led_on(LED_STATUS_R);
		led_on(LED_STATUS_G);
		led_on(LED_STATUS_B);

		zb_factoryReset();
	}
	else if (btNum == VK_SW2)
	{
	}
}

volatile u8 G_pwmTestPressed = 0;
void buttonShortPressed(u8 btNum)
{
	if (btNum == VK_SW1)
	{
		if (zb_isDeviceJoinedNwk())
		{
			gLightCtx.sta = !gLightCtx.sta;
			if (gLightCtx.sta)
			{
				sampleLight_onoff(ZCL_ONOFF_STATUS_ON);
			}
			else
			{
				sampleLight_onoff(ZCL_ONOFF_STATUS_OFF);
			}
		}
	}
	else if (btNum == VK_SW2)
	{
		/* toggle local permit Joining */
		//static u8 duration = 0;
		//duration = duration ? 0 : 0xff;
		//zb_nlmePermitJoiningRequest(duration);

		// todo remove test for color order
		if (G_pwmTestPressed == 0) {
			hwLight_colorUpdate_RGB(255,0,0);
			G_pwmTestPressed++;
		} else if (G_pwmTestPressed == 1) {
			hwLight_colorUpdate_RGB(0,255,0);
			G_pwmTestPressed++;
		} else if (G_pwmTestPressed == 2) {
			hwLight_colorUpdate_RGB(0,0,255);
			G_pwmTestPressed++;
		} else {
			G_pwmTestPressed = 0;
		}
	}
}

void keyScan_keyPressedCB(kb_data_t *kbEvt)
{
	//	u8 toNormal = 0;
	u8 keyCode = kbEvt->keycode[0];
	//	static u8 lastKeyCode = 0xff;

	buttonShortPressed(keyCode);

	if (keyCode == VK_SW1)
	{
		gLightCtx.keyPressedTime = clock_time();
		gLightCtx.state = APP_FACTORY_NEW_SET_CHECK;
	}
}

void keyScan_keyReleasedCB(u8 keyCode)
{
	gLightCtx.state = APP_STATE_NORMAL;
}

volatile u8 T_keyPressedNum = 0;
void app_key_handler(void)
{
	static u8 valid_keyCode = 0xff;

	if (gLightCtx.state == APP_FACTORY_NEW_SET_CHECK)
	{
		if (clock_time_exceed(gLightCtx.keyPressedTime, 5 * 1000 * 1000))
		{
			buttonKeepPressed(VK_SW1);
		}
	}

	if (kb_scan_key(0, 1))
	{
		T_keyPressedNum++;
		if (kb_event.cnt)
		{
			keyScan_keyPressedCB(&kb_event);
			if (kb_event.cnt == 1)
			{
				valid_keyCode = kb_event.keycode[0];
			}
		}
		else
		{
			keyScan_keyReleasedCB(valid_keyCode);
			valid_keyCode = 0xff;
		}
	}
}

#endif /* __PROJECT_TL_DIMMABLE_LIGHT__ */
