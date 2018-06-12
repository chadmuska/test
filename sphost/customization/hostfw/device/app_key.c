/**************************************************************************
 *
 *       Copyright (c) 2006-2018 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *	File name    : App_key.c
 *	Function     : About button(gpio,adc) implementation.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.07  Edward.Lu  "Add description and modify code"
 **************************************************************************/

#define HOST_DBG 0
#include "app_com_api.h"
#include "app_com_def.h"
#include "app_dead_process.h"
#include "customization/dev_init.h"
#include "gpio_custom.h"
#include "sp5k_dbg_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_utility_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/*use snap key for isr*/
#if SNAP_ISR
#define ENABLE_SNAP_ISR
#endif

#define ADC_CHECK(adcdef, adcbdy, adcval)   \
    (((adcdef*(100-adcbdy)/100<adcval) && (adcval<adcdef*(100+adcbdy)/100)) ? (1) : (0))

#define ADC_TYPE_BTN	  0
#define ADC_TYPE_DIAL	  1
#define ADC_VALUE_DEFAULT 0xffffffff
#define POWER_KEY_DBNC    100
#define KEY_PROF_ID       3 // proflog index

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT16 adcBtnADvalue[ADC_BTN_NUM] = {
	ADC_BTN_A_S1,
	ADC_BTN_A_S2,
	ADC_BTN_A_S3,
	ADC_BTN_A_S4,
	ADC_BTN_B_S1,
	ADC_BTN_B_S2,
	ADC_BTN_B_S3,
	ADC_BTN_B_S4
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*use snap key for isr*/
#ifdef ENABLE_SNAP_ISR
#define KEY_DEBNC_TIMER          60
#define KEY_EVENT                0x0100

SP5K_THREAD *pthr_key = NULL;
SP5K_EVENT_FLAGS_GROUP key_evt = 0;
static UINT32 s1_press, s2_press, s1_repeat, s2_repeat, skey_disable;
static UINT16 pre_debnc_time =   0xff;

void appSnapDebncTrig(void)
{
	sp5kOsEventFlagsSet(&key_evt, KEY_EVENT, TX_OR);
}

void appSnapDebncTimer(UINT16 uiTime)
{
    if(pre_debnc_time!=uiTime)
    {
        pre_debnc_time = uiTime;
	    sp5kTimerIsrReg( 0, pre_debnc_time, appSnapDebncTrig);
    }
}

void appSnapIsr(ULONG lParam)
{
	ULONG ulActualEvents;
	while(1)
	{
		sp5kOsEventFlagsGet(
			&key_evt,
     		KEY_EVENT,
            TX_OR_CLEAR,
            &ulActualEvents,
            TX_WAIT_FOREVER
		);

        if(s1_repeat)
        {
            HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: s1 repeat continue");
            profLogPrintf(KEY_PROF_ID, "<lester> key: isr s1 repeat continue");
            continue;
        }
        else if(s2_repeat){
            HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: s2 repeat continue");
            profLogPrintf(KEY_PROF_ID, "<lester> key: isr s2 repeat continue");
            continue;
        }
    	else if(s1_press==TRUE && s2_press==FALSE){ /* S1 release */
       		sp5kHostMsgSend(APP_KEY_RELEASE_S1,0);
            s1_press = FALSE;
            HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: isr s1 release");
            profLogPrintf(KEY_PROF_ID, "<lester> key: isr s1 release");
    	}
        else if(s1_press==FALSE && s2_press==TRUE){ /*S2 press + S1 release */
    		sp5kHostMsgSend(APP_KEY_RELEASE_S2,0);
            s2_press = FALSE;
            HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: isr s2 release");
            profLogPrintf(KEY_PROF_ID, "<lester> key: isr s2 release");
    	}
        else if(s1_press==TRUE && s2_press==TRUE){ /*S2 press + S1 press */
        	sp5kHostMsgSend(APP_KEY_RELEASE_S1,0);
    		sp5kHostMsgSend(APP_KEY_RELEASE_S2,0);
            s1_press = s2_press = FALSE;
            HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: isr s1 s2 release");
            profLogPrintf(KEY_PROF_ID, "<lester> key: isr s1 s2 release");
        }
        else
        {
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "key: isr other case %d %d",s1_press, s2_press );
        }
	}
}

void appSnap2IsrFall(void)
{
    if(skey_disable == DISABLE)
        return;

    if(s2_press==TRUE || s2_repeat==TRUE)
    {
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: isr s2 repeat");
        profLogPrintf(KEY_PROF_ID, "<lester> key: isr s2 repeat");
        return;
    }

    if(appNextStateGet()==APP_STATE_NULL && s1_press==TRUE)
    {
        s2_press = TRUE;
        sp5kHostMsgSend(APP_KEY_PRESS_S2,0);
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: isr s2 press");
        profLogPrintf(KEY_PROF_ID, "<lester> key: isr s2 press");
    }
}

void appSnap1IsrFall(void)
{
    if(skey_disable == DISABLE)
        return;

    if(s1_press==TRUE || s1_repeat==TRUE)
    {
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: isr s1 repeat");
        profLogPrintf(KEY_PROF_ID, "<lester> key: isr s1 repeat");
        return;
    }

    if(appNextStateGet()==APP_STATE_NULL)
    {
        s1_press = TRUE;
        sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: isr s1 press");
        profLogPrintf(KEY_PROF_ID, "<lester> key: isr s1 press");
    }
}

void appSnap2RepeatChk(UINT32 msgId, UINT32 msgParam)
{
    if(IS_VIDEO_GROUP(appActiveStateGet()))
    	appSnapDebncTimer(KEY_DEBNC_TIMER*20);
    else
        appSnapDebncTimer(KEY_DEBNC_TIMER);

    if(msgId==SP5K_MSG_GPIO_BUTTON_REPEAT && BTN_ID_S2==msgParam)
    {
        s2_repeat = TRUE;
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: s2 repeat");
        profLogPrintf(KEY_PROF_ID, "<lester> key: s2 repeat");
    }
    else if(msgId==SP5K_MSG_GPIO_BUTTON_RELEASE && BTN_ID_S2==msgParam)
    {
        s2_press = s2_repeat = FALSE;
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: s2 release");
        profLogPrintf(KEY_PROF_ID, "<lester> key: s2 release");
    }
}

void appSnap1RepeatChk(UINT32 msgId, UINT32 msgParam)
{

    if(IS_VIDEO_GROUP(appActiveStateGet()))
    	appSnapDebncTimer(KEY_DEBNC_TIMER*20);
    else
        appSnapDebncTimer(KEY_DEBNC_TIMER);

    if(msgId==SP5K_MSG_GPIO_BUTTON_REPEAT && BTN_ID_S1==msgParam)
    {
        s1_repeat = TRUE;
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: s1 repeat");
        profLogPrintf(KEY_PROF_ID, "<lester> key: s1 repeat");
    }
    else if(msgId==SP5K_MSG_GPIO_BUTTON_RELEASE && BTN_ID_S1==msgParam)
    {
        s1_press = s1_repeat = FALSE;
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "key: s1 release");
        profLogPrintf(KEY_PROF_ID, "<lester> key: s1 release");
    }
}

void appSnapCtrl(UINT32 en)
{
    profLogPrintf(KEY_PROF_ID, "<lester> key: skey ctrl %d", en);
    skey_disable = en;
}

void appSnapIsrInit(void)
{
    s1_press = s2_press = s1_repeat = s2_repeat = FALSE;

    sp5kOsEventFlagsCreate(&key_evt, "key_evt");
	pthr_key = sp5kOsThreadCreate("keythread", appSnapIsr,
									0, 4, 15, 3, TX_AUTO_START );
    appSnapDebncTimer(KEY_DEBNC_TIMER);

	sp5kGpioCfgSet( GRP_BUTTON_SNAP1, 1<<CFG_BUTTON_SNAP1, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_S1, GRP_BUTTON_SNAP1, CFG_BUTTON_SNAP1, POL_BUTTON_SNAP1, KEY_DEBNC_TIMER>>2, 0);

	sp5kGpioCfgSet( GRP_BUTTON_SNAP2, 1<<CFG_BUTTON_SNAP2, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_S2, GRP_BUTTON_SNAP2, CFG_BUTTON_SNAP2, POL_BUTTON_SNAP2, KEY_DEBNC_TIMER>>2, 0);

	sp5kGpioIsrReg(SP5K_GPIO_INT0, SP5K_TRIG_LVL_FALL, appSnap1IsrFall);
	sp5kGpioIntEnable(SP5K_GPIO_INT0, SP5K_TRIG_LVL_FALL);
	sp5kGpioIsrReg(SP5K_GPIO_INT1, SP5K_TRIG_LVL_FALL, appSnap2IsrFall);
	sp5kGpioIntEnable(SP5K_GPIO_INT1, SP5K_TRIG_LVL_FALL);
}

void appSnapIsrExit(void)
{
	UINT32 ret;
	ret = sp5kOsThreadTerminate(pthr_key);
	if ( ret == SUCCESS ) {
		ret = sp5kOsThreadDelete(pthr_key);
	}
	else/*add for cppcheck: 'ret' set but never used*/
		DBG_PRINT("%s,%d,%d\n",__FUNCTION__,__LINE__,ret);
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appGpioBtnInit
* Function         : init gpio button
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appGpioBtnInit(void)
{
	DBG_PRINT("key: %s \n", __FUNCTION__);
	/* GPIO 10 -> TELE */
	sp5kGpioCfgSet( GRP_BUTTON_TELE, 1<<CFG_BUTTON_TELE, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_TELE, GRP_BUTTON_TELE, CFG_BUTTON_TELE, POL_BUTTON_TELE, GPIO_DBNC, GPIO_RPT);

	/* GPIO 12 -> WIDE */
	sp5kGpioCfgSet( GRP_BUTTON_WIDE, 1<<CFG_BUTTON_WIDE, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_WIDE, GRP_BUTTON_WIDE, CFG_BUTTON_WIDE, POL_BUTTON_WIDE, GPIO_DBNC, GPIO_RPT);

	/* SP5K_GPIO_GRP_POCU 2 -> PB */
	sp5kGpioCfgSet( GRP_BUTTON_PLAY, 1<<CFG_BUTTON_PLAY, INPUT );
	sp5kButtonGpioCfgSet(BTN_ID_PB, GRP_BUTTON_PLAY, CFG_BUTTON_PLAY, POL_BUTTON_PLAY, GPIO_DBNC, GPIO_RPT);

	/* SP5K_GPIO_GRP_POCU 0 -> POWER */
	sp5kGpioCfgSet( GRP_BUTTON_POWER, 1<<CFG_BUTTON_POWER, INPUT );
	sp5kButtonGpioCfgSet( BTN_ID_POWER, GRP_BUTTON_POWER, CFG_BUTTON_POWER, POL_BUTTON_POWER, POWER_KEY_DBNC, GPIO_RPT );

    /*use snap key for isr*/
    #ifdef ENABLE_SNAP_ISR
    appSnapIsrInit();
    #else
	/* GPIO 10 -> S1 */
	sp5kGpioCfgSet( GRP_BUTTON_SNAP1, 1<<CFG_BUTTON_SNAP1, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_S1, GRP_BUTTON_SNAP1, CFG_BUTTON_SNAP1, POL_BUTTON_SNAP1, GPIO_DBNC, DISABLE);

    /* GPIO 8-> S2 */
	sp5kGpioCfgSet( GRP_BUTTON_SNAP2, 1<<CFG_BUTTON_SNAP2, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_S2, GRP_BUTTON_SNAP2, CFG_BUTTON_SNAP2, POL_BUTTON_SNAP2, GPIO_DBNC, DISABLE);
    #endif

	/* GPIO 7 -> TV */
	sp5kGpioCfgSet(GRP_TV_DETECT, 1<< CFG_TV_DETECT, INPUT);
	sp5kButtonGpioCfgSet(BTN_ID_TV, GRP_TV_DETECT, CFG_TV_DETECT, POL_TV_DETECT, GPIO_DBNC, DISABLE);

}

/*--------------------------------------------------------------------------*
* Function name    : appAdcBtnInit
* Function         : init adc button
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appAdcBtnInit(void)
{
	UINT32 i,btnIdx;
	/* config all ADC button */
	sp5kAdcInit();
	sp5kAdcEnable(ENABLE);
	btnIdx = 0;

	for (i=0 ; i<COUNT_OF(adcBtnADvalue) ; i++, btnIdx++) {
		UINT32 dev, adc=adcBtnADvalue[i];
		DBG_PRINT("key: i=%d adc=%d   adc>>14=%d  adcBtnAADvalue[i]=%d\n",i,adc ,adc>>14 ,adcBtnADvalue[i]);
		if (adc>>14) {
			dev = (adc>>14) << 1;
			adc &= 0x3fff;
			DBG_PRINT("%d	->->dev=%d   adc=%d\n",i,dev,adc);
		} else  {
			if(i<(COUNT_OF(adcBtnADvalue)/2)){
				dev = ADC_BTN_A_DEV;
			}else{
				dev = ADC_BTN_B_DEV;
			}
			DBG_PRINT("	&&dev=%d   \n",i,dev);
		}
		if(i<(COUNT_OF(adcBtnADvalue)/2)){
			sp5kButtonAdcCfgSet(btnIdx, ADC_BTN_A_CH, adc*(100-dev)/100, adc*(100+dev)/100, ADC_DBNC, ADC_RPT);
			DBG_PRINT("chn[%d] %d    %d	 %d    %d  %d\n",ADC_BTN_A_CH, btnIdx, adc*(100-dev)/100, adc*(100+dev)/100, ADC_DBNC, ADC_RPT);
		}else{
			sp5kButtonAdcCfgSet(btnIdx, ADC_BTN_B_CH, adc*(100-dev)/100, adc*(100+dev)/100, ADC_DBNC, ADC_RPT);
			DBG_PRINT("chn[%d] %d    %d	 %d    %d  %d\n",ADC_BTN_B_CH, btnIdx, adc*(100-dev)/100, adc*(100+dev)/100, ADC_DBNC, ADC_RPT);
		}
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBtnControl
* Function         : Card plug handle.
* Return value	   : void
* Parameter1	   : btnID (UINT32 BTN_ALL/btnCtrlGpioId_t/btnCtrlAdcId_t)
                     BTN_ALL include btnCtrlGpioId_t/btnCtrlAdcId_t.
* Parameter2	   : isEnable (UINT32 ENABLE/DISABLE)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appBtnControl(
	UINT32 btnID,
	UINT32 isEnable
)
{
	UINT32 i, msk, msk_base;

	/*-- Check GPIO Button group
	 */
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "key: btnId, BTN_GRP_GPIO: 0x%x, 0x%x", btnID, BTN_GRP_GPIO);
	if (btnID & BTN_GRP_GPIO) {
		DBG_PRINT("key: BTN (%s) : GPIO group ! \n", (!!isEnable ? "Enable" : "Disable") );
		DBG_PRINT("key: GPIO_BTN_NUM=%d   BTN_ID_GPIO_TOTAL=%d\n",GPIO_BTN_NUM,BTN_ID_GPIO_TOTAL);
		/*-- double check button configuratin !! */
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "key: GPIO_BTN_NUM, BTN_ID_GPIO_TOTAL: %d, %d", GPIO_BTN_NUM, BTN_ID_GPIO_TOTAL);
		if (GPIO_BTN_NUM != BTN_ID_GPIO_TOTAL) {
			DBG_PRINT("key: %s: !! FATAL !!, Check GPIO BTN CFG !! \n\n", __FUNCTION__);
		} else {
			msk_base = ((UINT32)1)<<BTN_SHIFT_GPIO_MSK;
			for (i=0; i<BTN_ID_GPIO_TOTAL; i++) {
				msk = msk_base << i;
				if (btnID & msk) {
					DBG_PRINT("key: GPIO BTN i(%d), msk(0x%08x) \n", i, msk);
					sp5kButtonEnable(SP5K_BUTTON_TYPE_GPIO, i, (!!isEnable ? ENABLE : DISABLE));
				}
			}
		}
	}

	/*-- Check ADC Button group
	 */
	if (btnID & BTN_GRP_ADC) {
		DBG_PRINT("key: BTN (%s) : ADC group ! \n", (!!isEnable ? "Enable" : "Disable") );
		DBG_PRINT("key: ADC_BTN_NUM = %d   BTN_ID_ADC_TOTAL = %d\n",ADC_BTN_NUM, BTN_ID_ADC_TOTAL);
		/*-- double check button configuratin !! */
		if (ADC_BTN_NUM != BTN_ID_ADC_TOTAL) {
			DBG_PRINT("key: %s: !! FATAL !!, Check ADC BTN CFG !! \n\n", __FUNCTION__);
			return;
		} else {
			msk_base = ((UINT32)1)<<BTN_SHIFT_ADC_MSK;
			for (i=0; i<BTN_ID_ADC_TOTAL; i++) {
				msk = msk_base << i;
				if (btnID & msk) {
					DBG_PRINT("key: ADC BTN i(%d), msk(0x%08x) \n", i, msk);
					sp5kButtonEnable(SP5K_BUTTON_TYPE_ADC, i, (!!isEnable ? ENABLE : DISABLE));
				}
			}
		}
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBtnEnable
* Function         : Enable button (gpio/adc)
* Return value	   : void
* Parameter1	   : btnID (UINT32 BTN_ALL/btnCtrlGpioId_t/btnCtrlAdcId_t)
                     BTN_ALL include btnCtrlGpioId_t/btnCtrlAdcId_t.
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appBtnEnable(
	UINT32 btnID
)
{
    /*HOST_PROF_LOG_PRINT(LEVEL_INFO, "key: Button Enable s %x ",btnID);*/

    #ifdef ENABLE_SNAP_ISR
    if(btnID==BTN_ALL)
        appSnapCtrl(ENABLE);
    #endif

	appBtnControl(btnID, ENABLE);
    /*HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "key: Button Enable e %x",btnID);*/
}

/*--------------------------------------------------------------------------*
* Function name    : appBtnDisable
* Function         : Disable button (gpio/adc)
* Return value	   : void
* Parameter1	   : btnID (UINT32 BTN_ALL/btnCtrlGpioId_t/btnCtrlAdcId_t)
                     BTN_ALL include btnCtrlGpioId_t/btnCtrlAdcId_t.
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appBtnDisable(
	UINT32 btnID
)
{
    /*HOST_PROF_LOG_PRINT(LEVEL_INFO, "key: Button Disable %x",btnID);*/
    profLogCallStack(KEY_PROF_ID);// prof id

    #ifdef ENABLE_SNAP_ISR
    if(btnID==BTN_ALL)
        appSnapCtrl(DISABLE);
    #endif

	appBtnControl(btnID, DISABLE);
    /*HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "key: Button Enable e %x",btnID);*/
}

/*--------------------------------------------------------------------------*
* Function name    : adcBtnIoLevelGet
* Function         : Get adc level that pressed or released.
* Return value	   : ret (UINT32 1:pressed/0:released)
* Parameter1	   : btnID (UINT32 switch case)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
adcBtnIoLevelGet(
	UINT32 btnID
)
{
	UINT32 ret=FALSE, ch_A=ADC_VALUE_DEFAULT, ch_B=ADC_VALUE_DEFAULT;
	UINT32 dev_A=ADC_BTN_A_DEV;
	UINT32 dev_B=ADC_BTN_B_DEV;
#if !defined(HW_17DVR)&&!defined(HW_19DVR)
	sp5kAdcRead(ADC_BTN_A_CH, &ch_A);
	sp5kAdcRead(ADC_BTN_B_CH, &ch_B);
#endif

	switch (btnID) {
	/*--- ADC Channel A ------------*/
	case BTN_UP:
        ret = ADC_CHECK(ADC_BTN_A_S1, dev_A, ch_A);
		break;

	case BTN_DOWN:
        ret = ADC_CHECK(ADC_BTN_A_S2, dev_A, ch_A);
		break;

	case BTN_LEFT:
        ret = ADC_CHECK(ADC_BTN_A_S3, dev_A, ch_A);
		break;

	case BTN_RIGHT:
        ret = ADC_CHECK(ADC_BTN_A_S4, dev_A, ch_A);
		break;

	/*--- ADC Channel B ------------*/
	case BTN_SET: /* OK */
        ret = ADC_CHECK(ADC_BTN_B_S1, dev_B, ch_B);
		break;

	case BTN_DEL: /* F1 */
        ret = ADC_CHECK(ADC_BTN_B_S2, dev_B, ch_B);
		break;

	case BTN_MENU: /* F2 */
        ret = ADC_CHECK(ADC_BTN_B_S3, dev_B, ch_B);
		break;

	case BTN_MODE: /* F3 */
        ret = ADC_CHECK(ADC_BTN_B_S4, dev_B, ch_B);
		break;

	default:
		HOST_ASSERT(0);
		break;
	}
	return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appBtnInit
* Function         : Button initialization that config gpio, adc and enable all button.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appBtnInit(
	void
)
{
	appGpioBtnInit();
#if !( defined(PRJ_JETDVR)||defined(PRJ_17DVR) ||defined(PRJ_19DVR))
	appAdcBtnInit();
	
#endif
	
	appBtnEnable(BTN_ALL);
}

/*--------------------------------------------------------------------------*
* Function name    : appBtnMsgToKey
* Function         : Let button message decode to app_key_msg.
* Return value	   : SUCCESS:appKeyMsg_e, FAIL:APP_KEY_ERROR
* Parameter1	   : msgId, message index (UINT32)
                     SP5K_MSG_GPIO_BUTTON_PRESS
                     SP5K_MSG_GPIO_BUTTON_REPEAT
                     SP5K_MSG_GPIO_BUTTON_RELEASE
                     SP5K_MSG_ADC_BUTTON_PRESS
                     SP5K_MSG_ADC_BUTTON_REPEAT
                     SP5K_MSG_ADC_BUTTON_RELEASE
* Parameter2	   : msgParam (btnCtrlGpioId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appBtnMsgToKey(
	UINT32 msgId,
	UINT32 msgParam
)
{
	UINT32 key = APP_KEY_ERROR;

	DBG_PRINT("msg2key: %s : [0x%x] [0x%x]\n",__FUNCTION__,msgId,msgParam);

	switch (msgId) {
	/* button message */
	case SP5K_MSG_GPIO_BUTTON_PRESS:
	case SP5K_MSG_GPIO_BUTTON_RELEASE:
	case SP5K_MSG_GPIO_BUTTON_REPEAT:

        #ifdef ENABLE_SNAP_ISR
        appSnap1RepeatChk(msgId, msgParam);
        appSnap2RepeatChk(msgId, msgParam);
        #endif
		key = appGpioMsg2Key(msgId, msgParam);
		break;
	case SP5K_MSG_ADC_BUTTON_PRESS:
	case SP5K_MSG_ADC_BUTTON_RELEASE:
	case SP5K_MSG_ADC_BUTTON_REPEAT:
		key = appAdcMsg2Key(msgId, msgParam);
		break;
	default:
		key = APP_KEY_ERROR;
		DBG_PRINT("msg2key: Unknow message : 0x%x (0x%x)\n", msgId, msgParam);
		break;
	}
	return 	key;
}

/*--------------------------------------------------------------------------*
* Function name    : appGpioMsg2Key
* Function         : Let gpio message decode to app_key_msg.
* Return value	   : SUCCESS:appKeyMsg_e, FAIL:APP_KEY_ERROR
* Parameter1	   : msgId, message index (UINT32)
                     SP5K_MSG_GPIO_BUTTON_PRESS
                     SP5K_MSG_GPIO_BUTTON_REPEAT
                     SP5K_MSG_GPIO_BUTTON_RELEASE
* Parameter2	   : msgParam (btnCtrlGpioId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appGpioMsg2Key(
	UINT32 msgId,
	UINT32 msgParam
)
{
	if (msgParam >= GPIO_BTN_NUM || msgId==SP5K_MSG_GPIO_BUTTON_REPEAT)
	{
 		return APP_KEY_ERROR;
 	}

	/*HOST_PROF_LOG_PRINT(LEVEL_INFO, "Get key message: 0x%x \n", (APP_KEY_MSG|(msgId<<4)|msgParam));*/
	return (APP_KEY_MSG|(msgId<<4)|msgParam);
}

/*--------------------------------------------------------------------------*
* Function name    : appAdcMsg2Key
* Function         : Let adc message decode to app_key_msg.
* Return value	   : SUCCESS:appKeyMsg_e, FAIL:APP_KEY_ERROR
* Parameter1	   : msgId, message index (UINT32)
                     SP5K_MSG_ADC_BUTTON_PRESS
                     SP5K_MSG_ADC_BUTTON_REPEAT
                     SP5K_MSG_ADC_BUTTON_RELEASE
* Parameter2	   : msgParam (btnCtrlAdcId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appAdcMsg2Key(
	UINT32 msgId,
	UINT32 msgParam
)
{
	if (msgParam < ADC_BTN_NUM/*by project defined from HW_SBC.io, HW_17DVR.io*/)
	{  /* ADC button decode */
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "Get key message: 0x%x \n", (APP_KEY_MSG|(msgId<<4)|msgParam));
		return (APP_KEY_MSG|(msgId<<4)|msgParam);
 	}
	else
	{
		DBG_PRINT("msg2key: appAdcMsg2Key err:[0x%x %x]\n", msgId, msgParam);
		return APP_KEY_ERROR;
	}
	return APP_KEY_ERROR;
}
