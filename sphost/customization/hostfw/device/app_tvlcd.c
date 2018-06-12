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
 *  File name    : App_tvlcd.c
 *  Function     : Display process about tv,lcd,HDMI 		   
 *  First editor :   
 *  First date   :   
 *  History      :
 *               : 2018.05.23  Edward.Lu  "Add description and modify code"
 **************************************************************************/

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_os_api.h"
#include "sp5k_cec_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_rsvblk_api.h"

#include "app_tvlcd.h"
#include "app_osd_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define HOST_DBG 0
#define TVLCD_INIT_THREAD_PRIORITY      8
#define TVLCD_INIT_THREAD_TIME_SLICE    5
#define EVENT_LCD_INIT                  0x0001
#define EVENT_TV_INIT                   0x0002
#define EVENT_TVLCD_INIT                (EVENT_LCD_INIT | EVENT_TV_INIT)
#ifdef HW_RDK_6330
#define BACKLIGHT_CONTROL_BY_GPIO       1
#else
#define BACKLIGHT_CONTROL_BY_GPIO       0
#endif
#define HDMI_EDID_VIDEO_ID_CODE_TOTAL  128
#define wait 1
#define nowait 0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
SP5K_EVENT_FLAGS_GROUP  evTvLcdInit;

static BOOL isDispFifInit = FALSE;
static BOOL gbIsLcdInit = FALSE; /* boolean flag to tell whether if the LCD has been init'd */
static appDispMode_e gCurrDispMode = APP_DISP_NONE;
BOOL isOsdInit = 0;
UINT8 HDMI_vic=0xFF;

UINT8 HDMIVideoFormat[HDMI_EDID_VIDEO_ID_CODE_TOTAL];

sp5kDispHdmiEedid_t *pvideoFmt_EX;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void disp5kSpiInit(UINT32 mode);
extern void disp5kSpiCtrl(UINT32 val, UINT32 bitsLen, UINT32 lsbFirst);
extern UINT32 disp5kSpiRead(UINT32 addr, UINT32 bitsLen, UINT32 lsbFirst);
extern UINT8 resolution;
extern UINT32 previousStateGet;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : HDMI_CUR_VIC_SUPPORTED
* Function         : HDMI mode current video ID code supported
* Return value     : result (BOOL TRUE/FALSE)
* Parameter1       : option (UINT16)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

BOOL HDMI_CUR_VIC_SUPPORTED(UINT16 nCurVideoIDCode)
{
	///printf("@@nCurVideoIDCode,result=%d,%d\n",nCurVideoIDCode,HDMIVideoFormat[nCurVideoIDCode]);
	if(HDMIVideoFormat[nCurVideoIDCode]) return TRUE;
	return FALSE;

}

/*--------------------------------------------------------------------------*
* Function name    : appEDIDVideoFormatParse
* Function         : HDMI mode edid video format parse
* Return value	   : result (BOOL TRUE/FALSE)
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

BOOL
appEDIDVideoFormatParse(void)
{
	BOOL result;
	UINT32 chnlId=0;

	memset(HDMIVideoFormat,0,sizeof(HDMIVideoFormat));
	result=sp5kDispHdmiEedidGet(chnlId,&pvideoFmt_EX);

	if(result==FAIL){
		printf("\n VideoFormatGet FAIL!!!\n");
		if(pvideoFmt_EX == NULL)
			printf("\n VideoFormatGet pvideoFmt_EX=NULL!!!\n");
        return FALSE;
	}
	else{
		if(pvideoFmt_EX == NULL){
			printf("\n VideoFormatGet pvideoFmt_EX=NULL!!!!!\n");
			return FALSE;
		}
		printf("\n VideoFormatGet_videoFmtNum=%d\n",pvideoFmt_EX->videoFmtNum);
		UINT32 i;
		for ( i = 0; i < pvideoFmt_EX->videoFmtNum; i++ ) {
			printf("\t%u. vic_(%d)\n",i,pvideoFmt_EX->videoFmtTbl[i]);
			if(pvideoFmt_EX->videoFmtTbl[i]>=HDMI_EDID_VIDEO_ID_CODE_TOTAL)
                continue;
			HDMIVideoFormat[pvideoFmt_EX->videoFmtTbl[i]]=1;
		}
	}
	printf("End of VideoFormatGet!\n");
    return TRUE;
}

/*--------------------------------------------------------------------------*
* Function name    : _dispClkSet
* Function         : display clock set
* Return value	   : void
* Parameter1	   : kHz (UINT32)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
_dispClkSet(
	UINT32 kHz
)
{
	#if SPCA5330
	extern void halPllDispPanelClkSet(UINT32, UINT32);
	halPllDispPanelClkSet(kHz/4, kHz);
	#else
	extern void halPllDispClkSet(UINT32, UINT32);
	halPllDispClkSet(kHz/4, kHz);
	#endif
}

/*--------------------------------------------------------------------------*
* Function name    : _dispSpiWrite
* Function         : display write by spi
* Return value	   : void
* Parameter1	   : address (UINT32)
* Parameter2	   : value (UINT32)
* Parameter3	   : bit length (UINT32)
* Parameter4	   : lsb/msb (UINT32 lsb=1/msb=0)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
_dispSpiWrite(
	UINT32 addr,
	UINT32 value,
	UINT32 bitlen,
	UINT32 lsbmsb
)
{
	UINT32 outval;


    #if SP5K_LCD_WDF9648XK7FLWA_UPS051
	outval = (addr<<8) | value;
    #elif SP5K_LCD_TD025THEEA_UPS052
	outval = (addr<<10) | value;
    #elif SP5K_LCD_A025BN01_UPS051 || SP5K_LCD_A025BN01_UPS052
	outval = (addr<<12) | value;
    #elif SP5K_LCD_DUMMY || SP5K_LCD_GPM1303A0_UPS051 || SP5K_LCD_TXDT250C_UPS052 \
	|| SP5K_LCD_GPM1177B1_UPS052 || SP5K_LCD_COGT350_LCM_H || SP5K_LCD_Y83482V01_UPS051 \
	|| SP5K_LCD_T2024PD0_LCM_H \
	|| SP5K_LCD_S6D04D2_DSI1_LCM || SP5K_LCD_VS020C2A_UPS052|| SP5K_LCD_GM15000_UPS052
	
	outval = value;
#else
	#error "disp spi undefined"
#endif

	disp5kSpiInit(0);
	disp5kSpiCtrl(outval, bitlen, lsbmsb);
}

/*--------------------------------------------------------------------------*
* Function name    : _dispSpiRead
* Function         : display read by spi
* Return value	   : 8 bits sdamask (UINT32 0~256)
* Parameter1	   : address (UINT32)
* Parameter2	   : bit length (UINT32)
* Parameter3	   : lsb/msb (UINT32 lsb=1/msb=0)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

UINT32
_dispSpiRead(
	UINT32 addr,
	UINT32 bitlen,
	UINT32 lsbmsb
)
{
	UINT32 outval;


    #if SP5K_LCD_WDF9648XK7FLWA_UPS051
	outval = (addr & 0xBF)|0x40;
    #elif SP5K_LCD_TD025THEEA_UPS052
	outval = ((addr & 0x3F) << 2)|0x02;
    #elif SP5K_LCD_A025BN01_UPS051||SP5K_LCD_A025BN01_UPS052
	outval = (addr<<12)|0x0800;
    #elif SP5K_LCD_DUMMY || SP5K_LCD_GPM1303A0_UPS051 || SP5K_LCD_TXDT250C_UPS052 || SP5K_LCD_GPM1177B1_UPS052 || SP5K_LCD_COGT350_LCM_H || SP5K_LCD_Y83482V01_UPS051 || SP5K_LCD_T2024PD0_LCM_H \
	|| SP5K_LCD_S6D04D2_DSI1_LCM || SP5K_LCD_VS020C2A_UPS052|| SP5K_LCD_GM15000_UPS052
	outval = addr;
#else
	#error "disp spi undefined"
#endif

	return disp5kSpiRead(outval, bitlen, lsbmsb);
}

/*--------------------------------------------------------------------------*
* Function name    : _dispLcdBacklightPwmCtrl
* Function         : LCD backlight mode ctrl by PWM
* Return value	   : void
* Parameter1	   : mode (UINT32 on=1/off=0)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
_dispLcdBacklightPwmCtrl(
	UINT32 mode
)
{
    #if SP5K_LCD_A025BN01_UPS052

#else 
	if (mode)
	{
		UINT32 chnlId=1, freqDivider=1, duty=35, startMsk=(1 << 1), stopMsk=0;
		sp5kPwmCfgSet(chnlId, freqDivider, duty);
		sp5kPwmTrig(startMsk, stopMsk);
	}
	else
	{
		UINT32 startMsk=0, stopMsk=(1 << 1);
		sp5kPwmTrig(startMsk, stopMsk);
	}
#endif
}

/*--------------------------------------------------------------------------*
* Function name    : _dispLcdInit
* Function         : LCD init
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
_dispLcdInit(
	void
)
{
	if(gbIsLcdInit)
		return;

	HOST_PROF_LOG_PRINT(LEVEL_INFO, "tvlcd: %s: s", __FUNCTION__);
	profLogAdd(0, "tvlcd: _dispLcdInit:s");

    #if (SP5K_LCD_A025BN01_UPS052) \
	|| (SP5K_LCD_S6D04D2_DSI1_LCM)
#else
	LCD_PWR_ON;        //PWR_LCD(1);
	APP_SLEEP_MS(10);  //Delay1ms(10);
	LCD_RESET_ON;      //LCD_RESET(1);
	APP_SLEEP_MS(1);   //Delay1ms(1);
	LCD_RESET_OFF;     //LCD_RESET(0);
	APP_SLEEP_MS(1);   //Delay1ms(1);
	LCD_RESET_ON;      //LCD_RESET(1);
	APP_SLEEP_MS(10);  //Delay1ms(10);
	/* %REVIEW% : standby or wakeup ?*/
	LCD_WAKEUP;        //LCD_WAKEUP(1);
#endif

	profLogAdd(0, "tvlcd: _dispLcdInit:e");
}

/*--------------------------------------------------------------------------*
* Function name    : _dispTvOut
* Function         : display TV output
* Return value	   : void
* Parameter1	   : dispMode (appDispMode_e)
* Parameter2	   : isWaitSwitchRdy (UINT32 wait/nowait)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
_dispTvOut(
	UINT32 dispMode,
	UINT32 isWaitSwitchRdy
)
{
	UINT32 chnlSrc;
	UINT32 devId, resId;

	DBG_PRINT("tvlcd: %s(%d,%d)\n",__FUNCTION__,dispMode,isWaitSwitchRdy);

	if ( dispMode == APP_DISP_TV_NTSC ) {
		devId = SP5K_DISP_CHNL_1_COMPOSITE_TV;
		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		resId = SP5K_DISP_TV_NTSC_J;
		#else
		resId = SP5K_DISP_TV_NTSC_M;
		#endif
	}
	else if( dispMode == APP_DISP_TV_PAL ) {
		devId = SP5K_DISP_CHNL_1_COMPOSITE_TV;
		resId = SP5K_DISP_TV_PAL;
	}
	#if KIT_WITH_HDMI
	else if ( dispMode == APP_DISP_HDMI ) {
		devId = SP5K_DISP_CHNL_1_HDMI_TV;
        #if 0   /*These make EDID can display by baseFW, Only enable when debug EDID*/
        {
            sp5kCecInstCfg_t cecCfg;
            SP5K_CEC_INST_CFG_CLEAR(&cecCfg);
            cecCfg.ddcChnlId     = 0;
            cecCfg.cecChnlId     = 0;
            cecCfg.rxMsgCallback = NULL;
            sp5kCecControl(0, SP5K_CEC_CTRL_OPEN, &cecCfg);
            sp5kCecControl(0, SP5K_CEC_CTRL_DBG_MSG_LOG_ENABLE_SET, 1);
            sp5kCecControl(0, SP5K_CEC_CTRL_START);
        }
        #endif

        if (appEDIDVideoFormatParse()==FALSE || pvideoFmt_EX->videoFmtNum==0)
        {
            printf("EDID READ FAIL, JUMP to HDMI_OUT\n");
            gCurrDispMode = APP_DISP_NONE;
            appStateChange(previousStateGet, STATE_PARAM_HDMI_PLUG);
            return;
        }

        printf("#### %s(#%d) dvi=%d 422=%d 444=%d \n\n\n", __FUNCTION__, __LINE__, pvideoFmt_EX->isDviMode, pvideoFmt_EX->isY422, pvideoFmt_EX->isY444 );
        if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1920X1080P_60HZ_16T9))  {resId = SP5K_EDID_1920X1080P_60HZ_16T9;printf("HDMI OUT VIC=16, 1920X1080P_60HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1920X1080I_120HZ_16T9)){resId = SP5K_EDID_1920X1080I_120HZ_16T9;printf("HDMI OUT VIC=46, 1920X1080I_120HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1920X1080I_100HZ_16T9)){resId = SP5K_EDID_1920X1080I_100HZ_16T9;printf("HDMI OUT VIC=40, 1920X1080I_100HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1920X1080I_60HZ_16T9)) {resId = SP5K_EDID_1920X1080I_60HZ_16T9;printf("HDMI OUT VIC=5,  1920X1080I_60HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1920X1080P_50HZ_16T9)) {resId = SP5K_EDID_1920X1080P_50HZ_16T9;printf("HDMI OUT VIC=31, 1920X1080P_50HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1920X1080I_50HZ_16T9)) {resId = SP5K_EDID_1920X1080I_50HZ_16T9;printf("HDMI OUT VIC=20, 1920X1080I_50HZ_16T9,\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1920X1080P_30HZ_16T9)) {resId = SP5K_EDID_1920X1080P_30HZ_16T9;printf("HDMI OUT VIC=34, 1920X1080P_30HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1920X1080P_25HZ_16T9)) {resId = SP5K_EDID_1920X1080P_25HZ_16T9;printf("HDMI OUT VIC=33, 1920X1080P_25HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1920X1080P_24HZ_16T9)) {resId = SP5K_EDID_1920X1080P_24HZ_16T9;printf("HDMI OUT VIC=32, 1920X1080P_24HZ_16T9\n");}

        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1280X720P_120HZ_16T9)) {resId = SP5K_EDID_1280X720P_120HZ_16T9;printf("HDMI OUT VIC=47, 1280X720P_120HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1280X720P_100HZ_16T9)) {resId = SP5K_EDID_1280X720P_100HZ_16T9;printf("HDMI OUT VIC=41, 1280X720P_100HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1280X720P_60HZ_16T9))  {resId = SP5K_EDID_1280X720P_60HZ_16T9;printf("HDMI OUT VIC=4,  1280X720P_60HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1280X720P_50HZ_16T9))  {resId = SP5K_EDID_1280X720P_50HZ_16T9;printf("HDMI OUT VIC=19, 1280X720P_50HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1280X720P_30HZ_16T9))  {resId = SP5K_EDID_1280X720P_30HZ_16T9;printf("HDMI OUT VIC=62, 1280X720P_30HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1280X720P_25HZ_16T9))  {resId = SP5K_EDID_1280X720P_25HZ_16T9;printf("HDMI OUT VIC=61, 1280X720P_25HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_1280X720P_24HZ_16T9))  {resId = SP5K_EDID_1280X720P_24HZ_16T9;printf("HDMI OUT VIC=60, 1280X720P_24HZ_16T9\n");}

        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X480P_60HZ_4T3))    {resId = SP5K_EDID_720X480P_60HZ_4T3;printf("HDMI OUT VIC=2,  720X480P_60HZ_4T3\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X480P_60HZ_16T9))   {resId = SP5K_EDID_720X480P_60HZ_16T9;printf("HDMI OUT VIC=3,  720X480P_60HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X480I_60HZ_4T3))    {resId = SP5K_EDID_720X480I_60HZ_4T3;printf("HDMI OUT VIC=6,  720X480I_60HZ_4T3\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X480I_60HZ_16T9))   {resId = SP5K_EDID_720X480I_60HZ_16T9;printf("HDMI OUT VIC=7,  720X480I_60HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X240P_60HZ_4T3))    {resId = SP5K_EDID_720X240P_60HZ_4T3;printf("HDMI OUT VIC=8,  720X240P_60HZ_4T3\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X240P_60HZ_16T9))   {resId = SP5K_EDID_720X240P_60HZ_16T9;printf("HDMI OUT VIC=9,  720X240P_60HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X576P_50HZ_4T3))    {resId = SP5K_EDID_720X576P_50HZ_4T3;printf("HDMI OUT VIC=17, 720X576P_50HZ_4T3\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X576P_50HZ_16T9))   {resId = SP5K_EDID_720X576P_50HZ_16T9;printf("HDMI OUT VIC=18, 720X576P_50HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X576I_50HZ_4T3))    {resId = SP5K_EDID_720X576I_50HZ_4T3;printf("HDMI OUT VIC=21, 720X576I_50HZ_4T3\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X576I_50HZ_16T9))   {resId = SP5K_EDID_720X576I_50HZ_16T9;printf("HDMI OUT VIC=22, 720X576I_50HZ_16T9\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X288P_50HZ_4T3))    {resId = SP5K_EDID_720X288P_50HZ_4T3;printf("HDMI OUT VIC=23, 720X288P_50HZ_4T3\n");}
        else if(HDMI_CUR_VIC_SUPPORTED(SP5K_EDID_720X288P_50HZ_16T9))   {resId = SP5K_EDID_720X288P_50HZ_16T9;printf("HDMI OUT VIC=24, 720X288P_50HZ_16T9\n");}
        else{                                       resId = SP5K_EDID_720X480I_60HZ_4T3;printf("Not Support, set to default : HDMI OUT 720X480I_60HZ_4T3\n");}

        if ( !pvideoFmt_EX->isY422 && !pvideoFmt_EX->isY444 ) {  /* for HDMI CTS Test ID 7-23. */
            sp5kDispCfgSet(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_HDMI_TV, 0, 0);
        }
        else {  /* for HDMI CTS Test ID 7-24. */
            sp5kDispCfgSet(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_HDMI_TV, 0, 2);
        }

        if (pvideoFmt_EX->isDviMode ) {  /* for HDMI CTS Test ID 7-33. */
            sp5kDispCfgSet(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_HDMI_TV, 3, 1);
            sp5kDispCfgSet(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_HDMI_TV, 4, (UINT32)&(pvideoFmt_EX->dtlTimeTbl[0]));
        }
        else {
            sp5kDispCfgSet(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_HDMI_TV, 3, 0);
        }

        ////Shen: Cause of 36bit cannot work on V35, thus always support 24 bits only.
        sp5kDispCfgSet(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_HDMI_TV, 1, 0);

        if ( !pvideoFmt_EX->is3dPresent ) {  /* for HDMI CTS Test ID 7-38. */
            resId &= ~SP5K_EDID_3D_STRUCT_MSK;
        }
        resolution = resId;
        if (HDMI_vic!= 0xFF)
            resolution = HDMI_vic;
    }
#endif
    else {  /* APP_DISP_NONE */
        return;
    }
    gCurrDispMode = dispMode;

    if (SP5K_DISP_CHNL_0 == sp5kDispActChnlGet(0)) {
        sp5kDispPowerOff(SP5K_DISP_CHNL_0, 0);
        sp5kDispChnlSrcSet(SP5K_DISP_CHNL_0, SP5K_DISP_SRC_NULL);
    }
    if (SP5K_DISP_CHNL_1 == sp5kDispActChnlGet(0)) {
        sp5kDispPowerOff(SP5K_DISP_CHNL_1, 0);
        sp5kDispChnlSrcSet(SP5K_DISP_CHNL_1, SP5K_DISP_SRC_NULL);
    }

    #if !DUAL_DISP_ENABLE
    sp5kDispChnlSrcSet(SP5K_DISP_CHNL_1, SP5K_DISP_SRC_1);
    sp5kDispPowerOn(SP5K_DISP_CHNL_1, devId, resId, 1);
    #endif

#if DUAL_DISP_ENABLE
    sp5kDispChnlSrcSet(SP5K_DISP_CHNL_0, SP5K_DISP_SRC_0);
    sp5kDispPowerOn(SP5K_DISP_CHNL_0, SP5K_DISP_CHNL_0_PANEL, 0, 1);
    sp5kDispChnlSrcSet(SP5K_DISP_CHNL_1, SP5K_DISP_SRC_1);
    sp5kDispPowerOn(SP5K_DISP_CHNL_1, devId, resId, 1);
#endif

	chnlSrc = sp5kDispChnlSrcGet(sp5kDispActChnlGet(0)) == SP5K_DISP_SRC_0 ?
	            SP5K_GFX_DISP_SRC_PRIMARY : SP5K_GFX_DISP_SRC_SECONDARY;
    #if DUAL_DISP_ENABLE
    sp5kGfxInitCfgSet(SP5K_GFX_DISP_SRC, SP5K_GFX_DISP_SRC_PRIMARY|SP5K_GFX_DISP_SRC_SECONDARY, 0, 0, 0);
    #else
    sp5kGfxInitCfgSet(SP5K_GFX_DISP_SRC, chnlSrc, 0, 0, 0);
    #endif

    if (!isWaitSwitchRdy)
    {
        return;
    }

    DBG_PRINT("tvlcd: before wait for SP5K_MSG_DISP_POWER_ON_READY !!! \n");

    appDispGfxInit();
}

/*--------------------------------------------------------------------------*
* Function name    : _dispLcdOut
* Function         : display LCD output
* Return value	   : void
* Parameter1	   : isWaitSwitchRdy (UINT32 wait/nowait)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
_dispLcdOut(
	UINT32 isWaitSwitchRdy
)
{
	/* %TODO% %REVIEW% :
	 * shall separate from different LCD panels
	 */
	UINT32 param, chnlSrc;

	DBG_PRINT("tvlcd: %s(%d)\n",__FUNCTION__,isWaitSwitchRdy);
	profLogAdd(0, "tvlcd: _dispLcdOut:s");

	gCurrDispMode = APP_DISP_LCD;

	if (!gbIsLcdInit)
	{
		gbIsLcdInit = TRUE;
		appDispGfxInit();
	}

    if (SP5K_DISP_SRC_NULL != sp5kDispChnlSrcGet(SP5K_DISP_CHNL_1)) {
		sp5kDispPowerOff(SP5K_DISP_CHNL_1, 0);
		sp5kDispChnlSrcSet(SP5K_DISP_CHNL_1, SP5K_DISP_SRC_NULL);
	}
	if (SP5K_DISP_SRC_NULL != sp5kDispChnlSrcGet(SP5K_DISP_CHNL_0)) {
		sp5kDispPowerOff(SP5K_DISP_CHNL_0, 0);
		sp5kDispChnlSrcSet(SP5K_DISP_CHNL_0, SP5K_DISP_SRC_NULL);
	}

    sp5kDispChnlSrcSet(SP5K_DISP_CHNL_0, SP5K_DISP_SRC_0);
    sp5kDispPowerOn(SP5K_DISP_CHNL_0, SP5K_DISP_CHNL_0_PANEL, 0, 1);

	chnlSrc = sp5kDispChnlSrcGet(sp5kDispActChnlGet(0)) == SP5K_DISP_SRC_0 ?
				SP5K_GFX_DISP_SRC_PRIMARY : SP5K_GFX_DISP_SRC_SECONDARY;
	sp5kGfxInitCfgSet(SP5K_GFX_DISP_SRC, chnlSrc, 0, 0, 0);

	_dispLcdInit();

	if (isWaitSwitchRdy)
	{
		DBG_PRINT("tvlcd: before wait for SP5K_MSG_DISP_POWER_ON_READY !!! \n");
		appHostMsgWait(SP5K_MSG_DISP_POWER_ON_READY, &param,0);
	}

	if (gbIsLcdInit)
	{
		appDispGfxInit();
		APP_SLEEP_MS(250);
		LCD_WAKEUP;
	}
	APP_SLEEP_MS(1);
}

/*--------------------------------------------------------------------------*
* Function name    : appLcdTurnOn
* Function         : LCD backlight mode on by GPIO
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void appLcdTurnOn(void)
{
	LCD_BACKLIGHT_ON;  //PwrLCDBackLight(0);

	sp5kDispAttrSet(SP5K_DISP_CHNL_0, SP5K_DISP_GLOB_ACTIVE, ENABLE);
}

/*--------------------------------------------------------------------------*
* Function name    : appLcdTurnOff
* Function         : LCD backlight mode off by GPIO
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void appLcdTurnOff(void)
{

	LCD_BACKLIGHT_OFF; //PwrLCDBackLight(0);

	#if !HW_EVB_SPE7001  /* ShangChian+, Switch to HDMI device can't set TV output enable */
	sp5kDispAttrSet(SP5K_DISP_CHNL_0, SP5K_DISP_GLOB_ACTIVE, DISABLE);
	#endif
}

/*--------------------------------------------------------------------------*
* Function name    : appDispGfxInit
* Function         : display osd init
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
appDispGfxInit(
	void
)
{
	profLogAdd(0, "tvlcd: disp gfx init -s");
	UINT32 dispHeight, dispWidth;

	if(!isDispFifInit)
	{
		profLogAdd(0, "tvlcd: disp fif init -s");
		if (fsFifOpen((const char *)FIF_DISP_DATA)==0)
		{
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "tvlcd: load %s fail", FIF_DISP_DATA);
		}
		profLogAdd(0, "tvlcd: disp fif init -e");
		isDispFifInit = TRUE;
	}
	printf("%s 0x%x\n",__FUNCTION__,gCurrDispMode);
	if(gCurrDispMode == APP_DISP_LCD)
	{
		//initial display display gamma
		sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA , (char *)RES_DISP_DISPGAMA);
		//initial display color matrix
		sp5kResourceFileLoad(SP5K_RES_DISP_COLOR_MATRIX, (char *)RES_DISP_DISPMAX);

		profLogAdd(0, "tvlcd: Osd LcdMode :s");
		appOsdInit_LCDModeSet(OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H);
		profLogAdd(0, "tvlcd: Osd LcdMode :e");
	}
	else
	{
		if(gCurrDispMode == APP_DISP_TV_NTSC){
			//sp5kDispDevSwitch( SP5K_DISP_DEV_NTSC );

			#if 1
			sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA , (char *)RES_DISP_DISPGAM0);
			//initial display color matrix
			sp5kResourceFileLoad(SP5K_RES_DISP_COLOR_MATRIX, (char *)RES_DISP_DISPMAX0); //TV
			#endif
			/* for better TV out display quality, window width must be multiple of osd buffer(320)	*/
			sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, 320, 240, 0, 0);
			sp5kDispAttrGet(SP5K_DISP_CHNL_1, SP5K_DISP_DEV_DIMENSION, (SINT32 *)&dispWidth, (SINT32 *)&dispHeight);

            #if defined(PRJ_17DVR)||defined(PRJ_19DVR)
            printf("%s W = %d H = %d \n" , __FUNCTION__ , dispWidth ,dispHeight);
			sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_IMG_WINDOW, 0, 0, dispWidth, dispHeight);  /*shrink image layer too*/
			sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_OSD_WINDOW, 0, 0, dispWidth, dispHeight);  /*shrink osd layer too*/
            #else
			sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_IMG_WINDOW, 42, 28, 640, 432);  /*shrink image layer too*/
            sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_OSD_WINDOW, 42, 28, 640, 432);  /*shrink osd layer too*/
		
            #endif
			profLogAdd(0, "tvlcd: Osd NTSCMode :s");
			appOsdInit_TVModeSet(gCurrDispMode);
			profLogAdd(0, "tvlcd: Osd NTSCMode :e");
		}
		else if(gCurrDispMode == APP_DISP_TV_PAL){ // Note:Black and white problem ICOM-3913
			#if 1
			sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA , (char *)RES_DISP_DISPGAM0);
			//initial display color matrix
			sp5kResourceFileLoad(SP5K_RES_DISP_COLOR_MATRIX, (char *)RES_DISP_DISPMAX0); //TV
			#endif
			/* for better TV out display quality, window width must be multiple of osd buffer(320)	*/
			sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, 320, 240, 0, 0);
			sp5kDispAttrGet(SP5K_DISP_CHNL_1, SP5K_DISP_DEV_DIMENSION, (SINT32 *)&dispWidth, (SINT32 *)&dispHeight);
			#if 1
			sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_IMG_WINDOW, 50, 32, 640, 512);  /*shrink image layer too*/
			sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_OSD_WINDOW, 50, 32, 640, 512);  /*shrink osd layer too*/
			#else
			sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_IMG_WINDOW, 0, 0, dispWidth, dispHeight);  /*shrink image layer too*/
			sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_OSD_WINDOW, 16, 4, dispWidth-28, dispHeight-4);  /*shrink osd layer too*/
			#endif
			profLogAdd(0, "tvlcd: Osd PALMode :s");
			appOsdInit_TVModeSet(gCurrDispMode);
			profLogAdd(0, "tvlcd: Osd PALMode :e");
		}
		else{  /* DISP HDMI */
			/* initial display display gamma */
			//sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA , (char *)"DISP\\DISPGAM0.BIN");
			/* initial display color matrix */
			//sp5kResourceFileLoad(SP5K_RES_DISP_COLOR_MATRIX, (char *)"DISP\\DISPMAX0.BIN");  //TV
			profLogAdd(0, "tvlcd: Osd HDMIMode :s");
			appOsdInit_TVModeSet(gCurrDispMode);
			profLogAdd(0, "tvlcd: Osd HDMIMode :e");
		}
	}
	profLogAdd(0, "tvlcd: disp gfx init -e");
}

/*--------------------------------------------------------------------------*
* Function name    : appLcdTvSwitch
* Function         : display mode switch
* Return value	   : void
* Parameter1	   : dispDev (appDispMode_e)
* Parameter2	   : isWaitSwitchRdy (UINT32 wait/nowait)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
appLcdTvSwitch(
	appDispMode_e dispDev,
	UINT32 isWaitSwitchRdy
)
{
	UINT32 mode;
    UINT32 appActiveStateTemp =appActiveStateGet();

	profLogAdd(0, "tvlcd: tvlcdsw:s");
	DBG_PRINT("tvlcd: %s(%d %d,%d)\n",__FUNCTION__,gCurrDispMode, dispDev,isWaitSwitchRdy);
	//set STANDBY mode
	sp5kModeGet(&mode);

printf("%s disp:(%d->%d) mode=0x%X\n", __FUNCTION__, gCurrDispMode, dispDev, mode);

	if(gCurrDispMode == dispDev)
	{
		profLogAdd(0, "tvlcd: tvlcdsw:e(do nothing)");
		return;
	}

	if(mode != SP5K_MODE_STANDBY)
	{
		profLogAdd(0, "tvlcd: tvlcdsw:standby");

		sp5kModeSet(SP5K_MODE_STANDBY);
		appHostMsgWaitExact(SP5K_MSG_MODE_READY,SP5K_MODE_STANDBY,1000);
	}

	if (dispDev == APP_DISP_LCD)
	{ /* TV to LCD */
		DBG_PRINT("tvlcd: TV->LCD \n");
		_dispLcdOut(isWaitSwitchRdy);
		if ((IS_LCD_OUT && !IS_BACKLIGHT_ON)&&(appActiveStateTemp!=APP_STATE_NULL))
		{
    		profLogPrintf(4,"[BL_1015]%s %d %d %d %d ",__FUNCTION__,__LINE__,IS_LCD_OUT,IS_BACKLIGHT_ON,appActiveStateGet());
			LCD_BACKLIGHT_ON;
		}
	}
	else
	{ /* LCD to TV or NTSC <-> PAL or HDMI*/
		DBG_PRINT("tvlcd: LCD->TV or NTSC <-> PAL \n");
		if (IS_LCD_ACTIVE)
		{
		    LCD_BACKLIGHT_OFF;
			appLcdTurnOff();
		}

		_dispTvOut(dispDev,isWaitSwitchRdy);
	}
	profLogAdd(0, "tvlcd: tvlcdsw:e");
}

/*--------------------------------------------------------------------------*
* Function name    : appCurrDispModeGet
* Function         : get current display mode
* Return value	   : gCurrDispMode (appDispMode_e)
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

appDispMode_e
appCurrDispModeGet(
	void
)
{
	return gCurrDispMode;
}

/*--------------------------------------------------------------------------*
* Function name    : _tvLcdInitThread
* Function         : display switch thread
* Return value	   : void
* Parameter1	   : init_data (ULONG)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
_tvLcdInitThread(
	ULONG init_data
)
{
	ULONG ulActualEvents;

	DBG_PRINT("%s start\n",__FUNCTION__);

	while(1)
	{
		sp5kOsEventFlagsGet(&evTvLcdInit,EVENT_TVLCD_INIT,TX_OR_CLEAR,&ulActualEvents,TX_WAIT_FOREVER);

		profLogPrintf(0, "tvlcd: _tvLcdInitThread(%d)",ulActualEvents);

		if (ulActualEvents & EVENT_LCD_INIT)
		{
			appLcdTvSwitch(APP_DISP_LCD,nowait);
		}
		else
		{
			appLcdTvSwitch(pUiSetting->TV,nowait);
		}

		if(!isOsdInit)
		{
			profLogAdd(0, "tvlcd: osd init");
			appOsdInit();
			isOsdInit = 1;
		}
	}
}

/*--------------------------------------------------------------------------*
* Function name    : _tvLcdInitStart
* Function         : set TV/LCD start
* Return value	   : void
* Parameter1	   : switch event (UINT32 EVENT_LCD_INIT/EVENT_TV_INIT)
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
_tvLcdInitStart(
	UINT32 event
)
{
	if((event == EVENT_LCD_INIT && gCurrDispMode == APP_DISP_LCD)
	  || (event != EVENT_LCD_INIT && gCurrDispMode == pUiSetting->TV))
	{
		profLogAdd(0, "tvlcd: tvLcdInitStart:(do nothing)");
		return;
	}
	DBG_PRINT("tvlcd: %s start\n",__FUNCTION__);
	profLogPrintf(0, "tvlcd: _tvLcdInitStart(%d)",event);
	#if TV_LCD_EVENT_FLAG_SET
	sp5kOsEventFlagsSet(&evTvLcdInit,event,TX_OR);
	#else
	if (event == EVENT_LCD_INIT)
	{
		sp5kOsEventFlagsSet(&evTvLcdInit,event,TX_OR);
	}
	else
	{
		appLcdTvSwitch(pUiSetting->TV,TRUE);
		if(!isOsdInit)
		{
			profLogAdd(0, "tvlcd: osd init");
			appOsdInit();
			isOsdInit = 1;
		}
	}
	#endif
}

/*--------------------------------------------------------------------------*
* Function name    : appTvLcdInit
* Function         : create display thread and you can set init data
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
appTvLcdInit(
	void
)
{
	SP5K_THREAD *ptrdTvLcd;
	ULONG init_data = 0;

	DBG_PRINT("tvlcd: %s start\n",__FUNCTION__);
	profLogAdd(0, "tvlcd: tvlcd init");

	sp5kOsEventFlagsCreate(&evTvLcdInit, "TvLcdInitEvent");

	ptrdTvLcd = sp5kOsThreadCreate(
		"TvLcdInitThread",
		_tvLcdInitThread,
		init_data,
		TVLCD_INIT_THREAD_PRIORITY,
		TVLCD_INIT_THREAD_PRIORITY,
		TVLCD_INIT_THREAD_TIME_SLICE,
		TX_AUTO_START);

	if(ptrdTvLcd==NULL)
		DBG_ASSERT(ptrdTvLcd);
}

/*--------------------------------------------------------------------------*
* Function name    : appTvLcdStart
* Function         : TV/LCD switch (EVENT_LCD_INIT/EVENT_TV_INIT)
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
appTvLcdStart(
	void
)
{
	profLogAdd(0, "tvlcd: tvlcd start");
	DBG_PRINT("tvlcd: %s start\n",__FUNCTION__);

	_tvLcdInitStart(EVENT_LCD_INIT);
}

/*--------------------------------------------------------------------------*
* Function name    : appTvLcdStop
* Function         : display stop
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 
void
appTvLcdStop(
	void
)
{
	profLogAdd(0, "tvlcd: tvlcd stop");

	if ( SP5K_DISP_CHNL_0 == sp5kDispActChnlGet(0) ) {
        sp5kDispPowerOff(SP5K_DISP_CHNL_0, 0);
        sp5kDispChnlSrcSet(SP5K_DISP_CHNL_0, SP5K_DISP_SRC_NULL);
    }
    if ( SP5K_DISP_CHNL_1 == sp5kDispActChnlGet(0) ) {
        sp5kDispPowerOff(SP5K_DISP_CHNL_1, 0);
        sp5kDispChnlSrcSet(SP5K_DISP_CHNL_1, SP5K_DISP_SRC_NULL);
    }

    LCD_BACKLIGHT_OFF;

	gCurrDispMode = APP_DISP_NONE;
	/*gbIsLcdInit = FALSE;*/
}

/*--------------------------------------------------------------------------*
* Function name    : appHDMIColorMatrix
* Function         : HDMI color matrix set
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
appHDMIColorMatrix(
    void
)
{
    if (pvideoFmt_EX != NULL)
    {
        if (!pvideoFmt_EX->isDviMode)
        {
            if ( pvideoFmt_EX->isY422 || pvideoFmt_EX->isY444 ) {  /* for HDMI CTS Test ID 7-23. */
    	    		UINT32 colorMtxA[] = {
    				0xFFD5, 0xFFAC, 0x007F,
    				0x004D, 0x0096, 0x001D,
    				0x007F, 0xFF95, 0xFFEC,
    			};
    			UINT32 colorMtxB[] = {
    				0x0408, 0x0004, 0x0408,
    			};

    		    printf("HDMI YUV ColorMatrix\n");
    			dispParaSet(SP5K_DISP_CHNL_1, DISP_PARA_COLOR_MATRIX, (SINT32)colorMtxA, (SINT32)colorMtxB, 0, 0, 0);
    			dispParaSet(SP5K_DISP_CHNL_1, DISP_PARA_COLOR_MATRIX_ENABLE, 1, 0, 0, 0, 0);
    		}
    		else {
    			UINT32 colorMtxA[] = {
    				0x0100, 0x0000, 0x0000,
    				0x0000, 0x0100, 0x0000,
    				0x0000, 0x0000, 0x0100,
    			};
    			UINT32 colorMtxB[] = {
    				0x0000, 0x0000, 0x0000,
    			};

    		    printf("HDMI RGB ColorMatrix\n");
    			dispParaSet(SP5K_DISP_CHNL_1, DISP_PARA_COLOR_MATRIX, (SINT32)colorMtxA, (SINT32)colorMtxB, 0, 0, 0);
    			dispParaSet(SP5K_DISP_CHNL_1, DISP_PARA_COLOR_MATRIX_ENABLE, 1, 0, 0, 0, 0);
    		}
        }
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appWaitOSDInitReady
* Function         : wait osd init
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.23  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	 

void
appWaitOSDInitReady(
    void
)
{
    do
    {
		APP_SLEEP_MS(20);
    } while (isOsdInit==0);
}
