/**************************************************************************
 *
 *       Copyright (c) 2002-2018 by iCatch Technology, Inc.
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
 **************************************************************************/
/**
 * @file command.c
 *
 * @author
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"

#include "api/cmd_api.h"
#include "api/sp5k_global_api.h"

#include "api/sp5k_cdsp_api.h"
#include "api/sp5k_disp_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_fs_api.h"
#include "api/sp5k_os_api.h"
#include "api/sp5k_gfx_api.h"
#include "api/sp5k_sensor_api.h"
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"
#include "api/sp5k_af_api.h"
#include "api/sp5k_modesw_api.h"
#include "api/sp5k_pb_api.h"
#include "api/sp5k_dcf_api.h"
#include "api/sp5k_dpof_api.h"
#include "api/sp5k_media_api.h"
#include "api/sp5k_aud_api.h"
#include "api/sp5k_dzoom_api.h"
#include "api/sp5k_dq_api.h"

#include "customization/dev_init.h"
#include "customization/app_init.h"

#include "middleware/sensor_model.h"
#include "middleware/proflog.h"

#include "debug_mod/dbg_api.h"
#include "api/sp5k_dbg_api.h"

#if SSLEN
#define DROPBOX_FLOW_TRIGGER 1
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define READ8(a) (*((volatile UINT8*)(a)))
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)
#define READ16(a) (*((volatile UINT16*)(a)))
#define WRITE16(a,b)	*(volatile UINT16*)(a) = (b)
#define READ32(a) (*((volatile UINT32*)(a)))
#define WRITE32(a,b)	*(volatile UINT32*)(a) = (b)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void senRegWR(UINT16 reg, UINT16 val);
extern void memdump(const void *mem, UINT32 len);
extern UINT32 appI2CRead(UINT32 subAddr,UINT32 *pdata);
extern UINT8 HDMI_vic;
extern UINT16 HDMI_IN_DLY;
extern UINT16 HDMI_OUT_DLY;
extern UINT32 appI2CWrite(UINT32 par0, UINT32 mask, UINT32 par1);
extern void sensorModeSet( UINT32 mode );
extern UINT32 sensorModeGet( );
extern UINT32 sensor_event_wait( UINT32 event );
extern UINT32 sensorTgPllClkExtSet(UINT32 clkHz);
extern void ADV7480FormatInfo(void);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appMonitorCmd( int argc, char *argv[] );

/**************************************************************************
 *                          L O C A L    D A T A                          *
 **************************************************************************/

#if defined(HW_KX1371)
static UINT32 ulCmdPortID; /*global variable*/
#endif

/**************************************************************************
 *                                                                        *
 *  Function Name: cmdAppInit                                             *
 *                                                                        *
 *  Purposes: Application installing modules for RS-232 command shell
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
#if CMD_TASK_INIT

void appSensorCaliModeset(UINT32 sensormode,UINT32 calimode,UINT32 GainIdx)
{
	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
	sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,0x30);
	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	sp5kModeWait(SP5K_MODE_STILL_PREVIEW);

	if(calimode==0)
	{
		/*free AE */
		sp5kAeExpAgcIrisTestModeSet(0,0);
		sp5kAeExpAgcIrisTestModeSet(1,0);
		/*printf(" pana  normal mode \n");*/
	}
	else
	{
		/*fixed AE */
		sp5kAeExpAgcIrisTestSet(0,200,GainIdx,SP5K_AE_EXP_AGC_DONT_CHANGE);
		sp5kAeExpAgcIrisTestSet(1,200,GainIdx,SP5K_AE_EXP_AGC_DONT_CHANGE);
		printf("Set Agc index = %d \n",(SINT32)GainIdx);
		sp5kAeExpAgcIrisTestModeSet(0,1);
		sp5kAeExpAgcIrisTestModeSet(1,1);
		/*printf(" pana calibration  mode \n"); */
	}

	sensorReloadExecute(calimode);

	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	sp5kModeWait(SP5K_MODE_STILL_PREVIEW);

	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
	sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,sensormode);
	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
}

static void afCVCB(UINT32 pid)
{
	
	static UINT8 cvcnt=0;

	cvcnt++;

	if(cvcnt%10==0)
	{
		UINT32 afwin[3][3];
		sp5kAfWinGet(pid, SP5K_AF_STAT_VAL, (UINT32 *)afwin);
		printf("%d\n",afwin[1][1]/2 + (afwin[0][0] + afwin[0][2]  + afwin[2][0] + afwin[2][2])/8);
		cvcnt=0;
	}
}

void
cmdAppInit(
	void
)
{
	cmdMemInit();   /* commands for dump/fill/search/ */
	cmdOsInit();    /* commands for displaying OS information */
	cmdDosInit();   /* commands for MS-DOS like FS */
	cmdGlobalInit();
	cmdGpioInit();	/* commands for GPIO/AD/PWM */
	cmdDiskInit();
#if SP5K_DISK_NAND
	cmdNandInit();	/* commands for NAND device */
#endif
#if SP5K_DISK_SPI
	cmdSpiInit();
#endif

	cmdSdInit();	/* commands for SD device */
#if 0
	cmdLmiInit();
	cmdOsdInit();	/* commands for OSD */
#endif
	cmdAaaInit();
	cmdCdspInit();	/* commands for CDSP */
	cmdFrontInit();
	cmdSnapInit();
	cmdUsbInit();
	cmdIspInit(); /*xian ++ */

	printf( "cmdDispInit() called.\n" );
	cmdDispInit();

	cmdDefaultSet(1);
	/*cmdGdbInit();*/
	cmdDramTestInit();
	cmdMediaInit();
#ifdef FPGA
	cmdKeyInit();
#endif
}


#define CMD_ARGC_MAX	36
/**************************************************************************
 *                                                                        *
 *  Function Name: customerCmd                                            *
 *                                                                        *
 *  Purposes: Entry function of customer defined command "custom"         *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
#if SP5K_ICAT_EIS
#include "api/sp5k_eis_rsc_algo_api.h"
#include "sp5k_eis_rsc_algo_api.h"
#include "sp5k_rsc_api.h"
#endif

#if RTMP
#define AUDIO_CODEC SP5K_MEDIA_AUDIO_AAC
#if SP5K_ICAT_EIS
void EIS_STREAMING();
#endif
#endif

#define AE_SIZE 16
void ae_Cb( const aaaProcInfo_t * pinfo, aaaProcResult_t * presult )
{
    sp5kExpLimit_t explimit;
    sp5kAgcLimit_t agclimit;
    UINT32 i , Luma=0;
    static SINT32 EXP = 0 , EXPLIMIT=0;
    static UINT32 AEInit = 0 ,AGC=0 , AGCLIMIT=0 ,  AGCSTEP = 1 , EXPSTEP = 1 ;
    #define FRAMERATE 96 /*1/60s EXP time to keep 60fps*/


    /*First Initial AE*/
    if(AEInit==0)
    {
        /*Check Sensor EXP Table Size*/
        sp5kAeExpLimitGet(/*sensorModeGet()*/0x30,&explimit);
        EXPLIMIT = explimit.expIdxMax;
        EXPSTEP = 1;/*16 Steps per EV EXP Table Format Only*/

        /*Check Sensor AGC Table Size*/
        sp5kAeAgcLimitGet(/*sensorModeGet()*/0x30,&agclimit);
        AGCLIMIT = agclimit.agcIdxMax-agclimit.agcIdxMin;
        if(AGCLIMIT>255)/*64 Steps per EV AGC Table Format*/
        {
            AGCSTEP=4;
        }
        else/*16 Steps per EV AGC Table Format*/
        {
            AGCSTEP=1;
        }

        /*Initial EXP/AGC Setting*/
        EXP=FRAMERATE;
        AGC=agclimit.agcIdxMin;

        AEInit = 1;
    }



    /*Get AE Statistics Data*/
    for(i=0;i<AE_SIZE*AE_SIZE;i++)
    {
    	Luma += pinfo->paewin_y[i];
    }

    Luma/=(AE_SIZE*AE_SIZE);



    /*AE Converge to AE Target 10000~12000*/
    if(Luma>12000)
    {
        if(AGC >=AGCSTEP)
            AGC=AGC-AGCSTEP;
        else if(EXP < EXPLIMIT)
            EXP=EXP+EXPSTEP;
    }
    else if(Luma<10000)
    {
        if(EXP > FRAMERATE)
            EXP=EXP-EXPSTEP;
        else if(AGC <= AGCLIMIT-AGCSTEP)
            AGC=AGC+AGCSTEP;
    }


    /*Set EXP/AGC*/
    sp5kPreviewExpAgcSet(EXP, AGC);
    sp5kStillCapExpAgcSet(EXP, AGC);

}

#define AWB_SIZE 16
void awb_Cb( const aaaProcInfo_t * pinfo, aaaProcResult_t * presult )
{
    UINT8 i , j;
    UINT32 R=0,GR=0,B=0,GB=0;
    UINT16 RGain=1024,BGain=1024;

    /*Get AWB Statistics Data*/
    for(j=0;j<AWB_SIZE;j++)
    for(i=0;i<AWB_SIZE;i++)
    {
        /*0:SP5K_CFA_BAYER_GRBG, 1:SP5K_CFA_BAYER_RGGB, 2:SP5K_CFA_BAYER_BGGR, 3:SP5K_CFA_BAYER_GBRG.*/
        if(pinfo->awb_bayer_fmt==0)
        {
            GR+=pinfo->pawb_bayer[j*2*AWB_SIZE*2+i*2];
            R+=pinfo->pawb_bayer[j*2*AWB_SIZE*2+i*2+1];
      B+=pinfo->pawb_bayer[(j*2+1)*AWB_SIZE*2+i*2];
            GB+=pinfo->pawb_bayer[(j*2+1)*AWB_SIZE*2+i*2+1];
        }
        else if(pinfo->awb_bayer_fmt==1)
        {
            R+=pinfo->pawb_bayer[j*2*AWB_SIZE*2+i*2];
            GR+=pinfo->pawb_bayer[j*2*AWB_SIZE*2+i*2+1];
      GB+=pinfo->pawb_bayer[(j*2+1)*AWB_SIZE*2+i*2];
            B+=pinfo->pawb_bayer[(j*2+1)*AWB_SIZE*2+i*2+1];
        }
        else if(pinfo->awb_bayer_fmt==2)
        {
            B+=pinfo->pawb_bayer[j*2*AWB_SIZE*2+i*2];
            GB+=pinfo->pawb_bayer[j*2*AWB_SIZE*2+i*2+1];
      GR+=pinfo->pawb_bayer[(j*2+1)*AWB_SIZE*2+i*2];
            R+=pinfo->pawb_bayer[(j*2+1)*AWB_SIZE*2+i*2+1];
        }
        else /*if(pinfo->awb_bayer_fmt==3)*/
        {
            GB+=pinfo->pawb_bayer[j*2*AWB_SIZE*2+i*2];
            B+=pinfo->pawb_bayer[j*2*AWB_SIZE*2+i*2+1];
      R+=pinfo->pawb_bayer[(j*2+1)*AWB_SIZE*2+i*2];
            GR+=pinfo->pawb_bayer[(j*2+1)*AWB_SIZE*2+i*2+1];
        }
    }


    /*Calculate WB Gain*/
    RGain = ((GR+GB)*1024)/(R*2);
    BGain = ((GR+GB)*1024)/(B*2);


    /*Set WB Gain*/
    sp5kIqModeSet( SP5K_MODE_STILL_PREVIEW  );
    sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_R_GAIN, RGain);
    sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_GR_GAIN, 1024);
    sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_B_GAIN, BGain);
    sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_GB_GAIN, 1024);
    sp5kIqModeSetDone( SP5K_MODE_STILL_PREVIEW );

    sp5kIqModeSet( SP5K_MODE_STILL_SNAP  );
    sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_R_GAIN, RGain);
    sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_GR_GAIN, 1024);
    sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_B_GAIN, BGain);
    sp5kIqCfgSet( SP5K_IQ_CFG_WB_GAIN_GB_GAIN, 1024);
    sp5kIqModeSetDone( SP5K_MODE_STILL_SNAP );
}


void
customerCmd
(
	int argc,
	char *argv[]
)
{

	if (!*argv)
	{
		printf("null custom command\n");
		return;
	}

	UINT32 i,v[CMD_ARGC_MAX];

	memset(v,0,sizeof(v));

    printf("Receive [%d] command......", argc);
	for (i=0 ; i<argc ; i++)
    {
		v[i] = strtoul(argv[i], NULL, 0);
        printf("v[%d]=\"%s\" ",(SINT32) i , argv[i]);
    }
    printf("\n");

	if (0) {
	}

#if defined(SMB_CLIENT_SUPPORT) && SMB_CLIENT_SUPPORT
	else if (!strcmp(*argv,"smbmount")){
		if(argc < 4){
			printf("command argument err!\n"
				"\tusage: smbmount <service> <account> <password>\n"
				"\tfor example: smbmount //172.28.53.199/smbserver icatchtek password\n");
		}
		else {
			extern int smbMount(char* serv, char* user, char* passwd);
			smbMount(argv[1], argv[2], argv[3]);
		}
	}
#endif
    else if (strcmp(*argv, "ASSERT")==0)
    {
        HOST_ASSERT(0);
    }
	#if (SPCA5330&&!SPCA6330)
	else if (!strcmp(*argv, "sdio")) {
		printf("dbg_sdio_cmd53rw_test vid=%x, did=%x\n", v[1], v[2]);
		dbg_sdio_cmd53rw_test(v[1], v[2]);
	}
	#endif

    #if SP5K_DIQ_HDR
   	else if (strcmp(*argv, "hdr")==0) {
        #if 0
    	    appAutoHDRLibEnable(v[1]);
            appVideoHDRModeSetting(v[1]);
            appAeHDRALGEnable(v[1]);
        #else
        	HDR_En = v[1];
		#endif
        printf("HDR setting %u\n", v[1]);
	}
    #endif
    #if SP5K_DIQ_AUTO_WDR
	else if (strcmp(*argv, "wdr")==0) {
	    appAutoWDRLibEable(v[1]); /*MUST enable IQ block*/
        printf("WDR %d\n", (SINT32)v[1]);
	}
    #endif
#if 0   /*FOR HDMI test*/
	else if (!strcmp(*argv, "hdmivic")) {
		HDMI_vic = v[1];
		printf("Fix HDMI VIC to vic[%d]\n", HDMI_vic);
	}
	else if (!strcmp(*argv, "hdmidly")) {
		if (argc != 1)
		{
			HDMI_IN_DLY = v[1];
			HDMI_OUT_DLY = v[2];
		}
		printf("Current HDMI_IN_DLY=%dms  HDMI_OUT_DLY=%dms\n", HDMI_IN_DLY, HDMI_OUT_DLY);
	}
#endif
	#if DROPBOX_FLOW_TRIGGER
	else if (!strcmp(*argv, "pcs")){
		printf("custom command[%s]\n", argv[0]);
		extern int app_test_pcs(int argc, UINT8 *argv[]);
		if (0 != app_test_pcs(argc, argv)) {
			printf("Test Fail:[%s]\n", argv[0]);
		}
		return;
	}
	#endif
	else if (!strcmp(*argv, "audiotest")) {
		printf("goes to audio\n\n");
		UINT32 hwAGCGain = 0x80000001;

		sp5kAudDevSrcSet(SP5K_AUD_DEV_REC, SP5K_AUD_DEV_REC_SRC_MIC_IN|SP5K_AUD_REC_CH_ALL);

		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_WAV );
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_UNKNOWN );
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM );
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, 44100 );
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, 16 );
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 2 );
		/*volume */
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC|SP5K_AUD_REC_CH_ALL, 32);
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST|SP5K_AUD_REC_CH_ALL, hwAGCGain);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_OFF);
		/* ALC off
		UINT16 AudAlcHb=800;
		UINT16 AudAlcLb=300;
		UINT8 AudAlcMaxVol=22;
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_MAX_VOL, AudAlcMaxVol);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_HB, AudAlcHb);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_LB, AudAlcLb);
		*/

		sp5kModeSet( SP5K_MODE_AUDIO_RECORD );
		sp5kModeWait(SP5K_MODE_AUDIO_RECORD);
		UINT32 i = 0;
		while(1){
			/*
			memdump(0xb0006050, 1);
			memdump(0xb000605c, 1);
			memdump(0xb0006068, 1);
			memdump(0xb00060c6, 4);
			memdump(0xb000612c, 4);
			memdump(0xb0006400, 1);
			memdump(0xb0006404, 2);
			memdump(0xb0006408, 2);
			*/
			sp5kTimeDelay(3, 500);
			i++;
			if( i > 10 ){
				break;
			}
		}

		sp5kModeSet( SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		return;
	}

	else if(!strcmp(*argv, "lcdpg")) {
		WRITE8(0xb000a600, v[1]);
		sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, 1);
	}

		/* ============== Print Screen Menu ============== */
	else if(!strcmp(*argv, "psmenu")) {
		printf("Start Printf Screen\n");

		sp5kGfxPageCapabDesc_t imgPage, osdPage;
		/* get PIP shown page capability */
		sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_PIP_0, &imgPage);
		/* get OSD shown page capability */
		sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_SHOWN, &osdPage);
		/* basic check to make sure the pages size are the same */
		if (imgPage.frmW != osdPage.frmW || imgPage.frmH != osdPage.frmH)
			return;

		/* build a gfx object based on imgPage data */
		sp5kGfxObj_t imgRgbObj = { imgPage.pbuf, NULL, imgPage.frmW, imgPage.frmH, imgPage.frmRoiX, imgPage.frmRoiY,
			imgPage.fmt, imgPage.frmRoiW, imgPage.frmRoiH };

		/* build a gfx object based on osdPage data */
		sp5kGfxObj_t osdRgbObj = { osdPage.pbuf, osdPage.pabuf, osdPage.frmW, osdPage.frmH, osdPage.frmRoiX, osdPage.frmRoiY,
			osdPage.fmt, osdPage.frmRoiW, osdPage.frmRoiH };
		/* merge OSD & image, put the result back to imgRgbObj */
		sp5kGfxObjectPerPixelBlend(&osdRgbObj, &imgRgbObj, &imgRgbObj);
		/* write result to D:\X.RGB */
		fsSimpleWrite((UINT8 *)"D:\\X.RGB", imgRgbObj.pbuf, imgRgbObj.bufW*imgRgbObj.bufH*2);
	}

	/* ============== snap ============== */
	else if(!strcmp(*argv, "raw")) {
		sp5kStillCapCfgSet(SP5K_CAPTURE_SAVE_RAW, (v[2]<<1 | v[1]) );
		printf("%s raw capture\n", v[1]?"Enable":"Disable");

	}
	else if (!strcmp(*argv, "stillsnap")) {
			UINT32 snapnum = 1, sp5kSnapMode = SP5K_CAPTURE_MODE_SINGLE;
			static UINT32 snapSensorMode=0x40;
			sp5kImgSize_t captSize;

			sp5kCapabilitySnap_t SnapCap;

			if(v[1]!=snapSensorMode){
				printf("Switch Snap Sensor Mode from 0x%02x to 0x%02x\n",snapSensorMode,v[1]);

				snapSensorMode=v[1];

				sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,0x30);
				sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP,snapSensorMode);

				sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
				sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
			}

			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,0x31);
			sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
			sp5kModeWait(SP5K_MODE_STILL_PREVIEW);

			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,0x30);
			sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
			sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
			sp5kModeCfgGet(SP5K_MODE_CFG_SNAP_CAP_GET,&SnapCap,sizeof(SnapCap));

			extern const sensorTgSnapCfg_t * psensorTgSnapCfgGet();
			const sensorTgSnapCfg_t *pscfg = psensorTgSnapCfgGet();

			sp5kFsDirChange((const UINT8 *)"D:\\DCIM\\100MEDIA");

#if STILL_SIZE_WIDTH&&STILL_SIZE_HEIGHT&&0
			captSize.width = STILL_SIZE_WIDTH;
			captSize.height = STILL_SIZE_HEIGHT;
#else
			captSize.width = pscfg->hsize;
			captSize.height = pscfg->vsize;
#endif
			printf("snap size W: %d, H: %d\n",captSize.width, captSize.height);

			sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, (UINT32)&captSize);

			sp5kStillCapCfgSet(SP5K_CAPTURE_MODE, sp5kSnapMode);

			sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_LENGTH, snapnum);

			sp5kModeSet(SP5K_MODE_STILL_SNAP);

			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 2000);

		}
		else if (!strcmp(*argv, "snapstate")) {

			extern void snapDbgAllState(void);
			snapDbgAllState();
		}
		/* ============== pb ============== */
		else if (!strcmp(*argv,  "pb")) {
			if (argc==1) {
				printf( "usage: pb\n"
						"usage: pb index type(0:full, 1:thm, 3:qv)\n"
						);
			} else {
				UINT32 mode,err;
				UINT8 filename[30];
				sp5kDcfAttrElm_t attr;

				sp5kModeGet(&mode);
				if(mode != SP5K_MODE_STANDBY){
					sp5kModeSet(SP5K_MODE_STANDBY);
					sp5kModeWait(SP5K_MODE_STANDBY);
				}

				err = sp5kDcfFsCurFileIdxSet(v[1]);
				err += sp5kDcfFsFileAttrGet(v[1], &attr, filename);
				if (err) {
					printf( "pb: no file (%x)\n", err);
					return;
				}
				printf( "pb:%s idx[1]=%x type[2]=%x \n",filename,v[1],v[2]);
				if (attr.fileType & (SP5K_DCF_FILETYPE_WAV|SP5K_DCF_FILETYPE_AVI)) {
#if SPCA6350 /* v35host */
					sp5kMediaPlayAttrSet(SP5K_MEDIA_ATTR_FILE_NAME, (UINT32)filename);
#else
					sp5kMediaPlayFileNameSet(filename);
#endif
					if (sp5kMediaPlayAttrParse()==SUCCESS) {
						sp5kMediaPlayControl(SP5K_MEDIA_PLAY_START, 0, 0);
						sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 10000);
						sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0);
					}
				} else if(attr.fileType & (SP5K_DCF_FILETYPE_JPG_MEMO|SP5K_DCF_FILETYPE_JPG)) {
					sp5kStillPlayCfgSet(SP5K_SPB_IMG_TYPE, v[2]);
					sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
					sp5kTimeDelay(3, 2000);
				} else {
					printf( "pb:incorrect file\n");
				}
			}
		}
		else if (!strcmp(*argv, "pbstate")) {
			extern void pbDbgAllState(void);
			pbDbgAllState();
		}
		/* ============== Video ============== */
		else if (strcmp(*argv, "recmov")==0) {
			UINT32 mode;
			sp5kModeGet(&mode);
			if(mode != SP5K_MODE_VIDEO_PREVIEW){
				sp5kZoomFactorSet(SP5K_MODE_VIDEO_PREVIEW, 1000);

				sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, 0x32);
				sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
				sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
			}

			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_MOV);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, 8000);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, 8);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 1);
#if SPCA6350 /* v35host */
#else
			static UINT16 AudAlcHb=800;
			static UINT16 AudAlcLb=300;
			static UINT8 AudAlcMaxVol=22;
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_MAX_VOL, AudAlcMaxVol);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_HB, AudAlcHb);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_LB, AudAlcLb);
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC_MUTE, 0);
#endif
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_ON);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, 1280);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, 720);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, 4);/* need sensor check */
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, 400*1024*8);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_BRC_TYPE, SP5K_MEDIA_CBR );
			sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, 25);
			sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST, 0xC0000014);
			sp5kZoomFactorSet(SP5K_MODE_VIDEO_RECORD, 1000);

			printf("movrec : start\n");
			sp5kModeSet(SP5K_MODE_VIDEO_RECORD);
			sp5kModeWait(SP5K_MODE_VIDEO_RECORD);

			sp5kTimeDelay(3, 6000);

			sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
			sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, 0x30);
			sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
			sp5kModeWait(SP5K_MODE_STILL_PREVIEW);

			printf("movrec : stop\n");
		}
		else if (!strcmp(*argv, "rec720")) {

			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, 1280);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, 720);

			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_MJPG);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_AVI);

			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, 44100);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, 16);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 1);
			sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC|SP5K_AUD_REC_CH_ALL, 32);

			sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, 0x32);
			sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
			sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

			printf("avirec : start\n");

			sp5kModeSet(SP5K_MODE_VIDEO_RECORD);
			sp5kModeWait(SP5K_MODE_VIDEO_RECORD);

			sp5kTimeDelay(3, 5000);

			sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
			sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, 0x30);
			sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
			sp5kModeWait(SP5K_MODE_STILL_PREVIEW);

			printf("avirec : stop\n");
		}
		else if (!strcmp(*argv, "type")) {
		{
			void *buf;
			UINT32 size;
			if (fsSimpleReadEx((UINT8*)argv[1], &buf, &size)==SUCCESS) {
				UINT32 ofs,len;
				ofs = v[2];
				if (ofs>size) ofs = size;
				len = v[3];
				if (len==0 || ofs+len>size) len = size - ofs;
				memdump(((UINT8*)buf+ofs), len);
				sp5kFree(buf);
				}
			}
		}
		else if (!strcmp(*argv,"dramdma") ) {
			UINT32 srcbyteaddr=0, dstbyteaddr, finaladdr=0, cmpaddr, num=0, k, b, error=0, block=1024;
			UINT32 testsize =1024*block;
			UINT32 emptyaddr=0xa203b440;
			/*UINT32 maximaddr=0xa4000000-testsize; 512Mbits*/
			UINT32 maximaddr=0xa303b440-testsize;/*1GMbits*/

			WRITE8 (0xb0001018, 0x01);
			printf("dram test\n");
			WRITE8 (0xb0003008, 0x00);           /*inten         */
			WRITE8 (0xb0003084, 0x00);           /*Src: SDRAM    */
			WRITE8 (0xb0003085, 0x00);           /*Src: Front    */
			WRITE32(0xb0003090, testsize-1);     /*byte cnt      */


			i=0;
			error=0;
			for(b=0;b<block;b++){
				for(k=0;k<=0xff;k++)WRITE8(emptyaddr+1024*b  +(k<<2),k);
				for(k=0;k<=0xff;k++)WRITE8(emptyaddr+1024*b+1+(k<<2),0xff-k);
				for(k=0;k<=0xff;k++)WRITE8(emptyaddr+1024*b+2+(k<<2),0xff-k);
				for(k=0;k<=0xff;k++)WRITE8(emptyaddr+1024*b+3+(k<<2),k);
			}
			srcbyteaddr=emptyaddr;
			dstbyteaddr=emptyaddr+testsize;

#if 1
			/*while(1){num++;*/
			for(num=0;num<2000;num++){
				printf("num=%d\n",(SINT32)num);
				while(dstbyteaddr<=maximaddr){
					if(error){/*printf("error=%d\n",error);*/break;}
					WRITE32(0xb0003088, srcbyteaddr);    /*srcaddr,byte  */
					WRITE32(0xb000308c, dstbyteaddr);    /*dstaddr,byte  */
					WRITE8 (0xb0003000, 0x04);           /*clear dma2_evt*/
					WRITE8 (0xb0003080, 0x01);           /*dma2start     */
					while((READ8(0xb0003000)&0x04)==0x00)/*printf("!")*/;
						i++;
					finaladdr  =dstbyteaddr;
					srcbyteaddr=dstbyteaddr;
					dstbyteaddr+=(testsize+i%16);
					printf("finaladdr=%x,i=%d\n",finaladdr,(SINT32) i );
					/*if(dstbyteaddr>=0xa0b01000)break;*/
					for(b=0;b<block;b++){
						if(error) break;
						cmpaddr=finaladdr+1024*b;
						if(!(cmpaddr%4)){
							for(k=0;k<=0xff;k++)if(READ32(cmpaddr +(k<<2))!= ((k<<24)|((0xff-k)<<16)|((0xff-k)<<8)|k)){error++;printf("32 %x %x\n",k,READ32(cmpaddr +(k<<2)));break;}
						} else {
							for(k=0;k<=0xff;k++)if(READ8(cmpaddr  +(k<<2))!=      k ){error++;printf("0 %x %x\n",     k,READ8(cmpaddr  +(k<<2)));break;}
							for(k=0;k<=0xff;k++)if(READ8(cmpaddr+1+(k<<2))!=(0xff-k)){error++;printf("1 %x %x\n",0xff-k,READ8(cmpaddr+1+(k<<2)));break;}
							for(k=0;k<=0xff;k++)if(READ8(cmpaddr+2+(k<<2))!=(0xff-k)){error++;printf("2 %x %x\n",0xff-k,READ8(cmpaddr+2+(k<<2)));break;}
							for(k=0;k<=0xff;k++)if(READ8(cmpaddr+3+(k<<2))!=      k ){error++;printf("3 %x %x\n",     k,READ8(cmpaddr+3+(k<<2)));break;}
						}
					}
				}

				srcbyteaddr=finaladdr;
				dstbyteaddr=finaladdr-(testsize+i%16);
				while(dstbyteaddr>=emptyaddr){
					if(error){/*printf("error=%d\n",error);*/break;}
					WRITE32(0xb0003088, srcbyteaddr);    /*srcaddr,byte  */
					WRITE32(0xb000308c, dstbyteaddr);    /*dstaddr,byte  */
					WRITE8 (0xb0003000, 0x04);           /*clear dma2_evt*/
					WRITE8 (0xb0003080, 0x01);           /*dma2start     */
					while((READ8(0xb0003000)&0x04)==0x00)/*printf("!")*/;
						i++;
					finaladdr  =dstbyteaddr;
					srcbyteaddr=dstbyteaddr;
					dstbyteaddr-=(testsize+i%16);
					printf("finaladdr=%x,i=%d\n",finaladdr,(SINT32) i );
					for(b=0;b<block;b++){
						if(error) break;
						cmpaddr=finaladdr+1024*b;
						if(!(cmpaddr%4)){
							for(k=0;k<=0xff;k++)if(READ32(cmpaddr +(k<<2))!= ((k<<24)|((0xff-k)<<16)|((0xff-k)<<8)|k)){error++;break;}
						} else {
							for(k=0;k<=0xff;k++)if(READ8(cmpaddr  +(k<<2))!=      k ){error++;break;/*printf("0 %x %x\n",     k,READ8(cmpaddr  +(k<<2)));*/}
							for(k=0;k<=0xff;k++)if(READ8(cmpaddr+1+(k<<2))!=(0xff-k)){error++;break;/*printf("1 %x %x\n",0xff-k,READ8(cmpaddr+1+(k<<2)));*/}
							for(k=0;k<=0xff;k++)if(READ8(cmpaddr+2+(k<<2))!=(0xff-k)){error++;break;/*printf("2 %x %x\n",0xff-k,READ8(cmpaddr+2+(k<<2)));*/}
							for(k=0;k<=0xff;k++)if(READ8(cmpaddr+3+(k<<2))!=      k ){error++;break;/*printf("3 %x %x\n",     k,READ8(cmpaddr+3+(k<<2)));*/}
						}
					}
				}

				if(error) break;
				else      printf(".");
				srcbyteaddr=finaladdr;
				dstbyteaddr=finaladdr+(testsize+i%16);
			}/*for{num} or while*/

			printf("num=%d,error=%d\n",(SINT32)num,(SINT32)error);

			WRITE8 (0xb0003008, 0xff);           /*inten         */
#endif
			WRITE8 (0xb0001018, 0x01);
		}
		else if (strcmp(*argv, "tgclk")==0) {
			UINT32 clk;
			if(v[1]!=0){
				clk=v[1];
			}
			else{
				clk=40500000;
			}

			sensorTgPllClkExtSet(clk);	/*40.5MHz*/
		}
		else if (strcmp(*argv, "tgw")==0) {
			UINT8 i;
			
			UINT8 regdata1[] = {
				0x10, 0x40, 0x00, 0x00,
			};

			regdata1[1]= (v[1] & 0xff00)>>8;
			regdata1[0]=  v[1] & 0x00ff;
			regdata1[3]= (v[2] & 0xff00)>>8;
			regdata1[2]=  v[2] & 0x00ff;

			printf("regdata1[0]=%x, regdata1[1]=%x, regdata1[2]=%x, regdata1[3]=%x\n",regdata1[0],regdata1[1],regdata1[2],regdata1[3]);
			i= READ8(0xb0009104);
			if (i==0) {
				UINT32 cmdportid =0;
				printf("\n sensorSerialAsyncWrite =%d\n", sensorSerialAsyncWrite( cmdportid, sizeof(regdata1)/4, regdata1 ));
			}
			else {
				printf(" spi not ready\n");
			}
		}
			/*********************tg adi read function ********************/
		else if (strcmp(*argv, "tgr")==0) {
		/*********************tg adi read function ********************/

		/*#include "sensor_model.h"*/
			typedef struct { /*ITG has this struct NULL*/
								UINT8 mode;
								UINT8 format2;
								UINT8 format3;
								UINT8 speed;
								UINT8 priority; /*always 0 for sensor(TG,AFE)*/
								UINT8 nbit_regdata;
								UINT8 nbit_regaddr;
			} sensorCmdport_t;

			static sensorCmdport_t sensor_cmdport_xtg_cfg1 = {
							  0x11,
							  0,
							  0,
							  8,
							  0,
							  16,
							  16,
			};
			static sensorCmdport_t sensor_cmdport_xtg_cfg2 = {
							  0x11,
							  0,
							  0,
							  8,
							  0,
							  8,
							  8,
			};
extern void sensorCmdportSet( UINT32 cmdportid, sensorCmdport_t *pcmdport );


#define SDA_MSK  (1 << 7)
#define CS_MSK	 (1 << 10)
#define SCL_LO	WRITE8(0xb0009152,0x00);
#define SCL_HI	WRITE8(0xb0009152,0x02);
#define SDA_LO	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SDA_MSK, 0)
#define SDA_HI	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SDA_MSK, SDA_MSK)
#define CS_LO	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, CS_MSK, 0)
#define CS_HI	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, CS_MSK, CS_MSK)
							/*1: set SIF1SEN1 GPIO mode  0: Function mode */

#define TGGPIO10_CFG(a) \
		if(1){\
			UINT8 tmp;\
		if(a){tmp= READ8(0xb0009035);WRITE8(0xb0009035,tmp|0x04);}\
		else {tmp= READ8(0xb0009035);WRITE8(0xb0009035,tmp&0x0B);}}

#if defined(HW_KX1308)|defined(HW_CK26200)|defined(HW_VSF3500Z)|defined(HW_VSD1200Z)
		/*FMIF24configuration */

#define SDATAin_CFG() \
		if(1){\
		UINT8 tmp;\
			tmp= READ8(0xb000404b);WRITE8(0xb000404b,tmp|0x01);\
			tmp= READ8(0xb000403b);WRITE8(0xb000403b,tmp&0xFE);}
							/*FMIF24GPI Value */
#define SDATAin_GET() ((READ8(0xb0004043)&0x01)==0x01)

printf("FMIF24 \n");

#else

/*LMI7 configuration */

printf("LMI7 \n");

#define SDATAin_CFG() \
		if(1){\
		UINT8 tmp;\
			tmp= READ8(0xb0001034);WRITE8(0xb0001034,tmp&0x7f);\
			tmp= READ8(0xb0001034);WRITE8(0xb0001034,tmp&0x7f);}
							/*LMI7 GPI Value */
#define SDATAin_GET() ((READ8(0xb0001038)&0x80)==0x80)

#endif


#define DELAY	{ UINT32 _delay; for (_delay=0 ; _delay<3000 ; _delay++); }
#define LO_BYTE(w)			((UINT8)(w))
#define HI_BYTE(w)			((UINT8)(((UINT16)(w) >> 8) & 0xFF))
		UINT8 i;
		UINT16 readback;
		UINT8 tmp1, tmp2;
		UINT32 cmdportid = 0;

	sensor_event_wait(0x04); /*wait VD*/
		/*STEP1. Switch ADI GPO2 as SDA ouput */
		/*STEP2. Switch ADI as Readback mode*/
		/*STEP3. Pre write a dummy data with the adress that you want to readback */

#if defined(SP5K_SENSOR_AFE_AD9004)
	/* ad9004*/
		printf("ad9004\n");

		static UINT8 regdata1[] = {
									0x2f, 0xc0, 0x00, 0x04,
									0x2b, 0xc0, 0x01, 0x00,
									/*dummy write*/
									0xff, 0xff, 0x00, 0x00,
								};
#else /* ad9010*/
		printf("ad9010\n");

		static UINT8 regdata1[] = {
									0x2f, 0xc0, 0x20, 0x00,
									0x2b, 0xc0, 0x01, 0x00,
									/*dummy write*/
									0xff, 0xff, 0x00, 0x00,
								};

#endif
			regdata1[8]=LO_BYTE(v[1]);
			regdata1[9]=HI_BYTE(v[1]);
			sensorSerialAsyncWrite( cmdportid, sizeof(regdata1)/4, regdata1);
			sensorSerialFlush(cmdportid);
			sensor_event_wait(0x04);


			/*STEP4. write a dummy data with the adress and readback the data from ADI GPO2 */
			/*STEP4.1. write address*/
			/*config as 16bit format write adress*/
			sensorCmdportSet(cmdportid,&sensor_cmdport_xtg_cfg2);
			static UINT8 regdata2[] = {
								0xff, 0xff,
			};
			regdata2[0]=LO_BYTE(v[1]);
			regdata2[1]=HI_BYTE(v[1]);
			sensorSerialAsyncWrite( cmdportid,1, regdata2);
			sensorSerialFlush(cmdportid);
			/*STEP4.2. read ADI GPO2 from LMI7. use clk idle state leve to generate the  clock */
			readback=0;
			SDATAin_CFG();
			TGGPIO10_CFG(1);/* TGGPIOSEN1  select to gpio */
			CS_LO;
			SCL_LO;
			DELAY;
			for ( i = 0; i < 16; i++ ) {
				SCL_HI;
				readback |= (SDATAin_GET()<<( i));
				DELAY;
				SCL_LO;
				DELAY;
			}
			CS_HI;
			SCL_HI;
			DELAY;
			/*STEP5. Switch ADI to Write mode. Switch ADI GPO2 ro GPO2*/
			static UINT8 regdata3[] = {
				0x2f, 0xc0, 0x00, 0x00,
				0x2b, 0xc0, 0x00, 0x00	};
			sensorCmdportSet(cmdportid,&sensor_cmdport_xtg_cfg1);
			sensorSerialAsyncWrite( cmdportid, sizeof(regdata3)/4, regdata3);
			sensorSerialFlush(cmdportid);
			/*return the reg value */
			tmp1 = HI_BYTE(readback);
			tmp2 = LO_BYTE(readback);
			readback = (UINT16)(tmp1)<<8 | tmp2;
			TGGPIO10_CFG(0);/* TGGPIOSEN1  select to 3-wire*/
			printf("[ADI SPI READ Test]Addr=[%x] Data=[%x]\n",v[1],readback);

			/*end tg adi read********************************************/
		}
		else if (strcmp(*argv, "mode")==0) {
		UINT32  sp5kmode = 0;
		UINT32  ret = SUCCESS;
		if (argc==1) {
			printf("# sp5k mode \n"
				  "# mode: SP5K mode index type( 0x88: SP5K_MODE_STANDBY ...)\n");
			ret = sp5kModeGet(&sp5kmode);
			printf("sp5kModeGet() = 0x%x, ret = 0x%X\n",sp5kmode,ret);
		}
		else {
			UINT32 i;
			printf("sp5kModeSet=>0x%X\n",v[1]);
			for (i=1; i<argc ; i++) {
				sp5kModeSet(v[i]);
				if (i<argc-1)
					sp5kModeWait(v[1]);
			}
		}
	}
	else if (!strcmp(*argv, "imgtest")) {
#if 0
		if (argc >= 2) {
			UINT32 mode;
			sp5kGfxPageCapabDesc_t pageCapa;

			sp5kModeGet(&mode);
			if (mode == SP5K_MODE_STANDBY) {
				if (v[1]) {
					sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_IMAGE, &pageCapa);
					printf("img buf=0x%x w=%d h=%d\n", (UINT32)pageCapa.pbuf, pageCapa.frmW, pageCapa.frmH);

					sp5kGfxRectFill(SP5K_GFX_PAGE_IMAGE, 0, 0, pageCapa.frmW, pageCapa.frmH, 0x80FFE0);
				}
				sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, v[1] ? 1 : 0, 0, 0, 0);
				printf("img layer: %d\n", v[1]);
			}
			else {
				printf("sp5k mode=0x%x\n", mode);
			}
		}
		else {
			printf("USAGE: %s <0/1>\n", argv[0]);
			printf("       0=off, 1=on\n");
		}
#endif
	}
	else if ( strcmp(*argv, "calimode")==0) {

		if(argc>3)
			appSensorCaliModeset(v[1],v[2],v[3]);
		else
			appSensorCaliModeset(v[1],v[2],SP5K_AE_EXP_AGC_DONT_CHANGE);

	}
#if SP5K_DIQ_FRMWRK
	else if ( strcmp(*argv, "diqct")==0) {
		sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_FIX_CT_IDX_SET,v[1],v[2],v[3]);
		printf("DIQ CT CFG %d,%d,%d \n",(SINT32)v[1],(SINT32)v[2],(SINT32)v[3]);
	}
	else if ( strcmp(*argv, "diqiso")==0) {
		sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_FIX_ISO_IDX_SET,v[1],v[2],v[3]);
		printf("DIQ ISO CFG %d,%d,%d \n",(SINT32)v[1],(SINT32)v[2],(SINT32)v[3]);
	}
#endif
	else if ( strcmp(*argv, "cv")==0) {

		if(v[1]==1)
		{
			cdspAfwinSize_t afParam = {3, 3, 0, 0, 256, 256, 128, 128};
			extern void sp5kAfInit(void);

			/*AF Init*/
			sp5kAfInit();

			/*AF Window ROI Init*/
			sp5kAfWinSizeSet(&afParam);

			/*AF Curve Parameter Init*/
			sp5kAfCfgSet(SP5K_AF_STAT_MODE,SP5K_AF_STAT_G_SQR);
			sp5kAfCfgSet(SP5K_AF_CFG_GAMMA_TYPE,2);
			sp5kAfCfgSet(SP5K_AF_GAIN,64);
			sp5kAfCfgSet(SP5K_AF_CORING_THR,0);
			sp5kAfCfgSet(SP5K_AF_CORING_VALUE,0);
			sp5kAfCfgSet(SP5K_AF_NOISE_THR,0);
			sp5kAfCfgSet(SP5K_AF_SINGLE_MODE,1);
			sp5kAfCfgSet(SP5K_AF_SINGLE_HPF,0);
			sp5kAfCfgSet(SP5K_AF_SINGLE_MEDIAN,0);
			sp5kAfCfgSet(SP5K_AF_SINGLE_LPF,9);
			sp5kAfCfgSet(SP5K_AF_CFG_FILTER_WEIGHT,0x80000107);

			/*AF Callback Register*/
			sp5kAfCustomCbSet(afCVCB);

			sp5kAfModeSet(SP5K_AF_MODE_ON);
		}
		else if(v[1]==0)
		{
			sp5kAfModeSet(SP5K_AF_MODE_OFF);
		}

	}
	else if ( strcmp(*argv, "cae")==0) {
		sp5kAeCfgSet(SP5K_AE_CONTINOUS_ENABLE,1);
		sp5kSystemCfgSet( SP5K_CDSP_INFO_CFG_GROUP, CDSP_INFO_CFG_AE_BIT_INCREASE, 6);
		sp5kAeCustomAewinSizeSet(AE_SIZE, AE_SIZE);
		sp5kAeCustomCbSet(AASTAT_INFO_FLAG_AEWIN_Y | AASTAT_INFO_FLAG_HIS_Y, ae_Cb);
		sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	}
	else if ( strcmp(*argv, "cawb")==0) {
		sp5kAwbCfgSet(SP5K_AWB_USE_BAYER_PATTERN, 1 );
		sp5kAwbCustomAwbwinSizeSet(AWB_SIZE , AWB_SIZE );
		sp5kAwbCfgSet(SP5K_AWB_ACCUM_PERIOD , 1 );
		sp5kAwbCustomCbSet(SP5K_AEAWB_INFO_BIT_AWB_RGB, awb_Cb );
		sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY );
	}
       else if ( strcmp(*argv, "awbroi")==0) {
		sp5kAwbCustomWinRoiSet(v[1],v[2],v[3],v[4]);
	}
	else if (strcmp(*argv, "mvtest")==0) {
#if 0
		if(v[1]==1){
			printf("mvtest \n");
			appMviewCreateTest();
		}
		else {
			appMviewDestroy();
		}
#endif
	}
	else if (strcmp(*argv, "sensus")==0) {
		if(v[1]==1){
			sensorSuspendExecute();
		}
		else if(v[1]==2){
			sensorResumeExecute();
		}
		else if(v[1]==3){
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);

			sensorReloadExecute(v[2]);

			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,0x30);
			sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
			sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
		}
		else if(v[1]==4){
		/*
		sensorStsCfg(SENSOR_SYS_CFG_AUTO_PWRSAVE_TIMEOUT, val);
		*/

			sp5kSystemCfgSet(SP5K_SENSOR_SYS_CFG,SENSOR_SYS_CFG_AUTO_PWRSAVE_TIMEOUT,v[2]);
		}
	}
	else if (strcmp(*argv, "pvvga")==0) {
		if(argc >= 2){
			SINT32 exp;
			UINT32 agc;

			sp5kPreviewExpAgcGet(&exp, &agc);
			printf("exp/agc: %d/%d => %d/%d\n", exp, (SINT32)agc, (SINT32)v[1], (SINT32)v[2]);
			if (v[1] && v[2]) sp5kPreviewExpAgcSet(v[1], v[2]);
			else if (v[1]) sp5kPreviewExpAgcSet(v[1], SP5K_AE_EXP_AGC_DONT_CHANGE);
			else if (v[2]) sp5kPreviewExpAgcSet(SP5K_AE_EXP_AGC_DONT_CHANGE, v[2]);
		}
		sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,0x30);
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
		sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
	}
	else if (strcmp(*argv, "pvaf")==0) {
		sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,0x31);
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
		sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
	}
	else if (strcmp(*argv, "pv720p")==0) {
		sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW,0x32);

		sp5kModeCfgSet( SP5K_MODE_CFG_PV_DISP_FIT, 1 );
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
		sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
		sp5kModeCfgSet( SP5K_MODE_CFG_PV_DISP_FIT, 0);
	}
	else if (strcmp(*argv, "sensormode")==0) {
		UINT32 senMode, mode;

		sp5kModeGet(&mode);
		senMode = sp5kSensorModeCfgGet(mode);

		printf("5K mode:%x sen mode:%x , %x\n", mode, senMode,sensorModeGet());

		if (argc >= 2) {

			sensorModeSet(v[1]);
			printf("set to %x",v[1]);
		}
	}
	else if (strcmp(*argv,"flip")==0) {
		printf("..flip:0x%x\n",v[1]);
		sp5kSystemCfgSet(SP5K_CDSP_FLIP_CFG,v[1]); /* param: SP5K_FLIP_VAL_NOFLIP / SP5K_FLIP_VAL_HFLIP / SP5K_FLIP_VAL_VFLIP / SP5K_FLIP_VAL_BOTHFLIP */
		sp5kSystemCfgSet(SP5K_CDSP_AEAF_SW_FLIP_CFG,SP5K_AA_SW_FLIP_CFG_AEWIN,v[1]); /* param: SP5K_FLIP_VAL_NOFLIP / SP5K_FLIP_VAL_HFLIP / SP5K_FLIP_VAL_VFLIP / SP5K_FLIP_VAL_BOTHFLIP */
		sp5kSystemCfgSet(SP5K_CDSP_AEAF_SW_FLIP_CFG,SP5K_AA_SW_FLIP_CFG_AFWIN,v[1]);

		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);

		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
		sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
	}
	else if (strcmp(*argv,"aeon")==0) {
		sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
		sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
		printf("aeon...\n");
	}
	else if (strcmp(*argv,"aeoff")==0) {
		sp5kAeModeSet(0);
		sp5kAwbModeSet(0);
		printf("aeoff...\n");
	}
	else if (strcmp(*argv, "pvae")==0) {
		SINT32 exp;
		UINT32 agc;

		sp5kPreviewExpAgcGet(&exp, &agc);
		printf("exp/agc: %d/%d => %d/%d\n", exp, (SINT32)agc, (SINT32)v[1], (SINT32)v[2]);
		if (v[1] && v[2]) sp5kPreviewExpAgcSet(v[1], v[2]);
		else if (v[1]) sp5kPreviewExpAgcSet(v[1], SP5K_AE_EXP_AGC_DONT_CHANGE);
		else if (v[2]) sp5kPreviewExpAgcSet(SP5K_AE_EXP_AGC_DONT_CHANGE, v[2]);
	}
	else if(!strcmp(*argv, "si2c")) {
		if (v[1]) {
			UINT32 val;
			appI2CRead(v[2], &val);
			printf("Read: %x %x\n", v[2], val);
		}
		else {
			printf("Write: %x\n", v[2]);
			appI2CWrite(v[2], v[3], v[4]);
		}
	}
	else if (!strcmp(*argv, "senw")) {
		senRegWR(v[1], v[2]);
	}
#if 0
	else if (!strcmp(*argv, "senr")) {
		extern UINT16 SONYsensorRead(UINT16 addr);
		printf("READ [0x%x] = [0x%x]\n",v[1],SONYsensorRead(v[1]));
	}
#endif
	else if (!strcmp(*argv, "dual")) {
		sp5kSensorCfgSet(SP5K_SENSOR_CFG_DUAL_RAW_SEL,v[1],0);

	}
    else if (!strcmp(*argv, "fullrange")) {
		if(v[1]==1){
    		sp5kIqModeSet(SP5K_MODE_STILL_PREVIEW);
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_Y_BRIGHT, 16 ); /* ecyc */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_U_BRIGHT, 0 ); /* ecuc */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_V_BRIGHT, 0 ); /* ecvc */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_Y_CONTRAST, 149 ); /* ecyratio */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_U_CONTRAST, 147 ); /* ecuratio */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_V_CONTRAST, 147 ); /* ecvratio */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_Y_OFFSET, 0 ); /* ecyoff */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_U_OFFSET, 0 ); /* ecuoff */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_V_OFFSET, 0 ); /* ecvoff */
    		sp5kIqModeSetDone(SP5K_MODE_STILL_PREVIEW);
		}else{
    		sp5kIqModeSet(SP5K_MODE_STILL_PREVIEW);
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_Y_BRIGHT, 0 ); /* ecyc */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_U_BRIGHT, 0 ); /* ecuc */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_V_BRIGHT, 0 ); /* ecvc */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_Y_CONTRAST, 128 ); /* ecyratio */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_U_CONTRAST, 128 ); /* ecuratio */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_V_CONTRAST, 128 ); /* ecvratio */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_Y_OFFSET, 0 ); /* ecyoff */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_U_OFFSET, 0 ); /* ecuoff */
    		sp5kIqCfgSet( SP5K_IQ_CFG_DIGI_EFF_V_OFFSET, 0 ); /* ecvoff */
    		sp5kIqModeSetDone(SP5K_MODE_STILL_PREVIEW);
		}
	}

#if 0 /*defined(HW_KX1371)*/
	else if (!strcmp(*argv, "vcminit")) {

#define FRONT_SSC_RESTART_ON		2
#define FRONT_SSC_RESTART_OFF		0
#define FRONT_SSC_READ_NO_SUBADDR	0
#define FRONT_SSC_READ_HAS_SUBADDR	1

#define   VCM_SPI_I2C_MODE		0x1C /*I2C slave addr for write*/
#define   VCM_FORMAT2			FRONT_SSC_READ_HAS_SUBADDR | FRONT_SSC_RESTART_OFF
#define   VCM_FORMAT3			0 /*don't change*/
#define   VCM_SPEED 			3 /*I2C clock frequency selection*/
#define   VCM_NBIT_REGDATA		8 /*ssc register data numbers */
#define   VCM_NBIT_REGADDR		8

		static const sensorCmdport_t VCM_cmdport_xtg_cfg = {
			.mode				= 	VCM_SPI_I2C_MODE,
			.format2			= 	VCM_FORMAT2,
			.format3			= 	VCM_FORMAT3,
			.speed				=	VCM_SPEED,
			.nbit_regdata		=	VCM_NBIT_REGDATA,
			.nbit_regaddr		= 	VCM_NBIT_REGADDR,
		};

		ulCmdPortID = sensorSerialCreate(&VCM_cmdport_xtg_cfg);

		printf("ulCmdPortID [0x%x] \n",ulCmdPortID);
	}
	else if (!strcmp(*argv, "vcmw")) {
		static UINT8 regdata[2];
		UINT32 rtn;

		regdata[0]=v[1]&0xFF;
		regdata[1]=v[2]&0xFF;

		rtn=sensorSerialBulkWrite(ulCmdPortID,1,regdata,0);
		tmrUsWait(100000);/*100ms*/

		printf("%d WRITE [0x%x] = [0x%x]\n",rtn,v[1],v[2]);
	}
	else if (!strcmp(*argv, "vcmr")) {
		static UINT8 regdata[2];
		UINT32 Addr = v[1];

		UINT32 rtn;

		regdata[0] = ( UINT8 )( Addr & 0x000000FF);

		rtn=sensorSerialBlockRead( ulCmdPortID, 1, regdata, 0); /*Data = regread[1]*/

		printf("%d read [0x%x] = [0x%x]\n",rtn,regdata[0],regdata[1]);
	}
	else if (!strcmp(*argv, "sdpwr")) {

		printf("enable SD PWR\n");
		appI2CWrite(0x06, 0xF0, 0xF0);
		appI2CWrite(0x02, 0x80, 0x80);
	}
	else if (!strcmp(*argv, "tvpwr")) {

		printf("enable TV PWR\n");
		appI2CWrite(0x04, 0xF8, 0x04);
		appI2CWrite(0x04, 0xF8, 0x0C);
		SET_TV_PWR_EN(1);
	}
	#if WITH_MT6622
	else if ( (!strcmp(*argv, "bt")) || (!strcmp(*argv, "bt_ana")) ) {
		#if 0 /*deprecate@20120528*/

		UINT32 dummyCnt;

		halRtcRegWrite(0x5, 0x7); /* cmd> rtcreg 0x05 0x7 */

		/* LMI-3 (connect to ANA_SWITCH_1): output, HIGH */
		/*sp5kGpioFuncSet(SP5K_GPIO_GRP_LMI, (1<<3), (1<<3) );*/
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<3), (1<<3) );
		sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, (1<<3), (1<<3) );
		profLogAdd(0, "V33 LMI-3 output HIGH (bt analog switch)");
		printf("V33 LMI-3 output HIGH (bt analog switch)\n");

		#if 0 /*Philip@20120528: no need following codes */
		/* LMI-4 (connect to RESET): output, LOW (follow with inverter to HIGH) */
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<4), (0<<4) );
		sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, (1<<4), (1<<4) );
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<4), (1<<4) );

		for(dummyCnt=0; dummyCnt<0x400; dummyCnt++) /*dummy delay*/;

		sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<4), (0<<4) );
		profLogAdd(0, "V33 LMI-4 output LOW");
		printf("V33 LMI-4 output LOW (follow with inverter to HIGH)\n");
		#endif

		#else /*Philip@20120528*/
		printf("bt already moved into bt0. You can skip \"bt\" in next time!!\n");
		#endif
	}
	else if ( (!strcmp(*argv, "bt0")) || (!strcmp(*argv, "bt_init")) ) {
		void main_init(UINT32 parm);
		void mtk_uart_init(UINT32 parm);
		void mtk_test_handler(UINT32 parm);

		char *e;
		UINT32 speed = 2096000;
		UINT32 btMainPrior = 16;

		#if 1 /*PhilipAdd@20120528*/
		halRtcRegWrite(0x5, 0x7); /* cmd> rtcreg 0x05 0x7 */

		/* LMI-3 (connect to ANA_SWITCH_1): output, HIGH */
		/*sp5kGpioFuncSet(SP5K_GPIO_GRP_LMI, (1<<3), (1<<3) );*/
		sp5kGpioWrite(SP5K_GPIO_GRP_LMI, (1<<3), (1<<3) );
		sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI, (1<<3), (1<<3) );
		profLogAdd(0, "V33 LMI-3 output HIGH (bt)");
		printf("V33 LMI-3 output HIGH (bt)\n");
		#endif

		printf("[MTKBT] Create BT thread\n");
		if (argc == 3) {
			btMainPrior = v[2];
			speed = v[1];
		}
		else if (argc == 2) {
			speed = v[1];
		}
		else if (argc == 1) {
			#if 0 /* 1 Mbps */ /*OK*/
			speed = 1024000;
			#endif
			#if 0 /* 1.5 Mbps */ /*OK*/
			speed = 1536000; /*1572864*/
			#endif
			#if 0 /* 2 Mbps */ /*OK*/
			speed = 2096000;
			#endif
			#if 1 /* 2.5 Mbps */ /*OK*/
			speed = 2621440;
			#endif
			#if 0 /* 3.2 Mbps *//*OK, but easier Rx FIFO Full */
			speed = 3200000;
			#endif
		}

		printf("btMainPrior=%d\n", btMainPrior);
		printf("speed=%d\n", speed);
		/*btMainPrior: NG: 1, 4, 6, 10. OK: 16, 17, 15*/
		sp5kOsThreadCreate("MTKBT", main_init, speed, btMainPrior/*16*/, btMainPrior/*16*/, 0, TX_AUTO_START);
		printf("[MTKBT] Create BT UART thread\n");
		sp5kOsThreadCreate("MTKBTUART", mtk_uart_init, 0, 5/*15*/, 5/*15*/, 0, TX_AUTO_START);

		#if 1 /*PhilipAdd@20120528*/
		printf("[MTKBT] Create MTKTEST thread\n");
		sp5kOsThreadCreate("MTKTEST", mtk_test_handler, 0, 17, 17, 0, TX_AUTO_START);
		#endif

		printf("[MTKBT] Create BT EINT thread (for polling)\n");
		extern void mtk_eint_init(UINT32 parm);
		mtk_eint_init(10);
	}
	else if ( (!strcmp(*argv, "bt1")) || (!strcmp(*argv, "bt_test_init")) ) {
		#if 0 /*deprecate@20120528*/
		void mtk_test_handler(UINT32 parm);
		printf("[MTKBT] Create MTKTEST thread\n");
		sp5kOsThreadCreate("MTKTEST", mtk_test_handler, 0, 17, 17, 0, TX_AUTO_START);

		#else /*Philip@20120528*/
		printf("bt1 already moved into bt0. You can skip \"bt1\" in next time!!\n");
		#endif
	}
	else if ( (!strcmp(*argv, "btr1")) || (!strcmp(*argv, "bt_poweron")) ) {
		void mtk_test_poweron(void);
		mtk_test_poweron();
	}
	else if ( (!strcmp(*argv, "btr2")) || (!strcmp(*argv, "bt_poweroff")) ) {
		void mtk_test_poweroff(void);
		mtk_test_poweroff();
	}
	else if ( (!strcmp(*argv, "btr3")) || (!strcmp(*argv, "bt_send_string")) ) {
		void mtk_test_sendbuf(UINT8 *buf, UINT32 size);
		if (argc == 2 && argv[1])
		{
			UINT32 len = strlen(argv[1]);
			UINT8 *buf = sp5kMalloc(len + 1);
			memcpy(buf, argv[1], len + 1);
			buf[len] = '\0';
			mtk_test_sendbuf(buf, len + 1);
		}
	}
	else if ( (!strcmp(*argv, "btr4")) || (!strcmp(*argv, "bt_send_file")) ) {
		#if 0
		BOOL mtk_test_sendfile(char *path);
		if (argc == 2 && argv[1])
		{
			mtk_test_sendfile(argv[1]);
		}
		#else
		UINT32 btSendFd, btSendSz, btTxSz;
		UINT8 *pBtSendBuf, *pTx;
		UINT32 i=0, loopCnt=0, remainSz=0;

		if (argc == 3) {
			loopCnt = v[2];
		}
		else {
			loopCnt = 1;
		}

		btSendFd = sp5kFsFileOpen(argv[1], SP5K_FS_OPEN_RDONLY);
		if (btSendFd == 0) {
			printf("btr4 <filename>: file \"%s\"not exist", argv[1]);
			return FALSE;
		}
		else {
			btSendSz = sp5kFsFileSizeGet(btSendFd);
			pBtSendBuf = sp5kMalloc(btSendSz+1);
			ASSERT_MSG(pBtSendBuf, "malloc %d return NULL\n", btSendSz);

			sp5kFsFileRead(btSendFd, pBtSendBuf, btSendSz);
			pBtSendBuf[btSendSz] = '\0';

			sp5kFsFileClose(btSendFd);

			pTx = pBtSendBuf;
			remainSz = btSendSz;
			if ( remainSz > (24*1024) ) {
				btTxSz = (24*1024);
				remainSz -= (24*1024);
			}
			else {
				btTxSz = remainSz;
				remainSz = 0;
			}

			for (i=0; i<loopCnt; i++) {
				profLogPrintf(1, "[%d] sendbuf(0x%08x, %d)", i, pTx, btTxSz);
				mtk_test_sendbuf(pTx, btTxSz);
				profLogPrintf(1, "[%d] sendbuf--", i);

				if ( remainSz > (24*1024) ) {
					pTx += (24*1024);
					btTxSz = (24*1024);
					remainSz -= (24*1024);
				}
				else if (remainSz==0) {
					pTx = pBtSendBuf;
					remainSz = btSendSz;
					if ( remainSz > (24*1024) ) {
						btTxSz = (24*1024);
						remainSz -= (24*1024);
					}
					else {
						btTxSz = remainSz;
						remainSz = 0;
					}
				}
				else {
					pTx += (24*1024);
					btTxSz = remainSz;
					remainSz = 0;
				}
			}

			sp5kFree(pBtSendBuf);
			pBtSendBuf = 0;
			profLogDump(1);
		}
	#endif
	}
	else if ( (!strcmp(*argv, "btr5")) || (!strcmp(*argv, "bt_search")) ) {
		void mtk_test_search(void);
		mtk_test_search();
	}
	else if (!strcmp(*argv, "bt_tx_test")) {
		void mtk_btrf_tx_test(UINT8 type);
		if (argc == 2 && (v[1]<3) )
		{
			mtk_btrf_tx_test(v[1]);
		}
		else
		{
			mtk_btrf_tx_test(0);
		}
	}
	else if (!strcmp(*argv, "bt_tx_test_end")) {
		void mtk_btrf_tx_test_end(void);
		mtk_btrf_tx_test_end();
	}
	else if (!strcmp(*argv, "bt_rx_test")) {
		void mtk_btrf_rx_test(UINT8 type);
		if (argc == 2 && (v[1]<=1) )
		{
			mtk_btrf_rx_test(v[1]);
		}
		else
		{
			mtk_btrf_rx_test(0);
		}
	}
	else if (!strcmp(*argv, "bt_rx_test_end")) {
		void mtk_btrf_rx_test_end(void);
		mtk_btrf_rx_test_end();
	}
	else if (!strcmp(*argv, "bt_gps")) {
		#if 0
		void mtk_test_set_gps(void);
		printf("bt_gps\n");
		mtk_test_set_gps();

		#else
		printf("bt_gps already moved into gps0. You can skip \"bt_gps\" in next time!!\n");
		#endif
	}
	else if (!strcmp(*argv, "bt_gpio")) {
		void mtk_test_set_gpio(UINT32 uInput, UINT32 uValue);
		mtk_test_set_gpio(v[1], v[2]);
	}
	else if (!strcmp(*argv, "bt_reset")) {
		void mtk_test_reset_led(UINT32 uInput);
		mtk_test_reset_led(v[1]);
	}
	else if (!strcmp(*argv, "bt_led")) {
		void mtk_test_set_gpio_always_high(UINT32 uInput, UINT32 uValue);
		mtk_test_set_gpio_always_high(v[1], v[2]);
	}
	else if (!strcmp(*argv, "bt_deep")) {
		void V33_PowerOffBT(void);
		V33_PowerOffBT();
	}
	else if ( (!strcmp(*argv, "bt_dump")) ) {
		profLogDump(0);
	}
	else if ( (!strcmp(*argv, "bt_debug")) ) {
		void mtk_test_ssp_debug(void);
		mtk_test_ssp_debug();
	}
	else if (!strcmp(*argv, "bt_log")) {
		void mtk_test_log(UINT8 level);
		char *e;
		UINT32 log;

		if (argc == 2 && argv[1])
		{
			log = strtoul(argv[1], &e, 0);
			printf("bt_log(%d)\n", log);
			mtk_test_log(log);
		}
	}
	else if ( (!strcmp(*argv, "bt_sniff")) ) {
		void mtk_test_sniff(void);
		mtk_test_sniff();
	}
	else if ( (!strcmp(*argv, "bt_unsniff")) ) {
		void mtk_test_unsniff(void);
		mtk_test_unsniff();
	}
	else if (!strcmp(*argv, "v33suspend")) {
		extern UINT32 sp5kSuspendResume(UINT32 resumeSrc);

		printf("Suspending CPU!\n");
		/*pglobalReg->gpioint, pglobalReg->gpiointen*/
		printf("gpioint (sts)=0x%02x gpiointen=0x%02x\n",
			READ32(0xb0000000),  READ32(0xb0000004));
		printf("[0x0008]=0x%08x, [0x000c]=0x%08x\n",
			READ32(0xb0000008),  READ32(0xb000000c));

		printf("Set POWERONSRC1 rising to wakeup CPU.\n");
		tmrUsHardWait(1000);
		halPllSuspend((1 << 25) /*HAL_RESUME_SRC_PWRON1_RISE*/);

		printf("Resuming CPU!\n");

		printf("gpioint (sts)=0x%02x gpiointen=0x%02x\n",
			READ32(0xb0000000),  READ32(0xb0000004));
		printf("[0x0008]=0x%08x, [0x000c]=0x%08x\n",
			READ32(0xb0000008),  READ32(0xb000000c));
	}
	#endif /* WITH_MT6622 */
	#if WITH_MT3339
	else if ( (!strcmp(*argv, "gps0")) || (!strcmp(*argv, "gps_poweron")) ) {
		extern UINT32 gpsOpPowerOn(void);
		extern UINT32 gpsPktBufInit(void);
		extern void mtk_test_set_gps(void);

		printf("gps0\n");

		gpsOpPowerOn();
		gpsPktBufInit();
		mtk_test_set_gps();
	}
	else if ( (!strcmp(*argv, "gps1")) || (!strcmp(*argv, "gps_rx")) ) {
		extern UINT32 gpsPollingTask(UINT32 param);
		UINT32 sleep_interval;

		printf("gps1\n");

		if (argc==2) {
			sleep_interval = v[1];
		}
		else {
			sleep_interval = 10;
		}
		mSpiTxIndTask = sp5kOsThreadCreate("SPI_TXIND", gpsPollingTask, sleep_interval, 14, 14, 1, TX_AUTO_START);
	}
	else if ( (!strcmp(*argv, "gps2")) || (!strcmp(*argv, "gps_poweroff")) ) {
		extern UINT32 gpsPktBufDeInit(void);
		extern UINT32 gpsOpPowerOff(void);

		printf("gps2\n");

		if (mSpiTxIndTask) {
			sp5kOsThreadDelete(mSpiTxIndTask);
			mSpiTxIndTask = 0;
		}

		gpsPktBufDeInit();
		gpsOpPowerOff();
	}
	#endif /* WITH_MT3339 */
#endif
#if 0
	else if (!strcmp(*argv, "libexample")) {
		printf("%d = %d+%d\n", libExample1(12, 34), 12, 34);
	}
#endif
	else
	{
		appMonitorCmd(argc, argv);
	}
}

#endif /*CMD_TASK_INIT*/

#define CHART_W  384
#define CHART_H  256

typedef struct _lyr {
	SINT32 x0, y0, x1, y1;
	SINT32 h0, v0, h1, v1;
}lyr_t;

#if ((SP5K_ICAT_EIS) && RTMP)

void EIS_STREAMING()
{

	{
		printf("MEDIA_REC_STREAM_2ND\n");
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_BRC_TYPE, SP5K_MEDIA_VBR);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH, 1920);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, 1080);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_INPUT_FRAME_RATE,15);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_FRAME_RATE, 15);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_MAX_BITRATE, 1200000);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC, MEDIA_VIDEO_H264);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_VLC_BUF_CNT, 4);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_VLC_BUF_SIZE, 1400000);/*SHEN: bitrate -> byterate*/
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_GOP_NO, 15);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IPPP);

		{
			UINT32 h264Qp;
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_H264_QP, &h264Qp);
			h264Qp = h264Qp & 0xFF000000;
			h264Qp = h264Qp | (20 << 16) | (38 << 8) | (31 << 0);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_QP, h264Qp);
		}
		{
			UINT32 h264QpI;
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_H264_QP_I, &h264QpI);
			h264QpI = h264QpI & 0xFF000000;
			h264QpI = h264QpI | (33 << 8) | ( 43<< 0);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_QP_I, h264QpI);
		}
	}

	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CODEC, AUDIO_CODEC );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, 44100/*VIDEO_AUDIO_SAMPLE_RATE*/ );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, 16/*VIDEO_AUDIO_SAMPLE_BIT*/ );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 1/*VIDEO_AUDIO_CHANNEL*/ );

	{
		appAeFrameRateSet(30);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, 0x36/*SENSOR_MODE_1080P_30FPS*/);
	}


/****************************	REC 	*************************************************/
	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

	appVideo_SetRscEis(1, 1, SP5K_EIS_ALGO_COMP_TYPE_IME);

	sp5kSystemCfgSet(SP5K_CDSP_SYS_CFG,CDSP_SYS_CFG_DIGI_EFF_Y_REMAP,1);

    sp5kModeSet(SP5K_MODE_VIDEO_RECORD);
    sp5kModeWait(SP5K_MODE_VIDEO_RECORD);


}
#endif
