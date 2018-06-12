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
 *                                                                        
 *  Author: Ricky Lin
 *                                                                        
 *  Version: 0.01, 2014.4.11:

 | sensor size				   | sensor info						 | mode    |	Data rate(Mbps)|   Frame Rate |Pclknx(Mpixel/lane)|

 | Preview mode

 | 1632 x 1224 @ 30fps		   | bin2-	 UXGA@30FPS					 | 0x30    |  (  330 Mbps)	   |	30.00	   | 33 		|
 |	816 x  612 @120fps	   | bin4-	VGA@120FPS 				 	 | 0x31    |  (  680 Mbps)	   |   120.10	   | 68		|
 | 1626 x  912 @ 30fps		   | bin2-	720P@60FPS(0x33+dummy)		 | 0x32    |  (  500 Mbps)	   |	30.00	   | 50		|
 | 1626 x  912 @ 60fps		   | bin2-	720P@60fps					 | 0x33    |  (  500 Mbps)	   |	60.04	   | 50		|
 | 3280 x 1824 @ 30fps		   |	   1080P@30fps					 | 0x34    |  (  680 Mbps)	   |	30.00	   | 50		|
 |	816 x  612 @ 30fps	   	   | bin4-	VGA@30FPS(0x30+dummy)	 	 | 0x35    |  (  680 Mbps)	   |	30.00	   | 68		|
 | 1632 x 1224 @ 60fps		   | bin2-	UXGA@60FPS					 | 0x36    |  (  650 Mbps)	   |	60.01	   | 65 		|
 | 1632 x 1224 @ 30fps		   | bin2-	UXGA@60FPS(0x36+dummy)	 | 0x37    |  (  650 Mbps)	   |	30.00	   | 65 		|
 | 3264x 2448  @ 30fps		   | 8M size preview					 | 0x38    |  (  640 Mbps)	   |	29.98	   | 64 		|

 | Snap mode				   

 | 3264 x 2448 @ 30fps		   | 8M size preview					 | 0x40    |  (  640 Mbps)	   |	29.98	   | 64 		|
 | 1632 x 1224 @ 30fps		   | bin2-UXGA30FPS					 | 0x41    |  (  330 Mbps)	   |	30.00	   | 33 		|

 *  20150903 Version 2.00.14
 *         1.Add the comport configuration for burning spi of 7002.
 *         2.Add the I2C read/write for reading/writing SPCA7002 use.
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_fake-2.00.14"
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

#define USEFUL 0
#if SP5K_DIQ_FRMWRK
#define USE_AGC_DELAY_SYNC			   (1)
#else
#define USE_AGC_DELAY_SYNC			   (0)
#endif

#if SP5K_SENSOR_DUAL_MODE
#define DUAL_RAW                        1
#else
#define DUAL_RAW                        0
#endif
#define MASK_EXP_CMD                    0
#define EXT_HD_LINE					   (32)
#define SENSOR_PROF_LOG_EN             (1)


#define CMD_LEN(a)			(sizeof(a)/sizeof(a[0]))

#if SENSOR_PROF_LOG_EN
    #define PROF_ID                                 (0)
    #define SENSOR_PROF_LOG_ADD(str)                profLogAdd(PROF_ID, str)
    #define SENSOR_PROF_LOG_PRINT(fmt, arg...)      profLogPrintf(PROF_ID, fmt, ##arg)
 
#else
    #define SENSOR_PROF_LOG_ADD(str)
    #define SENSOR_PROF_LOG_PRINT(fmt,arg...)
#endif

/*MIPI clock convert */
/*horiz. and vertical size*/                                    

	#define PREVIEW_UXGA_HSIZE                		(1632)/*(1632-2)*/      /* 3264/2 = 1632  */
	#define PREVIEW_UXGA_VSIZE                		(1224)/*(1224-2)*/	    /* 2448/2 = 1224  */
	#define PREVIEW_UXGA_HOFFSET 					(0)  
	#define PREVIEW_UXGA_VOFFSET 					(0)	
	#define PREVIEW_UXGA_BAYERHS_SIZE				(0)/*Dont scale*/
	#define PREVIEW_UXGA_LINETOTAL					(870)  /*Important! fine tune frame rate*/
	#define PREVIEW_UXGA_FRAMETOTAL					(1271) /*Important! fine tune frame rate, MIN value: 1271*/
	#define PREVIEW_UXGA_PCLKNX		  		 		33000
	#define PREVIEW_UXGA_PCLK1X	 	 		 		132000 /*no effect for frame rate*/
	#define PREVIEW_UXGA_CDSPCLK					0
	#define PREVIEW_UXGA_EXTHD						36

    #define PREVIEW_VGA_HSIZE						(816)       /* 3264/4 = 816 */ 
    #define PREVIEW_VGA_VSIZE						(612)       /* 2448/4 = 612 */
	#define PREVIEW_VGA_HOFFSET					 	(0)
    #define PREVIEW_VGA_VOFFSET 					(0) 
	#define PREVIEW_VGA_BAYERHS_SIZE				(0)/*Dont scale*/	
	#define PREVIEW_VGA_LINETOTAL					(860)								
	#define PREVIEW_VGA_FRAMETOTAL 					(617)	
	#define PREVIEW_VGA_PCLKNX						68000
	#define PREVIEW_VGA_PCLK1X						264000 /*660Mbps*/
	#define PREVIEW_VGA_CDSPCLK 					0
	#define PREVIEW_VGA_EXTHD						40

 
 	#define PREVIEW_720P_HSIZE               		(1626)		                   
	#define PREVIEW_720P_VSIZE               		(912)    
	#define PREVIEW_720P_HOFFSET					(0) 
	#define PREVIEW_720P_VOFFSET					(0)
	#define PREVIEW_720P_BAYERHS_SIZE				(0)/*Dont scale*/
	#define PREVIEW_720P_LINETOTAL					(862) 							
	#define PREVIEW_720P_FRAMETOTAL 				(925)
	#define PREVIEW_720P_PCLKNX 					50000
	#define PREVIEW_720P_PCLK1X 					200000 /*500Mbps*/
	#define PREVIEW_720P_CDSPCLK					0
	#define PREVIEW_720P_EXTHD						38


	/* For preview size, 3280*1188, 30fps */
	/*200Mpixel/s is too fast for 6330*/
	#define PREVIEW_1080P_HSIZE              		(3280)		
	#define PREVIEW_1080P_VSIZE  	         		(1824)	 	
	#define PREVIEW_1080P_HOFFSET					 0						 
	#define PREVIEW_1080P_VOFFSET					(0)	 
	#define PREVIEW_1080P_BAYERHS_SIZE				(0)/*Dont scale*/
	#define PREVIEW_1080P_LINETOTAL 				(862)					
	#define PREVIEW_1080P_FRAMETOTAL				(1845)				 
	#define PREVIEW_1080P_PCLKNX 					50000
	#define PREVIEW_1080P_PCLK1X 					200000 /*500Mbps*/
	#define PREVIEW_1080P_CDSPCLK					0
	#define PREVIEW_1080P_EXTHD						38


	#define PREVIEW_UXGA60_HSIZE					(1632)/*(1632-2)*/		/* 3264/2 = 1632  */
	#define PREVIEW_UXGA60_VSIZE					(1224)/*(1224-2)*/		/* 2448/2 = 1224  */
	#define PREVIEW_UXGA60_HOFFSET					(0)  
	#define PREVIEW_UXGA60_VOFFSET					(0) 
	#define PREVIEW_UXGA60_BAYERHS_SIZE				(0)/*Dont scale*/
	#define PREVIEW_UXGA60_LINETOTAL				(860)							
	#define PREVIEW_UXGA60_FRAMETOTAL				(1231) 
	#define PREVIEW_UXGA60_PCLKNX					66000
	#define PREVIEW_UXGA60_PCLK1X					256000 /*650Mbps*/
	#define PREVIEW_UXGA60_CDSPCLK					0
	#define PREVIEW_UXGA60_EXTHD 					38


	#define PREVIEW_FULL_HSIZE 						(3264)		
	#define PREVIEW_FULL_VSIZE 						(2448)		
	#define PREVIEW_FULL_HOFFSET					 0						 
	#define PREVIEW_FULL_VOFFSET					(0)  
	#define PREVIEW_FULL_BAYERHS_SIZE				(0)/*Dont scale*/
	#define PREVIEW_FULL_LINETOTAL					(860)		
	#define PREVIEW_FULL_FRAMETOTAL 				(2465)				 
	#define PREVIEW_FULL_PCLKNX						 64000
	#define PREVIEW_FULL_PCLK1X						256000 /*not the same with measured fpclktgle!!!*/
	#define PREVIEW_FULL_CDSPCLK					0
	#define PREVIEW_FULL_EXTHD						40


    /* For Capture 3280x2464 */
	#define CAPTURE_HSIZE							(3264)		
	#define CAPTURE_VSIZE							(2448)		
	#define CAPTURE_HOFFSET 						(0) 
	#define CAPTURE_VOFFSET							(0)  
	#define CAPTURE_LINETOTAL						(2660)	
	#define CAPTURE_FRAMETOTAL						(2465)
	#define CAPTURE_PCLKNX 							 64000
	#define CAPTURE_PCLK1X 							256000
	#define CAPTURE_CDSPCLK							0

    /* For Capture2 1632x1224 */
	#define CAPTURE2_HSIZE							(1632)		
	#define CAPTURE2_VSIZE							(1224)		
	#define CAPTURE2_HOFFSET						(0) 
	#define CAPTURE2_VOFFSET 						(0)  
	#define CAPTURE2_LINETOTAL						(860)
	#define CAPTURE2_FRAMETOTAL 					(1231)
	#define CAPTURE2_PCLKNX 						 32000
	#define CAPTURE2_PCLK1X 						128000 /*320Mbps*/
	#define CAPTURE2_CDSPCLK						3330	


/*****************************************************************************************************/

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL						  TYPE_CMOS		/* TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						                                                        TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			         TG_CMOS_SP5K_SLAVE		/* TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
														     TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/
#define	PREVIEW_UXGA_CFAPATTERN	         BAYER_RGGB     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	PREVIEW_VGA_CFAPATTERN	         BAYER_RGGB     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_720P_CFAPATTERN          BAYER_RGGB
#define PREVIEW_1080P_CFAPATTERN         BAYER_RGGB
#define PREVIEW_FULL_CFAPATTERN          BAYER_RGGB

#define	SNAP_CFAPATTERN					 BAYER_RGGB     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP2_CFAPATTERN				 BAYER_RGGB     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM						  0     /* snap dummy Vd need numbers*/            
#define	SNAP_READOUTVD_NUM		        		  1     /* snap readout Vd need numbers*/             
#define	SNAP_CAPTUREVD_NUM		        		  1     /* for typical CCD==n_dummy + n_readout + 1*/ 
#define	SNAP_EARLY_NUM		                      0	    /* for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM	                      0		/* It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM							  0	 	/* It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN                    4/*0*//* a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/
#define	VSHUTTER_RESEND 	                      0		/* sony FALSE;panasonic/ITG TRUE*/
#if  USE_AGC_DELAY_SYNC
#define EXP_FRAME_NUM							  1
#else
#define	EXP_FRAME_NUM       	                  2
#endif
#define	PREVIEW_UXGA_HDNUM_CMDPORT		        (400)	  	
#define PREVIEW_UXGA60_HDNUM_CMDPORT			(400)		
#define	PREVIEW_VGA_HDNUM_CMDPORT		        (400)		
#define	PREVIEW_720P_HDNUM_CMDPORT		        (400)		
#define	PREVIEW_1080P_HDNUM_CMDPORT	            (400)	
#define	PREVIEW_FULL_HDNUM_CMDPORT	            (400)	
#define	CAPTURE_HDNUM_CMDPORT	                (400)  
#define	CAPTURE2_HDNUM_CMDPORT	                (400)  
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
    #define	TG_SPI_I2C_MODE     0x78                /* I2C slave addr for SPCA7002 write */
    #define	TG_FORMAT2          FRONT_SSC_READ_HAS_SUBADDR |SENSOR_SERIAL_FORMAT2_SIF1_PIN_TG0
    #define	TG_FORMAT3          0
    #define	TG_SPEED            0  
    #define	TG_NBIT_REGADDR     16                  /* ssc/spi register address numbers */
    #define	TG_NBIT_REGDATA     8                   /* ssc/spi register data numbers */
	#define TG_I2C_SPI_FREQ_KHZ  400
/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
static void sensorDoFuncInit();
static void sensorDoFuncStop();
static void sensor_cmdport_exp_write(UINT32 nreg,const UINT8 *pdata);
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
static void sensor_exp_write_redo(UINT32 smode,SINT32 *expIdx,UINT32 *vcount,UINT8 *vtbl_type,UINT32 *int_us);
extern UINT32 sensorModeGet();
extern void sensor_front_fake_tg_pattern(UINT32 type);
extern void sensor_front_fake_preview();
extern void sensor_front_fake_snap();
extern void sensor_front_fake_tg_en(UINT32 sensel,UINT32 en);
extern void ros_thread_sleep(UINT32 ms);
/******************** uCode ****************************/
#if(SP5K_SENSOR_FAKE&&(defined(HW_SBC)))
UINT32 cmdportid_fake_spca7002_seq=0xff;
#endif
static const UINT8 cmd_init_code_SWRstOn[][3]={
	{0x01,0x03, 0x01}
};
static const UINT8 cmd_init_code_SWRstOff[][3]={
	{0x01,0x03, 0x00}
};
static const UINT8 cmd_init_code_StreamOn[][3]={
	{0x01,0x00, 0x01}
};
static const UINT8 cmd_init_code_StreamOff[][3]={
	{0x01,0x00, 0x00}
};

static const UINT8 cmd_uxga_30fps_code[][3]={	
	{0x01,0x00, 0x00},
};

static const UINT8 cmd_uxga_60fps_code[][3]={	
	{0x01,0x00, 0x00},
};
/*vga 120fps */
static const UINT8 cmd_vga_code[][3]={
	{0x01,0x00, 0x00},
};

static const UINT8 cmd_720P_code[][3]={
	{0x01,0x00,0x00 },
};

static const UINT8 cmd_1080P_code[][3]={
	{0x01,0x00,0x00},
};

static const UINT8 cmd_still_code[][3]={
	{0x01,0x00, 0x00},
};

/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg =     
{                                                
	.sensel=0,
	.Lvdseolsel=0,
	.Lvdseofsel=2,
	.mipi=1,
#if DUAL_RAW
	.spsel=0x103,
#else
	.spsel=0xF,
#endif
	.wordwidth=1,/*10bit*/
	.decpixelnum=3,
	.wordcntsel=0,
	.SenPinSelNum =4,
	.SenPinSel={0x0,0x1,0x2,0x3},	
};                                               
/*For dual sensor */
static const sensorLVDSCfg_t sensorLVDSCfg2 =     
{                                                
	.sensel=1,
	.Lvdseolsel=0,
	.Lvdseofsel=2,
	.mipi=1,
	.spsel=0x103,
	.wordwidth=1,/*10bit*/
	.decpixelnum=3,
	.wordcntsel=0,
	.SenPinSelNum =4,
	.SenPinSel={0x0,0x1,0x2,0x3},	
};                                               

static const sensorTgPreviewCfg_t sensorTgPreviewUXGACfg = {
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
		.xSize = PREVIEW_UXGA_HSIZE,
		.ySize = PREVIEW_UXGA_VSIZE,
	},
	.hsize = 					PREVIEW_UXGA_HSIZE,
	.vsize =                    PREVIEW_UXGA_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_FAKE_RAW,
#endif
	.cfa_pattern =              PREVIEW_UXGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 30007000,
	.sensor_pclk = PREVIEW_UXGA_PCLK1X,
	.sensor_htotal = PREVIEW_UXGA_LINETOTAL,			
    .pix_100khz =PREVIEW_UXGA_CDSPCLK,
    .bayerhs_size	=PREVIEW_UXGA_BAYERHS_SIZE, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewUXGA60FPSCfg = {
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
		.xSize = PREVIEW_UXGA_HSIZE,
		.ySize = PREVIEW_UXGA_VSIZE,
	},
	.hsize = 					PREVIEW_UXGA_HSIZE,
	.vsize =                    PREVIEW_UXGA_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_FAKE_RAW,
#endif
	.cfa_pattern =              PREVIEW_UXGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 60015000,
	.sensor_pclk = PREVIEW_UXGA60_PCLK1X,
	.sensor_htotal = PREVIEW_UXGA_LINETOTAL,			
	.pix_100khz =PREVIEW_UXGA60_CDSPCLK,
	.bayerhs_size	=PREVIEW_UXGA60_BAYERHS_SIZE, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewUXGA30FPSCfg = {
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
		.xSize = PREVIEW_UXGA_HSIZE,
		.ySize = PREVIEW_UXGA_VSIZE,
	},
	.hsize = 					PREVIEW_UXGA_HSIZE,
	.vsize =                    PREVIEW_UXGA_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_FAKE_RAW,
#endif
	.cfa_pattern =              PREVIEW_UXGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 30007000,
	.sensor_pclk = PREVIEW_UXGA60_PCLK1X,
	.sensor_htotal = PREVIEW_UXGA_LINETOTAL,			
	.pix_100khz =PREVIEW_UXGA60_CDSPCLK,
	.bayerhs_size	=PREVIEW_UXGA60_BAYERHS_SIZE, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewVGACfg = {
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
		.xSize = PREVIEW_VGA_HSIZE,
		.ySize = PREVIEW_VGA_VSIZE,
	},
	.hsize = 					PREVIEW_VGA_HSIZE,
	.vsize =                    PREVIEW_VGA_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_FAKE_RAW,
#endif
	.cfa_pattern =              PREVIEW_VGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 120100000,
	.sensor_pclk = PREVIEW_VGA_PCLK1X,
	.sensor_htotal = PREVIEW_VGA_LINETOTAL,			
	.pix_100khz =PREVIEW_VGA_CDSPCLK,
	.bayerhs_size	=PREVIEW_VGA_BAYERHS_SIZE, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewVGA30FPSCfg = {
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
		.xSize = PREVIEW_VGA_HSIZE,
		.ySize = PREVIEW_VGA_VSIZE,
	},
	.hsize = 					PREVIEW_VGA_HSIZE,
	.vsize =                    PREVIEW_VGA_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_FAKE_RAW,
#endif
	.cfa_pattern =              PREVIEW_VGA_CFAPATTERN,
	.hratio = 4000, /*4:3 sensor*/
	.vratio = 3000,
	.fps_mhz = 30001000,
	.sensor_pclk = PREVIEW_VGA_PCLK1X,
	.sensor_htotal = PREVIEW_VGA_LINETOTAL,			
	.pix_100khz =PREVIEW_VGA_CDSPCLK,
	.bayerhs_size	=PREVIEW_VGA_BAYERHS_SIZE, 
};

static const sensorTgPreviewCfg_t sensorTgPreview1080PCfg = {
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
		.xSize = PREVIEW_1080P_HSIZE,
		.ySize = PREVIEW_1080P_VSIZE,
	},
	.hsize =					PREVIEW_1080P_HSIZE,
	.vsize =					PREVIEW_1080P_VSIZE,
	.sensor_model =				SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_FAKE_RAW,
#endif

	.cfa_pattern =				PREVIEW_1080P_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 30007000,
	.sensor_pclk = PREVIEW_1080P_PCLK1X,
	.sensor_htotal = PREVIEW_1080P_LINETOTAL, 		
	.pix_100khz =PREVIEW_1080P_CDSPCLK,
	.bayerhs_size	=PREVIEW_1080P_BAYERHS_SIZE, 
};

static const sensorTgPreviewCfg_t sensorTgPreviewFullCfg = {
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
		.xSize = PREVIEW_FULL_HSIZE,
		.ySize = PREVIEW_FULL_VSIZE,
	},
	.hsize =					PREVIEW_FULL_HSIZE,
	.vsize =					PREVIEW_FULL_VSIZE,
	.sensor_model =				SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_FAKE_RAW,
#endif
	.cfa_pattern =				PREVIEW_FULL_CFAPATTERN,
	.hratio = 4000,
	.vratio = 3000,
	.fps_mhz = 30007000,
	.sensor_pclk = PREVIEW_FULL_PCLK1X,
	.sensor_htotal = PREVIEW_FULL_LINETOTAL, 		
	.pix_100khz =PREVIEW_FULL_CDSPCLK,
	.bayerhs_size	=PREVIEW_FULL_BAYERHS_SIZE, 
};


static const sensorTgPreviewCfg_t sensorTgPreview720PCfg = {
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
		.xSize = PREVIEW_720P_HSIZE,
		.ySize = PREVIEW_720P_VSIZE,
	},
	.hsize =					PREVIEW_720P_HSIZE,
	.vsize = 					PREVIEW_720P_VSIZE,  
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
    .model_info =               SENSORMODEL_DUAL_MIPI_RAW,
#else
    .model_info =               SENSORMODEL_FAKE_RAW,
#endif
	.cfa_pattern =              PREVIEW_720P_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 30007000,
	.sensor_pclk = PREVIEW_720P_PCLK1X,
	.sensor_htotal = PREVIEW_720P_LINETOTAL, 		
	.pix_100khz =PREVIEW_720P_CDSPCLK,
	.bayerhs_size	=PREVIEW_720P_BAYERHS_SIZE, 
};

static const sensorTgPreviewCfg_t sensorTgPreview720P60FPSCfg = {
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
		.xSize = PREVIEW_720P_HSIZE,
		.ySize = PREVIEW_720P_VSIZE,
	},
	.hsize =					PREVIEW_720P_HSIZE,
	.vsize = 					PREVIEW_720P_VSIZE,  
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_FAKE_RAW,
#endif
	.cfa_pattern =              PREVIEW_720P_CFAPATTERN,
	.hratio = 3200,
	.vratio = 1800,
	.fps_mhz = 60046000,
	.sensor_pclk = PREVIEW_720P_PCLK1X,
	.sensor_htotal = PREVIEW_720P_LINETOTAL, 		
	.pix_100khz =PREVIEW_720P_CDSPCLK,
	.bayerhs_size	=PREVIEW_720P_BAYERHS_SIZE, 
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
#if DUAL_RAW
		.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
		.model_info =				SENSORMODEL_FAKE_RAW,
#endif
		.cfa_pattern =			   SNAP_CFAPATTERN,
		.n_dummy =				   SNAP_DUMMYVD_NUM,        	
		.n_readout =			   SNAP_READOUTVD_NUM,      
		.n_capvd =				   SNAP_CAPTUREVD_NUM,      
		.n_snap_early =			   SNAP_EARLY_NUM,    
		.skip_snap_readout_vd =	   SKIP_SNAP_READOUTVD_EN,  
		.fldsel_start =			   FLDSEL_START_NUM,        
		.fldsel_end =			   FLDSEL_END_NUM,          
		.hratio = 4000, /*4:3 sensor*/
		.vratio = 3000,
		.pix_100khz =CAPTURE_CDSPCLK,
};

static  sensorTgSnapCfg_t sensorTgSnap2RollCfg = {
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
		.hsize =                   CAPTURE2_HSIZE,
		.vsize =				   CAPTURE2_VSIZE,
		.sensor_model =			   SENSORMODEL,/*TYPE_HSCMOS,*/
#if DUAL_RAW
		.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else	
		.model_info =				SENSORMODEL_FAKE_RAW,
#endif
		.cfa_pattern =			   SNAP2_CFAPATTERN,
		.n_dummy =				   SNAP_DUMMYVD_NUM,        	
		.n_readout =			   SNAP_READOUTVD_NUM,      
		.n_capvd =				   SNAP_CAPTUREVD_NUM,      
		.n_snap_early =			   SNAP_EARLY_NUM,    
		.skip_snap_readout_vd =	   SKIP_SNAP_READOUTVD_EN,  
		.fldsel_start =			   FLDSEL_START_NUM,        
		.fldsel_end =			   FLDSEL_END_NUM,          
		.hratio = 4000, /*4:3 sensor*/
		.vratio = 3000,
		.pix_100khz =CAPTURE2_CDSPCLK,
};
#if 0
static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg, 
    &sensorTgSnap2RollCfg, 
};
#endif
static const sensorSyncSigCfg_t sensorSyncSigPreviewUXGACfg = {
	.linetotal       = PREVIEW_UXGA_LINETOTAL,
	.frametotal      = PREVIEW_UXGA_FRAMETOTAL,
	.hoffset         = PREVIEW_UXGA_HOFFSET,
	.voffset         = PREVIEW_UXGA_VOFFSET,
	.hsize           = PREVIEW_UXGA_HSIZE,
	.vsize           = PREVIEW_UXGA_VSIZE,
	.lastlinepix     = PREVIEW_UXGA_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_UXGA_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_UXGA_PCLKNX,
	.pclk1x 		 = PREVIEW_UXGA_PCLK1X,
	.ext_hdnum		 = PREVIEW_UXGA_EXTHD,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewUXGA60fpsCfg = {
	.linetotal       = PREVIEW_UXGA60_LINETOTAL,
	.frametotal      = PREVIEW_UXGA60_FRAMETOTAL,
	.hoffset         = PREVIEW_UXGA60_HOFFSET,
	.voffset         = PREVIEW_UXGA60_VOFFSET,
	.hsize           = PREVIEW_UXGA60_HSIZE,
	.vsize           = PREVIEW_UXGA60_VSIZE,
	.lastlinepix     = PREVIEW_UXGA60_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_UXGA60_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_UXGA60_PCLKNX,
	.pclk1x 		 = PREVIEW_UXGA60_PCLK1X,
	.ext_hdnum		 = PREVIEW_UXGA60_EXTHD,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewVGACfg = {
	.linetotal       = PREVIEW_VGA_LINETOTAL,
	.frametotal      = PREVIEW_VGA_FRAMETOTAL,
	.hoffset         = PREVIEW_VGA_HOFFSET,
	.voffset         = PREVIEW_VGA_VOFFSET,
	.hsize           = PREVIEW_VGA_HSIZE,
	.vsize           = PREVIEW_VGA_VSIZE,
	.lastlinepix     = PREVIEW_VGA_HSIZE,
	.conf_hdpol      = 0,  
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_VGA_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_VGA_PCLKNX,
	.pclk1x 		 = PREVIEW_VGA_PCLK1X,
	.ext_hdnum		 = PREVIEW_VGA_EXTHD,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview1080PCfg = {
	.linetotal       = PREVIEW_1080P_LINETOTAL,
	.frametotal      = PREVIEW_1080P_FRAMETOTAL,
	.hoffset         = PREVIEW_1080P_HOFFSET,
	.voffset         = PREVIEW_1080P_VOFFSET,
	.hsize           = PREVIEW_1080P_HSIZE,
	.vsize           = PREVIEW_1080P_VSIZE,
	.lastlinepix     = PREVIEW_1080P_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1080P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_1080P_PCLKNX,
	.pclk1x 		 = PREVIEW_1080P_PCLK1X,
	.ext_hdnum		 = PREVIEW_1080P_EXTHD,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview720PCfg = {
	.linetotal       = PREVIEW_720P_LINETOTAL,
	.frametotal      = PREVIEW_720P_FRAMETOTAL,
	.hoffset         = PREVIEW_720P_HOFFSET,
	.voffset         = PREVIEW_720P_VOFFSET,
	.hsize           = PREVIEW_720P_HSIZE,
	.vsize           = PREVIEW_720P_VSIZE,
	.lastlinepix     = PREVIEW_720P_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_720P_PCLKNX,
	.pclk1x 		 = PREVIEW_720P_PCLK1X,
	.ext_hdnum		 = PREVIEW_720P_EXTHD,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFullCfg = {
	.linetotal       = PREVIEW_FULL_LINETOTAL,
	.frametotal      = PREVIEW_FULL_FRAMETOTAL,
	.hoffset         = PREVIEW_FULL_HOFFSET,
	.voffset         = PREVIEW_FULL_VOFFSET,
	.hsize           = PREVIEW_FULL_HSIZE,
	.vsize           = PREVIEW_FULL_VSIZE,
	.lastlinepix     = PREVIEW_FULL_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_FULL_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_FULL_PCLKNX,
	.pclk1x 		 = PREVIEW_FULL_PCLK1X,
	.ext_hdnum		 = PREVIEW_FULL_EXTHD,
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
	.ext_hdnum		 = 0,
};

static  sensorSyncSigCfg_t sensorSyncSigSnap2Cfg = {
	.linetotal         = CAPTURE2_LINETOTAL,
	.frametotal        = CAPTURE2_FRAMETOTAL,
	.hoffset           = CAPTURE2_HOFFSET,
	.voffset           = CAPTURE2_VOFFSET,
	.hsize             = CAPTURE2_HSIZE,
	.vsize             = CAPTURE2_VSIZE,
	.lastlinepix       = CAPTURE2_HSIZE,
	.conf_hdpol        = 0,
	.conf_vdpol        = 0,
	.hdnum_cmdport     = CAPTURE2_HDNUM_CMDPORT,
	.fpclk			 = CAPTURE2_PCLKNX,
	.pclk1x 		 = CAPTURE2_PCLK1X,
	.ext_hdnum		 = 0,
};

typedef struct {
	const sensorTgPreviewCfg_t *pstpPrev;
	const sensorSyncSigCfg_t *ssPrevCfg;
	const UINT8 *pCmdSet;
	const UINT16 CmdLen;
} sensorPrevModeSet_t;

typedef struct {
	const sensorTgSnapCfg_t *pstpSnap;
	const sensorSyncSigCfg_t *ssSnapCfg;
	const UINT8 *pCmdSet;
	const UINT16 CmdLen;
} sensorSnapModeSet_t;

sensorPrevModeSet_t sensorPrevModeTbl[]=
{
	[0x00]={&sensorTgPreviewUXGACfg 		,&sensorSyncSigPreviewUXGACfg		,cmd_uxga_30fps_code[0] ,CMD_LEN(cmd_uxga_30fps_code)},
	[0x01]={&sensorTgPreviewVGACfg			,&sensorSyncSigPreviewVGACfg		,cmd_vga_code[0]		,CMD_LEN(cmd_vga_code)},
	[0x02]={&sensorTgPreview720PCfg 		,&sensorSyncSigPreview720PCfg		,cmd_720P_code[0]		,CMD_LEN(cmd_720P_code)},
	[0x03]={&sensorTgPreview720P60FPSCfg	,&sensorSyncSigPreview720PCfg		,cmd_720P_code[0]		,CMD_LEN(cmd_720P_code)},
	[0x04]={&sensorTgPreview1080PCfg		 ,&sensorSyncSigPreview1080PCfg		,cmd_1080P_code[0]		,CMD_LEN(cmd_1080P_code)},
	[0x05]={&sensorTgPreviewVGA30FPSCfg 	,&sensorSyncSigPreviewVGACfg		,cmd_vga_code[0]		,CMD_LEN(cmd_vga_code)},
	[0x06]={&sensorTgPreviewUXGA60FPSCfg	,&sensorSyncSigPreviewUXGA60fpsCfg	,cmd_uxga_60fps_code[0] ,CMD_LEN(cmd_uxga_60fps_code)},
	[0x07]={&sensorTgPreviewUXGA30FPSCfg	,&sensorSyncSigPreviewUXGA60fpsCfg	,cmd_uxga_60fps_code[0] ,CMD_LEN(cmd_uxga_60fps_code)},   
	[0x08]={&sensorTgPreviewFullCfg 		,&sensorSyncSigPreviewFullCfg		,cmd_still_code[0]		,CMD_LEN(cmd_still_code)},
};


sensorSnapModeSet_t sensorSnapModeTbl[]=
{
	[0x00]={&sensorTgSnapRollCfg 		,&sensorSyncSigSnapCfg		,cmd_still_code[0]		,CMD_LEN(cmd_still_code)},
	[0x01]={&sensorTgSnap2RollCfg 		,&sensorSyncSigSnap2Cfg		,cmd_uxga_30fps_code[0] ,CMD_LEN(cmd_uxga_30fps_code)},
};

static sensorTgCfg_t sensorTgCfg = {
	.tg_type = TGTYPE,
	.n_preview_submode = sizeof(sensorPrevModeTbl) /
			sizeof(sensorPrevModeTbl[0]),
	.n_snap_submode = sizeof(sensorSnapModeTbl) /
			sizeof(sensorSnapModeTbl[0]),
	.vshutter_need_resend = VSHUTTER_RESEND,
	.exp_effect_frame_count = EXP_FRAME_NUM,
};

static sensorTgDev_t sensorTgDev = {
	.tgCfg = &sensorTgCfg,
	.tgSvc = &(const sensorTgSvc_t) {
		.resload =		sensorCfgInit,
		.resstop =		sensorCfgStop,
		.init =			sensorDoFuncInit,
		.stop =			sensorDoFuncStop, /****TODO***/
		.pwrsave  =     sensorDoFuncPowerSave,
		.expwrite =		sensor_cmdport_exp_write,
		.expvtbldo =	sensor_cmdport_exp_vtbl_do,
		.expwriteRedo=	sensor_exp_write_redo,
		.crop =			NULL, /****TODO***/
	},
#if USEFUL
	.previewCfg =		sensorTgPreviewCfgTbl,
	.snapCfg =			sensorTgSnapCfgTbl,
	.defSyncSigCfg  =	&sensorSyncSigPreviewUXGACfg,
#endif
	.verStr =			MODEL_VERSION,
};

void
sensorTg_Fake_Install()
{
	static const sensorTgPreviewCfg_t	*sensorTgPreviewCfgTbl[sizeof(sensorPrevModeTbl)/sizeof(sensorPrevModeTbl[0])];
	static const sensorTgSnapCfg_t		   *sensorTgSnapCfgTbl[sizeof(sensorSnapModeTbl)/sizeof(sensorSnapModeTbl[0])];
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

#if(SP5K_SENSOR_FAKE&&(defined(HW_SBC)))
void sensorFakeSPCA7002_I2CDataWrite(UINT32 a_udwAddr, UINT32 a_udwData)
{
	UINT32 cmdportid;
	cmdportid = sensorTgCfg.cmdportid_xtg;
	static UINT8 SENSOR_SIF1DMA_VARBUF regdata[3];
	regdata[0]=(UINT8)((a_udwAddr>>8)&0xFF);
	regdata[1]=(UINT8)(a_udwAddr&0xFF);
	regdata[2]=(UINT8)(a_udwData&0xFF);
	sensorSerialDirectBufSnoop(regdata, sizeof(regdata));
	sensorSerialBulkWrite(cmdportid,1,regdata,0);
	profLogPrintf(3,"");
}

UINT32 sensorFakeSPCA7002_I2CDataRead(UINT32 a_udwAddr)
{
	UINT32 cmdportid;
	static UINT8 SENSOR_SIF1DMA_VARBUF regread[3];
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=(UINT8)((a_udwAddr>>8)&0xFF);
	regread[1]=(UINT8)(a_udwAddr&0xFF);
	regread[2]=0x00;/*clear*/
	sensorSerialDirectBufSnoop(regread, sizeof(regread));
	sensorSerialBlockRead(cmdportid,1,regread,0);
	tmrUsWait(2000);
	/*profLogPrintf(1,"\nREAD [%x] = [%x]\n",addr,regread[2]);*/
	return regread[2];
}
#endif

static void
sensorDoFuncInit()
{
	/*UINT32 cmdportid;*/
		
	SENSOR_PROF_LOG_ADD("sensor_fake sensorDoFuncInit++ 6350 ");
	SENSOR_PROF_LOG_PRINT("________Sensor driver ver <<%s>>", MODEL_VERSION);
	SENSOR_PROF_LOG_PRINT("________Sensor driver build time <<%s>>", BUILD_TIME);
	SENSOR_PROF_LOG_ADD("sensorDoFuncInit()_sensor_fake_enter_....");
	sensorTgPllClkCfgSet(SENSOR_TGPLL_CFG_DRIVING,4);/*40uA*/
	sensorTgPllClkExtSet(24000000);/*24MHz*/												

    /*don't do sensor power on sequence due to fake sensor*/


	/* Master clock and Pixel clock configuration here */										
	/*****TODO*****front reset after these???*/ 												

    /*TG PLL=MCLK out=24MHz*/
    sensorLVDSCfgSet(&sensorLVDSCfg);
#if DUAL_RAW	
    sensorLVDSCfgSet(&sensorLVDSCfg2);
#endif
    sensorSyncSigInit(sensorTgDev.defSyncSigCfg);

	tmrUsWait(30000);/*30ms*/	

    sensor_front_fake_tg_pattern(0x0D);

	SENSOR_PROF_LOG_ADD("sensorDoFuncInit()___done");
	SENSOR_PROF_LOG_ADD("sensor_fake sensorDoFuncInit--");
	printf("sensorDoFuncInit()___done \n"); 
}

static void
sensorDoFuncStop()
{

    /*don't do sensor power off sequence due to fake sensor*/
}

static UINT16 isEnterPVinSnapMode=0;

static void
sensorDoFuncPreviewBegin()
{
	UINT32 smode;                                                                                                                             
  	SENSOR_PROF_LOG_ADD("preview begin");                                                             
	/*UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;  */

/*Ricky[DM]*/
    if(isEnterPVinSnapMode)    
    {
	    sensorEventVvalidWait(1);
        isEnterPVinSnapMode=0;
    }

  	SENSOR_PROF_LOG_ADD("preview cmd done wait done..");  
  	smode=sensorModeGet();

	SENSOR_PROF_LOG_PRINT("mode %x i2c send +++..",smode);
#if 1
    sensorSyncSigInit(sensorPrevModeTbl[smode&0xF].ssPrevCfg);

    sensor_front_fake_preview();    
    
    profLogAdd(0, "Ricky[DM] sensor_front_fake_tg_en() in pv begin");

#else
  	if(smode==0x30){        
  		/*UXGA 320Mbps 30fps*/
  		sensorSerialBulkWrite(cmdportid, sizeof(cmd_uxga_30fps_code)/sizeof(cmd_uxga_30fps_code[0]), cmd_uxga_30fps_code[0],0);  
  		sensorSyncSigInit(&sensorSyncSigPreviewUXGACfg);
    }else if(smode==0x36 || smode==0x37){
    	/*UXGA 640Mbps 60fps, 30fps(dummy)*/
    	sensorSerialBulkWrite(cmdportid, sizeof(cmd_uxga_60fps_code)/sizeof(cmd_uxga_60fps_code[0]), cmd_uxga_60fps_code[0],0);  
    	sensorSyncSigInit(&sensorSyncSigPreviewUXGA60fpsCfg);                                                           
	}else if(smode==0x31 || smode==0x35){
		/*VGA 120fps, 30fps(dummy)*/
		sensorSerialBulkWrite(cmdportid, sizeof(cmd_vga_code)/sizeof(cmd_vga_code[0]), cmd_vga_code[0],0);
		sensorSyncSigInit(&sensorSyncSigPreviewVGACfg);														   
	}else if(smode==0x32 || smode==0x33){
		/*720P 60fps, 30fps(dummy)*/
		sensorSerialBulkWrite(cmdportid, sizeof(cmd_720P_code)/sizeof(cmd_720P_code[0]), cmd_720P_code[0],0);
		sensorSyncSigInit(&sensorSyncSigPreview720PCfg);  
	}
	else if(smode==0x34){
		/*1080P 30fps*/
 		sensorSerialBulkWrite(cmdportid, sizeof(cmd_1080P_code)/sizeof(cmd_1080P_code[0]), cmd_1080P_code[0],0);
		sensorSyncSigInit(&sensorSyncSigPreview1080PCfg);     
	}
	else if(smode==0x38){
		/*Full size*/
		sensorSerialBulkWrite(cmdportid, sizeof(cmd_still_code)/sizeof(cmd_still_code[0]), cmd_still_code[0],0);
		sensorSyncSigInit(&sensorSyncSigPreviewFullCfg);	  
	}
#endif

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
    isEnterPVinSnapMode=1;

    SENSOR_PROF_LOG_ADD("SnapExpBegin s");  
	UINT32 smode;
	/*UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;*/
    smode=sensorModeGet();
    profLogInit(2,0x8000);
#if 1
	sensorSyncSigInit(sensorSnapModeTbl[smode&0xF].ssSnapCfg);
	/*sensor_front_fake_tg_pattern(0x0F);*/
    /*sensor_front_fake_tg_en(0,1);*/
        
    sensor_front_fake_snap();

#if 1
    ros_thread_sleep(50);
#endif
    
#else		
        if(smode==0x40){
            sensorSerialBulkWrite(cmdportid,(sizeof(cmd_still_code)/sizeof(cmd_still_code[0])), cmd_still_code[0],0);
            sensorSyncSigInit(&sensorSyncSigSnapCfg);  
            SENSOR_PROF_LOG_ADD("smode: 0x40");  
        }else{
            /* 0x41 */
            sensorSerialBulkWrite(cmdportid,(sizeof(cmd_uxga_30fps_code)/sizeof(cmd_uxga_30fps_code[0])), cmd_uxga_30fps_code[0],0);
            sensorSyncSigInit(&sensorSyncSigSnap2Cfg);  
            SENSOR_PROF_LOG_ADD("smode: 0x41");  
        }
#endif
        
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
		.speed_khz    = TG_I2C_SPI_FREQ_KHZ,
	};

#if(SP5K_SENSOR_FAKE&&(defined(HW_SBC)))
	static const sensorCmdport_t sensor_cmdport_seq_cfg = {
		.mode		  = TG_SPI_I2C_MODE,
		.format2	  = TG_FORMAT2,
		.format3	  = TG_FORMAT3,
		.speed		  = TG_SPEED,
		.nbit_regdata = 64,
		.nbit_regaddr = 16,
		.speed_khz    = TG_I2C_SPI_FREQ_KHZ,
	};
#endif

	ptcfg->cmdportid_exp =
	ptcfg->cmdportid_xtg = sensorSerialCreate(&sensor_cmdport_xtg_cfg);

#if(SP5K_SENSOR_FAKE&&(defined(HW_SBC)))
	cmdportid_fake_spca7002_seq = sensorSerialCreate(&sensor_cmdport_seq_cfg);
#endif

#if DUAL_RAW
	halFrontSerialCfgSet(FRONT_SIF1_PIN_CFG,SENSOR_SERIAL_FORMAT2_SIF1_PIN_TG1);
#endif
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
sensor_exp_write_redo(UINT32 smode,SINT32 *expIdx,UINT32 *vcount,UINT8 *vtbl_type,UINT32 *int_us)
{
	UINT32 snap_frm_org_length,snap_frm_org_fps	;
	UINT32 snap_frm_new_length,snap_frm_new_fps;
	UINT32 max_intv_ms;
	UINT32 interval_ms;

    /*Ricky[DM] TBD*/
    return;

	if(*int_us==0)
		return;
	
	if(smode==0x40){
		snap_frm_org_length =2482;
		snap_frm_org_fps	=30;	
	}
	else{ /*0x41*/
		snap_frm_org_length =1240;
		snap_frm_org_fps	=30;	
	}

	/*max frm rate, sensor register limitation */
	max_intv_ms=(1000*0xFFFF)/(snap_frm_org_fps*snap_frm_org_length);
	interval_ms=*int_us/1000;
	if(interval_ms > max_intv_ms)
	{
		printf("interval exceed max(%u us)",max_intv_ms*1000);
		interval_ms=max_intv_ms;
	}
	snap_frm_new_length=interval_ms*snap_frm_org_fps*snap_frm_org_length/1000;
	snap_frm_new_fps=1000*100/(interval_ms);
	snap_frm_new_length= (snap_frm_new_length>0xFFFF)?0xFFFF:snap_frm_new_length; /*must <=0xFFFF*/

    if(snap_frm_new_length<snap_frm_org_length)
	    snap_frm_new_length=snap_frm_org_length;

	profLogPrintf(3,"interval delay us=%d, exp idx= %d, new frmlength =%d new frm rate= %d.%02d FPS",*int_us,*expIdx,snap_frm_new_length,snap_frm_new_fps/100,snap_frm_new_fps%100);

	static UINT8 regData[][3] SENSOR_SIF1DMA_VARBUF ={
		{0x03,0x40,0x00},
		{0x03,0x41,0x00},
		
	};
	regData[0][2] = (snap_frm_new_length>>8)&0xff;
	regData[1][2] =  snap_frm_new_length&0xff;

	profLogPrintf(3,"[%x %x %x] [%x %x %x] ",regData[0][0],regData[0][1],regData[0][2],regData[1][0],regData[1][1],regData[1][2]);
	sensorSerialDirectBufSnoop(regData[0], sizeof(regData));
	sensorSerialBulkWrite(0,sizeof(regData)/sizeof(regData[0]),regData[0],0);

}

static void
sensor_cmdport_exp_write(UINT32 nreg, const UINT8 *pdata)
{
    /*don't control SIF due to fake sensor*/
}

static void
sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type, UINT32 vcount)
{
}

#if SP5K_SENSOR_FAKE
void sensorCmdPortExpLineSend(UINT16 expLine)
{	
	SENSOR_PROF_LOG_PRINT("sensorCmdPortExpLineSend() expLine=%d",expLine);

    /*don't control SIF due to fake sensor*/
}
#endif

static void
sensorDoFuncPowerSave(UINT32 senmode)
{
	/*UINT32 cmdportid= sensorTgCfg.cmdportid_xtg;  */

    if(senmode==SENSOR_MODE_POWERSAVE){     
        sensor_front_fake_tg_en(0,0);
        sensorTgPllClkCtrl(SENSOR_TGPLL_CTRL_EN,0);      /*disable tgpll*/ 
      
    }else{
        sensorDoFuncInit();
    }
}
