/**************************************************************************
 *
 *       Copyright (c) 2004-2018 by iCatch Technology, Inc.
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
 *                                                                        *
 *  Author: Shenkai Lin                                                   *
 *                                                                        *
 *  ver. 0.00.01, 20141218
 | sensor size				   | sensor info						| mode    |	Data rate(Mbps)|   Frame Rate |Pclknx(MHz)|
 | Preview mode
 | 1928 x 1080 @ 30fps HDR		   | 					 				| 0x30    |  (  445.5 Mbps)	   |	30.00	   | 148.5		|
 | 1928 x 1080 @ 60fps HDR		   | 	 								| 0x31    |  (  445.5 Mbps)	   |	60.00	   | 148.5		|
 | 1928 x 1080 @ 30fps LINEAR	   | 					 				| 0x32    |  (  445.5 Mbps)	   |	60.00	   | 148.5		|
 | 1928 x 1080 @ 60fps LINEAR	   | 					 				| 0x33    |  (  445.5 Mbps)	   |	60.00	   | 148.5		|

 | Snap mode
 | 2304 x 1296 @ 30fps		   | 8M size preview					 | 0x40    |  (  640 Mbps)	   |	29.98	   | 80 		|
 *
 *  ver. 0.00.02, 20150812,	Shane Chuang
 *	1. Add the sensor related information settings. ex: OB and sensitivity status of all sensor mode.
 *  ver. 0.00.03, 20150907,	Shane Chuang
 *	1. add sensro AWB function,need set digital gain before DLO.
 *  ver. 0.00.04, 20151021,	Shane Chuang
 *	1.modify pre-gain offset
 *  ver. 0.00.05, 20151105,	Shane Chuang
 *	1.ALTM HW change to FW
 *  ver. 0.00.06, 20160112,	Shane Chuang
 *	1.add snap linear mode
 *	2.fix preview linear mode abnormal
 *  ver. 0.00.07, 20160118,	Shane Chuang
 *	1.add sensor flip 
 *  ver. 0.00.08, 20160118,	Shane Chuang
 *	1.add sensor flip & mirror
 *  ver. 0.00.09, 20160307,	Shane Chuang
 *	1.New create sensor mode 0x34 for 2x binning linear mode
 *  ver. 0.00.10, 20160608,	Shane Chuang
 *	1.add function "sensor_cmdport_frame_write"
 *  ver. 0.00.11, 20160902,	Shane Chuang
 *	1.Modify the PLL of HDR 30fps and linear 30fps sensor mode to use 40.5MHz to improve
 *                 the power consumption.
 *	2.fix HDR 1080p60F frame rate not enought 60fps
 *  ver. 0.00.12, 20161121,	Shane Chuang
 *	1.fix mode switch crash
 *  ver. 0.00.13, 20161226,	Shane Chuang
 *	1.fix capture exp not correct
 *  ver. 0.00.14, 20170307,   Vincent Tsao
 *    1. Add a new sensor mode which is cropped from 960x540@120fps (912x512@120fps to keep the frame rate and max. FOV).
 *  ver. 0.00.15, 20170315,   Shane Chuang
 *    1. Add a new sensor mode which is cropped from 1280x720@90fps
 *	2. Fix sensor mode 0x35 exp table abnormal
 *  ver. 0.00.16, 20170421,   Shane Chuang
 *    1. Add the mirror/flip setting and status of sensor to sensor driver.
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_ar0230-0.00.16"
#define SENSORRES_LOG ""
#define BUILD_TIME        __DATE__  "-"  __TIME__

#include <stdio.h>
#include "common.h"
#include "customization/dev_init.h"
#ifdef SP5K_SENSOR_PIN_CB
#include GPIO_INC
#endif
#include "../flow/sysregs.h"
#include "../flow/hal_front.h"
#include "../flow/hal_gpio.h"
#include "../flow/sensor_model.h"
#include "../flow/sensor_model_snap.h"
#include "../flow/sensor_def.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_sensor_api.h"
#include "api/sp5k_dbg_api.h"
#include "middleware/proflog.h"
#include "api/sp5k_media_api.h"
#include "../../../../../../include/thirdparty/iq/app_awbalg_api.h"


#define CMD_LEN(a)			(sizeof(a)/sizeof(a[0]))

#define SENSOR_INFO_OB 		         SENSOR_CUST_INFO_00
#define SENSOR_INFO_SENSITIVITY_STS  SENSOR_CUST_INFO_01
#define SENSOR_INFO_MIRROR_FLIP_STS  SENSOR_CUST_INFO_02

#define AR0230_BLC_VAL   168 /*12bit-0x301E*/

#define AR0230_MIRROR_FLIP_INFO   0  /*0: normal,  1: mirror,  2:flip,  3: mirror+flip*/


#if SP5K_DIQ_FRMWRK
#define USE_AGC_DELAY_SYNC			   (1)
#else
#define USE_AGC_DELAY_SYNC			   (0)
#endif

#define MASK_EXP_CMD                    0
#define EXT_HD_LINE					   (42)
#define SENSOR_PROF_LOG_EN             (1)

#if SENSOR_PROF_LOG_EN
    #define PROF_ID                                 (3)
    #define SENSOR_PROF_LOG_ADD(str)                profLogAdd(PROF_ID, str)
    #define SENSOR_PROF_LOG_PRINT(fmt, arg...)      profLogPrintf(PROF_ID, fmt, ##arg)

#else
    #define SENSOR_PROF_LOG_ADD(str)
    #define SENSOR_PROF_LOG_PRINT(fmt,arg...)
#endif

	/* HDR 30fps*/ /* 0x30 */
	#define PREVIEW_1080P_30FPS_HSIZE				1920
	#define PREVIEW_1080P_30FPS_VSIZE				1080			/*760*/
	#define PREVIEW_1080P_30FPS_HOFFSET 			0
	#define PREVIEW_1080P_30FPS_VOFFSET 			0			/*2*/
	#define PREVIEW_1080P_30FPS_BAYERHS_SIZE		(0) 		/*Dont scale*/
	#define PREVIEW_1080P_30FPS_LINETOTAL			559
	#define PREVIEW_1080P_30FPS_FRAMETOTAL			1207
	#define PREVIEW_1080P_30FPS_PCLKNX				20250
	#define PREVIEW_1080P_30FPS_PCLK1X				81000
	#define PREVIEW_1080P_30FPS_CDSPCLK 			0			 /*use 0 to make basefw calculate the CDSP CLK.*/
	#define PREVIEW_1080P_30FPS_SENSOR_EXPLINE		1085
	#define PREVIEW_1080P_30FPS_FRAMERATE_KFPS		30000
	#define PREVIEW_1080P_30FPS_EXT_LINES			36

	/* HDR 60fps*/ /* 0x31 */
	#define PREVIEW_1080P_60FPS_HSIZE				1920
	#define PREVIEW_1080P_60FPS_VSIZE				1080			/*760*/
	#define PREVIEW_1080P_60FPS_HOFFSET 			0
	#define PREVIEW_1080P_60FPS_VOFFSET 			0			/*2*/
	#define PREVIEW_1080P_60FPS_BAYERHS_SIZE		(0) 		/*Dont scale*/
	#define PREVIEW_1080P_60FPS_LINETOTAL			550
	#define PREVIEW_1080P_60FPS_FRAMETOTAL			1125
	#define PREVIEW_1080P_60FPS_PCLKNX				37125
	#define PREVIEW_1080P_60FPS_PCLK1X				148500
	#define PREVIEW_1080P_60FPS_CDSPCLK 			150000 /*use 0 to make basefw calculate the CDSP CLK.*/
	#define PREVIEW_1080P_60FPS_SENSOR_EXPLINE		1065
	#define PREVIEW_1080P_60FPS_FRAMERATE_KFPS		60000
	#define PREVIEW_1080P_60FPS_EXT_LINES			39

	/* Linear 30fps*/ /* 0x32 */
 	#define PREVIEW_1080P30_LINEAR_HSIZE            1920
	#define PREVIEW_1080P30_LINEAR_VSIZE            1080			/*760*/
	#define PREVIEW_1080P30_LINEAR_HOFFSET			0
	#define PREVIEW_1080P30_LINEAR_VOFFSET			0   		/*2*/
	#define PREVIEW_1080P30_LINEAR_BAYERHS_SIZE		(0)			/*Dont scale*/
	#define PREVIEW_1080P30_LINEAR_LINETOTAL		559
	#define PREVIEW_1080P30_LINEAR_FRAMETOTAL 		1207
	#define PREVIEW_1080P30_LINEAR_PCLKNX 			20250
	#define PREVIEW_1080P30_LINEAR_PCLK1X 			81000
	#define PREVIEW_1080P30_LINEAR_CDSPCLK			0		 /*use 0 to make basefw calculate the CDSP CLK.*/
	#define PREVIEW_1080P30_LINEAR_SENSOR_EXPLINE	1085
	#define PREVIEW_1080P30_LINEAR_FRAMERATE_KFPS	30000
	#define PREVIEW_1080P30_LINEAR_EXT_LINES		36

	/* Linear 60fps*/ /* 0x33 */
 	#define PREVIEW_1080P60_LINEAR_HSIZE            1920
	#define PREVIEW_1080P60_LINEAR_VSIZE            1080			/*760*/
	#define PREVIEW_1080P60_LINEAR_HOFFSET			0
	#define PREVIEW_1080P60_LINEAR_VOFFSET			0   		/*2*/
	#define PREVIEW_1080P60_LINEAR_BAYERHS_SIZE		(0)			/*Dont scale*/
	#define PREVIEW_1080P60_LINEAR_LINETOTAL		550
	#define PREVIEW_1080P60_LINEAR_FRAMETOTAL 		1125
	#define PREVIEW_1080P60_LINEAR_PCLKNX 			37125
	#define PREVIEW_1080P60_LINEAR_PCLK1X 			148500
	#define PREVIEW_1080P60_LINEAR_CDSPCLK			150000 /*use 0 to make basefw calculate the CDSP CLK.*/
	#define PREVIEW_1080P60_LINEAR_SENSOR_EXPLINE	1065
	#define PREVIEW_1080P60_LINEAR_FRAMERATE_KFPS	60000
	#define PREVIEW_1080P60_LINEAR_EXT_LINES		39

	/* 2X Binning Linear 60fps*/ /* 0x34 */
	#define PREVIEW_2XBIN_60FPS_LINEAR_HSIZE			960
	#define PREVIEW_2XBIN_60FPS_LINEAR_VSIZE			540 
	#define PREVIEW_2XBIN_60FPS_LINEAR_HOFFSET			0
	#define PREVIEW_2XBIN_60FPS_LINEAR_VOFFSET			0			/*2*/
	#define PREVIEW_2XBIN_60FPS_LINEAR_BAYERHS_SIZE 	(0) 		/*Dont scale*/
	#define PREVIEW_2XBIN_60FPS_LINEAR_LINETOTAL		550
	#define PREVIEW_2XBIN_60FPS_LINEAR_FRAMETOTAL		1125
	#define PREVIEW_2XBIN_60FPS_LINEAR_PCLKNX			37125
	#define PREVIEW_2XBIN_60FPS_LINEAR_PCLK1X			148500
	#define PREVIEW_2XBIN_60FPS_LINEAR_CDSPCLK			150000 /*use 0 to make basefw calculate the CDSP CLK.*/
	#define PREVIEW_2XBIN_60FPS_LINEAR_SENSOR_EXPLINE	1125
	#define PREVIEW_2XBIN_60FPS_LINEAR_FRAMERATE_KFPS	60000
	#define PREVIEW_2XBIN_60FPS_LINEAR_EXT_LINES		39

	/* Linear 960x540 120fps */ /* 0x35 */
	#define PREVIEW_QUARTER_LINEAR_HSIZE			912
	#define PREVIEW_QUARTER_LINEAR_VSIZE			512 
	#define PREVIEW_QUARTER_LINEAR_HOFFSET			24/*(960-912)/2=24*/
	#define PREVIEW_QUARTER_LINEAR_VOFFSET			14/*(540-512)/2=14*/			/*2*/
	#define PREVIEW_QUARTER_LINEAR_BAYERHS_SIZE 	(0) 		/*Dont scale*/
	#define PREVIEW_QUARTER_LINEAR_LINETOTAL		550
	#define PREVIEW_QUARTER_LINEAR_FRAMETOTAL		562
	#define PREVIEW_QUARTER_LINEAR_PCLKNX			37125
	#define PREVIEW_QUARTER_LINEAR_PCLK1X			148500
	#define PREVIEW_QUARTER_LINEAR_CDSPCLK			0 /*use 0 to make basefw calculate the CDSP CLK.*/
	#define PREVIEW_QUARTER_LINEAR_SENSOR_EXPLINE	562
	#define PREVIEW_QUARTER_LINEAR_FRAMERATE_KFPS	120110
	#define PREVIEW_QUARTER_LINEAR_EXT_LINES		39	

	/* Linear 1280x720 120fps */ /* 0x36 */
	#define PREVIEW_720P90_LINEAR_HSIZE				1280
	#define PREVIEW_720P90_LINEAR_VSIZE				720			
	#define PREVIEW_720P90_LINEAR_HOFFSET			0
	#define PREVIEW_720P90_LINEAR_VOFFSET			0
	#define PREVIEW_720P90_LINEAR_BAYERHS_SIZE 		(0) 		/*Dont scale*/
	#define PREVIEW_720P90_LINEAR_LINETOTAL			550
	#define PREVIEW_720P90_LINEAR_FRAMETOTAL		761
	#define PREVIEW_720P90_LINEAR_PCLKNX			37125
	#define PREVIEW_720P90_LINEAR_PCLK1X			148500
	#define PREVIEW_720P90_LINEAR_CDSPCLK			0 /*use 0 to make basefw calculate the CDSP CLK.*/
	#define PREVIEW_720P90_LINEAR_SENSOR_EXPLINE	761
	#define PREVIEW_720P90_LINEAR_FRAMERATE_KFPS	88700
	#define PREVIEW_720P90_LINEAR_EXT_LINES			39	
	
	/*30fps*//*0x40*/
    /* For Capture 2688x1512 */
	#define CAPTURE_HSIZE							1920
	#define CAPTURE_VSIZE							1080
	#define CAPTURE_HOFFSET 						0
	#define CAPTURE_VOFFSET							0
	#define CAPTURE_BAYERHS_SIZE					(0)			/*Dont scale*/
	#define CAPTURE_LINETOTAL						559
	#define CAPTURE_FRAMETOTAL						1207
	#define CAPTURE_PCLKNX 							20250
	#define CAPTURE_PCLK1X 							81000
	#define CAPTURE_CDSPCLK							0
	#define CAPTURE_FRAMERATE_KFPS					30000
	#define CAPTURE_EXT_LINES						36

/*****************************************************************************************************/

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL						 TYPE_CMOS				/* TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						                                                        		TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			         		 TG_CMOS_SP5K_SLAVE		/* TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
														     		TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/

#define	PREVIEW_1080P_30FPS_CFAPATTERN	         	BAYER_GRBG     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	PREVIEW_QUARTER_LINEAR_CFAPATTERN	        BAYER_GRBG     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/

#define	SNAP_CFAPATTERN					 			BAYER_GRBG     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM						  	0     /* snap dummy Vd need numbers*/
#define	SNAP_READOUTVD_NUM		        		  	1     /* snap readout Vd need numbers*/
#define	SNAP_CAPTUREVD_NUM		        		  	1     /* for typical CCD==n_dummy + n_readout + 1*/
#define	SNAP_EARLY_NUM		                      	0	    /* for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM	                      	0		/* It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM							  	0	 	/* It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN                    	0		/*0*//* a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	                      	0		/* sony FALSE;panasonic/ITG TRUE*/
#if  USE_AGC_DELAY_SYNC
#define EXP_FRAME_NUM							  	1
#else
#define	EXP_FRAME_NUM       	                  	1/*2*/
#endif
#define	PREVIEW_1080P_30FPS_HDNUM_CMDPORT		    (600)
#define	CAPTURE_HDNUM_CMDPORT	                	(600)
#define PREVIEW_QUARTER_HDNUM_CMDPORT               (360)
/**************create the command port condition  define ****************************/
/* UINT8 port_id is dynamic allocated by sensor_serial.c::sensorSerialCreate()
 *  stored in sensor{Tg/Afe}Cfg.cmdportid_{xtg,exp,afe,agc,dcoffset}
 *
 * structure of physical definitions
 * mode:[0]H spi/L ssc
 *   ssc [7:1]slave addr
 *   spi [7]sensel;[6]sengpio;[5:4]spi type;[3:1]cdsini;
 *     ([7:6]==LL:sensel1,HL:sensel2,LH:sengpio,HH:no sen pin)
 * ssc
 *   format2:[1:0]readCtr(no [4] go)
 *   format3:(stopdelay and clkien??)
 * spi
 *   format2:sensel gpio [7:4]sensel H/L(idle,open,active,close);[3:0]gpio_grp
 *   format3:[5:0]sensel gpio gpio_pin
 */
    #define	TG_SPI_I2C_MODE     0x20                /* I2C slave addr for write */
    #define	TG_FORMAT2          FRONT_SSC_READ_HAS_SUBADDR |SENSOR_SERIAL_FORMAT2_SIF1_PIN_TG0
    #define	TG_FORMAT3          0
    #define	TG_SPEED            22
    #define	TG_NBIT_REGADDR     16                  /* ssc/spi register address numbers */
    #define	TG_NBIT_REGDATA     16                   /* ssc/spi register data numbers */
/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_frame_write(UINT32 smode,UINT32 kfps_max,UINT32 kfps_min,UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type,UINT32 vcount);
static void sensorDoFuncPreviewBegin();
static void sensorDoFuncPreviewShortExp();
static void sensorDoFuncPreviewExpEnd();
static void sensorDoFuncSnapExpBegin();
static void sensorDoFuncSnapShortExp();
static void sensorDoFuncSnapExpEnd();
static void sensorDoFuncSnapReadoutField0();
static void sensorDoFuncSnapFieldEnd();
static void sensorDoFuncSnapReadoutEnd();
static void sensorDoFuncSnapBshutterBegin();
static void sensorDoFuncSnapBshutterFollow();
static void sensorDoFuncSnapBshutterEnd();
static void sensorDoFuncSnapNext(UINT32 next_mode);
static void sensorDoFuncPowerSave(UINT32 senmode);
/*static void sensor_exp_write_redo(UINT32 smode,SINT32 *expIdx,UINT32 *vcount,UINT8 *vtbl_type,UINT32 *int_us);*/
/*static void sensor_cmdport_begin(UINT32 smode,UINT32 expIdx,UINT32 agcIdx,UINT32 exp2Idx,UINT32 agc2Idx);*/
/*static void sensor_cmdport_end(void);*/
UINT8  ar0230_sensor_hdr_status_get();

extern UINT32 sensorModeGet();
extern void halFrontDataPinRemapCfg(UINT32 remap0,UINT32 remap1,UINT32 remap2,UINT32 remap3);
extern void sensor_PregainDelay(UINT8 flag);
extern UINT32 ae_preview_agc_get(void);

/*static UINT32 sensorSuspended;*/
static UINT8 hdrEnable = 0;
static UINT32 snap2pv=0;

/*static const UINT8 *gpdata;*/

static const UINT32 PrevSensitivityTbl[]={1000/*mode 0x30*/,1000/*mode 0x31*/,1000/*mode 0x32*/,1000/*mode 0x33*/,2000/*mode 0x34*/,1000/*mode 0x35*/,1000/*mode 0x36*/};
static const UINT32 SnapSensitivityTbl[]={1000/*mode 0x40*/,1000/*mode 0x41*/};

/******************** uCode ****************************/
static const UINT8 cmd_reg_reset_code[][4] SENSOR_SIF1DMA_BUF = {
	{0x30, 0x1A, 0x00, 0x59},
};

static const UINT8 cmd_stream_on_code[][4] SENSOR_SIF1DMA_BUF = {
	{0x30, 0x1A, 0x00, 0x5C},
};

static const UINT8 cmd_stream_off_code[][4] SENSOR_SIF1DMA_BUF = {
	{0x30, 0x1A, 0x00, 0x58},
};

static const UINT8 cmd_pll1_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x2A, 0x00, 0x06},
	{0x30, 0x2C, 0x00, 0x01},
	{0x30, 0x2E, 0x00, 0x04},
	{0x30, 0x30, 0x00, 0x42},
	{0x30, 0x36, 0x00, 0x0C},
	{0x30, 0x38, 0x00, 0x01},
};

static const UINT8 cmd_pll2_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x2A, 0x00, 0x0C},
	{0x30, 0x2C, 0x00, 0x01},
	{0x30, 0x2E, 0x00, 0x04},
	{0x30, 0x30, 0x00, 0x42},
	{0x30, 0x36, 0x00, 0x0C},
	{0x30, 0x38, 0x00, 0x02},
};

static const UINT8 cmd_pll3_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x2A, 0x00, 0x0C},
	{0x30, 0x2C, 0x00, 0x01},
	{0x30, 0x2E, 0x00, 0x04},
	{0x30, 0x30, 0x00, 0x48},
	{0x30, 0x36, 0x00, 0x0C},
	{0x30, 0x38, 0x00, 0x02},
};

static const UINT8 cmd_init_code_general[][4] SENSOR_SIF1DMA_BUF ={
	{0x3E, 0xD6, 0x34, 0xB3},
	{0x24, 0x36, 0x00, 0x0E},
	{0x32, 0x0C, 0x01, 0x80},
	{0x32, 0x0E, 0x03, 0x00},
	{0x32, 0x10, 0x05, 0x00},
	{0x32, 0x04, 0x0B, 0x6D},
	{0x30, 0xFE, 0x00, 0x80},
	{0x3E, 0xD8, 0x7B, 0x99},
	{0x3E, 0xDC, 0x9B, 0xA8},
	{0x3E, 0xDA, 0x9B, 0x9B},
	{0x30, 0x92, 0x00, 0x6F},
	{0x3E, 0xEC, 0x1C, 0x04},
	{0x30, 0xBA, 0x77, 0x9C},
	{0x3E, 0xF6, 0xA7, 0x0F},
	{0x30, 0x44, 0x04, 0x10},
	{0x3E, 0xD0, 0xFF, 0x44},
	{0x3E, 0xD4, 0x03, 0x1F},
	{0x30, 0xFE, 0x00, 0x80},
	{0x3E, 0xE2, 0x88, 0x66},
	{0x3E, 0xE4, 0x66, 0x23},
	{0x3E, 0xE6, 0x22, 0x63},
	{0x30, 0xE0, 0x42, 0x83},
	{0x30, 0xF0, 0x12, 0x83},
};

static const UINT8 cmd_init_code_2[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0xB0, 0x01, 0x18},
	{0x31, 0xAC, 0x0C, 0x0C},
};

static const UINT8 cmd_init_code_3[][4] SENSOR_SIF1DMA_BUF ={
	/*{0x30, 0xBA, 0x77, 0x0C},*/
	{0x31, 0xE0, 0x02, 0x00},
};

static const UINT8 cmd_init_code_4[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x82, 0x00, 0x09},
	{0x30, 0xBA, 0x76, 0x9C},
	{0x31, 0xE0, 0x02, 0x00},
};

static const UINT8 cmd_lowgaininit_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x96, 0x04, 0x80},
	{0x30, 0x98, 0x04, 0x80},
	{0x32, 0x06, 0x0B, 0x08},
	{0x32, 0x08, 0x1E, 0x13},
	{0x32, 0x02, 0x00, 0x80},
	{0x32, 0x00, 0x00, 0x02},
	{0x31, 0x00, 0x00, 0x00},
};

static const UINT8 cmd_highgaininit_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x96, 0x07, 0x80},
	{0x30, 0x98, 0x07, 0x80},
	{0x32, 0x06, 0x1C, 0x0E},
	{0x32, 0x08, 0x4E, 0x39},
	{0x32, 0x02, 0x00, 0xB0},
	{0x32, 0x00, 0x00, 0x02},
	{0x31, 0x00, 0x00, 0x04},
};

static const UINT8 cmd_init_code_5[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0x8E, 0x00, 0x00},
};

static const UINT8 cmd_DG_before_DLO[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0x8E, 0x02, 0x00},
};

static const UINT8 cmd_altm_2DMC_fast[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0x98, 0x18, 0x3C},
};

static const UINT8 cmd_altm_2DMC_slow[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0x98, 0x40, 0x3C},
};

static const UINT8 cmd_altm_2DMC_middle[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0x98, 0x18, 0x3C},
};

static const UINT8 cmd_linear_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x88, 0x82, 0x42},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x72, 0x9B},
	{0x30, 0x86, 0x4A, 0x31},
	{0x30, 0x86, 0x43, 0x42},
	{0x30, 0x86, 0x8E, 0x03},
	{0x30, 0x86, 0x2A, 0x14},
	{0x30, 0x86, 0x45, 0x78},
	{0x30, 0x86, 0x7B, 0x3D},
	{0x30, 0x86, 0xFF, 0x3D},
	{0x30, 0x86, 0xFF, 0x3D},
	{0x30, 0x86, 0xEA, 0x2A},
	{0x30, 0x86, 0x04, 0x3D},
	{0x30, 0x86, 0x10, 0x2A},
	{0x30, 0x86, 0x05, 0x2A},
	{0x30, 0x86, 0x15, 0x35},
	{0x30, 0x86, 0x2A, 0x05},
	{0x30, 0x86, 0x3D, 0x10},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x2A, 0x04},
	{0x30, 0x86, 0x2A, 0x14},
	{0x30, 0x86, 0x3D, 0xFF},
	{0x30, 0x86, 0x3D, 0xFF},
	{0x30, 0x86, 0x3D, 0xEA},
	{0x30, 0x86, 0x2A, 0x04},
	{0x30, 0x86, 0x62, 0x2A},
	{0x30, 0x86, 0x28, 0x8E},
	{0x30, 0x86, 0x00, 0x36},
	{0x30, 0x86, 0x2A, 0x08},
	{0x30, 0x86, 0x3D, 0x64},
	{0x30, 0x86, 0x7A, 0x3D},
	{0x30, 0x86, 0x04, 0x44},
	{0x30, 0x86, 0x2C, 0x4B},
	{0x30, 0x86, 0x8F, 0x03},
	{0x30, 0x86, 0x43, 0x0D},
	{0x30, 0x86, 0x2D, 0x46},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x5F, 0x16},
	{0x30, 0x86, 0x53, 0x0D},
	{0x30, 0x86, 0x16, 0x60},
	{0x30, 0x86, 0x3E, 0x4C},
	{0x30, 0x86, 0x29, 0x04},
	{0x30, 0x86, 0x29, 0x84},
	{0x30, 0x86, 0x8E, 0x03},
	{0x30, 0x86, 0x2A, 0xFC},
	{0x30, 0x86, 0x5C, 0x1D},
	{0x30, 0x86, 0x57, 0x54},
	{0x30, 0x86, 0x49, 0x5F},
	{0x30, 0x86, 0x53, 0x05},
	{0x30, 0x86, 0x53, 0x07},
	{0x30, 0x86, 0x4D, 0x2B},
	{0x30, 0x86, 0xF8, 0x10},
	{0x30, 0x86, 0x16, 0x4C},
	{0x30, 0x86, 0x09, 0x55},
	{0x30, 0x86, 0x56, 0x2B},
	{0x30, 0x86, 0xB8, 0x2B},
	{0x30, 0x86, 0x98, 0x4E},
	{0x30, 0x86, 0x11, 0x29},
	{0x30, 0x86, 0x94, 0x60},
	{0x30, 0x86, 0x5C, 0x19},
	{0x30, 0x86, 0x5C, 0x1B},
	{0x30, 0x86, 0x45, 0x48},
	{0x30, 0x86, 0x45, 0x08},
	{0x30, 0x86, 0x45, 0x88},
	{0x30, 0x86, 0x29, 0xB6},
	{0x30, 0x86, 0x8E, 0x01},
	{0x30, 0x86, 0x2A, 0xF8},
	{0x30, 0x86, 0x3E, 0x02},
	{0x30, 0x86, 0x2A, 0xFA},
	{0x30, 0x86, 0x3F, 0x09},
	{0x30, 0x86, 0x5C, 0x1B},
	{0x30, 0x86, 0x29, 0xB2},
	{0x30, 0x86, 0x3F, 0x0C},
	{0x30, 0x86, 0x3E, 0x03},
	{0x30, 0x86, 0x3E, 0x15},
	{0x30, 0x86, 0x5C, 0x13},
	{0x30, 0x86, 0x3F, 0x11},
	{0x30, 0x86, 0x3E, 0x0F},
	{0x30, 0x86, 0x5F, 0x2B},
	{0x30, 0x86, 0x90, 0x2A},
	{0x30, 0x86, 0xF2, 0x2B},
	{0x30, 0x86, 0x80, 0x3E},
	{0x30, 0x86, 0x06, 0x3F},
	{0x30, 0x86, 0x06, 0x60},
	{0x30, 0x86, 0x29, 0xA2},
	{0x30, 0x86, 0x29, 0xA3},
	{0x30, 0x86, 0x5F, 0x4D},
	{0x30, 0x86, 0x1C, 0x2A},
	{0x30, 0x86, 0xFA, 0x29},
	{0x30, 0x86, 0x83, 0x45},
	{0x30, 0x86, 0xA8, 0x3E},
	{0x30, 0x86, 0x07, 0x2A},
	{0x30, 0x86, 0xFB, 0x3E},
	{0x30, 0x86, 0x29, 0x45},
	{0x30, 0x86, 0x88, 0x24},
	{0x30, 0x86, 0x3E, 0x08},
	{0x30, 0x86, 0x2A, 0xFA},
	{0x30, 0x86, 0x5D, 0x29},
	{0x30, 0x86, 0x92, 0x88},
	{0x30, 0x86, 0x10, 0x2B},
	{0x30, 0x86, 0x04, 0x8B},
	{0x30, 0x86, 0x16, 0x86},
	{0x30, 0x86, 0x8D, 0x48},
	{0x30, 0x86, 0x4D, 0x4E},
	{0x30, 0x86, 0x2B, 0x80},
	{0x30, 0x86, 0x4C, 0x0B},
	{0x30, 0x86, 0x60, 0x3F},
	{0x30, 0x86, 0x30, 0x2A},
	{0x30, 0x86, 0xF2, 0x3F},
	{0x30, 0x86, 0x10, 0x29},
	{0x30, 0x86, 0x82, 0x29},
	{0x30, 0x86, 0x83, 0x29},
	{0x30, 0x86, 0x43, 0x5C},
	{0x30, 0x86, 0x15, 0x5F},
	{0x30, 0x86, 0x4D, 0x1C},
	{0x30, 0x86, 0x2A, 0xFA},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x8E, 0x00},
	{0x30, 0x86, 0x2A, 0x98},
	{0x30, 0x86, 0x3F, 0x0A},
	{0x30, 0x86, 0x4A, 0x0A},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x0B, 0x43},
	{0x30, 0x86, 0x16, 0x8E},
	{0x30, 0x86, 0x03, 0x2A},
	{0x30, 0x86, 0x9C, 0x45},
	{0x30, 0x86, 0x78, 0x3F},
	{0x30, 0x86, 0x07, 0x2A},
	{0x30, 0x86, 0x9D, 0x3E},
	{0x30, 0x86, 0x30, 0x5D},
	{0x30, 0x86, 0x29, 0x44},
	{0x30, 0x86, 0x88, 0x10},
	{0x30, 0x86, 0x2B, 0x04},
	{0x30, 0x86, 0x53, 0x0D},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x3E, 0x08},
	{0x30, 0x86, 0x8E, 0x01},
	{0x30, 0x86, 0x2A, 0x98},
	{0x30, 0x86, 0x8E, 0x00},
	{0x30, 0x86, 0x76, 0x9C},
	{0x30, 0x86, 0x77, 0x9C},
	{0x30, 0x86, 0x46, 0x44},
	{0x30, 0x86, 0x16, 0x16},
	{0x30, 0x86, 0x90, 0x7A},
	{0x30, 0x86, 0x12, 0x44},
	{0x30, 0x86, 0x4B, 0x18},
	{0x30, 0x86, 0x4A, 0x04},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x06, 0x43},
	{0x30, 0x86, 0x16, 0x05},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x07, 0x43},
	{0x30, 0x86, 0x16, 0x58},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x5A, 0x43},
	{0x30, 0x86, 0x16, 0x45},
	{0x30, 0x86, 0x58, 0x8E},
	{0x30, 0x86, 0x03, 0x2A},
	{0x30, 0x86, 0x9C, 0x45},
	{0x30, 0x86, 0x78, 0x7B},
	{0x30, 0x86, 0x3F, 0x07},
	{0x30, 0x86, 0x2A, 0x9D},
	{0x30, 0x86, 0x53, 0x0D},
	{0x30, 0x86, 0x8B, 0x16},
	{0x30, 0x86, 0x86, 0x3E},
	{0x30, 0x86, 0x23, 0x45},
	{0x30, 0x86, 0x58, 0x25},
	{0x30, 0x86, 0x3E, 0x10},
	{0x30, 0x86, 0x8E, 0x01},
	{0x30, 0x86, 0x2A, 0x98},
	{0x30, 0x86, 0x8E, 0x00},
	{0x30, 0x86, 0x3E, 0x10},
	{0x30, 0x86, 0x8D, 0x60},
	{0x30, 0x86, 0x12, 0x44},
	{0x30, 0x86, 0x4B, 0x2C},
	{0x30, 0x86, 0x2C, 0x2C},
};

static const UINT8 cmd_hdr_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x88, 0x80, 0x00},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x72, 0x9B},
	{0x30, 0x86, 0x4A, 0x31},
	{0x30, 0x86, 0x43, 0x42},
	{0x30, 0x86, 0x8E, 0x03},
	{0x30, 0x86, 0x2A, 0x14},
	{0x30, 0x86, 0x45, 0x78},
	{0x30, 0x86, 0x7B, 0x3D},
	{0x30, 0x86, 0xFF, 0x3D},
	{0x30, 0x86, 0xFF, 0x3D},
	{0x30, 0x86, 0xEA, 0x2A},
	{0x30, 0x86, 0x04, 0x3D},
	{0x30, 0x86, 0x10, 0x2A},
	{0x30, 0x86, 0x05, 0x2A},
	{0x30, 0x86, 0x15, 0x35},
	{0x30, 0x86, 0x2A, 0x05},
	{0x30, 0x86, 0x3D, 0x10},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x2A, 0x04},
	{0x30, 0x86, 0x2A, 0x14},
	{0x30, 0x86, 0x3D, 0xFF},
	{0x30, 0x86, 0x3D, 0xFF},
	{0x30, 0x86, 0x3D, 0xEA},
	{0x30, 0x86, 0x2A, 0x04},
	{0x30, 0x86, 0x62, 0x2A},
	{0x30, 0x86, 0x28, 0x8E},
	{0x30, 0x86, 0x00, 0x36},
	{0x30, 0x86, 0x2A, 0x08},
	{0x30, 0x86, 0x3D, 0x64},
	{0x30, 0x86, 0x7A, 0x3D},
	{0x30, 0x86, 0x04, 0x44},
	{0x30, 0x86, 0x2C, 0x4B},
	{0x30, 0x86, 0x8F, 0x00},
	{0x30, 0x86, 0x43, 0x0C},
	{0x30, 0x86, 0x2D, 0x63},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x8E, 0x03},
	{0x30, 0x86, 0x2A, 0xFC},
	{0x30, 0x86, 0x5C, 0x1D},
	{0x30, 0x86, 0x57, 0x54},
	{0x30, 0x86, 0x49, 0x5F},
	{0x30, 0x86, 0x53, 0x05},
	{0x30, 0x86, 0x53, 0x07},
	{0x30, 0x86, 0x4D, 0x2B},
	{0x30, 0x86, 0xF8, 0x10},
	{0x30, 0x86, 0x16, 0x4C},
	{0x30, 0x86, 0x08, 0x55},
	{0x30, 0x86, 0x56, 0x2B},
	{0x30, 0x86, 0xB8, 0x2B},
	{0x30, 0x86, 0x98, 0x4E},
	{0x30, 0x86, 0x11, 0x29},
	{0x30, 0x86, 0x04, 0x29},
	{0x30, 0x86, 0x84, 0x29},
	{0x30, 0x86, 0x94, 0x60},
	{0x30, 0x86, 0x5C, 0x19},
	{0x30, 0x86, 0x5C, 0x1B},
	{0x30, 0x86, 0x45, 0x48},
	{0x30, 0x86, 0x45, 0x08},
	{0x30, 0x86, 0x45, 0x88},
	{0x30, 0x86, 0x29, 0xB6},
	{0x30, 0x86, 0x8E, 0x01},
	{0x30, 0x86, 0x2A, 0xF8},
	{0x30, 0x86, 0x3E, 0x02},
	{0x30, 0x86, 0x2A, 0xFA},
	{0x30, 0x86, 0x3F, 0x09},
	{0x30, 0x86, 0x5C, 0x1B},
	{0x30, 0x86, 0x29, 0xB2},
	{0x30, 0x86, 0x3F, 0x0C},
	{0x30, 0x86, 0x3E, 0x02},
	{0x30, 0x86, 0x3E, 0x13},
	{0x30, 0x86, 0x5C, 0x13},
	{0x30, 0x86, 0x3F, 0x11},
	{0x30, 0x86, 0x3E, 0x0B},
	{0x30, 0x86, 0x5F, 0x2B},
	{0x30, 0x86, 0x90, 0x2A},
	{0x30, 0x86, 0xF2, 0x2B},
	{0x30, 0x86, 0x80, 0x3E},
	{0x30, 0x86, 0x04, 0x3F},
	{0x30, 0x86, 0x06, 0x60},
	{0x30, 0x86, 0x29, 0xA2},
	{0x30, 0x86, 0x29, 0xA3},
	{0x30, 0x86, 0x5F, 0x4D},
	{0x30, 0x86, 0x19, 0x2A},
	{0x30, 0x86, 0xFA, 0x29},
	{0x30, 0x86, 0x83, 0x45},
	{0x30, 0x86, 0xA8, 0x3E},
	{0x30, 0x86, 0x07, 0x2A},
	{0x30, 0x86, 0xFB, 0x3E},
	{0x30, 0x86, 0x29, 0x45},
	{0x30, 0x86, 0x88, 0x21},
	{0x30, 0x86, 0x3E, 0x08},
	{0x30, 0x86, 0x2A, 0xFA},
	{0x30, 0x86, 0x5D, 0x29},
	{0x30, 0x86, 0x92, 0x88},
	{0x30, 0x86, 0x10, 0x2B},
	{0x30, 0x86, 0x04, 0x8B},
	{0x30, 0x86, 0x16, 0x85},
	{0x30, 0x86, 0x8D, 0x48},
	{0x30, 0x86, 0x4D, 0x4E},
	{0x30, 0x86, 0x2B, 0x80},
	{0x30, 0x86, 0x4C, 0x0B},
	{0x30, 0x86, 0x60, 0x3F},
	{0x30, 0x86, 0x28, 0x2A},
	{0x30, 0x86, 0xF2, 0x3F},
	{0x30, 0x86, 0x0F, 0x29},
	{0x30, 0x86, 0x82, 0x29},
	{0x30, 0x86, 0x83, 0x29},
	{0x30, 0x86, 0x43, 0x5C},
	{0x30, 0x86, 0x15, 0x5F},
	{0x30, 0x86, 0x4D, 0x19},
	{0x30, 0x86, 0x2A, 0xFA},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x8E, 0x00},
	{0x30, 0x86, 0x2A, 0x98},
	{0x30, 0x86, 0x3F, 0x06},
	{0x30, 0x86, 0x12, 0x44},
	{0x30, 0x86, 0x4A, 0x04},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x05, 0x43},
	{0x30, 0x86, 0x16, 0x58},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x5A, 0x43},
	{0x30, 0x86, 0x16, 0x06},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x07, 0x43},
	{0x30, 0x86, 0x16, 0x8E},
	{0x30, 0x86, 0x03, 0x2A},
	{0x30, 0x86, 0x9C, 0x45},
	{0x30, 0x86, 0x78, 0x7B},
	{0x30, 0x86, 0x3F, 0x07},
	{0x30, 0x86, 0x2A, 0x9D},
	{0x30, 0x86, 0x3E, 0x2E},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x25, 0x3E},
	{0x30, 0x86, 0x06, 0x8E},
	{0x30, 0x86, 0x01, 0x2A},
	{0x30, 0x86, 0x98, 0x8E},
	{0x30, 0x86, 0x00, 0x12},
	{0x30, 0x86, 0x44, 0x4B},
	{0x30, 0x86, 0x03, 0x43},
	{0x30, 0x86, 0x2D, 0x46},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0xA3, 0x43},
	{0x30, 0x86, 0x16, 0x5D},
	{0x30, 0x86, 0x0D, 0x29},
	{0x30, 0x86, 0x44, 0x88},
	{0x30, 0x86, 0x10, 0x2B},
	{0x30, 0x86, 0x04, 0x53},
	{0x30, 0x86, 0x0D, 0x8B},
	{0x30, 0x86, 0x16, 0x85},
	{0x30, 0x86, 0x44, 0x8E},
	{0x30, 0x86, 0x03, 0x2A},
	{0x30, 0x86, 0xFC, 0x5C},
	{0x30, 0x86, 0x1D, 0x8D},
	{0x30, 0x86, 0x60, 0x57},
	{0x30, 0x86, 0x54, 0x49},
	{0x30, 0x86, 0x5F, 0x53},
	{0x30, 0x86, 0x05, 0x53},
	{0x30, 0x86, 0x07, 0x4D},
	{0x30, 0x86, 0x2B, 0xF8},
	{0x30, 0x86, 0x10, 0x16},
	{0x30, 0x86, 0x4C, 0x08},
	{0x30, 0x86, 0x55, 0x56},
	{0x30, 0x86, 0x2B, 0xB8},
	{0x30, 0x86, 0x2B, 0x98},
	{0x30, 0x86, 0x4E, 0x11},
	{0x30, 0x86, 0x29, 0x04},
	{0x30, 0x86, 0x29, 0x84},
	{0x30, 0x86, 0x29, 0x94},
	{0x30, 0x86, 0x60, 0x5C},
	{0x30, 0x86, 0x19, 0x5C},
	{0x30, 0x86, 0x1B, 0x45},
	{0x30, 0x86, 0x48, 0x45},
	{0x30, 0x86, 0x08, 0x45},
	{0x30, 0x86, 0x88, 0x29},
	{0x30, 0x86, 0xB6, 0x8E},
	{0x30, 0x86, 0x01, 0x2A},
	{0x30, 0x86, 0xF8, 0x3E},
	{0x30, 0x86, 0x02, 0x2A},
	{0x30, 0x86, 0xFA, 0x3F},
	{0x30, 0x86, 0x09, 0x5C},
	{0x30, 0x86, 0x1B, 0x29},
	{0x30, 0x86, 0xB2, 0x3F},
	{0x30, 0x86, 0x0C, 0x3E},
	{0x30, 0x86, 0x02, 0x3E},
	{0x30, 0x86, 0x13, 0x5C},
	{0x30, 0x86, 0x13, 0x3F},
	{0x30, 0x86, 0x11, 0x3E},
	{0x30, 0x86, 0x0B, 0x5F},
	{0x30, 0x86, 0x2B, 0x90},
	{0x30, 0x86, 0x2A, 0xF2},
	{0x30, 0x86, 0x2B, 0x80},
	{0x30, 0x86, 0x3E, 0x04},
	{0x30, 0x86, 0x3F, 0x06},
	{0x30, 0x86, 0x60, 0x29},
	{0x30, 0x86, 0xA2, 0x29},
	{0x30, 0x86, 0xA3, 0x5F},
	{0x30, 0x86, 0x4D, 0x1C},
	{0x30, 0x86, 0x2A, 0xFA},
	{0x30, 0x86, 0x29, 0x83},
	{0x30, 0x86, 0x45, 0xA8},
	{0x30, 0x86, 0x3E, 0x07},
	{0x30, 0x86, 0x2A, 0xFB},
	{0x30, 0x86, 0x3E, 0x29},
	{0x30, 0x86, 0x45, 0x88},
	{0x30, 0x86, 0x24, 0x3E},
	{0x30, 0x86, 0x08, 0x2A},
	{0x30, 0x86, 0xFA, 0x5D},
	{0x30, 0x86, 0x29, 0x92},
	{0x30, 0x86, 0x88, 0x10},
	{0x30, 0x86, 0x2B, 0x04},
	{0x30, 0x86, 0x8B, 0x16},
	{0x30, 0x86, 0x86, 0x8D},
	{0x30, 0x86, 0x48, 0x4D},
	{0x30, 0x86, 0x4E, 0x2B},
	{0x30, 0x86, 0x80, 0x4C},
	{0x30, 0x86, 0x0B, 0x60},
	{0x30, 0x86, 0x3F, 0x28},
	{0x30, 0x86, 0x2A, 0xF2},
	{0x30, 0x86, 0x3F, 0x0F},
	{0x30, 0x86, 0x29, 0x82},
	{0x30, 0x86, 0x29, 0x83},
	{0x30, 0x86, 0x29, 0x43},
	{0x30, 0x86, 0x5C, 0x15},
	{0x30, 0x86, 0x5F, 0x4D},
	{0x30, 0x86, 0x1C, 0x2A},
	{0x30, 0x86, 0xFA, 0x45},
	{0x30, 0x86, 0x58, 0x8E},
	{0x30, 0x86, 0x00, 0x2A},
	{0x30, 0x86, 0x98, 0x3F},
	{0x30, 0x86, 0x06, 0x4A},
	{0x30, 0x86, 0x73, 0x9D},
	{0x30, 0x86, 0x0A, 0x43},
	{0x30, 0x86, 0x16, 0x0B},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x8E, 0x03},
	{0x30, 0x86, 0x2A, 0x9C},
	{0x30, 0x86, 0x45, 0x78},
	{0x30, 0x86, 0x3F, 0x07},
	{0x30, 0x86, 0x2A, 0x9D},
	{0x30, 0x86, 0x3E, 0x12},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x3F, 0x04},
	{0x30, 0x86, 0x8E, 0x01},
	{0x30, 0x86, 0x2A, 0x98},
	{0x30, 0x86, 0x8E, 0x00},
	{0x30, 0x86, 0x91, 0x76},
	{0x30, 0x86, 0x9C, 0x77},
	{0x30, 0x86, 0x9C, 0x46},
	{0x30, 0x86, 0x44, 0x16},
	{0x30, 0x86, 0x16, 0x90},
	{0x30, 0x86, 0x7A, 0x12},
	{0x30, 0x86, 0x44, 0x4B},
	{0x30, 0x86, 0x4A, 0x00},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x63, 0x43},
	{0x30, 0x86, 0x16, 0x08},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x50, 0x43},
	{0x30, 0x86, 0x16, 0x65},
	{0x30, 0x86, 0x43, 0x16},
	{0x30, 0x86, 0x66, 0x43},
	{0x30, 0x86, 0x16, 0x8E},
	{0x30, 0x86, 0x03, 0x2A},
	{0x30, 0x86, 0x9C, 0x45},
	{0x30, 0x86, 0x78, 0x3F},
	{0x30, 0x86, 0x07, 0x2A},
	{0x30, 0x86, 0x9D, 0x5D},
	{0x30, 0x86, 0x0C, 0x29},
	{0x30, 0x86, 0x44, 0x88},
	{0x30, 0x86, 0x10, 0x2B},
	{0x30, 0x86, 0x04, 0x53},
	{0x30, 0x86, 0x0D, 0x8B},
	{0x30, 0x86, 0x16, 0x86},
	{0x30, 0x86, 0x3E, 0x1F},
	{0x30, 0x86, 0x45, 0x58},
	{0x30, 0x86, 0x28, 0x3E},
	{0x30, 0x86, 0x06, 0x8E},
	{0x30, 0x86, 0x01, 0x2A},
	{0x30, 0x86, 0x98, 0x8E},
	{0x30, 0x86, 0x00, 0x8D},
	{0x30, 0x86, 0x60, 0x12},
	{0x30, 0x86, 0x44, 0x4B},
	{0x30, 0x86, 0x2C, 0x2C},
	{0x30, 0x86, 0x2C, 0x2C},
};

static const UINT8 cmd_altm_enable_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x24, 0x20, 0x00, 0x00},
	{0x24, 0x40, 0x00, 0x00},
	{0x24, 0x42, 0x00, 0x80},
	{0x30, 0x1E, 0x00, 0x00},
	{0x24, 0x50, 0x00, 0x00},
	{0x32, 0x0A, 0x00, 0x80},
	{0x31, 0xD0, 0x00, 0x00},
	{0x24, 0x00, 0x00, 0x02},
	{0x24, 0x10, 0x00, 0x7F},
	{0x24, 0x12, 0x00, 0x2D},
	{0x24, 0x44, 0x80, 0x00},
	{0x24, 0x46, 0x00, 0x00},
	{0x24, 0x38, 0x00, 0x10},
	{0x24, 0x3A, 0x00, 0x12},
	{0x24, 0x3C, 0xFF, 0xFF},
	{0x24, 0x3E, 0x01, 0x00},
};

static const UINT8 cmd_altm_bypass_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x24, 0x00, 0x00, 0x01},
	{0x30, 0x1E, 0x00, 0xA8},
	{0x24, 0x50, 0x00, 0x00},
	{0x32, 0x0A, 0x00, 0x80},
};

static const UINT8 cmd_altm_fw_enable_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x24, 0x00, 0x00, 0x00},
	{0x24, 0x04, 0x00, 0x42},
	{0x24, 0x08, 0x01, 0x9B},
	{0x24, 0x0C, 0x50, 0xB6},	
};

static const UINT8 cmd_2dmc_on_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0x90, 0x00, 0x00},
	{0x31, 0x8A, 0x0E, 0x74},
	{0x31, 0x8C, 0xC0, 0x00},
	{0x31, 0x92, 0x04, 0x00},
	{0x31, 0x98, 0x40, 0x3C},
};

static const UINT8 cmd_2dmc_off_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0x8C, 0x00, 0x00},
};

static const UINT8 cmd_dlo_enable_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0x90, 0xE0, 0x00},
	{0x31, 0x94, 0x0B, 0xB8},
	{0x31, 0x96, 0x0E, 0x74},
};

static const UINT8 cmd_dlo_disable_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0x90, 0x00, 0x00},
};

/*static const UINT8 cmd_adacd_lowdcg_code[][4]={
	{0x32, 0x06, 0x0B, 0x08},
	{0x32, 0x08, 0x1E, 0x13},
	{0x32, 0x02, 0x00, 0x80},
	{0x32, 0x00, 0x02, 0x00},
};*/

static const UINT8 cmd_adacd_disable_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x32, 0x00, 0x00, 0x00},
};

static const UINT8 cmd_adacd_lowlight_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x32, 0x00, 0x00, 0x02},
};

/*static const UINT8 cmd_adacd_highdcg_code[][4]={
	{0x32, 0x06, 0x1C, 0x0E},
	{0x32, 0x08, 0x4E, 0x39},
	{0x32, 0x02, 0x00, 0xB0},
	{0x32, 0x00, 0x00, 0x02},
};*/

static const UINT8 cmd_companding_enable_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0xAC, 0x10, 0x0C},
	{0x31, 0xD0, 0x00, 0x01},
};

static const UINT8 cmd_companding_disable_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0xD0, 0x00, 0x00},
};
#if 0
static const UINT8 cmd_linear_lowdcg_code[][4]={
	{0x30, 0x60, 0x00, 0x0B},
	{0x30, 0x96, 0x00, 0x80},
	{0x30, 0x98, 0x00, 0x80},
	{0x32, 0x06, 0x0B, 0x08},
	{0x32, 0x08, 0x1E, 0x13},
	{0x32, 0x02, 0x00, 0x80},
	{0x32, 0x00, 0x00, 0x02},
	{0x31, 0x00, 0x00, 0x00},
};

static const UINT8 cmd_hdr_lowdcg_code[][4]={
	{0x30, 0x60, 0x00, 0x0B},
	{0x30, 0x96, 0x04, 0x80},
	{0x30, 0x98, 0x04, 0x80},
	{0x32, 0x06, 0x0B, 0x08},
	{0x32, 0x08, 0x1E, 0x13},
	{0x32, 0x02, 0x00, 0x80},
	{0x32, 0x00, 0x00, 0x02},
	{0x31, 0x00, 0x00, 0x00},
};
#endif
static const UINT8 cmd_hdr_ratio_32x_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x82, 0x00, 0x0C},
};

static const UINT8 cmd_hdr_ratio_16x_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x82, 0x00, 0x08},
};

static const UINT8 cmd_hdr_ratio_8x_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x82, 0x00, 0x04},
};

static const UINT8 cmd_hdr_ratio_4x_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0x82, 0x00, 0x00},
};

static const UINT8 cmd_hispi_set_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x31, 0xAE, 0x03, 0x04},
	{0x31, 0xC6, 0x04, 0x02},
	{0x30, 0x6E, 0x90, 0x10},
	{0x30, 0x1A, 0x00, 0x58},
};

/* 1080p 30fps*/
static const UINT8 cmd_1080P_30fps_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0xb0, 0x01, 0x18},
	{0x30, 0x04, 0x00, 0x00},
	{0x30, 0x08, 0x07, 0x87},
	{0x30, 0x02, 0x00, 0x00},
	{0x30, 0x06, 0x04, 0x37},
	{0x30, 0xa2, 0x00, 0x01},
	{0x30, 0xa6, 0x00, 0x01},
#if (AR0230_MIRROR_FLIP_INFO == 1)	
		{0x30, 0x40, 0x40, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 2)
		{0x30, 0x40, 0x80, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 3)
		{0x30, 0x40, 0xc0, 0x00},
#else
		{0x30, 0x40, 0x00, 0x00},
#endif
	{0x30, 0x0C, 0x04, 0x5e},
	/*{0x30, 0x64, 0x19, 0x82},*//*embedded_stats_en*/
};

/* 1080p 60fps*/
static const UINT8 cmd_1080P_60fps_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0xb0, 0x01, 0x18},
	{0x30, 0x04, 0x00, 0x00},
	{0x30, 0x08, 0x07, 0x87},
	{0x30, 0x02, 0x00, 0x00},
	{0x30, 0x06, 0x04, 0x37},
	{0x30, 0xa2, 0x00, 0x01},
	{0x30, 0xa6, 0x00, 0x01},
#if (AR0230_MIRROR_FLIP_INFO == 1)	
	{0x30, 0x40, 0x40, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 2)	
	{0x30, 0x40, 0x80, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 3)
	{0x30, 0x40, 0xc0, 0x00},
#else
	{0x30, 0x40, 0x00, 0x00},
#endif
	{0x30, 0x0C, 0x04, 0x4c},
	/*{0x30, 0x64, 0x19, 0x82},*//*embedded_stats_en*/
};

/* 1080p 30fps*/
static const UINT8 cmd_1080P30_Linear_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0xb0, 0x01, 0x18},
	{0x30, 0x04, 0x00, 0x00},
	{0x30, 0x08, 0x07, 0x87},
	{0x30, 0x02, 0x00, 0x00},
	{0x30, 0x06, 0x04, 0x37},
	{0x30, 0xa2, 0x00, 0x01},
	{0x30, 0xa6, 0x00, 0x01},
#if (AR0230_MIRROR_FLIP_INFO == 1)	
	{0x30, 0x40, 0x40, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 2)	
	{0x30, 0x40, 0x80, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 3)
	{0x30, 0x40, 0xc0, 0x00},
#else
	{0x30, 0x40, 0x00, 0x00},
#endif
	{0x30, 0x0C, 0x04, 0x5e},
};

/* 1080p 60fps*/
static const UINT8 cmd_1080P60_Linear_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0xb0, 0x01, 0x18},
	{0x30, 0x04, 0x00, 0x00},
	{0x30, 0x08, 0x07, 0x87},
	{0x30, 0x02, 0x00, 0x00},
	{0x30, 0x06, 0x04, 0x37},
	{0x30, 0xa2, 0x00, 0x01},
	{0x30, 0xa6, 0x00, 0x01},
#if (AR0230_MIRROR_FLIP_INFO == 1)	
	{0x30, 0x40, 0x40, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 2)	
	{0x30, 0x40, 0x80, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 3)
	{0x30, 0x40, 0xc0, 0x00},
#else
	{0x30, 0x40, 0x00, 0x00},
#endif
	{0x30, 0x0C, 0x04, 0x4c},
	/*{0x30, 0x12, 0x04, 0x60},*/
};

static const UINT8 cmd_2XBin30_Linear_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0xb0, 0x01, 0x18},
	{0x30, 0x04, 0x00, 0x00},
	{0x30, 0x08, 0x07, 0x7d},
	{0x30, 0x02, 0x00, 0x00},
	{0x30, 0x06, 0x04, 0x35},
	{0x30, 0xa2, 0x00, 0x03},
	{0x30, 0xa6, 0x00, 0x03},
	{0x30, 0x40, 0x30, 0x20},	
#if (AR0230_MIRROR_FLIP_INFO == 1)		
	{0x30, 0x40, 0x70, 0x20},
#elif (AR0230_MIRROR_FLIP_INFO == 2)
	{0x30, 0x40, 0xb0, 0x20},
#elif (AR0230_MIRROR_FLIP_INFO == 3)
	{0x30, 0x40, 0xf0, 0x20},
#else
	{0x30, 0x40, 0x30, 0x20},
#endif
	{0x30, 0x0C, 0x04, 0x4c},
	/*{0x30, 0x12, 0x04, 0x60},*/
};

static const UINT8 cmd_Quarter_Linear_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0xb0, 0x01, 0x18},
	{0x30, 0x04, 0x00, 0x00},
	{0x30, 0x08, 0x07, 0x87},
	{0x30, 0x02, 0x00, 0x00},
	{0x30, 0x06, 0x04, 0x37},
	{0x30, 0xa2, 0x00, 0x03},
	{0x30, 0xa6, 0x00, 0x03},
#if (AR0230_MIRROR_FLIP_INFO == 1)		
	{0x30, 0x40, 0x40, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 2)
	{0x30, 0x40, 0x80, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 3)
	{0x30, 0x40, 0xC0, 0x00},
#else
	{0x30, 0x40, 0x00, 0x00},
#endif
	{0x30, 0x0C, 0x04, 0x4c},
	/*{0x30, 0x12, 0x02, 0x00},*/
};

/* 720p 90fps*/
static const UINT8 cmd_720P90_Linear_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0xb0, 0x01, 0x18},
	{0x30, 0x04, 0x01, 0x4c},
	{0x30, 0x08, 0x06, 0x4b},
	{0x30, 0x02, 0x00, 0xb8},
	{0x30, 0x06, 0x03, 0x87},
	{0x30, 0xa2, 0x00, 0x01},
	{0x30, 0xa6, 0x00, 0x01},
#if (AR0230_MIRROR_FLIP_INFO == 1)		
	{0x30, 0x40, 0x40, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 2)
	{0x30, 0x40, 0x80, 0x00},
#elif (AR0230_MIRROR_FLIP_INFO == 3)
	{0x30, 0x40, 0xc0, 0x00},
#else
	{0x30, 0x40, 0x00, 0x00},
#endif
	{0x30, 0x0C, 0x04, 0x4c},
	/*{0x30, 0x12, 0x04, 0x60},*/
};

/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg =
{
	.t = FRONT_RX_BIAS_10UA,
	.sensel=0,
	.Lvdseolsel=1,
	.Lvdseofsel=2,
	.mipi=0,
	.samectrlword=1,/*In HiSpi case, all data lane must use the same control word in SOF, SOL, EOL and EOF*/
	.spsel=0xF,
	.wordwidth=3,/*10bit*/
	.decpixelnum=3,
	.wordcntsel=0,
	.SenPinSelNum =4,
	.SenPinSel={0x0,0x1,0x2,0x3},
	.Ctrlword_sol =  0x800 ,
	.Ctrlword_sof =  0x800 ,/*0xc00*/
	.Ctrlword_eol =  0xa00 ,
	.Ctrlword_eof =  0xe00 ,
	/*.settle = 18,*/
};

/*for solving V35 auto decod word count erro*/
static const sensorLVDSCfg_t sensorLVDSCfg_VGA =
{
	.t = FRONT_RX_BIAS_10UA,
	.sensel=0,
	.Lvdseolsel=1,
	.Lvdseofsel=2,
	.mipi=0,
	.samectrlword=1,/*In HiSpi case, all data lane must use the same control word in SOF, SOL, EOL and EOF*/
	.spsel=0xF,
	.wordwidth=3,/*10bit*/
	.decpixelnum=3,
	.wordcntsel=0,
	.SenPinSelNum =4,
	.SenPinSel={0x0,0x1,0x2,0x3},
	.Ctrlword_sol =  0x800 ,
	.Ctrlword_sof =  0x800 ,
	.Ctrlword_eol =  0xa00 ,
	.Ctrlword_eof =  0xe00 ,
};

static const sensorTgPreviewCfg_t sensorTgPreview1080P_30fps_Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewBegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_1080P_30FPS_HSIZE,
		.ySize = PREVIEW_1080P_30FPS_VSIZE,
	},
	.hsize =					PREVIEW_1080P_30FPS_HSIZE,
	.vsize = 					PREVIEW_1080P_30FPS_VSIZE,
	.sensor_model =             SENSORMODEL,
    .model_info =               SENSORMODEL_MIPI_RAW,
	.cfa_pattern =              PREVIEW_1080P_30FPS_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_1080P_30FPS_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_1080P_30FPS_PCLK1X,
	.sensor_htotal = PREVIEW_1080P_30FPS_LINETOTAL,
	.pix_100khz = (UINT16)PREVIEW_1080P_30FPS_CDSPCLK,
	.bayerhs_size = PREVIEW_1080P_30FPS_BAYERHS_SIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreview1080P_60fps_Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewBegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_1080P_60FPS_HSIZE,
		.ySize = PREVIEW_1080P_60FPS_VSIZE,
	},
	.hsize =					PREVIEW_1080P_60FPS_HSIZE,
	.vsize = 					PREVIEW_1080P_60FPS_VSIZE,
	.sensor_model =             SENSORMODEL,
    .model_info =               SENSORMODEL_MIPI_RAW,
	.cfa_pattern =              PREVIEW_1080P_30FPS_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_1080P_60FPS_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_1080P_60FPS_PCLK1X,
	.sensor_htotal = PREVIEW_1080P_60FPS_LINETOTAL,
	.pix_100khz = (UINT16)PREVIEW_1080P_60FPS_CDSPCLK,
	.bayerhs_size = PREVIEW_1080P_60FPS_BAYERHS_SIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreview1080P30_Linear_Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewBegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_1080P30_LINEAR_HSIZE,
		.ySize = PREVIEW_1080P30_LINEAR_VSIZE,
	},
	.hsize =					PREVIEW_1080P30_LINEAR_HSIZE,
	.vsize = 					PREVIEW_1080P30_LINEAR_VSIZE,
	.sensor_model =             SENSORMODEL,
    .model_info =               SENSORMODEL_MIPI_RAW,
	.cfa_pattern =              PREVIEW_1080P_30FPS_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_1080P30_LINEAR_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_1080P30_LINEAR_PCLK1X,
	.sensor_htotal = PREVIEW_1080P30_LINEAR_LINETOTAL,
	.pix_100khz = (UINT16)PREVIEW_1080P30_LINEAR_CDSPCLK,
	.bayerhs_size = PREVIEW_1080P30_LINEAR_BAYERHS_SIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreview1080P60_Linear_Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewBegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_1080P60_LINEAR_HSIZE,
		.ySize = PREVIEW_1080P60_LINEAR_VSIZE,
	},
	.hsize =					PREVIEW_1080P60_LINEAR_HSIZE,
	.vsize = 					PREVIEW_1080P60_LINEAR_VSIZE,
	.sensor_model =             SENSORMODEL,
    .model_info =               SENSORMODEL_MIPI_RAW,
	.cfa_pattern =              PREVIEW_1080P_30FPS_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_1080P60_LINEAR_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_1080P60_LINEAR_PCLK1X,
	.sensor_htotal = PREVIEW_1080P60_LINEAR_LINETOTAL,
	.pix_100khz = (UINT16)PREVIEW_1080P60_LINEAR_CDSPCLK,
	.bayerhs_size = PREVIEW_1080P60_LINEAR_BAYERHS_SIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreview2XBin30_Linear_Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewBegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_2XBIN_60FPS_LINEAR_HSIZE,
		.ySize = PREVIEW_2XBIN_60FPS_LINEAR_VSIZE,
	},
	.hsize =					PREVIEW_2XBIN_60FPS_LINEAR_HSIZE,
	.vsize = 					PREVIEW_2XBIN_60FPS_LINEAR_VSIZE,
	.sensor_model =             SENSORMODEL,
    .model_info =               SENSORMODEL_MIPI_RAW,
	.cfa_pattern =              PREVIEW_1080P_30FPS_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_2XBIN_60FPS_LINEAR_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_2XBIN_60FPS_LINEAR_PCLK1X,
	.sensor_htotal = PREVIEW_2XBIN_60FPS_LINEAR_LINETOTAL,
	.pix_100khz = (UINT16)PREVIEW_2XBIN_60FPS_LINEAR_CDSPCLK,
	.bayerhs_size = PREVIEW_2XBIN_60FPS_LINEAR_BAYERHS_SIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreviewQuarter_Linear_Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin =         sensorDoFuncPreviewBegin,
		.previewShortExp =      sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =  NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =    NULL,
		.previewExpEnd =        sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_QUARTER_LINEAR_HSIZE,
		.ySize = PREVIEW_QUARTER_LINEAR_VSIZE,
	},
	.hsize =					PREVIEW_QUARTER_LINEAR_HSIZE,
	.vsize = 					PREVIEW_QUARTER_LINEAR_VSIZE,
	.sensor_model =             SENSORMODEL,
    .model_info =               SENSORMODEL_MIPI_RAW,
	.cfa_pattern =              PREVIEW_QUARTER_LINEAR_CFAPATTERN,
	.hratio = 912,
	.vratio = 512,
	.fps_mhz = (PREVIEW_QUARTER_LINEAR_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_QUARTER_LINEAR_PCLK1X,
	.sensor_htotal = PREVIEW_QUARTER_LINEAR_LINETOTAL,
	.pix_100khz = (UINT16)PREVIEW_QUARTER_LINEAR_CDSPCLK,
	.bayerhs_size = PREVIEW_QUARTER_LINEAR_BAYERHS_SIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreview720P90_Linear_Cfg = {
	.fpdo = &(const sensorPreviewDo_t) {
		.previewBegin = 		sensorDoFuncPreviewBegin,
		.previewShortExp =		sensorDoFuncPreviewShortExp,
		.previewLongExpBegin =	NULL,
		.previewLongExpFollow = NULL,
		.previewLongExpEnd =	NULL,
		.previewExpEnd =		sensorDoFuncPreviewExpEnd,
	},
	.crop = &( const sensorTgCrop_t ) {
		.xOffset = 0,
		.yOffset = 0,
		.xSize = PREVIEW_720P90_LINEAR_HSIZE,
		.ySize = PREVIEW_720P90_LINEAR_VSIZE,
	},
	.hsize =					PREVIEW_720P90_LINEAR_HSIZE,
	.vsize =					PREVIEW_720P90_LINEAR_VSIZE,
	.sensor_model = 			SENSORMODEL,
	.model_info =				SENSORMODEL_MIPI_RAW,
	.cfa_pattern =				PREVIEW_1080P_30FPS_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_720P90_LINEAR_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_720P90_LINEAR_PCLK1X,
	.sensor_htotal = PREVIEW_720P90_LINEAR_LINETOTAL,
	.pix_100khz = (UINT16)PREVIEW_720P90_LINEAR_CDSPCLK,
	.bayerhs_size = PREVIEW_720P90_LINEAR_BAYERHS_SIZE,
};

static  sensorTgSnapCfg_t sensorTgSnapRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			.psnapEarly = (sensorDoFunc_t []){
				/*sensorDoFuncSnapEarly0, 	*/
			},
			.snapExpBegin =        sensorDoFuncSnapExpBegin,
			.snapBshutterBegin =   sensorDoFuncSnapBshutterBegin,
			.snapBshutterFollow =  sensorDoFuncSnapBshutterFollow,
			.snapBshutterEnd =     sensorDoFuncSnapBshutterEnd,
			.snapLongExpBegin =    NULL,
			.snapLongExpFollow =   NULL,
			.snapLongExpEnd =      NULL,
			.snapShortExp =        sensorDoFuncSnapShortExp,
			.snapExpEnd =          sensorDoFuncSnapExpEnd,
			.psnapDummy =          NULL,
			.psnapReadoutField = (sensorDoFunc_t []) {
				sensorDoFuncSnapReadoutField0,
			},
			.snapFieldEnd =        sensorDoFuncSnapFieldEnd,
			.snapReadoutEnd =      sensorDoFuncSnapReadoutEnd,
			.snapNext = 		   sensorDoFuncSnapNext,
		},
        .crop = NULL,
 		.hsize =                   CAPTURE_HSIZE,
		.vsize =				   CAPTURE_VSIZE,
		.sensor_model =			   SENSORMODEL,/*TYPE_HSCMOS,*/
		.model_info =			   SENSORMODEL_MIPI_RAW,
		.cfa_pattern =			   SNAP_CFAPATTERN,
		.n_dummy =				   SNAP_DUMMYVD_NUM,
		.n_readout =			   SNAP_READOUTVD_NUM,
		.n_capvd =				   SNAP_CAPTUREVD_NUM,
		.n_snap_early =			   SNAP_EARLY_NUM,
		.skip_snap_readout_vd =	   SKIP_SNAP_READOUTVD_EN,
		.fldsel_start =			   FLDSEL_START_NUM,
		.fldsel_end =			   FLDSEL_END_NUM,
		.hratio = 1600, /*16:9 sensor*/
		.vratio = 900,
		.pix_100khz =(UINT16)CAPTURE_CDSPCLK,
};

static  sensorTgSnapCfg_t sensorTgSnapHDRRollCfg = {
	.fpdo = &(const sensorSnapDo_t) {
			.psnapEarly = (sensorDoFunc_t []){
				/*sensorDoFuncSnapEarly0, 	*/
			},
			.snapExpBegin =        sensorDoFuncSnapExpBegin,
			.snapBshutterBegin =   sensorDoFuncSnapBshutterBegin,
			.snapBshutterFollow =  sensorDoFuncSnapBshutterFollow,
			.snapBshutterEnd =     sensorDoFuncSnapBshutterEnd,
			.snapLongExpBegin =    NULL,
			.snapLongExpFollow =   NULL,
			.snapLongExpEnd =      NULL,
			.snapShortExp =        sensorDoFuncSnapShortExp,
			.snapExpEnd =          sensorDoFuncSnapExpEnd,
			.psnapDummy =          NULL,
			.psnapReadoutField = (sensorDoFunc_t []) {
				sensorDoFuncSnapReadoutField0,
			},
			.snapFieldEnd =        sensorDoFuncSnapFieldEnd,
			.snapReadoutEnd =      sensorDoFuncSnapReadoutEnd,
			.snapNext = 		   sensorDoFuncSnapNext,
		},
        .crop = NULL,
 		.hsize =                   CAPTURE_HSIZE,
		.vsize =				   CAPTURE_VSIZE,
		.sensor_model =			   SENSORMODEL,/*TYPE_HSCMOS,*/
		.model_info =			   SENSORMODEL_MIPI_RAW,
		.cfa_pattern =			   SNAP_CFAPATTERN,
		.n_dummy =				   SNAP_DUMMYVD_NUM,
		.n_readout =			   SNAP_READOUTVD_NUM,
		.n_capvd =				   SNAP_CAPTUREVD_NUM,
		.n_snap_early =			   SNAP_EARLY_NUM,
		.skip_snap_readout_vd =	   SKIP_SNAP_READOUTVD_EN,
		.fldsel_start =			   FLDSEL_START_NUM,
		.fldsel_end =			   FLDSEL_END_NUM,
		.hratio = 1600, /*16:9 sensor*/
		.vratio = 900,
		.pix_100khz =(UINT16)CAPTURE_CDSPCLK,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview1080P_30fps_Cfg = {
	.linetotal       = PREVIEW_1080P_30FPS_LINETOTAL,
	.frametotal      = PREVIEW_1080P_30FPS_FRAMETOTAL,
	.hoffset         = PREVIEW_1080P_30FPS_HOFFSET,
	.voffset         = PREVIEW_1080P_30FPS_VOFFSET,
	.hsize           = PREVIEW_1080P_30FPS_HSIZE,
	.vsize           = PREVIEW_1080P_30FPS_VSIZE,
	.lastlinepix     = PREVIEW_1080P_30FPS_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1080P_30FPS_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_1080P_30FPS_PCLKNX,
	.pclk1x 		 = PREVIEW_1080P_30FPS_PCLK1X,
	.ext_hdnum		 = PREVIEW_1080P_30FPS_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview1080P_60fps_Cfg = {
	.linetotal       = PREVIEW_1080P_60FPS_LINETOTAL,
	.frametotal      = PREVIEW_1080P_60FPS_FRAMETOTAL,
	.hoffset         = PREVIEW_1080P_60FPS_HOFFSET,
	.voffset         = PREVIEW_1080P_60FPS_VOFFSET,
	.hsize           = PREVIEW_1080P_60FPS_HSIZE,
	.vsize           = PREVIEW_1080P_60FPS_VSIZE,
	.lastlinepix     = PREVIEW_1080P_60FPS_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1080P_30FPS_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_1080P_60FPS_PCLKNX,
	.pclk1x 		 = PREVIEW_1080P_60FPS_PCLK1X,
	.ext_hdnum		 = PREVIEW_1080P_60FPS_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview1080P30_Linear_Cfg = {
	.linetotal       = PREVIEW_1080P30_LINEAR_LINETOTAL,
	.frametotal      = PREVIEW_1080P30_LINEAR_FRAMETOTAL,
	.hoffset         = PREVIEW_1080P30_LINEAR_HOFFSET,
	.voffset         = PREVIEW_1080P30_LINEAR_VOFFSET,
	.hsize           = PREVIEW_1080P30_LINEAR_HSIZE,
	.vsize           = PREVIEW_1080P30_LINEAR_VSIZE,
	.lastlinepix     = PREVIEW_1080P30_LINEAR_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1080P_30FPS_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_1080P30_LINEAR_PCLKNX,
	.pclk1x 		 = PREVIEW_1080P30_LINEAR_PCLK1X,
	.ext_hdnum		 = PREVIEW_1080P30_LINEAR_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview1080P60_Linear_Cfg = {
	.linetotal       = PREVIEW_1080P60_LINEAR_LINETOTAL,
	.frametotal      = PREVIEW_1080P60_LINEAR_FRAMETOTAL,
	.hoffset         = PREVIEW_1080P60_LINEAR_HOFFSET,
	.voffset         = PREVIEW_1080P60_LINEAR_VOFFSET,
	.hsize           = PREVIEW_1080P60_LINEAR_HSIZE,
	.vsize           = PREVIEW_1080P60_LINEAR_VSIZE,
	.lastlinepix     = PREVIEW_1080P60_LINEAR_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1080P_30FPS_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_1080P60_LINEAR_PCLKNX,
	.pclk1x 		 = PREVIEW_1080P60_LINEAR_PCLK1X,
	.ext_hdnum		 = PREVIEW_1080P60_LINEAR_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview2XBin30_Linear_Cfg = {
	.linetotal       = PREVIEW_2XBIN_60FPS_LINEAR_LINETOTAL,
	.frametotal      = PREVIEW_2XBIN_60FPS_LINEAR_FRAMETOTAL,
	.hoffset         = PREVIEW_2XBIN_60FPS_LINEAR_HOFFSET,
	.voffset         = PREVIEW_2XBIN_60FPS_LINEAR_VOFFSET,
	.hsize           = PREVIEW_2XBIN_60FPS_LINEAR_HSIZE,
	.vsize           = PREVIEW_2XBIN_60FPS_LINEAR_VSIZE,
	.lastlinepix     = PREVIEW_2XBIN_60FPS_LINEAR_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1080P_30FPS_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_2XBIN_60FPS_LINEAR_PCLKNX,
	.pclk1x 		 = PREVIEW_2XBIN_60FPS_LINEAR_PCLK1X,
	.ext_hdnum		 = PREVIEW_2XBIN_60FPS_LINEAR_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewQuarter_Linear_Cfg = {
	.linetotal       = PREVIEW_QUARTER_LINEAR_LINETOTAL,
	.frametotal      = PREVIEW_QUARTER_LINEAR_FRAMETOTAL,
	.hoffset         = PREVIEW_QUARTER_LINEAR_HOFFSET,
	.voffset         = PREVIEW_QUARTER_LINEAR_VOFFSET,
	.hsize           = PREVIEW_QUARTER_LINEAR_HSIZE,
	.vsize           = PREVIEW_QUARTER_LINEAR_VSIZE,
	.lastlinepix     = PREVIEW_QUARTER_LINEAR_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1080P_30FPS_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_QUARTER_LINEAR_PCLKNX,
	.pclk1x 		 = PREVIEW_QUARTER_LINEAR_PCLK1X,
	.ext_hdnum		 = PREVIEW_QUARTER_LINEAR_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview720P90_Linear_Cfg = {
	.linetotal       = PREVIEW_720P90_LINEAR_LINETOTAL,
	.frametotal      = PREVIEW_720P90_LINEAR_FRAMETOTAL,
	.hoffset         = PREVIEW_720P90_LINEAR_HOFFSET,
	.voffset         = PREVIEW_720P90_LINEAR_VOFFSET,
	.hsize           = PREVIEW_720P90_LINEAR_HSIZE,
	.vsize           = PREVIEW_720P90_LINEAR_VSIZE,
	.lastlinepix     = PREVIEW_720P90_LINEAR_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1080P_30FPS_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_720P90_LINEAR_PCLKNX,
	.pclk1x 		 = PREVIEW_720P90_LINEAR_PCLK1X,
	.ext_hdnum		 = PREVIEW_720P90_LINEAR_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static  sensorSyncSigCfg_t sensorSyncSigSnapCfg = {
	.linetotal       = CAPTURE_LINETOTAL,
	.frametotal      = CAPTURE_FRAMETOTAL,
	.hoffset         = CAPTURE_HOFFSET,
	.voffset         = CAPTURE_VOFFSET,
	.hsize           = CAPTURE_HSIZE,
	.vsize           = CAPTURE_VSIZE,
	.lastlinepix     = CAPTURE_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
	.fpclk			 = CAPTURE_PCLKNX,
	.pclk1x 		 = CAPTURE_PCLK1X,
	.ext_hdnum		 = CAPTURE_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static  sensorSyncSigCfg_t sensorSyncSigSnapHDRCfg = {
	.linetotal       = CAPTURE_LINETOTAL,
	.frametotal      = CAPTURE_FRAMETOTAL,
	.hoffset         = CAPTURE_HOFFSET,
	.voffset         = CAPTURE_VOFFSET,
	.hsize           = CAPTURE_HSIZE,
	.vsize           = CAPTURE_VSIZE,
	.lastlinepix     = CAPTURE_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
	.fpclk			 = CAPTURE_PCLKNX,
	.pclk1x 		 = CAPTURE_PCLK1X,
	.ext_hdnum		 = CAPTURE_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

typedef struct {
	const sensorTgPreviewCfg_t *pstpPrev;
	const sensorSyncSigCfg_t *ssPrevCfg;
	const UINT8 *pCmdSet;
	const UINT16 CmdLen;
	/*Removed:struct member 'sensorPrevModeSet_t::ucModeInfo' is never used.*/
	/*const UINT8  ucModeInfo;*/ /*Bit 0:  binning mode =>1,  non binning mode =>0*/
						 	 /*Bit 1:  HDR mode =>1,  HDR mode =>0*/
} sensorPrevModeSet_t;

typedef struct {
	const sensorTgSnapCfg_t *pstpSnap;
	const sensorSyncSigCfg_t *ssSnapCfg;
	const UINT8 *pCmdSet;
	const UINT16 CmdLen;
} sensorSnapModeSet_t;

static sensorPrevModeSet_t sensorPrevModeTbl[]=
{
	[0x00]={&sensorTgPreview1080P_30fps_Cfg		,&sensorSyncSigPreview1080P_30fps_Cfg		,cmd_1080P_30fps_code[0] 			,CMD_LEN(cmd_1080P_30fps_code)},
	[0x01]={&sensorTgPreview1080P_60fps_Cfg		,&sensorSyncSigPreview1080P_60fps_Cfg		,cmd_1080P_60fps_code[0] 			,CMD_LEN(cmd_1080P_60fps_code)},
	[0x02]={&sensorTgPreview1080P30_Linear_Cfg		,&sensorSyncSigPreview1080P30_Linear_Cfg	,cmd_1080P30_Linear_code[0] 			,CMD_LEN(cmd_1080P30_Linear_code)},
	[0x03]={&sensorTgPreview1080P60_Linear_Cfg		,&sensorSyncSigPreview1080P60_Linear_Cfg	,cmd_1080P60_Linear_code[0] 			,CMD_LEN(cmd_1080P60_Linear_code)},
	[0x04]={&sensorTgPreview2XBin30_Linear_Cfg		,&sensorSyncSigPreview2XBin30_Linear_Cfg	,cmd_2XBin30_Linear_code[0] 			,CMD_LEN(cmd_2XBin30_Linear_code)},
	[0x05]={&sensorTgPreviewQuarter_Linear_Cfg		,&sensorSyncSigPreviewQuarter_Linear_Cfg	,cmd_Quarter_Linear_code[0] 			,CMD_LEN(cmd_Quarter_Linear_code)},	
	[0x06]={&sensorTgPreview720P90_Linear_Cfg		,&sensorSyncSigPreview720P90_Linear_Cfg	,cmd_720P90_Linear_code[0] 			,CMD_LEN(cmd_720P90_Linear_code)},
};

static sensorSnapModeSet_t sensorSnapModeTbl[]=
{
	[0x00]={&sensorTgSnapHDRRollCfg 	,&sensorSyncSigSnapHDRCfg	,cmd_1080P_30fps_code[0]		,CMD_LEN(cmd_1080P_30fps_code)},
	[0x01]={&sensorTgSnapRollCfg 		,&sensorSyncSigSnapCfg		,cmd_1080P30_Linear_code[0]		,CMD_LEN(cmd_1080P30_Linear_code)},
};

static sensorTgCfg_t sensorTgCfg = {
	.tg_type = TGTYPE,
	.n_preview_submode = 		sizeof(sensorPrevModeTbl) /
								sizeof(sensorPrevModeTbl[0]),
	.n_snap_submode = 			sizeof(sensorSnapModeTbl) /
								sizeof(sensorSnapModeTbl[0]),
	.vshutter_need_resend = 	VSHUTTER_RESEND,
	.exp_effect_frame_count = 	EXP_FRAME_NUM,
};

static  sensorTgDev_t sensorTgDev = {
	.tgCfg = &sensorTgCfg,
	.tgSvc = &(const sensorTgSvc_t) {
		.resload =		sensorCfgInit,
		.resstop =		sensorCfgStop,
		.init =			sensorDoFuncInit,
		.stop =			sensorDoFuncStop, /****TODO***/
		.pwrsave  =     sensorDoFuncPowerSave,
		.expwrite =		sensor_cmdport_exp_write,
		.expvtbldo =	sensor_cmdport_exp_vtbl_do,

		/*If using frmwrite function, disable expwriteRedo.*/
		.expwriteRedo=	NULL,
		.frmwrite= 		sensor_cmdport_frame_write,
		.cmdportBegin = NULL,/*sensor_cmdport_begin,*/
		.cmdportEnd =   NULL,/*sensor_cmdport_end,*/

		.crop =			NULL, /****TODO***/
	},

	.verStr =			MODEL_VERSION,
};

UINT32
sensorAR0230Read(UINT16 addr)
{
	UINT32 cmdportid;
	static UINT8 regread[4];
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;

	SENSOR_PROF_LOG_ADD("sensorAR0230Read++");
	sensorSerialBlockRead(cmdportid,1,regread,0);
	while((READ8(0xb0009105)&0x01)==0x01);/*sif1  CPU  trigger busy*/
	SENSOR_PROF_LOG_PRINT("READ [%x] = [%x]",addr,regread[2]);
	SENSOR_PROF_LOG_ADD("sensorAR0230Read--");
	return ((regread[2]<<8)|regread[3]);
}

void
sensorAR0230Write(UINT16 addr, UINT16 data)
{

	UINT32 cmdportid;

	cmdportid = sensorTgCfg.cmdportid_xtg;
	static UINT8 regdata1[4] ;
	regdata1[0]=  ( UINT8 )( addr>>8);
	regdata1[1]=  ( UINT8 )( addr & 0x000000ff );
	regdata1[2]=  ( UINT8 )( (data & 0x0000ff00)>>8 );
	regdata1[3]=  ( UINT8 )( data & 0x000000ff );
	sensorSerialDirectBufSnoop(regdata1, sizeof(regdata1));
	sensorSerialBulkWrite( cmdportid, 1, regdata1, 0);

}

void
sensorTg_ar0230_Install()
{
	static const sensorTgPreviewCfg_t		*sensorTgPreviewCfgTbl[sizeof(sensorPrevModeTbl)/sizeof(sensorPrevModeTbl[0])];
	static const sensorTgSnapCfg_t			*sensorTgSnapCfgTbl[sizeof(sensorSnapModeTbl)/sizeof(sensorSnapModeTbl[0])];
	UINT16 prev_num=sizeof(sensorPrevModeTbl)/sizeof(sensorPrevModeTbl[0]);
	UINT16 snap_num=sizeof(sensorSnapModeTbl)/sizeof(sensorSnapModeTbl[0]);
	UINT16 i;

	for(i=0;i<prev_num;i++)
		sensorTgPreviewCfgTbl[i]=sensorPrevModeTbl[i].pstpPrev;

	for(i=0;i<snap_num;i++)
		sensorTgSnapCfgTbl[i]=sensorSnapModeTbl[i].pstpSnap;

	sensorTgDev.previewCfg=sensorTgPreviewCfgTbl;
	sensorTgDev.snapCfg=sensorTgSnapCfgTbl;
	sensorTgDev.defSyncSigCfg=sensorPrevModeTbl[0].ssPrevCfg;/*default syncsig */

	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}

static void ar0230_PwrOn_Seq(UINT32 sel)
{
	UINT32 sensel=sel?SENSOR_DEV_POWER_ID_CMOS_SEC_SEL:0;

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_RST_ON);  /* RESET LOW then High */
    tmrUsWait(1000); /*1ms*/
#if 0
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_SYS_ON);
    tmrUsWait(1000); /*1ms*/
#endif

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_ON);  /*2.8v on */
    tmrUsWait(1000); /*1ms*/

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_IO_ON);   /*1.8v on */
    tmrUsWait(1000); /*1ms*/

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_ON); /*1.2v on */
    tmrUsWait(1000); /*1ms*/

	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_RST_OFF);  /* RESET LOW then High */
    tmrUsWait(2000); /*1ms*/

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_RST_ON);  /* RESET LOW then High */

}

static void ar0230_PwrOff_Seq(UINT32 sel)
{
	UINT32 sensel=sel?SENSOR_DEV_POWER_ID_CMOS_SEC_SEL:0;

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_RST_OFF);
    tmrUsWait(1000);
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_OFF);  /*2.8v off */
    tmrUsWait(1000);
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_IO_OFF);   /*1.8v off */
    tmrUsWait(1000);
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_OFF); /*1.2v off */
    tmrUsWait(1000);
}

static void AR0230_SensorInfoCfgSet(void)
{
	sensorInfoSet(SENSOR_MODE_PREVIEW,SENSOR_INFO_OB,AR0230_BLC_VAL,sizeof(sensorPrevModeTbl)/sizeof(sensorPrevModeTbl[0]),0/*opt*/);
	sensorInfoSet(SENSOR_MODE_SNAP,SENSOR_INFO_OB,AR0230_BLC_VAL,sizeof(sensorSnapModeTbl)/sizeof(sensorSnapModeTbl[0]),0/*opt*/);
	sensorInfoSet(SENSOR_MODE_PREVIEW,SENSOR_INFO_SENSITIVITY_STS,(UINT32)PrevSensitivityTbl,sizeof(PrevSensitivityTbl)/sizeof(PrevSensitivityTbl[0]),1/*opt*/);
	sensorInfoSet(SENSOR_MODE_SNAP,SENSOR_INFO_SENSITIVITY_STS,(UINT32)SnapSensitivityTbl,sizeof(SnapSensitivityTbl)/sizeof(SnapSensitivityTbl[0]),1/*opt*/);
	sensorInfoSet(SENSOR_MODE_PREVIEW,SENSOR_INFO_MIRROR_FLIP_STS,(UINT32)AR0230_MIRROR_FLIP_INFO,sizeof(sensorPrevModeTbl)/sizeof(sensorPrevModeTbl[0]),0);
	sensorInfoSet(SENSOR_MODE_SNAP,SENSOR_INFO_MIRROR_FLIP_STS,(UINT32)AR0230_MIRROR_FLIP_INFO,sizeof(sensorSnapModeTbl)/sizeof(sensorSnapModeTbl[0]),0);	 	
}

static void
sensorDoFuncInit()
{

	UINT32 cmdportid;

	SENSOR_PROF_LOG_ADD("AR0230 sensorDoFuncInit++ 6350 ");
	SENSOR_PROF_LOG_PRINT("________Sensor driver ver <<%s>>", MODEL_VERSION);
	SENSOR_PROF_LOG_PRINT("________Sensor driver build time <<%s>>", BUILD_TIME);
	SENSOR_PROF_LOG_ADD("sensorDoFuncInit()_ar0230_enter_....");
	sensorTgPllClkCfgSet(SENSOR_TGPLL_CFG_DRIVING,4);/*40uA*/
	sensorTgPllClkExtSet(27000000);/*27MHz*/

	/* Master clock and Pixel clock configuration here */
	/*****TODO*****front reset after these???*/
    ar0230_PwrOn_Seq(0);

#ifdef SEN_REMAP_0
	sensorModelApiCfgSet(SENSOR_API_PHY0_LANE_REMAP,SEN_REMAP_0,0xFFFFFFFF);
	profLogPrintf(0,"halFrontDataPinRemapCfg(%x)",SEN_REMAP_0);
#else
    profLogPrintf(0,"Use Default Pin ReMap....");
	sensorModelApiCfgSet(SENSOR_API_PHY0_LANE_REMAP,0xFFFFBA10,0xFFFFFFFF);
#endif

	cmdportid = sensorTgCfg.cmdportid_xtg;
	tmrUsWait(10000);
	printf("sensor DoFunInit model-ID =\n ");
	UINT16 id;
	UINT16 i;
	id=sensorAR0230Read(0x3000); /*sensor ID-MSB */
	printf("sensor id =0x%x\n",id);
	for(i=0;i<3;i++)
	{
		if(id==0x1056)
			continue;
		else
		{
			id=sensorAR0230Read(0x3000); /*sensor ID-MSB */
			printf("retry read sensor ID\n");
			printf("sensor id =0x%x\n",id);
		}
	}
	tmrUsWait(10000);

	/*set gain valid time delay 1vd*/
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_reg_reset_code)/sizeof(cmd_reg_reset_code[0]), cmd_reg_reset_code[0],0);
	tmrUsWait(2000);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_pll3_code)/sizeof(cmd_pll3_code[0]), cmd_pll3_code[0],0);
	tmrUsWait(5000);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_init_code_general)/sizeof(cmd_init_code_general[0]), cmd_init_code_general[0],0);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_init_code_2)/sizeof(cmd_init_code_2[0]), cmd_init_code_2[0],0);

	sensorSerialBulkWrite(cmdportid,sizeof(cmd_hdr_code)/sizeof(cmd_hdr_code[0]), cmd_hdr_code[0],0);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_hdr_ratio_16x_code)/sizeof(cmd_hdr_ratio_16x_code[0]), cmd_hdr_ratio_16x_code[0],0);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_init_code_3)/sizeof(cmd_init_code_3[0]), cmd_init_code_3[0],0);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_altm_enable_code)/sizeof(cmd_altm_enable_code[0]), cmd_altm_enable_code[0],0);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_altm_fw_enable_code)/sizeof(cmd_altm_fw_enable_code[0]), cmd_altm_fw_enable_code[0],0);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_2dmc_on_code)/sizeof(cmd_2dmc_on_code[0]), cmd_2dmc_on_code[0],0);

	if( ae_preview_agc_get() < 52 )/*HCG thr*/
	{
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_lowgaininit_code)/sizeof(cmd_lowgaininit_code[0]), cmd_lowgaininit_code[0],0);
	}
	else
	{
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_highgaininit_code)/sizeof(cmd_highgaininit_code[0]), cmd_highgaininit_code[0],0);
	}
	/*sensorSerialBulkWrite(cmdportid,sizeof(cmd_init_code_5)/sizeof(cmd_init_code_5[0]), cmd_init_code_5[0],0);*/
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_DG_before_DLO)/sizeof(cmd_DG_before_DLO[0]), cmd_DG_before_DLO[0],0);

	sensorSerialBulkWrite(cmdportid,sizeof(cmd_hispi_set_code)/sizeof(cmd_hispi_set_code[0]), cmd_hispi_set_code[0],0);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_1080P_30fps_code)/sizeof(cmd_1080P_30fps_code[0]), cmd_1080P_30fps_code[0],0);

	tmrUsWait(5000);/*30ms*/
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_stream_on_code)/sizeof(cmd_stream_on_code[0]), cmd_stream_on_code[0],0);
	
	hdrEnable = 1;

	#if 1
    /*TG PLL=MCLK out=24MHz*/
    sensorLVDSCfgSet(&sensorLVDSCfg_VGA);
	#endif

    sensorSyncSigInit(sensorTgDev.defSyncSigCfg);

	/*halFrontLvdsSet(1);*/
	tmrUsWait(30000);/*30ms*/

	WRITE8(0xb000980d,READ8(0xb000980d)|0x4); /*WorkAround, Enable the SOF carry an image line selection.*/

	SENSOR_PROF_LOG_ADD("sensorDoFuncInit()___done");
	SENSOR_PROF_LOG_ADD("AR0230 sensorDoFuncInit--");
	printf("sensorDoFuncInit()___done \n");

}

static void
sensorDoFuncStop()
{
#if SPCA6350
    sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_OFF); /*1.2v on */

    tmrUsWait(5000); /*1ms*/

    sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_ON);  /*2.8v on */
    tmrUsWait(5000); /*1ms*/

    sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_CMOS_PWR_IO_ON);   /*1.8v on */

    tmrUsWait(5000); /*1ms*/

#else
    sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SEN_OFF);
#endif
}

static void
sensorDoFuncSensorMIPIClkLaneChk()
{
	UINT8 ucTmrCnt;

	ucTmrCnt = 0;
	while (1)
	{
		if (((READ8(0xb00098b8)&0x1) == 1)||(ucTmrCnt == 100))/*Check MIPI clock lane is in low power state or not.*/
		{
			if (ucTmrCnt == 100)
			{
			  	SENSOR_PROF_LOG_ADD("#@# MIPI Clk lane Check timeout!");
				  printf("@#@ Error MIPI Clk lane LP state check timeout!");
			}

			break;
		}

		tmrUsWait(5000);
		ucTmrCnt++;

	  	SENSOR_PROF_LOG_PRINT("#@# MIPI Clk lane Check![%d]",ucTmrCnt);
	}

  	SENSOR_PROF_LOG_ADD("#@# MIPI Clk lane Check Done!");
}

static void
ar0230_sensor_set(
	UINT8 hdrModeEn
)
{
	if( hdrModeEn )
	{
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_hdr_code)/sizeof(cmd_hdr_code[0]), cmd_hdr_code[0],0);
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_hdr_ratio_16x_code)/sizeof(cmd_hdr_ratio_16x_code[0]), cmd_hdr_ratio_16x_code[0],0);
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_init_code_3)/sizeof(cmd_init_code_3[0]), cmd_init_code_3[0],0);
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_altm_enable_code)/sizeof(cmd_altm_enable_code[0]), cmd_altm_enable_code[0],0);
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_altm_fw_enable_code)/sizeof(cmd_altm_fw_enable_code[0]), cmd_altm_fw_enable_code[0],0);		
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_2dmc_on_code)/sizeof(cmd_2dmc_on_code[0]), cmd_2dmc_on_code[0],0);
		/*sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_hdr_lowdcg_code)/sizeof(cmd_hdr_lowdcg_code[0]), cmd_hdr_lowdcg_code[0],0);*/
	}
	else
	{
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_linear_code)/sizeof(cmd_linear_code[0]), cmd_linear_code[0],0);
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_init_code_4)/sizeof(cmd_init_code_4[0]), cmd_init_code_4[0],0);
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_altm_bypass_code)/sizeof(cmd_altm_bypass_code[0]), cmd_altm_bypass_code[0],0);
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_2dmc_off_code)/sizeof(cmd_2dmc_off_code[0]), cmd_2dmc_off_code[0],0);
		/*sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_linear_lowdcg_code)/sizeof(cmd_linear_lowdcg_code[0]), cmd_linear_lowdcg_code[0],0);*/
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_adacd_disable_code)/sizeof(cmd_adacd_disable_code[0]), cmd_adacd_disable_code[0],0);
		sensorSerialBulkWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_companding_disable_code)/sizeof(cmd_companding_disable_code[0]), cmd_companding_disable_code[0],0);
	}
}

UINT8
ar0230_sensor_hdr_status_get(
	void
)
{
	return hdrEnable;
}

static void
sensorDoFuncPreviewBegin()
{
	UINT32 cmdportid, smode;
	UINT8 hdrStateChanged = 0;
	/*static UINT8 ucIsFormerHDRmode=0;*/
  	SENSOR_PROF_LOG_ADD("preview begin");

	cmdportid = sensorTgCfg.cmdportid_xtg;
	/*sensorEventVvalidWait(1);*/
  	SENSOR_PROF_LOG_ADD("preview cmd done wait done..");
  	smode=sensorModeGet();

	SENSOR_PROF_LOG_PRINT("mode %x i2c send +++..",smode);

	if( smode >= 0x32 )
	{
		if( hdrEnable == 1 )
		{
			hdrStateChanged = 1;
		}
		hdrEnable = 0;
	}
	else
	{
		if( hdrEnable == 0 )
		{
			hdrStateChanged = 1;
		}
		hdrEnable = 1;
	}
	sensorEventVvalidWait(1);

	sensorSerialBulkWrite(cmdportid,sizeof(cmd_stream_off_code)/sizeof(cmd_stream_off_code[0]),cmd_stream_off_code[0],0);

	ae_preview_evset_write();

	if (smode==0x30 || smode==0x32)
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_pll3_code)/sizeof(cmd_pll3_code[0]), cmd_pll3_code[0],0);
	else
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_pll1_code)/sizeof(cmd_pll1_code[0]), cmd_pll1_code[0],0);	

	if( hdrStateChanged )
	{
		ar0230_sensor_set(hdrEnable);
	}
	sensorSerialFlush(cmdportid);
	sensorSerialBulkWrite(cmdportid,sensorPrevModeTbl[smode&0xF].CmdLen, sensorPrevModeTbl[smode&0xF].pCmdSet,0);
	sensorSyncSigInit(sensorPrevModeTbl[smode&0xF].ssPrevCfg);
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_stream_on_code)/sizeof(cmd_stream_on_code[0]),cmd_stream_on_code[0],0);
	WRITE8(0xb000980d,READ8(0xb000980d)|0x4); /*WorkAround, Enable the SOF carry an image line selection.*/

	SENSOR_PROF_LOG_PRINT("mode %x i2c send ---..",smode);

}

static void
sensorDoFuncPreviewShortExp()
{
}

static void
sensorDoFuncPreviewExpEnd()
{
}
static void
sensorDoFuncSnapExpBegin()
{
	UINT32 cmdportid, smode;
	UINT8 hdrStateChanged = 0;	
    SENSOR_PROF_LOG_ADD("SnapExpBegin s");
	cmdportid = sensorTgCfg.cmdportid_xtg;
	smode=sensorModeGet();
	profLogInit(2,0x8000);

	sensorSerialFlush(cmdportid);
	sensorSerialBulkWrite(cmdportid,1, cmd_stream_off_code[0],0); /*Stream off must be sent 1stly to make sensor could stop stream after vvalid falling.*/
	sensorDoFuncSensorMIPIClkLaneChk();/*Use this function to check the MIPI clock lane is in low power state or not.  Avoid using vvalid falling wait. */

	SENSOR_PROF_LOG_PRINT("mode %x i2c send +++..",smode);

	if( smode == 0x40 )
	{
		if( hdrEnable == 0 )
			hdrStateChanged = 1;
		
		hdrEnable = 1;
	}
	else
	{
		if( hdrEnable == 1 )
			hdrStateChanged = 1;

		hdrEnable = 0;
	}
	sensorSerialBulkWrite(cmdportid,sizeof(cmd_pll3_code)/sizeof(cmd_pll3_code[0]), cmd_pll3_code[0],0);	

	if( hdrStateChanged )
	{
		ar0230_sensor_set(hdrEnable);
	}

	sensorSerialBulkWrite(cmdportid,sensorSnapModeTbl[smode&0xF].CmdLen, sensorSnapModeTbl[smode&0xF].pCmdSet,0);
	sensorSyncSigInit(sensorSnapModeTbl[smode&0xF].ssSnapCfg);
	sensorSerialBulkWrite(cmdportid,1, cmd_stream_on_code[0],0); /*Stream off must be sent 1stly to make sensor could stop stream after vvalid falling.*/
	snap2pv = 1;
	
    SENSOR_PROF_LOG_ADD("SnapExpBegin e");

}

static void
sensorDoFuncSnapBshutterBegin()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapBshutterBegin()");
}

static void
sensorDoFuncSnapBshutterFollow()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapBshutterFollow()");
}

static void
sensorDoFuncSnapBshutterEnd()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapBshutterEnd()");
}

static void
sensorDoFuncSnapShortExp()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapShortExp()");
}

static void
sensorDoFuncSnapExpEnd()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapExpEnd()");
}

static void
sensorDoFuncSnapReadoutField0()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapReadoutField0()");
}

static void
sensorDoFuncSnapFieldEnd()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapFieldEnd()");
}

static void
sensorDoFuncSnapReadoutEnd()
{
	SENSOR_PROF_LOG_ADD("sensorDoFuncSnapReadoutEnd()");
}

static void
sensorDoFuncSnapNext(UINT32 mode)
{
	if((mode &0xf0)==0x30)
	    sensorEventVdWait(1);
}

#ifdef EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
DECLARE_EXPTBL();
#endif

static void
sensorCfgInit()
{

	sensorTgCfg_t *ptcfg = &sensorTgCfg;
	/*xtg registers for separate serial port*/
	static const sensorCmdport_t sensor_cmdport_xtg_cfg = {
		.mode         = TG_SPI_I2C_MODE,
		.format2      = TG_FORMAT2,
		.format3      = TG_FORMAT3,
		.speed        = TG_SPEED,
		.nbit_regdata = TG_NBIT_REGDATA,
		.nbit_regaddr = TG_NBIT_REGADDR,
	};
	ptcfg->cmdportid_exp =
	ptcfg->cmdportid_xtg = sensorSerialCreate(&sensor_cmdport_xtg_cfg);

	/* if support burst
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_BURST; */
	/* if support seq
	 * ptcfg->cmdportid_exp = ptcfg->cmdportid_xtg |
	 * SENSOR_CMDPORTID_OP_SEQ; */
	sensor_bshut_init();  /*bshut must init before exptable*/
#ifdef EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
		sensor_exptbl_create(exptbl_stream, sizeof(exptbl_stream));
#else
		sensor_exptbl_init();
#endif

	AR0230_SensorInfoCfgSet();

}

static void
sensorCfgStop()
{
	sensorTgCfg_t *ptcfg = &sensorTgCfg;
	/*call this if this sensor driver supports exp table*/
	sensor_exptbl_delete();
	/*xtg registers for separate serial port*/
	sensorSerialDelete(ptcfg->cmdportid_xtg);
	sensorLVDSEnSet(0);
	/*Disable mipi interface*/
}

static void
sensor_cmdport_frame_write(UINT32 smode,UINT32 kfps_max,UINT32 kfps_min,UINT32 nreg,const UINT8 *pdata)
{
	UINT32 frm_org_length,frm_org_kfps ;
	UINT32 frm_new_length;
	static UINT32 udwFrmLegthRec;
	static UINT8 regData[][4] SENSOR_SIF1DMA_VARBUF = {
		{0x30,0x0A,0x00,0x00},
	};

	/*profLogPrintf(3,"[MODE 0x%x FPS MAX=%d FPS MIN=%d NREG=%d]",smode,kfps_max,kfps_min,nreg);*/

	smode = sensorModeGet();

	/*UINT32 i;*/
	/*
	for(i=0;i<nreg;i++)
		profLogPrintf(3,"[%x %x %x]",pdata[i*3+0],pdata[i*3+1],pdata[i*3+2]);
	*/
	if ((smode&0x30)==0x30)
	{
		frm_org_length = sensorPrevModeTbl[smode&0xF].ssPrevCfg->frametotal;
		switch (smode)
		{
			default:
			case 0x30:
			case 0x32:				
				frm_org_kfps = PREVIEW_1080P_30FPS_FRAMERATE_KFPS;
				break;
				
			case 0x31:
			case 0x33:
			case 0x34:				
				frm_org_kfps = PREVIEW_1080P_60FPS_FRAMERATE_KFPS;
				break;	
			case 0x35:
				frm_org_kfps = PREVIEW_QUARTER_LINEAR_FRAMERATE_KFPS;
				break;
			case 0x36:
				frm_org_kfps = PREVIEW_720P90_LINEAR_FRAMERATE_KFPS;
				break;				
		}
	}
	else /* smode >= 0x40*/
	{

		frm_org_length = sensorSnapModeTbl[smode&0xF].ssSnapCfg->frametotal;
		switch (smode)
		{
			default:
			case 0x40:
				frm_org_kfps = CAPTURE_FRAMERATE_KFPS;
				break;
		}

		/*profLogPrintf(3,"@#@ 0x40, frm_org_length=%d,frm_org_kfps=%d",frm_org_length,frm_org_kfps);*/
	}

	/*profLogPrintf(3,"@#@frm_org_kfps=%d",frm_org_kfps);*/

	if(!kfps_max)
	{
		frm_new_length = frm_org_length;
		if (frm_org_length != udwFrmLegthRec)
		{			
			regData[0][2] = (frm_new_length>>8)&0xff;
			regData[0][3] = (frm_new_length)&0xff;
			sensorSerialDirectBufSnoop(regData[0], sizeof(regData));
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,sizeof(regData)/sizeof(regData[0]),regData[0]);
			/*profLogPrintf(3,"[OrgFrmlength=%d]",frm_org_length);	*/
		}
		
		goto End;
	}

	if(kfps_max<frm_org_kfps)
	{
		frm_new_length = frm_org_kfps*frm_org_length/kfps_max;

		/*profLogPrintf(3,"@#@ frm_org=%d,kfps_org=%d,frm_new=%d",frm_org_length,frm_org_kfps,frm_new_length);*/

		regData[0][2] = (frm_new_length>>8)&0xff;
		regData[0][3] = (frm_new_length)&0xff;
		sensorSerialDirectBufSnoop(regData[0], sizeof(regData));
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,sizeof(regData)/sizeof(regData[0]),regData[0]);
	}
	else /*kfps_max >=frm_org_kfps*/ /* setting maximum frame rate	> original maximum frame rate,	use the orignal frame length.*/
	{
		frm_new_length = frm_org_length;
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,nreg,pdata);
		/*profLogPrintf(3,"#$# Over original maximum, use original maximum frame rate!");*/
	}
	
	End:
		udwFrmLegthRec = frm_new_length;
}

#if 0
static void
sensor_pregain_delay_decision(UINT32 nreg2, const UINT8 *pdata2)
{
	static UINT32 frmlinecnt_record = 0;
	UINT32 frmlinecnt = 0;

	frmlinecnt =  pdata2[11]+(pdata2[14]<<8);
	if(frmlinecnt_record == 0){
		frmlinecnt_record = frmlinecnt;
	}

	if(frmlinecnt!=frmlinecnt_record){
		sensor_PregainDelay(0);
	}
	else{
		sensor_PregainDelay(1);
	}

	frmlinecnt_record = frmlinecnt;
}

static void
sensor_cmdport_begin(UINT32 smode,UINT32 expIdx,UINT32 agcIdx,UINT32 exp2Idx,UINT32 agc2Idx)
{
#if 0
	/*group hold*/
	static UINT8 SENSOR_SIF1DMA_VARBUF regdata1[3] ;
	regdata1[0]=  0x32;
	regdata1[1]=  0x08;
	regdata1[2]=  0x00;
	sensorSerialDirectBufSnoop(regdata1, sizeof(regdata1));
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regdata1);
#endif
}

extern UINT8  preGainDelay;
static void
sensor_cmdport_end(void)
{
#if 0
	/*group end and launch */
	/*if(preGainDelay || sensorModeGet()==0x41 || sensorModeGet()==0x35 ||	sensorModeGet()==0x39 || sensorModeGet()==0x3A)*/
	if(preGainDelay || sensorModeGet()==0x3A)
	{
		static UINT8 SENSOR_SIF1DMA_VARBUF regdata1[2][3]={
			{0x32,0x08,0x10},
			{0x32,0x08,0xA0},
		};
		sensorSerialDirectBufSnoop(regdata1[0], sizeof(regdata1));
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 2, regdata1[0]);
		/*profLogPrintf(3,"@#@ No Timing reset!!!");*/
	}
	else
	{
		static UINT8 SENSOR_SIF1DMA_VARBUF regdata1[4][3]={
			{0x38,0x23,0x01},/*timming re-set on*/
			{0x38,0x23,0x00},/*timming re-set off*/
			{0x32,0x08,0x10},
			{0x32,0x08,0xA0},
		};
		sensorSerialDirectBufSnoop(regdata1[0], sizeof(regdata1));
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 4, regdata1[0]);
		/*profLogPrintf(3,"@#@ Timing reset!!!");*/
	}
#endif
}
#endif

static void
sensor_minimum_framelength_get(UINT8 sensorMode, UINT16 *minFrmLength, UINT16 *maxExpLine)
{
	switch( sensorMode )
	{
		case 0x31:
			*minFrmLength = PREVIEW_1080P_60FPS_FRAMETOTAL;
			*maxExpLine = PREVIEW_1080P_60FPS_SENSOR_EXPLINE;
			break;
		case 0x32:
			*minFrmLength = PREVIEW_1080P30_LINEAR_FRAMETOTAL;
			*maxExpLine = PREVIEW_1080P30_LINEAR_SENSOR_EXPLINE;
			break;
		case 0x33:
			*minFrmLength = PREVIEW_1080P60_LINEAR_FRAMETOTAL;
			*maxExpLine = PREVIEW_1080P60_LINEAR_SENSOR_EXPLINE;
			break;
		case 0x34:
			*minFrmLength = PREVIEW_2XBIN_60FPS_LINEAR_FRAMETOTAL;
			*maxExpLine = PREVIEW_2XBIN_60FPS_LINEAR_SENSOR_EXPLINE;
			break;			
		case 0x35:
			*minFrmLength = PREVIEW_QUARTER_LINEAR_FRAMETOTAL;
			*maxExpLine = PREVIEW_QUARTER_LINEAR_SENSOR_EXPLINE;
			break;		
		case 0x36:
			*minFrmLength = PREVIEW_720P90_LINEAR_FRAMETOTAL;
			*maxExpLine = PREVIEW_720P90_LINEAR_SENSOR_EXPLINE;
			break;				
		default:
		case 0x30:
			*minFrmLength = PREVIEW_1080P_30FPS_FRAMETOTAL;
			*maxExpLine = PREVIEW_1080P_30FPS_SENSOR_EXPLINE;
			break;
	}
}

static void
ar0230_altm_2DMC_update(void)
{
	UINT32 lvtmp,lvreal;
	static UINT8 curLv = 0; /* 0:fast, 1:middle, 2:slow */

	appAWBALG_WBParamGet(AWB_PARA_LV,&lvtmp);

	lvreal = lvtmp/8;

	if( lvreal <= 8 )
	{
		/*curLv = 0;*/
		if( curLv != 0 )
		{
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_altm_2DMC_slow)/sizeof(cmd_altm_2DMC_slow[0]), cmd_altm_2DMC_slow[0]);
			sensorSerialFlush(sensorTgCfg.cmdportid_exp);
			curLv = 0;
		}
	}
	else if( lvreal>=12 )
	{
		/*curLv = 2;*/
		if( curLv != 2 )
		{
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_altm_2DMC_fast)/sizeof(cmd_altm_2DMC_fast[0]), cmd_altm_2DMC_fast[0]);
			sensorSerialFlush(sensorTgCfg.cmdportid_exp);
			curLv = 2;
		}
	}
	else /*if((lvreal >= 11)&&(lvreal<9))*/
	{
		/*curLv = 1;*/
		if( curLv != 1 )
		{
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_xtg,sizeof(cmd_altm_2DMC_middle)/sizeof(cmd_altm_2DMC_middle[0]), cmd_altm_2DMC_middle[0]);
			sensorSerialFlush(sensorTgCfg.cmdportid_exp);
			curLv = 1;
		}
	}
}

static void
sensor_cmdport_exp_write(UINT32 nreg, const UINT8 *pdata)
{
	UINT16 minFrmLength, maxExpLine, frmLength, expLine;
	UINT8 smode;

  	smode = (UINT8)(sensorModeGet());

	sensor_minimum_framelength_get( smode, &minFrmLength, &maxExpLine );

	static UINT8 SENSOR_SIF1DMA_VARBUF regdata1[2][4]={
		{0x30,0x0A,0x00, 0x00},
		{0x30,0x12,0x00, 0x00},
	};

	expLine = (pdata[2]<<8)+pdata[3];

	if( expLine > maxExpLine )
	{
		frmLength = minFrmLength + (expLine-maxExpLine);
	}
	else
	{
		frmLength = minFrmLength;
	}	

	regdata1[0][2] = (frmLength&0xff00)>>8;
	regdata1[0][3] = (frmLength&0x00ff);
	regdata1[1][2] = (expLine&0xff00)>>8;
	regdata1[1][3] = (expLine&0x00ff);

	sensorSerialDirectBufSnoop(regdata1[0], sizeof(regdata1));
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 2, (const UINT8*)regdata1);
	sensorSerialFlush(sensorTgCfg.cmdportid_exp);

	ar0230_altm_2DMC_update();

}

static void
sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type, UINT32 vcount)
{
}

void ar0230_sensorCmdPortExpLineSend(UINT16 expLine)
{
#if 0
	static UINT8 regData[][3] SENSOR_SIF1DMA_VARBUF ={
		{0x35,0x00,0x00},
		{0x35,0x01,0x00},
		{0x35,0x02,0x00},

	};
	regData[0][2] = (expLine>>12)&0x0f;
	regData[1][2] = (expLine>>4)&0xff;
	regData[2][2] = ((expLine)&0x0f)<<4;


	SENSOR_PROF_LOG_PRINT("expLine(reg0) = %d [%x %x %x]",expLine,regData[0][0],regData[0][1],regData[0][2]);
	SENSOR_PROF_LOG_PRINT("expLine(reg1) = %d [%x %x %x]",expLine,regData[1][0],regData[1][1],regData[1][2]);
	SENSOR_PROF_LOG_PRINT("expLine(reg2) = %d [%x %x %x]",expLine,regData[2][0],regData[2][1],regData[2][2]);
	sensorSerialDirectBufSnoop(regData[0], sizeof(regData));
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,3, regData[0]);
	sensorSerialFlush(sensorTgCfg.cmdportid_exp);
	sensorEventNewFrameWait(1);
	expLine = expLine;
#endif
}

#if SP5K_SENSOR_AR0230
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	ar0230_sensorCmdPortExpLineSend(expLine);
}
#endif

static void
sensorDoFuncPowerSave(UINT32 senmode)
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;

    if(senmode==SENSOR_MODE_POWERSAVE){
        /* Set sensor to stream off mode (EVB has NOT Power IC) */
        /* turn off MCK, XHS, XVS */
        sensorTgPllClkCtrl(SENSOR_TGPLL_CTRL_EN,0);      /*disable tgpll*/
        halFrontSyncSigCfgSet(FRONT_SYNC_SIGGEN_EN,0,0); /*sigen disable*/
        ar0230_PwrOff_Seq(0);
    }else{
        sensorSerialFlush(cmdportid);    /*avoid ae not flush*/
        sensorDoFuncInit();
    }
}

