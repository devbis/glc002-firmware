/********************************************************************************************************
 * @file    sampleLightCtrl.c
 *
 * @brief   This is the source file for sampleLightCtrl
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
#include "zcl_include.h"
#include "sampleLight.h"
#include "sampleLightCtrl.h"
#include "helpers.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */
#define PWM_FREQUENCY 4800 // 4.8khz
#define PWM_FULL_DUTYCYCLE 100
#define PMW_MAX_TICK (PWM_CLOCK_SOURCE / PWM_FREQUENCY)

/**********************************************************************
 * TYPEDEFS
 */

/**********************************************************************
 * GLOBAL VARIABLES
 */

/**********************************************************************
 * FUNCTIONS
 */
extern void sampleLight_updateOnOff(void);
extern void sampleLight_updateColor(void);

extern void sampleLight_onOffInit(void);
extern void sampleLight_colorInit(void);

/*********************************************************************
 * @fn      pwmSetDuty
 *
 * @brief
 *
 * @param   ch			-	PWM channel
 * 			dutycycle	-	level * PWM_FULL_DUTYCYCLE
 *
 * @return  None
 */
void pwmSetDuty(u8 ch, u16 dutycycle)
{
#ifdef ZCL_LEVEL_CTRL
	u32 cmp_tick = ((u32)dutycycle * PMW_MAX_TICK) / (ZCL_LEVEL_ATTR_MAX_LEVEL * PWM_FULL_DUTYCYCLE);
	drv_pwm_cfg(ch, (u16)cmp_tick, PMW_MAX_TICK);
#endif
}

/*********************************************************************
 * @fn      pwmInit
 *
 * @brief
 *
 * @param   ch			-	PWM channel
 * 			dutycycle	-	level * PWM_FULL_DUTYCYCLE
 *
 * @return  None
 */
void pwmInit(u8 ch, u16 dutycycle)
{
	pwmSetDuty(ch, dutycycle);
}

/*********************************************************************
 * @fn      hwLight_init
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void hwLight_init(void)
{
	drv_pwm_init();

	R_LIGHT_PWM_SET();
	G_LIGHT_PWM_SET();
	B_LIGHT_PWM_SET();

	COOL_LIGHT_PWM_SET();
	WARM_LIGHT_PWM_SET();

	pwmInit(R_LIGHT_PWM_CHANNEL, 0);
	pwmInit(G_LIGHT_PWM_CHANNEL, 0);
	pwmInit(B_LIGHT_PWM_CHANNEL, 0);
	pwmInit(COOL_LIGHT_PWM_CHANNEL, 0);
	pwmInit(WARM_LIGHT_PWM_CHANNEL, 0);
}

/*********************************************************************
 * @fn      hwLight_onOffUpdate
 *
 * @brief
 *
 * @param   onOff - onOff attribute value
 *
 * @return  None
 */
void hwLight_onOffUpdate(u8 onOff)
{
	if (onOff)
	{
		drv_pwm_start(R_LIGHT_PWM_CHANNEL);
		drv_pwm_start(G_LIGHT_PWM_CHANNEL);
		drv_pwm_start(B_LIGHT_PWM_CHANNEL);
		drv_pwm_start(COOL_LIGHT_PWM_CHANNEL);
		drv_pwm_start(WARM_LIGHT_PWM_CHANNEL);
	}
	else
	{
		drv_pwm_stop(R_LIGHT_PWM_CHANNEL);
		drv_pwm_stop(G_LIGHT_PWM_CHANNEL);
		drv_pwm_stop(B_LIGHT_PWM_CHANNEL);
		drv_pwm_stop(COOL_LIGHT_PWM_CHANNEL);
		drv_pwm_stop(WARM_LIGHT_PWM_CHANNEL);
	}
}

/*********************************************************************
 * @fn      temperatureToCW
 *
 * @brief
 *
 * @param   [in]colorTemperatureMireds	-	colorTemperatureMireds attribute value
 * 			[in]level					-	level attribute value
 * 			[out]C						-	cool light PWM
 * 			[out]W						-	warm light PWM
 *
 * @return  None
 */
void temperatureToCW(u16 temperatureMireds, u8 level, u8 *C, u8 *W)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	*W = (u8)(((temperatureMireds - pColor->colorTempPhysicalMinMireds) * level) / (pColor->colorTempPhysicalMaxMireds - pColor->colorTempPhysicalMinMireds));
	*C = level - (*W);
}

/**
 * @brief This function returns the light level percentage according to the zigbee dimming curve
 * Original formula in zigbee spec: powf(10, ((level-1)/(253.f/3.f)) - 1) / 100.f;

 * @param level the level value
 * @return float the percentage of light output between 0.f and 1.f
 */
float getZBLightLevelPercentage(u8 level) {
	bool negativeRoot = ((3*level) - 256) < 0;
    float fLevelOpt;
    if (negativeRoot) {
        fLevelOpt = (_fpow(1/_fsqrt(10, 253), abs((3*level) - 256)));
    } else {
        fLevelOpt = (_fpow(_fsqrt(10, 253), (3*level) - 256));
    }

	return fLevelOpt / 99.998390f;
}

/*********************************************************************
 * @fn      hwLight_colorUpdate_colorTemperature
 *
 * @brief
 *
 * @param   colorTemperatureMireds	-	colorTemperatureMireds attribute value
 * 			level					-	level attribute value
 *
 * @return  None
 */
void hwLight_colorUpdate_colorTemperature(u16 colorTemperatureMireds, u8 level)
{
	u8 C = 0;
	u8 W = 0;

	temperatureToCW(colorTemperatureMireds, level, &C, &W);

	u16 gammaCorrectC = ((u16)C * C) / ZCL_LEVEL_ATTR_MAX_LEVEL;
	u16 gammaCorrectW = ((u16)W * W) / ZCL_LEVEL_ATTR_MAX_LEVEL;

	pwmSetDuty(COOL_LIGHT_PWM_CHANNEL, gammaCorrectC * PWM_FULL_DUTYCYCLE);
	pwmSetDuty(WARM_LIGHT_PWM_CHANNEL, gammaCorrectW * PWM_FULL_DUTYCYCLE);
	pwmSetDuty(R_LIGHT_PWM_CHANNEL, 0);
	pwmSetDuty(G_LIGHT_PWM_CHANNEL, 0);
	pwmSetDuty(B_LIGHT_PWM_CHANNEL, 0);
}

/*********************************************************************
 * @fn      hsvToRGB
 *
 * @brief
 *
 * @param   [in]hue			-	hue attribute value
 * 			[in]saturation	-	saturation attribute value
 * 			[in]level		-	level attribute value
 * 			[out]R			-	R light PWM
 * 			[out]G			-	G light PWM
 * 			[out]B			-	B light PWM
 *
 * @return  None
 */
void hsvToRGB(u16 hue, u8 saturation, u8 level, u8 *R, u8 *G, u8 *B, bool enhanced)
{
	u8 region;
	u8 remainder;
	u8 p, q, t;

	u16 rHue = (u32)hue * 360 / (enhanced ? ZCL_COLOR_ATTR_ENHANCED_HUE_MAX : ZCL_COLOR_ATTR_HUE_MAX);
	u8 rS = saturation;
	u8 rV = level;

	if (saturation == 0)
	{
		*R = rV;
		*G = rV;
		*B = rV;
		return;
	}

	if (rHue < 360)
	{
		region = rHue / 60;
	}
	else
	{
		region = 0;
	}

	remainder = (rHue - (region * 60)) * 4;

	p = (rV * (255 - rS)) >> 8;
	q = (rV * (255 - ((rS * remainder) >> 8))) >> 8;
	t = (rV * (255 - ((rS * (255 - remainder)) >> 8))) >> 8;

	if (region == 0)
	{
		*R = rV;
		*G = t;
		*B = p;
	}
	else if (region == 1)
	{
		*R = q;
		*G = rV;
		*B = p;
	}
	else if (region == 2)
	{
		*R = p;
		*G = rV;
		*B = t;
	}
	else if (region == 3)
	{
		*R = p;
		*G = q;
		*B = rV;
	}
	else if (region == 4)
	{
		*R = t;
		*G = p;
		*B = rV;
	}
	else
	{
		*R = rV;
		*G = p;
		*B = q;
	}
}

/*********************************************************************
 * @fn      hwLight_colorUpdate_HSV2RGB
 *
 * @brief
 *
 * @param   hue			-	hue attribute value
 * 			saturation	-	saturation attribute value
 * 			level		-	level attribute value
 * 			bool		-   whether the enhanced calculation should be used or not
 *
 * @return  None
 */
void hwLight_colorUpdate_HSV2RGB(u16 hue, u8 saturation, u8 level, bool enhanced)
{
	u8 R = 0;
	u8 G = 0;
	u8 B = 0;

	hsvToRGB(hue, saturation, level, &R, &G, &B, enhanced);

	hwLight_colorUpdate_RGB(R, G, B);
}

/**
 * @brief Updates the PWM channel duty based on RGB colors.
 * 
 * @param R the red component from 0-255
 * @param G the green component from 0-255
 * @param B the blue component from 0-255
 */
void hwLight_colorUpdate_RGB(u8 R, u8 G, u8 B)
{
	u16 gammaCorrectR = ((u16)R * R) / ZCL_LEVEL_ATTR_MAX_LEVEL;
	u16 gammaCorrectG = ((u16)G * G) / ZCL_LEVEL_ATTR_MAX_LEVEL;
	u16 gammaCorrectB = ((u16)B * B) / ZCL_LEVEL_ATTR_MAX_LEVEL;

	pwmSetDuty(PWM_R_CHANNEL, gammaCorrectR * PWM_FULL_DUTYCYCLE);
	pwmSetDuty(PWM_G_CHANNEL, gammaCorrectG * PWM_FULL_DUTYCYCLE);
	pwmSetDuty(PWM_B_CHANNEL, gammaCorrectB * PWM_FULL_DUTYCYCLE);
	pwmSetDuty(COOL_LIGHT_PWM_CHANNEL, 0);
	pwmSetDuty(WARM_LIGHT_PWM_CHANNEL, 0);
}

float LINEAR_TO_SRGB_GAMMA_CORRECTION(float v)
{
	// Optimization for embedded devices without math libraries: a ^ (m / n) == nth_root(a) ^ m
	// This uses a gamma value of 2.2 hence the (5/11)
	return v <= 0.0031308f ? 12.92f * v : 1.055f * _fpow(_fsqrt(v, 11), 5) - 0.055f;
}

void hwLight_colorUpdate_XY2RGB(u16 xI, u16 yI, u8 level)
{
	float x = xI / 65536.0f;
	float y = yI / 65536.0f;

	// This does not locate the closest point in the gamma spectrum of the lamps. possible #todo
	const float z = 1.f - x - y;

	float Y = level / (float)ZCL_LEVEL_ATTR_MAX_LEVEL;
	float X = yI == 0 ? 0.0f : (x * Y) / y;
	float Z = yI == 0 ? 0.0f : (z * Y) / y;
	
	// S-RGB D65 http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
	float r = max2(X * 3.2404542f + Y * -1.5371385f + Z * -0.4985314f,0);
	float g = max2(X * -0.9692660f + Y * 1.8760108f + Z * 0.0415560f,0);
	float b = max2(X * 0.0556434f + Y * -0.2040259f + Z * 1.0572252f,0);
	
	// Apply LINEAR => SRGB Gamma correction
	r = LINEAR_TO_SRGB_GAMMA_CORRECTION(r);
	g = LINEAR_TO_SRGB_GAMMA_CORRECTION(g);
	b = LINEAR_TO_SRGB_GAMMA_CORRECTION(b);

	float maxComponent = max3(r, g, b);
	if (maxComponent > 1.0f) {
		r /= maxComponent;
		g /= maxComponent;
		b /= maxComponent;
	}

	hwLight_colorUpdate_RGB(round(r * 255), round(g * 255), round(b * 255));
}

/*********************************************************************
 * @fn      light_adjust
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void light_adjust(void)
{
	sampleLight_colorInit();
	sampleLight_onOffInit();
}

/*********************************************************************
 * @fn      light_fresh
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void light_fresh(void)
{
	sampleLight_updateColor();
	sampleLight_updateOnOff();
	gLightCtx.lightAttrsChanged = TRUE;
}

/*********************************************************************
 * @fn      light_applyUpdate
 *
 * @brief
 *
 * @param
 *
 * @return  None
 */
void light_applyUpdate(u8 *curLevel, u16 *curLevel256, s32 *stepLevel256, u16 *remainingTime, u8 minLevel, u8 maxLevel, bool wrap)
{
	if ((*stepLevel256 > 0) && ((((s32)*curLevel256 + *stepLevel256) / 256) > maxLevel))
	{
		*curLevel256 = (wrap) ? ((u16)minLevel * 256 + ((*curLevel256 + *stepLevel256) - (u16)maxLevel * 256) - 256)
							  : ((u16)maxLevel * 256);
	}
	else if ((*stepLevel256 < 0) && ((((s32)*curLevel256 + *stepLevel256) / 256) < minLevel))
	{
		*curLevel256 = (wrap) ? ((u16)maxLevel * 256 - ((u16)minLevel * 256 - ((s32)*curLevel256 + *stepLevel256)) + 256)
							  : ((u16)minLevel * 256);
	}
	else
	{
		*curLevel256 += *stepLevel256;
	}

	if (*stepLevel256 > 0)
	{
		*curLevel = (*curLevel256 + 127) / 256;
	}
	else
	{
		*curLevel = *curLevel256 / 256;
	}

	if (*remainingTime == 0)
	{
		*curLevel256 = ((u16)*curLevel) * 256;
		*stepLevel256 = 0;
	}
	else if (*remainingTime != 0xFFFF)
	{
		*remainingTime = *remainingTime - 1;
	}

	light_fresh();
}


void light_computeUpdate_16(u16 *curLevel, u32 *curLevel256, s32 *stepLevel256, u16 minLevel, u16 maxLevel, bool wrap) {
	if ((*stepLevel256 > 0) && ((((s32)*curLevel256 + *stepLevel256) / 256) > maxLevel))
	{
		*curLevel256 = (wrap) ? ((u32)minLevel * 256 + ((*curLevel256 + *stepLevel256) - (u32)maxLevel * 256) - 256)
							  : ((u32)maxLevel * 256);
	}
	else if ((*stepLevel256 < 0) && ((((s32)*curLevel256 + *stepLevel256) / 256) < minLevel))
	{
		*curLevel256 = (wrap) ? ((u32)maxLevel * 256 - ((u32)minLevel * 256 - ((s32)*curLevel256 + *stepLevel256)) + 256)
							  : ((u32)minLevel * 256);
	}
	else
	{
		*curLevel256 += *stepLevel256;
	}

	if (*stepLevel256 > 0)
	{
		*curLevel = (*curLevel256 + 127) / 256;
	}
	else
	{
		*curLevel = *curLevel256 / 256;
	}
}

/*********************************************************************
 * @fn      light_applyUpdate_16
 *
 * @brief
 *
 * @param
 *
 * @return  None
 */
void light_applyUpdate_16(u16 *curLevel, u32 *curLevel256, s32 *stepLevel256, u16 *remainingTime, u16 minLevel, u16 maxLevel, bool wrap)
{
	light_computeUpdate_16(curLevel, curLevel256, stepLevel256, minLevel, maxLevel, wrap);

	if (*remainingTime == 0)
	{
		*curLevel256 = ((u32)*curLevel) * 256;
		*stepLevel256 = 0;
	}
	else if (*remainingTime != 0xFFFF)
	{
		*remainingTime = *remainingTime - 1;
	}

	light_fresh();
}

/*********************************************************************
 * @fn      light_applyXYUpdate_16
 *
 * @brief   Updates the X and Y value (or two independent values at once)
 *
 * @param
 *
 * @return  None
 */
void light_applyXYUpdate_16(u16 *curX, u32 *curX256, s32 *stepX256, u16 *curY, u32 *curY256, s32 *stepY256, u16 *remainingTime, u16 minLevel, u16 maxLevel, bool wrap)
{
	// First update both components at once
	light_computeUpdate_16(curX, curX256, stepX256, minLevel, maxLevel, wrap);
	light_computeUpdate_16(curY, curY256, stepY256, minLevel, maxLevel, wrap);

	// Then count down the single time
	if (*remainingTime == 0)
	{
		*curX256 = ((u32)*curX) * 256;
		*stepX256 = 0;

		*curY256 = ((u32)*curY) * 256;
		*stepY256 = 0;
	}
	else if (*remainingTime != 0xFFFF)
	{
		*remainingTime = *remainingTime - 1;
	}

	light_fresh();
}

/*********************************************************************
 * @fn      light_blink_TimerEvtCb
 *
 * @brief
 *
 * @param   arg
 *
 * @return  0: timer continue on; -1: timer will be canceled
 */
s32 light_blink_TimerEvtCb(void *arg)
{
	u32 interval = 0;

	if (gLightCtx.sta == gLightCtx.oriSta)
	{
		if (gLightCtx.times)
		{
			gLightCtx.times--;
			if (gLightCtx.times <= 0)
			{
				if (gLightCtx.oriSta)
				{
					hwLight_onOffUpdate(ZCL_CMD_ONOFF_ON);
				}
				else
				{
					hwLight_onOffUpdate(ZCL_CMD_ONOFF_OFF);
				}

				gLightCtx.timerLedEvt = NULL;
				return -1;
			}
		}
	}

	gLightCtx.sta = !gLightCtx.sta;
	if (gLightCtx.sta)
	{
		hwLight_onOffUpdate(ZCL_CMD_ONOFF_ON);
		interval = gLightCtx.ledOnTime;
	}
	else
	{
		hwLight_onOffUpdate(ZCL_CMD_ONOFF_OFF);
		interval = gLightCtx.ledOffTime;
	}

	return interval;
}

/*********************************************************************
 * @fn      light_blink_start
 *
 * @brief
 *
 * @param   times 		- counts
 * @param   ledOnTime	- on times, ms
 * @param   ledOffTime	- off times, ms
 *
 * @return  None
 */
void light_blink_start(u8 times, u16 ledOnTime, u16 ledOffTime)
{
	u32 interval = 0;
	zcl_onOffAttr_t *pOnoff = zcl_onoffAttrGet();

	gLightCtx.oriSta = pOnoff->onOff;
	gLightCtx.times = times;

	if (!gLightCtx.timerLedEvt)
	{
		if (gLightCtx.oriSta)
		{
			hwLight_onOffUpdate(ZCL_CMD_ONOFF_OFF);
			gLightCtx.sta = 0;
			interval = ledOffTime;
		}
		else
		{
			hwLight_onOffUpdate(ZCL_CMD_ONOFF_ON);
			gLightCtx.sta = 1;
			interval = ledOnTime;
		}
		gLightCtx.ledOnTime = ledOnTime;
		gLightCtx.ledOffTime = ledOffTime;

		gLightCtx.timerLedEvt = TL_ZB_TIMER_SCHEDULE(light_blink_TimerEvtCb, NULL, interval);
	}
}

/*********************************************************************
 * @fn      light_blink_stop
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void light_blink_stop(void)
{
	if (gLightCtx.timerLedEvt)
	{
		TL_ZB_TIMER_CANCEL(&gLightCtx.timerLedEvt);

		gLightCtx.times = 0;
		if (gLightCtx.oriSta)
		{
			hwLight_onOffUpdate(ZCL_CMD_ONOFF_ON);
		}
		else
		{
			hwLight_onOffUpdate(ZCL_CMD_ONOFF_OFF);
		}
	}
}

#endif /* __PROJECT_TL_DIMMABLE_LIGHT__ */
