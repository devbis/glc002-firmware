/********************************************************************************************************
 * @file    sampleLight.h
 *
 * @brief   This is the header file for sampleLight
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

#ifndef _SAMPLE_LIGHT_H_
#define _SAMPLE_LIGHT_H_

/**********************************************************************
 * CONSTANT
 */
#define SAMPLE_LIGHT_ENDPOINT 0x01
#define SAMPLE_TEST_ENDPOINT 0x02

/**********************************************************************
 * TIMER CONSTANTS
 */
#define ZCL_LEVEL_CHANGE_INTERVAL 20 // 50 steps a second, every 20ms
#define ZCL_COLOR_CHANGE_INTERVAL 20 // see above
#define ZCL_ONOFF_TIMER_INTERVAL  20 // the timer interval to change the offWaitTime/onTime attribute of the ONOFF cluster

#define ZCL_REMAINING_TIME_INTERVAL 100 // 1/10th of a second according to the zigbee spec

// Map the required time to our internal steps
#define INTERP_STEPS_FROM_ONE_TENTH(remTime, base) ((remTime * ZCL_REMAINING_TIME_INTERVAL)/base)

/**********************************************************************
 * TYPEDEFS
 */
typedef struct
{
	u8 keyType; /* ERTIFICATION_KEY or MASTER_KEY key for touch-link or distribute network
				 SS_UNIQUE_LINK_KEY or SS_GLOBAL_LINK_KEY for distribute network */
	u8 key[16]; /* the key used */
} app_linkKey_info_t;

typedef struct
{
	ev_timer_event_t *timerLedEvt;
	u32 keyPressedTime;

	u16 ledOnTime;
	u16 ledOffTime;
	u8 oriSta; // original state before blink
	u8 sta;	   // current state in blink
	u8 times;  // blink times
	u8 state;

	bool bdbFindBindFlg;
	bool lightAttrsChanged;

	app_linkKey_info_t tcLinkKey;
} app_ctx_t;

/**
 *  @brief Defined for basic cluster attributes
 */
typedef struct
{
	u8 zclVersion;
	u8 appVersion;
	u8 stackVersion;
	u8 hwVersion;
	u8 manuName[ZCL_BASIC_MAX_LENGTH];
	u8 modelId[ZCL_BASIC_MAX_LENGTH];
	u8 swBuildId[ZCL_BASIC_MAX_LENGTH];
	u8 powerSource;
	u8 deviceEnable;
} zcl_basicAttr_t;

/**
 *  @brief Defined for identify cluster attributes
 */
typedef struct
{
	u16 identifyTime;
} zcl_identifyAttr_t;

/**
 *  @brief Defined for group cluster attributes
 */
typedef struct
{
	u8 nameSupport;
} zcl_groupAttr_t;

/**
 *  @brief Defined for scene cluster attributes
 */
typedef struct
{
	u8 sceneCount;
	u8 currentScene;
	u8 nameSupport;
	bool sceneValid;
	u16 currentGroup;
} zcl_sceneAttr_t;

/**
 *  @brief Defined for on/off cluster attributes
 */
typedef struct
{
	u16 onTime;
	u16 offWaitTime;
	u8 startUpOnOff;
	bool onOff;
	bool globalSceneControl;
} zcl_onOffAttr_t;

/**
 *  @brief Defined for level cluster attributes
 */
typedef struct
{
	u16 remainingTime;
	u8 curLevel;
	u8 startUpCurrentLevel;
} zcl_levelAttr_t;

/**
 *  @brief Defined for color control cluster attributes
 */
typedef struct
{
	u8 colorMode;
	u8 options;
	u8 enhancedColorMode;
	u8 numOfPrimaries;
	u16 colorCapabilities;
	u8 currentHue;
	u8 currentSaturation;
	u16 currentX;
	u16 currentY;
	u16 enhancedCurrentHue;
	u8 colorLoopActive;
	u8 colorLoopDirection;
	u16 colorLoopTime;
	u16 colorLoopStartEnhancedHue;
	u16 colorLoopStoredEnhancedHue;
	u16 colorTemperatureMireds;
	u16 colorTempPhysicalMinMireds;
	u16 colorTempPhysicalMaxMireds;
	u16 startUpColorTemperatureMireds;
} zcl_lightColorCtrlAttr_t;

/**
 *  @brief Defined for saving on/off attributes
 */
typedef struct
{
	u8 startUp;
	bool lastState;
} zcl_nv_onOff_t;

/**
 *  @brief Defined for saving level attributes
 */
typedef struct
{
	u8 startUp;
	u8 lastLevel;
} zcl_nv_level_t;

/**
 *  @brief Defined for saving color control attributes
 */
typedef struct
{
	u16 startUpMireds;
	u16 lastMireds;
} zcl_nv_colorCtrl_t;

/**********************************************************************
 * GLOBAL VARIABLES
 */
extern app_ctx_t gLightCtx;
extern bdb_commissionSetting_t g_bdbCommissionSetting;
extern bdb_appCb_t g_zbDemoBdbCb;

extern u8 SAMPLELIGHT_CB_CLUSTER_NUM;
extern const zcl_specClusterInfo_t g_sampleLightClusterList[];
extern const af_simple_descriptor_t sampleLight_simpleDesc;
#if AF_TEST_ENABLE
extern const af_simple_descriptor_t sampleTestDesc;
#endif

/* Attributes */
extern zcl_basicAttr_t g_zcl_basicAttrs;
extern zcl_identifyAttr_t g_zcl_identifyAttrs;
extern zcl_groupAttr_t g_zcl_groupAttrs;
extern zcl_sceneAttr_t g_zcl_sceneAttrs;
extern zcl_onOffAttr_t g_zcl_onOffAttrs;
extern zcl_levelAttr_t g_zcl_levelAttrs;
extern zcl_lightColorCtrlAttr_t g_zcl_colorCtrlAttrs;

#define zcl_sceneAttrGet() &g_zcl_sceneAttrs
#define zcl_onoffAttrGet() &g_zcl_onOffAttrs
#define zcl_levelAttrGet() &g_zcl_levelAttrs
#define zcl_colorAttrGet() &g_zcl_colorCtrlAttrs

/**********************************************************************
 * FUNCTIONS
 */
void sampleLight_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg);

status_t sampleLight_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sampleLight_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sampleLight_sceneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sampleLight_onOffCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sampleLight_levelCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sampleLight_colorCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);

void sampleLight_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf);
void sampleLight_leaveIndHandler(nlme_leave_ind_t *pLeaveInd);
void sampleLight_otaProcessMsgHandler(u8 evt, u8 status);
u8 sampleLight_nwkUpdateIndicateHandler(nwkCmd_nwkUpdate_t *pNwkUpdate);

void sampleLight_onoff(u8 cmd);

void zcl_sampleLightAttrsInit(void);
nv_sts_t zcl_onOffAttr_save(void);
nv_sts_t zcl_levelAttr_save(void);
nv_sts_t zcl_colorCtrlAttr_save(void);

#if AF_TEST_ENABLE
void afTest_rx_handler(void *arg);
void afTest_dataSendConfirm(void *arg);
#endif

#endif /* _SAMPLE_LIGHT_H_ */
