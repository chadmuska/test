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
 *  Author: Vance Huang                                                    *
 *                                                                        *
 *  ver. 0.00.02, 20141016, Vincent Tsao
 *    1.Modify the frmwrite callback function to make all sensor modes could support 
 *       to set maxmimum framerate function.
 *    2.Add the sensor line total parameters and correct some frame total parameters.
 *
 *  ver. 0.00.03, 20141021, Vincent Tsao
 *    1.Correct the parameters(sensor_pclk and sensor_htotal) in structure "sensorTgPreviewCfg_t".
 *       These two parameters should be the same with PCLK1X and Linetotal(not sensor linetotal).
 *    2.Avoid using the pointer address (not 4 bytes alignment, ex:(pdata+3*3)) to the s"ensorSerialAsyncWrite".
 *       This would cause the sending I2C data is not as the expected one.
 *    3.Correct the following sensor mode extension line number. 0x30,0x32,0x34~0x3C,0x3E.
 *       (According to the excel calculation result.)
 *
 *  ver. 0.00.04, 20141118, Vincent Tsao
 *    1.Add 32 extension lines to snap mode(0x40).
 *
 *  ver. 0.00.05, 20141120, Vincent Tsao
 *    1.Add new sensor mode full HD(1080P) 60fps, 2lane.
 *
 *  ver. 0.00.06, 20141209, Vincent Tsao
 *    1.Remove the full HD(1080P) 60fps, 2lane from sensor mode 0x3F.
 *    2.Add new sensor mode 2688x720 120fps(0x3F) to improve the image's sawtooth phenomenon.
 *    3.The frame length(VTS) calculation of binning mode in sensor_cmdport_frame_write must be
 *      4N+1 in OV4689.
 *    4.Remove the compiler flag SP5K_4K2K_MODE to make "frmwrite" could work on both preview and
 *       snap mode.
 *
 *  ver. 0.00.07, 20141215, Vincent Tsao
 *    1. Add the settings which needs to be set whe returning from HDR mode to normal preview mode.
 *
 *  ver. 0.00.08, 20141219, Vincent Tsao
 *    1. Correct the sensor_cmdport_frame_write to support to adjust the new frame length when
 *        exposure line count is greater than the calculated new frame length when maximum frame rate
 *        feature is enabled.
 *    2. Add the sensor_pclk and sensor_htotal to all sensorTgSnapCfg_t.
 *
 *  ver. 0.00.09, 20141219, Vincent Tsao
 *    1. Adjust the TG_SPEED parameters. This parameter is the divider in calcaulting the I2C clock.
 *        Currently the value is too samll and will cause the I2C clock over 400KHz.
 *
 *  ver. 0.00.0A, 20141230, Vincent Tsao
 *    1. Change the timing of setting middle exp/gain and short exp/gain. Change the setting timing to
 *        the position between group hold and group release.
 *    2. Assign the I2C frequency parameter of the sensorCmdport_t(speed_khz) 
 *	    to get correct I2C clock frequency even the register clock changed.
 *
 *  ver. 0.00.0B, 20150114, Vincent Tsao
 *    1. Modify the mode swith flow. Here avoid using Vvalid event wait. In power on, this Vvalid event
 *        would not happen because it will be cleared by basefw.  Change to polling the sensor MIPI
 *        clock lane is in low power state or not.  If in low power state, the next sensor mode settings
 *        could be sent. This modification could also speed up the sensor mode switch performance.
 *        The 1st output frame of sensor would get correct exp/gain and could be captured.
 *    2. Change snap flow to catch the 1st frame after mode switch.
 *
 *  ver. 0.00.0C, 20150116, Vincent Tsao
 *    1.  Correct the parameters for maximum frame rate function use.
 *    2.  Add the data lane HS trail timing adjustment. This adjustment could fix the issue that 
 *         some SPCA6350M EVB would have drop frame issue which results from the HD counter does not work correctly.
 *         The adjustment could enlarge the time interval before entering the low power state and make the front HD counter 
 *         work correctly again.
 *
 *  ver. 0.01.0D, 20150121, Vincent Tsao
 *    1.  Add the timeout in checking sensor MIPI whcih is at low power state or not.
 *    2.  Correct the polling MIPI low power state function.
 *
 *  ver. 0.01.0E, 20150202, Vincent Tsao
 *    1.  Correct the power up/down sequence of OV4689. (PWDNB must be controlled. Follow ov4689 power up case 1.)
 *    2.  Reset the sensor mode record(ucSenModeRec) when entering power save mode to
 *        avoid executing the group hold/end in mode switch.  It might cause sensor output
 *        incorrect data.
 *
 *  ver. 0.01.0F, 20150225, Vincent Tsao
 *     1. Update the HDR mode front parameters and the H total value in sensor register settings.
 *     2. Update the AE table of HDR mode(0x3A) and increase the Tv to 14 in full size (0x33) AE table.
 *
 *  ver. 0.01.10, 20150318, Vincent Tsao
 *     1. Add the log message when entering ov4689 power save function.
 *
 *  ver. 0.01.11, 20150407, Vincent Tsao
 *     1. Change the behavior when entering power save mode.  Here only pull down the 
 *        PWDN pin to make sensor enter low power mode.
 *     2. Remove the unexpected TGGPIO configuration using register address value writing dirrectly
 *        in "sensorDoFuncInit".
 *     3. Update the content of "sensorDoFuncStop".
 *
 *  ver. 0.01.12, 20150410, Vincent Tsao
 *     1. Add a new sensor mode 720x530@240fps. Replace the setttings of 0x3b sensor mode with this
 *         new sensor mode.
 *     2. Correct the Hoffset value of 0x37 and 0x3e sensor mode to make it FOV correct.
 *
 *  ver. 0.01.13, 20150521, Vincent Tsao
 *     1. Add a new sensor mode 672x378@240fps (full FOV).  Replace the settings of 0x3c sensor mode with
 *         this new sensor mode.
 *     2. Correct the value of PClk1x and HTotal.  The MIPI speed of sensor mode 0x3c is actually 360Mbps/lane.(Actually measured value).
 *
 *  ver. 0.01.14, 20150526, Vincent Tsao
 *     1. Remove the unnecessary time delay to speed up the initialization.
 *     2. Correct the PREVIEW_720P_30fps_SCALE_SENSOR_MIPIOUT_VSIZE.
 *
 *  ver. 0.01.15, 20150602, Vincent Tsao
 *     1. Replace the sensor mode 0x37 with new sensor mode settings 2176x1224 @30fps. (crop from full size, for LDC use.)
 *
 *  ver. 0.01.16, 20150608, Vincent Tsao
 *     1. Add waiting one Vd event in preview begin callback function.
 *
 *  ver. 0.01.17, 20150615, Vincent Tsao
 *     1. Modify the sensor output V size of 0x3b (720x540@240fps) to be 544. But V35 front only
 *         takes 540 lines. This is the workaround to fix the issue that the brightness of last line
 *         would be higher than others.
 *
 *  ver. 0.01.1A, 20150709, Vincent Tsao
 *     1. Change to use "sensorSerialAsyncWrite" to send the sensor register settings in sensorDoFuncInit 
 *         to avoid CPU waiting.
 *     2. Modify the time of polling low power state of MIPI clock lane to be 2ms.
 *     3. Add "SENSOR_SIF1DMA_BUF" to sensor register settings to enable SIF1 DMA to 
 *        send the sensor register settings.
 *     4. Waiting the 1 VD frame 1stly when entering previewbegin function from capture to preview.
 *         This could avoid that the timing of executing previewbegin is delayed and the VD wait would not
 *         get any signal in burst capture.
 *
 *  ver. 0.01.1B, 20150714, Vincent Tsao
 *     1. Correct the specified MIPI Vsize of HDR mode (0x3a) to be 3024. If using 3040,
 *         original VD falling would shift to next frame and cause the frame rate to be half
 *         of the original setting.
 *
 *  ver. 0.02.1C, 20150807, Vincent Tsao
 *     1. Add the sensor related information settings. ex: OB and sensitivity status of all sensor mode.
 *
 *  ver. 0.03.1D, 20150918, Vincent Tsao
 *     1. Add the serdes configuation to driver when OV4689 needs to connect to serdes.
 *
 *  ver. 0.04.1E, 20151117, Vincent Tsao
 *     1. Change the settings of sensor mode 0x32 (Cropped bin2 30fps) to use 720P@30fps binning sum settings.
 *
 *  ver. 0.05.01, 20151207, Vincent Tsao
 *     1. Add the configuration of OV4689 with connection to serdes(SPCA100/SPCA101).
 *     2. Change the output size from 1920x1080 to 2560x1440 when connecting to serdes.
 *     3. Add the hot plug in/out mechanism when connecting to serdes.
 *
 *  ver. 5.05.02, 20160219, Vincent Tsao
 *     1. Enable V35 MCLK for SPCA101 use. The HW of Raiden did not has crystal for SPCA101.
 *         The needed clock of spca101 is from V35.
 *
 *  ver. 5.05.04, 20160223, Vincent Tsao
 *     1. Add IOTrap value polling function for SPCA100 and SPCA101 before processing serdes
 *         initialization.
 *     2. Enable power save callback function.
 *
 *  ver. 5.05.05, 20160307, Vincent Tsao
 *     1. Change the Hsize and Vsize value for FIFO threshold calculation in serdes.
 *         The value must take offset into consideration.
 *
 *  ver. 5.05.06, 20160415, Vincent Tsao
 *     1. Change the HDR mode MIPI speed to 720Mbps/lane.
 *
 *  ver. 5.05.07, 20160901, Vincent Tsao
 *     1. Add the mirror flip configuration to sensor driver.
 *
 *  ver. 5.05.08, 20170330, Vincent Tsao
 *     1. Add the HTC fast capture flow to sensor driver.
 *
 *  ver. 5.05.09, 20170405, Vincent Tsao
 *     1. Move the front and cdsp reset after streaming on sensor. Because here the I2C before streaming on uses
 *         async write+flush.  Use front reset would influence the the I2C command correctness to sensor.
 *         Move the front and cdsp reset after streaming on sensor.
 *
 *  ver. 5.06.0A, 20170817, Vincent Tsao
 *     1. Add the function to check the sensor product ID(R2A version).
 *        The extra settings for R2A version are also added here. 
 *
 *  ver. 5.06.0B, 20170904, Vincent Tsao
 *     1. Add the mechanism that would send R2A extra setting to sensor directly and
 *        retry to read the OTP value when getting all 0 result in OTP value at 1st time.
 *
 *  ver. 5.06.0C, 20170904, Vincent Tsao
 *     1. Correct the 1st sensor mode check after boot in sensorDoFuncPreviewBegin. 
 *        This sensor mode check must be changed to match the real sensor mode setting
 *        used in sensorDoFuncInit or sensorDoFuncFastCapInit.
 *
 *  ver. 5.06.0D, 20170904, Vincent Tsao
 *     1. Change the g_ucPreSenMode to 0 when entering power save mode.
 *
 *  ver. 5.06.0E, 20171026, Vincent Tsao
 *     1. Change the g_ucR2aVerFlag initial value to be 0. This would always take ov4689 sensor
 *         as non-R2A(old) version.
 *
 *  ver. 5.07.0F, 20171106, Vincent Tsao
 *     1. Add to set the CDSP clock configuration in fast capture init.
 *        Move the reset front and cdsp before streaming on sensor.
 *
 *  ver. 5.07.10, 20171108, Vincent Tsao
 *     1. Remove the front and cdsp reset in "sensorDoFuncFastCapInit".
 *         This action is not needed in sensor driver now.
 **************************************************************************/

#define MODEL_VERSION "$unp_tg_ov4689-5.07.0F"
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
#include "../flow/sensor_custom.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_sensor_api.h"
#include "api/sp5k_dbg_api.h"
#include "middleware/proflog.h"

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
#include "serdes_driver/serdes_api.h"
#define IOTRAP_POLLINGCNT   200
#define IOTRAP_POLLINGTIME  5000/*ms*/
#endif

#define USEFUL 0
#define HDR_test 0	/*HDR test enable*/
#define CMD_LEN(a)			(sizeof(a)/sizeof(a[0]))

#define SENSOR_INFO_OB 		         SENSOR_CUST_INFO_00
#define SENSOR_INFO_SENSITIVITY_STS  SENSOR_CUST_INFO_01

#define OV4689_BLC_VAL   64 /*12bit*/

#if SP5K_DIQ_FRMWRK
#define USE_AGC_DELAY_SYNC			   (1)
#else
#define USE_AGC_DELAY_SYNC			   (0)
#endif

#if (SP5K_SENSOR_DUAL_MODE == 2)/*Dual raw case*/
#define DUAL_RAW                        1
#else
#define DUAL_RAW                        0
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


#define HTC_FAST_CAPTURE        0

#define MIPICLKLANE_POLLINGCNT   250
#define MIPICLKLANE_POLLINGTIME  2000/*ms*/
#define MIPICLKLANE_LPSTATE_MASK (1<<0)
#define MIPICLKLANE_LPSTATE      1


#define MIRROR_FLIP_EN  0/*0:normal,1:mirror,2:flip,3:mirror+flip*/

	/*MIPI clock convert */                                                 
	/*horiz. and vertical size*/  
	/*30fps*/
	#if 0
	#define PREVIEW_VGA_HSIZE						1344       
    #define PREVIEW_VGA_VSIZE						756       
	#define PREVIEW_VGA_HOFFSET					    0
    #define PREVIEW_VGA_VOFFSET 					2 
	#define PREVIEW_VGA_BAYERHS_SIZE				(0)			/*Dont scale*/
	#define PREVIEW_VGA_HOUTPUT 					(1344/4)
	#define PREVIEW_VGA_LINETOTAL					1568								
	#define PREVIEW_VGA_FRAMETOTAL 					797	
	#define PREVIEW_VGA_PCLKNX						37500
	#define PREVIEW_VGA_PCLK1X						120000
	#define PREVIEW_VGA_CDSPCLK 					1200
	#define PREVIEW_VGA_SENSOR_LINETOTAL			5018
	#define PREVIEW_VGA_FRAMERATE_KFPS 				30000

	#endif
	/*30fps*/ /* 0x30 */
 	#define PREVIEW_720P_30fps_SCALE_HSIZE               	1344		                   
	#define PREVIEW_720P_30fps_SCALE_VSIZE               	756			/*760*/
	/*Following size is only used when EOF or EOL generation counts EOL or pixels.*/  
	#define PREVIEW_720P_30fps_SCALE_SENSOR_MIPIOUT_HSIZE	1344						   
	#define PREVIEW_720P_30fps_SCALE_SENSOR_MIPIOUT_VSIZE	760 		/*760*/  	
	#define PREVIEW_720P_30fps_SCALE_HOFFSET				0  
	#define PREVIEW_720P_30fps_SCALE_VOFFSET				2   		/*2*/
	#define PREVIEW_720P_30fps_SCALE_BAYERHS_SIZE			(0)			/*Dont scale*/
	#define PREVIEW_720P_30fps_SCALE_HOUTPUT 				(1344/4) 
	#define PREVIEW_720P_30fps_SCALE_LINETOTAL				1255								
	#define PREVIEW_720P_30fps_SCALE_FRAMETOTAL 			797
	#define PREVIEW_720P_30fps_SCALE_PCLKNX 				30000
	#define PREVIEW_720P_30fps_SCALE_PCLK1X 				120000
	#define PREVIEW_720P_30fps_SCALE_CDSPCLK				0 /*use 0 to make basefw calculate the CDSP CLK.*/
	#define PREVIEW_720P_30fps_SCALE_SENSOR_LINETOTAL		5018
	#define PREVIEW_720P_30fps_SCALE_FRAMERATE_KFPS			30000
	#define PREVIEW_720P_30fps_SCALE_EXT_LINES				35

	/*60fps*/ /* 0x38 */
 	#define PREVIEW_720P_60fps_SCALE_HSIZE               	1344		                   
	#define PREVIEW_720P_60fps_SCALE_VSIZE               	756			/*760*/  
	#define PREVIEW_720P_60fps_SCALE_HOFFSET				0  
	#define PREVIEW_720P_60fps_SCALE_VOFFSET				2   		/*2*/
	#define PREVIEW_720P_60fps_SCALE_BAYERHS_SIZE			(0)			/*Dont scale*/
	#define PREVIEW_720P_60fps_SCALE_HOUTPUT 				(1344/4) 
	#define PREVIEW_720P_60fps_SCALE_LINETOTAL				376								
	#define PREVIEW_720P_60fps_SCALE_FRAMETOTAL 			1594
	#define PREVIEW_720P_60fps_SCALE_PCLKNX 				36000
	#define PREVIEW_720P_60fps_SCALE_PCLK1X 				144000
	#define PREVIEW_720P_60fps_SCALE_CDSPCLK				0 /*use 0 to make basefw calculate the CDSP CLK.*/
	#define PREVIEW_720P_60fps_SCALE_SENSOR_LINETOTAL		1252
	#define PREVIEW_720P_60fps_SCALE_FRAMERATE_KFPS			60130
	#define PREVIEW_720P_60fps_SCALE_EXT_LINES	 			42

	/*120fps*/ /* 0x31 */
 	#define PREVIEW_720P_120fps_SCALE_HSIZE               	1344		                   
	#define PREVIEW_720P_120fps_SCALE_VSIZE               	756			/*760*/  
	#define PREVIEW_720P_120fps_SCALE_HOFFSET				0  
	#define PREVIEW_720P_120fps_SCALE_VOFFSET				2   		/*2*/
	#define PREVIEW_720P_120fps_SCALE_BAYERHS_SIZE			(0)			/*Dont scale*/
	#define PREVIEW_720P_120fps_SCALE_HOUTPUT 				(1344/4) 
	#define PREVIEW_720P_120fps_SCALE_LINETOTAL				374								
	#define PREVIEW_720P_120fps_SCALE_FRAMETOTAL 			801
	#define PREVIEW_720P_120fps_SCALE_PCLKNX 				36000
	#define PREVIEW_720P_120fps_SCALE_PCLK1X 				144000
	#define PREVIEW_720P_120fps_SCALE_CDSPCLK				0
	#define PREVIEW_720P_120fps_SCALE_SENSOR_LINETOTAL		1246
	#define PREVIEW_720P_120fps_SCALE_FRAMERATE_KFPS 		120260
	#define PREVIEW_720P_120fps_SCALE_EXT_LINES				42

	/*30fps*/ /* 0x36 */
	#define PREVIEW_720P_SCALE_BINSUM_HSIZE 				1344						   
	#define PREVIEW_720P_SCALE_BINSUM_VSIZE 				756 		/*760*/  
	#define PREVIEW_720P_SCALE_BINSUM_HOFFSET				0  
	#define PREVIEW_720P_SCALE_BINSUM_VOFFSET				2			/*2*/
	#define PREVIEW_720P_SCALE_BINSUM_BAYERHS_SIZE			(0) 		/*Dont scale*/
	#define PREVIEW_720P_SCALE_BINSUM_HOUTPUT				(1344/4) 
	#define PREVIEW_720P_SCALE_BINSUM_LINETOTAL 			374 							
	#define PREVIEW_720P_SCALE_BINSUM_FRAMETOTAL			3205
	#define PREVIEW_720P_SCALE_BINSUM_PCLKNX				36000
	#define PREVIEW_720P_SCALE_BINSUM_PCLK1X				144000
	#define PREVIEW_720P_SCALE_BINSUM_CDSPCLK				0
	#define PREVIEW_720P_SCALE_BINSUM_SENSOR_LINETOTAL		1246
	#define PREVIEW_720P_SCALE_BINSUM_FRAMERATE_KFPS		30050
	#define PREVIEW_720P_SCALE_BINSUM_EXT_LINES 			42

	/*120fps*/ /* 0x32 */
	#define PREVIEW_720P_CROP_HSIZE               	1280 		/*1344*/		                   
	#define PREVIEW_720P_CROP_VSIZE               	720  		/*760*/
	#define PREVIEW_720P_CROP_HOFFSET				32			/*32*/
	#define PREVIEW_720P_CROP_VOFFSET				20			/*20*/
	#define PREVIEW_720P_CROP_BAYERHS_SIZE			(0)			/*Dont scale*/
	#define PREVIEW_720P_CROP_HOUTPUT 				(1344/4)
	#define PREVIEW_720P_CROP_LINETOTAL				376							
	#define PREVIEW_720P_CROP_FRAMETOTAL 			797
	#define PREVIEW_720P_CROP_PCLKNX 				36000
	#define PREVIEW_720P_CROP_PCLK1X 				144000
	#define PREVIEW_720P_CROP_CDSPCLK				0
	#define PREVIEW_720P_CROP_SENSOR_LINETOTAL		1252
	#define PREVIEW_720P_CROP_FRAMERATE_KFPS		120260
	#define PREVIEW_720P_CROP_EXT_LINES				42


	/*30fps*/
	#define PREVIEW_QUARTER_HSIZE               	1344 			                   
	#define PREVIEW_QUARTER_VSIZE               	756 
		/*Following size is only used when EOF or EOL generation counts EOL or pixels.*/  
	#define PREVIEW_QUARTER_SENSOR_MIPIOUT_HSIZE	1344							   
	#define PREVIEW_QUARTER_SENSOR_MIPIOUT_VSIZE	760 	
	#define PREVIEW_QUARTER_HOFFSET					0			                            
	#define PREVIEW_QUARTER_VOFFSET					2			                            
	#define PREVIEW_QUARTER_BAYERHS_SIZE			(0)			/*Dont scale*/                 
	#define PREVIEW_QUARTER_HOUTPUT 				(1344/4)                                 
	#define PREVIEW_QUARTER_LINETOTAL				1255							                       
	#define PREVIEW_QUARTER_FRAMETOTAL 				797                                      
	#define PREVIEW_QUARTER_PCLKNX 					30000                                      
	#define PREVIEW_QUARTER_PCLK1X 					120000                                     
	#define PREVIEW_QUARTER_CDSPCLK					1200     
	#define PREVIEW_QUARTER_SENSOR_LINETOTAL		5018
	#define PREVIEW_QUARTER_FRAMERATE_KFPS			30000
	#define PREVIEW_QUARTER_EXT_LINES				35


	/*30fps*/ /* 0x37 */
	#define PREVIEW_QUARTER1440P30_HSIZE            1008
	#define PREVIEW_QUARTER1440P30_VSIZE            756		
	#define PREVIEW_QUARTER1440P30_HOFFSET 			168 
	#define PREVIEW_QUARTER1440P30_VOFFSET 			2			
	#define PREVIEW_QUARTER1440P30_BAYERHS_SIZE		(0)			/*Dont scale*/
	#define PREVIEW_QUARTER1440P30_HOUTPUT 			(1008/4) 
	#define PREVIEW_QUARTER1440P30_LINETOTAL		376								
	#define PREVIEW_QUARTER1440P30_FRAMETOTAL		3195 
	#define PREVIEW_QUARTER1440P30_PCLKNX			36000
	#define PREVIEW_QUARTER1440P30_PCLK1X			144000
	#define PREVIEW_QUARTER1440P30_CDSPCLK 			0
	#define PREVIEW_QUARTER1440P30_SENSOR_LINETOTAL	1252
	#define PREVIEW_QUARTER1440P30_FRAMERATE_KFPS	29998
	#define PREVIEW_QUARTER1440P30_EXT_LINES		42


	/*60fps*/ /* 0x3E */
	#define PREVIEW_QUARTER1440P60_HSIZE            1008
	#define PREVIEW_QUARTER1440P60_VSIZE            756		
	#define PREVIEW_QUARTER1440P60_HOFFSET 			168 
	#define PREVIEW_QUARTER1440P60_VOFFSET 			2			
	#define PREVIEW_QUARTER1440P60_BAYERHS_SIZE		(0)			/*Dont scale*/
	#define PREVIEW_QUARTER1440P60_HOUTPUT 			(1008/4) 
	#define PREVIEW_QUARTER1440P60_LINETOTAL		376								
	#define PREVIEW_QUARTER1440P60_FRAMETOTAL		1594 
	#define PREVIEW_QUARTER1440P60_PCLKNX			36000
	#define PREVIEW_QUARTER1440P60_PCLK1X			144000
	#define PREVIEW_QUARTER1440P60_CDSPCLK 			0
	#define PREVIEW_QUARTER1440P60_SENSOR_LINETOTAL	1252
	#define PREVIEW_QUARTER1440P60_FRAMERATE_KFPS	60140
	#define PREVIEW_QUARTER1440P60_EXT_LINES		42


	/*60fps*/ /* 0x34 */
	#define PREVIEW_1080P_HSIZE              		1920		
	#define PREVIEW_1080P_VSIZE  	         		1080	 	
	#define PREVIEW_1080P_HOFFSET					0						 
	#define PREVIEW_1080P_VOFFSET					0 
	#define PREVIEW_1080P_BAYERHS_SIZE				(0)			/*Dont scale*/
	#define PREVIEW_1080P_HOUTPUT 					(1920/4)
	#define PREVIEW_1080P_LINETOTAL 				521					
	#define PREVIEW_1080P_FRAMETOTAL				1230				 
	#define PREVIEW_1080P_PCLKNX 					38400
	#define PREVIEW_1080P_PCLK1X 					153600
	#define PREVIEW_1080P_CDSPCLK					0
	#define PREVIEW_1080P_SENSOR_LINETOTAL			1626
	#define PREVIEW_1080P_FRAMERATE_KFPS			60000
	#define PREVIEW_1080P_EXT_LINES					40


	/*30fps*/ /* Removed */
	#define PREVIEW_1440P30_HSIZE                	2016
	#define PREVIEW_1440P30_VSIZE                	1512		
	#define PREVIEW_1440P30_HOFFSET 				336 
	#define PREVIEW_1440P30_VOFFSET 				4			
	#define PREVIEW_1440P30_BAYERHS_SIZE			(0)			/*Dont scale*/
	#define PREVIEW_1440P30_HOUTPUT 				(2016/4) 
	#define PREVIEW_1440P30_LINETOTAL				771								
	#define PREVIEW_1440P30_FRAMETOTAL				3108 
	#define PREVIEW_1440P30_PCLKNX					72000
	#define PREVIEW_1440P30_PCLK1X					288000
	#define PREVIEW_1440P30_CDSPCLK 				0
	#define PREVIEW_1440P30_SENSOR_LINETOTAL		1284
	#define PREVIEW_1440P30_FRAMERATE_KFPS			30070
	#define PREVIEW_1440P30_EXT_LINES				42


	/*60fps*/ /* 0x3D */
	#define PREVIEW_1440P60_HSIZE                	2016
	#define PREVIEW_1440P60_VSIZE                	1512		
	#define PREVIEW_1440P60_HOFFSET 				336 
	#define PREVIEW_1440P60_VOFFSET 				4			
	#define PREVIEW_1440P60_BAYERHS_SIZE			(0)			/*Dont scale*/
	#define PREVIEW_1440P60_HOUTPUT 				(2016/4) 
	#define PREVIEW_1440P60_LINETOTAL				770								
	#define PREVIEW_1440P60_FRAMETOTAL				1556 
	#define PREVIEW_1440P60_PCLKNX					72000
	#define PREVIEW_1440P60_PCLK1X					288000
	#define PREVIEW_1440P60_CDSPCLK 				0
	#define PREVIEW_1440P60_SENSOR_LINETOTAL		1282	
	#define PREVIEW_1440P60_FRAMERATE_KFPS			60140
	#define PREVIEW_1440P60_EXT_LINES				42


	/*75fps*/
	#define PREVIEW_FULL_HSIZE 						2688		
	#define PREVIEW_FULL_VSIZE 						1512		
	#define PREVIEW_FULL_HOFFSET					0						 
	#define PREVIEW_FULL_VOFFSET					4  
	#define PREVIEW_FULL_BAYERHS_SIZE				(0)			/*Dont scale*/
	#define PREVIEW_FULL_HOUTPUT 					(2688/4)
	#define PREVIEW_FULL_LINETOTAL					929
	#define PREVIEW_FULL_FRAMETOTAL 				1554				 
	#define PREVIEW_FULL_PCLKNX						108000  
	#define PREVIEW_FULL_PCLK1X						350000
	#define PREVIEW_FULL_CDSPCLK					3500
	#define PREVIEW_FULL_SENSOR_LINETOTAL 			1032
	#define PREVIEW_FULL_FRAMERATE_KFPS				74830

	/*30fps*/ /* 0x36 */
	#define PREVIEW_Full30_HSIZE                	2688
	#define PREVIEW_Full30_VSIZE                	1512		/*1520*/
	#define PREVIEW_Full30_HOFFSET 					0 
	#define PREVIEW_Full30_VOFFSET 					4			/*4*/
	#define PREVIEW_Full30_BAYERHS_SIZE				(0)			/*Dont scale*/
	#define PREVIEW_Full30_HOUTPUT 					(2688/4) 
	#define PREVIEW_Full30_LINETOTAL				771								
	#define PREVIEW_Full30_FRAMETOTAL				3108 
	#define PREVIEW_Full30_PCLKNX					72000
	#define PREVIEW_Full30_PCLK1X					288000
	#define PREVIEW_Full30_CDSPCLK 					0
	#define PREVIEW_Full30_SENSOR_LINETOTAL			1284
	#define PREVIEW_Full30_FRAMERATE_KFPS 			30070
	#define PREVIEW_Full30_EXT_LINES				42


	/*30fps*/ /* 0x37 */
	#define PREVIEW_Full30_CROP_HSIZE					2176
	#define PREVIEW_Full30_CROP_VSIZE					1224
	#define PREVIEW_Full30_CROP_HOFFSET					256    /*(2688-2176)/2=*/
	#define PREVIEW_Full30_CROP_VOFFSET					148	   /*(1520-1224)/2=148*/
	#define PREVIEW_Full30_CROP_BAYERHS_SIZE 			(0)    /*Dont scale*/
	#define PREVIEW_Full30_CROP_HOUTPUT					(2688/4) 
	#define PREVIEW_Full30_CROP_LINETOTAL				771 							
	#define PREVIEW_Full30_CROP_FRAMETOTAL				3108 
	#define PREVIEW_Full30_CROP_PCLKNX					72000
	#define PREVIEW_Full30_CROP_PCLK1X					288000
	#define PREVIEW_Full30_CROP_CDSPCLK					0
	#define PREVIEW_Full30_CROP_SENSOR_LINETOTAL 		1284
	#define PREVIEW_Full30_CROP_FRAMERATE_KFPS			30070
	#define PREVIEW_Full30_CROP_EXT_LINES				42

	
	/*27.5fps*/ /* 0x35 */
	#define PREVIEW_Full27p5_HSIZE                	2688
	#define PREVIEW_Full27p5_VSIZE                	1512		
	#define PREVIEW_Full27p5_HOFFSET 				0 
	#define PREVIEW_Full27p5_VOFFSET 				4			
	#define PREVIEW_Full27p5_BAYERHS_SIZE			(0)			/*Dont scale*/
	#define PREVIEW_Full27p5_HOUTPUT 				(2688/4) 
	#define PREVIEW_Full27p5_LINETOTAL				771								
	#define PREVIEW_Full27p5_FRAMETOTAL				3396 
	#define PREVIEW_Full27p5_PCLKNX					72000
	#define PREVIEW_Full27p5_PCLK1X					288000
	#define PREVIEW_Full27p5_CDSPCLK 				0
	#define PREVIEW_Full27p5_SENSOR_LINETOTAL 		1284
	#define PREVIEW_Full27p5_FRAMERATE_KFPS			27520
	#define PREVIEW_Full27p5_EXT_LINES	 			42


	/*55fps*/ /* 0x39 */
	#define PREVIEW_Full55_HSIZE                	2688
	#define PREVIEW_Full55_VSIZE                	1512		
	#define PREVIEW_Full55_HOFFSET 					0 
	#define PREVIEW_Full55_VOFFSET 					4			
	#define PREVIEW_Full55_BAYERHS_SIZE				(0)			/*Dont scale*/
	#define PREVIEW_Full55_HOUTPUT 					(2688/4) 
	#define PREVIEW_Full55_LINETOTAL				771								
	#define PREVIEW_Full55_FRAMETOTAL				1698 
	#define PREVIEW_Full55_PCLKNX					72000
	#define PREVIEW_Full55_PCLK1X					288000
	#define PREVIEW_Full55_CDSPCLK 					0
	#define PREVIEW_Full55_SENSOR_LINETOTAL			1284
	#define PREVIEW_Full55_FRAMERATE_KFPS 			55040
	#define PREVIEW_Full55_EXT_LINES				42


	/*60fps*/ /* 0x33 */
	#define PREVIEW_Full60_HSIZE                	2688
	#define PREVIEW_Full60_VSIZE                	1512		/*1520*/
	#define PREVIEW_Full60_HOFFSET 					0 
	#define PREVIEW_Full60_VOFFSET 					4			/*4*/
	#define PREVIEW_Full60_BAYERHS_SIZE				(0)			/*Dont scale*/
	#define PREVIEW_Full60_HOUTPUT 					(2688/4) 
	#define PREVIEW_Full60_LINETOTAL				770								
	#define PREVIEW_Full60_FRAMETOTAL				1556 
	#define PREVIEW_Full60_PCLKNX					72000
	#define PREVIEW_Full60_PCLK1X					288000
	#define PREVIEW_Full60_CDSPCLK 					0
	#define PREVIEW_Full60_SENSOR_LINETOTAL			1282
	#define PREVIEW_Full60_FRAMERATE_KFPS			60140
	#define PREVIEW_Full60_EXT_LINES				42

	/*60fps, 2lane case*/ /* */
	#define PREVIEW_FullHD60_2LANE_HSIZE			2560
	#define PREVIEW_FullHD60_2LANE_VSIZE			1440
	#define PREVIEW_FullHD60_2LANE_HOFFSET			0 
	#define PREVIEW_FullHD60_2LANE_VOFFSET			0			
	#define PREVIEW_FullHD60_2LANE_BAYERHS_SIZE 	(0) 		/*Dont scale*/
	#define PREVIEW_FullHD60_2LANE_HOUTPUT			(2560/4) 
#if (!SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	#define PREVIEW_FullHD60_2LANE_LINETOTAL		1828 							
	#define PREVIEW_FullHD60_2LANE_FRAMETOTAL		1486 
	#define PREVIEW_FullHD60_2LANE_PCLKNX			81600
	#define PREVIEW_FullHD60_2LANE_PCLK1X			163200
	#define PREVIEW_FullHD60_2LANE_CDSPCLK			0
	#define PREVIEW_FullHD60_2LANE_SENSOR_LINETOTAL 2688
	#define PREVIEW_FullHD60_2LANE_FRAMERATE_KFPS	30040
	#define PREVIEW_FullHD60_2LANE_EXT_LINES		37
#else
	#define PREVIEW_FullHD60_2LANE_LINETOTAL		1613	
	#define PREVIEW_FullHD60_2LANE_FRAMETOTAL		1486 
	#define PREVIEW_FullHD60_2LANE_PCLKNX			72000
	#define PREVIEW_FullHD60_2LANE_PCLK1X			144000	
	#define PREVIEW_FullHD60_2LANE_CDSPCLK			0
	#define PREVIEW_FullHD60_2LANE_SENSOR_LINETOTAL 2688
	#define PREVIEW_FullHD60_2LANE_FRAMERATE_KFPS	30040
	#define PREVIEW_FullHD60_2LANE_EXT_LINES		37
#endif

	/*120fps*/ /* 0x3F */
	#define PREVIEW_QUARTER_VBIN_HFULL_HSIZE			2688
	#define PREVIEW_QUARTER_VBIN_HFULL_VSIZE			756
	#define PREVIEW_QUARTER_VBIN_HFULL_HOFFSET			0 
	#define PREVIEW_QUARTER_VBIN_HFULL_VOFFSET			2			
	#define PREVIEW_QUARTER_VBIN_HFULL_BAYERHS_SIZE 	(0) 		
	#define PREVIEW_QUARTER_VBIN_HFULL_HOUTPUT			(2688/4) 
	#define PREVIEW_QUARTER_VBIN_HFULL_LINETOTAL		749							
	#define PREVIEW_QUARTER_VBIN_HFULL_FRAMETOTAL		801 
	#define PREVIEW_QUARTER_VBIN_HFULL_PCLKNX			72000
	#define PREVIEW_QUARTER_VBIN_HFULL_PCLK1X			288000
	#define PREVIEW_QUARTER_VBIN_HFULL_CDSPCLK			0
	#define PREVIEW_QUARTER_VBIN_HFULL_SENSOR_LINETOTAL 1248
	#define PREVIEW_QUARTER_VBIN_HFULL_FRAMERATE_KFPS	120010
	#define PREVIEW_QUARTER_VBIN_HFULL_EXT_LINES		42

	/*240fps*/ /* 0x3B */
	#define PREVIEW_QUARTER_CROP_240_HSIZE				720
	#define PREVIEW_QUARTER_CROP_240_VSIZE				540
	#define PREVIEW_QUARTER_CROP_240_HOFFSET			0 
	#define PREVIEW_QUARTER_CROP_240_VOFFSET			0			
	#define PREVIEW_QUARTER_CROP_240_BAYERHS_SIZE 		(0) 		
	#define PREVIEW_QUARTER_CROP_240_HOUTPUT			(720/4) 
	#define PREVIEW_QUARTER_CROP_240_LINETOTAL			211 						
	#define PREVIEW_QUARTER_CROP_240_FRAMETOTAL			593 /*593 is because the frame total must be 4N+1.*/
	#define PREVIEW_QUARTER_CROP_240_PCLKNX				30000
	#define PREVIEW_QUARTER_CROP_240_PCLK1X				120000
	#define PREVIEW_QUARTER_CROP_240_CDSPCLK			0
	#define PREVIEW_QUARTER_CROP_240_SENSOR_LINETOTAL 	843
	#define PREVIEW_QUARTER_CROP_240_FRAMERATE_KFPS		240050
	#define PREVIEW_QUARTER_CROP_240_EXT_LINES			47

	/*Binning 4, 240fps*/ /* 0x3C */
	#define PREVIEW_BIN4_HSIZE					672
	#define PREVIEW_BIN4_VSIZE					378
	#define PREVIEW_BIN4_HOFFSET				0 
	#define PREVIEW_BIN4_VOFFSET				2 /*(380-378)/2=1, take 2.*/			
	#define PREVIEW_BIN4_BAYERHS_SIZE			(0) 		
	#define PREVIEW_BIN4_HOUTPUT				(672/4) 
	#define PREVIEW_BIN4_LINETOTAL				258 						
	#define PREVIEW_BIN4_FRAMETOTAL 			581 /*581 is because the frame total must be 4N+1.*/
	#define PREVIEW_BIN4_PCLKNX 				36000
	#define PREVIEW_BIN4_PCLK1X 				144000
	#define PREVIEW_BIN4_CDSPCLK				1180/*Temporarily assign the CDSP clock. Using basefw calculaed value would has error.*/
	#define PREVIEW_BIN4_SENSOR_LINETOTAL		860
	#define PREVIEW_BIN4_FRAMERATE_KFPS 		240160
	#define PREVIEW_BIN4_EXT_LINES				46


#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	/*30fps*//*0x40*/
	/* For Capture 1920x1080 */
	#define CAPTURE_HSIZE							PREVIEW_FullHD60_2LANE_HSIZE		
	#define CAPTURE_VSIZE							PREVIEW_FullHD60_2LANE_VSIZE		
	#define CAPTURE_HOFFSET 						PREVIEW_FullHD60_2LANE_HOFFSET 
	#define CAPTURE_VOFFSET 						PREVIEW_FullHD60_2LANE_VOFFSET 
	#define CAPTURE_BAYERHS_SIZE					PREVIEW_FullHD60_2LANE_BAYERHS_SIZE		/*Dont scale*/
	#define CAPTURE_HOUTPUT 						PREVIEW_FullHD60_2LANE_HOUTPUT
	#define CAPTURE_LINETOTAL						PREVIEW_FullHD60_2LANE_LINETOTAL 	
	#define CAPTURE_FRAMETOTAL						PREVIEW_FullHD60_2LANE_FRAMETOTAL
	#define CAPTURE_PCLKNX							PREVIEW_FullHD60_2LANE_PCLKNX
	#define CAPTURE_PCLK1X							PREVIEW_FullHD60_2LANE_PCLK1X
	#define CAPTURE_CDSPCLK 						PREVIEW_FullHD60_2LANE_CDSPCLK
	#define CAPTURE_SENSOR_LINETOTAL				PREVIEW_FullHD60_2LANE_SENSOR_LINETOTAL
	#define CAPTURE_FRAMERATE_KFPS					PREVIEW_FullHD60_2LANE_FRAMERATE_KFPS
	#define CAPTURE_EXT_LINES						32
#else
	/*60fps*//*0x40*/
    /* For Capture 2688x1512 */
	#define CAPTURE_HSIZE							2688		
	#define CAPTURE_VSIZE							1512		
	#define CAPTURE_HOFFSET 						0 
	#define CAPTURE_VOFFSET							4 
	#define CAPTURE_BAYERHS_SIZE					(0)			/*Dont scale*/
	#define CAPTURE_HOUTPUT							(2688/4)
	#define CAPTURE_LINETOTAL						771		
	#define CAPTURE_FRAMETOTAL						1554
	#define CAPTURE_PCLKNX 							72000
	#define CAPTURE_PCLK1X 							288000
	#define CAPTURE_CDSPCLK							0
	#define CAPTURE_SENSOR_LINETOTAL				1284
	#define CAPTURE_FRAMERATE_KFPS					60140
	#define CAPTURE_EXT_LINES						32
#endif

	/*90fps*/
	/*For HDR mode 1280x720*/
	#define HDR720p_HSIZE								1280		
	#define HDR720p_VSIZE								2160		
	#define HDR720p_HOFFSET 							0 
	#define HDR720p_VOFFSET								0 
	#define HDR720p_BAYERHS_SIZE						(640)		/*scale*/
	#define HDR720p_HOUTPUT								(1280/4)
	#define HDR720p_LINETOTAL							1075		
	#define HDR720p_FRAMETOTAL							773
	#define HDR720p_PCLKNX 								75000
	#define HDR720p_PCLK1X 								240000
	#define HDR720p_CDSPCLK								3000
	#define HDR720p_SENSOR_LINETOTAL					1720
	#define HDR720p_FRAMERATE_KFPS						90250

	/*71fps*/
	/*For HDR mode 2688x1520*/
	#define HDR_HSIZE								2688		
	#define HDR_VSIZE								4560		/*1520*/
	#define HDR_HOFFSET 							0 
	#define HDR_VOFFSET								0 
	#define HDR_BAYERHS_SIZE						(1344)		/*scale*/
	#define HDR_HOUTPUT								(2688/4)
	#define HDR_LINETOTAL							925		
	#define HDR_FRAMETOTAL							1554
	#define HDR_PCLKNX 								102000
	#define HDR_PCLK1X 								327000
	#define HDR_CDSPCLK								3270
	#define HDR_SENSOR_LINETOTAL					1088
	#define HDR_FRAMERATE_KFPS						70970

	/* 60fps */ /*0x3A*/
	/*For HDR 2mode  2688x1520*/
	#define HDR2mode_HSIZE							2688		
	#define HDR2mode_VSIZE							3024		/*3040*/
		/*Following size is only used when EOF or EOL generation counts EOL or pixels.*/  
	#define HDR2mode_SENSOR_MIPIOUT_HSIZE			2688		
	#define HDR2mode_SENSOR_MIPIOUT_VSIZE			3024
	#define HDR2mode_HOFFSET 						0 
	#define HDR2mode_VOFFSET						0 
	#define HDR2mode_BAYERHS_SIZE					(0)		/*scale*/
	#define HDR2mode_HOUTPUT						(2688/4)
	#define HDR2mode_LINETOTAL						770		
	#define HDR2mode_FRAMETOTAL						1558
	#define HDR2mode_PCLKNX 						72000
	#define HDR2mode_PCLK1X 						288000
	#define HDR2mode_CDSPCLK						0
	#define HDR2mode_SENSOR_LINETOTAL 				1282
	#define HDR2mode_FRAMERATE_KFPS					60000
	#define HDR2mode_EXT_LINES		 				44

/*****************************************************************************************************/

/**************sensorCfgInit table define ****************************/
#define SENSORMODEL						 TYPE_CMOS				/* TYPE_CMOS,TYPE_SONYCCD,TYPE_SHARPCCD,TYPE_SHARPNEWCCD,TYPE_PANACCD,
						                                                        		TYPE_SONYNEWCCD,TYPE_PANANEWCCD,TYPE_FOVEONCMOS,TYPE_FFMCCD*/
#define	TGTYPE			         		 TG_CMOS_SP5K_SLAVE		/* TG_CMOS_SP5K_SLAVE ,TG_CMOS_SP5K_MASTER,TG_CCD_EXT_SP5K_SLAVE,
														     		TG_CCD_EXT_SP5K_MASTER ,TG_CCD_INT*/

#define	PREVIEW_VGA_CFAPATTERN	         BAYER_BGGR     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define PREVIEW_720P_CFAPATTERN          BAYER_BGGR
#define PREVIEW_QUARTER_CFAPATTERN       BAYER_BGGR
#define PREVIEW_QUARTER1440P_CFAPATTERN  BAYER_BGGR
#define PREVIEW_1080P_CFAPATTERN         BAYER_BGGR
#define PREVIEW_FULL_CFAPATTERN          BAYER_BGGR
#define PREVIEW_BIN4_CFAPATTERN  		 BAYER_BGGR

#define	SNAP_CFAPATTERN					 BAYER_BGGR     /* BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,*/
#define	SNAP_DUMMYVD_NUM						  0     /* snap dummy Vd need numbers*/            
#define	SNAP_READOUTVD_NUM		        		  1     /* snap readout Vd need numbers*/             
#define	SNAP_CAPTUREVD_NUM		        		  1     /* for typical CCD==n_dummy + n_readout + 1*/ 
#define	SNAP_EARLY_NUM		                      0	    /* for the sony CCD snap mode,early control VD numbers of the subctrl signal*/
#define	FLDSEL_START_NUM	                      0		/* It arrange the start field data to the start position of the sdram,0,1,2 */
#define	FLDSEL_END_NUM							  0	 	/* It arrange the end field data to the end position of the sdram,0,1,2 */
#define	SKIP_SNAP_READOUTVD_EN                    4		/*0*//* a vd between still exp and readout,'1':cmos frame mode,'0':ccd,cmos rolling mode*/

#define	VSHUTTER_RESEND 	                      0		/* sony FALSE;panasonic/ITG TRUE*/
#if  USE_AGC_DELAY_SYNC
#define EXP_FRAME_NUM							  1
#else
#define	EXP_FRAME_NUM       	                  1/*2*/
#endif	  	
#define	PREVIEW_VGA_HDNUM_CMDPORT		        (400)		
#define	PREVIEW_720P_HDNUM_CMDPORT		        (400)
#define	PREVIEW_QUARTER_HDNUM_CMDPORT	        (400)
#define	PREVIEW_QUARTER1440P_HDNUM_CMDPORT	    (400)	
#define	PREVIEW_1080P_HDNUM_CMDPORT	            (400)
#define	PREVIEW_1440P_HDNUM_CMDPORT	            (400)	
#define	PREVIEW_FULL_HDNUM_CMDPORT	            (400)
#define PREVIEW_QUARTER_CROP_240_HDNUM_CMDPORT  (250)
#define PREVIEW_BIN4_HDNUM_CMDPORT  			(160)
#define	CAPTURE_HDNUM_CMDPORT	                (400)  
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
    #define	TG_SPI_I2C_MODE     0x6C                /* I2C slave addr for write */
    #define	TG_FORMAT2          FRONT_SSC_READ_HAS_SUBADDR |SENSOR_SERIAL_FORMAT2_SIF1_PIN_TG0
    #define	TG_FORMAT3          0
    #define	TG_SPEED            0  /*RegClk/((TGSPEED*4)+4)/2, (TGSPEED*4) must be greater than 6.
									   TGSPEED multiple 4  is by fw.*/
    #define	TG_NBIT_REGADDR     16                  /* ssc/spi register address numbers */
    #define	TG_NBIT_REGDATA     8                   /* ssc/spi register data numbers */
	#define TG_SPI_I2C_FREQ_KHz		400
/*prototypes of sensor model methods*/
static void sensorCfgInit();
static void sensorCfgStop();
#if (HTC_FAST_CAPTURE)
static void sensorDoFuncFastCapInit();
#else
static void sensorDoFuncInit();
#endif
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
#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
static UINT32 sensorStsCheck(void);
/*static UINT32 sensorConnectProcess(void);*/
extern void sensor_vd_watchdog_start(UINT32 pri);
extern void sensor_vd_watchdog_stop(UINT32 pri);
#endif
/*static void sensor_exp_write_redo(UINT32 smode,SINT32 *expIdx,UINT32 *vcount,UINT8 *vtbl_type,UINT32 *int_us);*/
static void sensor_cmdport_begin(UINT32 smode,UINT32 expIdx,UINT32 agcIdx,UINT32 exp2Idx,UINT32 agc2Idx);
static void sensor_cmdport_end(void);
static void sensorDoFuncSensorMIPIClkLaneChk();

extern UINT32 sensorModeGet();
extern void halFrontDataPinRemapCfg(UINT32 remap0,UINT32 remap1,UINT32 remap2,UINT32 remap3);
extern void sensor_PregainDelay(UINT8 flag);
void sensor_HDR_EXP_do(void);

/*static UINT32 sensorSuspended;*/
static BOOL   fpvsusp;
static UINT8  HDR_ev;
static const UINT8 *gpdata;
static UINT8 ucSenModeRec=0xff;
static UINT8 g_ucBoot1st=0;
static UINT8 g_ucIsSnap2Pv=0;

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
#define DEF_SENSOR_MODE       0x30 
#define DEF_INIT_AFE_CODE     cmd_full_hd_60fps_2lane_code
#define DEF_SYNC_SIG_CFG      sensorSyncSigPreviewFullHD_60FPS_2LANECfg
#define DEF_PREV_MODE_CFG     sensorTgPreviewFullHD_60FPS_2LANECfg
#else
#define DEF_SENSOR_MODE       0x30 
#define DEF_INIT_AFE_CODE     cmd_720P_60fps_code
#define DEF_SYNC_SIG_CFG      sensorSyncSigPreview720P_30fps_SCALECfg
#define DEF_PREV_MODE_CFG     sensorTgPreview720P_30fps_SCALECfg
#endif

static const UINT32 PrevSensitivityTbl[]={1000/*mode 0x30*/,1000/*mode 0x31*/,1000/*mode 0x32*/,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000};
static const UINT32 SnapSensitivityTbl[]={1000/*mode 0x40*/};
#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
static UINT32 g_Cmdportids100;
static UINT32 g_Cmdportids101;
UINT8 g_ucRetryInit=0;
#define VD_WATCHDOG_EN 1
#endif

static UINT16 g_uwSensorID=0;
static UINT8 g_ucR2aVerFlag=0x0;/*Set the version flag to be 0 would make flow to use old sensor configuration flow.*/
static UINT8 g_ucPreSenMode=0xff;
/******************** uCode ****************************/
static UINT8 g_ucMiddleExpData[][3] SENSOR_SIF1DMA_VARBUF = {
	{0x35,0x0A,0x00},
	{0x35,0x0B,0x00},
	{0x35,0x0C,0x00},
};

static UINT8 g_ucShortExpData[][3] SENSOR_SIF1DMA_VARBUF = {
	{0x35,0x10,0x00},
	{0x35,0x11,0x00},
	{0x35,0x12,0x00},
};

static const UINT8 cmd_init_code_SWRstOn[][3] SENSOR_SIF1DMA_BUF ={
	{0x01,0x03, 0x01}
};
static const UINT8 cmd_init_code_SWRstOff[][3] SENSOR_SIF1DMA_BUF ={
	{0x01,0x03, 0x00}
};
static const UINT8 cmd_init_code_StreamOn[][3] SENSOR_SIF1DMA_BUF ={
	{0x01,0x00, 0x01}
};
static const UINT8 cmd_init_code_StreamOff[][3] SENSOR_SIF1DMA_BUF ={
	{0x01,0x00, 0x00}
};

static const UINT8 cmd_bin_sum_en[][3] SENSOR_SIF1DMA_BUF ={
	{0x36,0x31, 0x40} /*0x3631 bit 5,  0: enable bin sum, 1: bin avg.*/
};

static const UINT8 cmd_bin_avg_en[][3] SENSOR_SIF1DMA_BUF ={
	{0x36,0x31, 0x60} /*0x3631 bit 5,  0: enable bin sum, 1: bin avg.*/
};

static const UINT8 cmd_sensor_otp_memory_read[][3] SENSOR_SIF1DMA_BUF ={
	/*{0x01,0x00,0x01},*/
	{0x50,0x00,0xd3},
	{0x3d,0x84,0x40},/*OTP manual mode*/

	{0x3d,0x88,0x70},/*Start addr*/
	{0x3d,0x89,0x00},

	{0x3d,0x8a,0x70},/*End addr*/
	{0x3d,0x8b,0x0f},
	
	{0x3d,0x81,0x01},/*OTP load enable*/
};

static const UINT8 cmd_stream_off_extra_setting_1_for_r2a_ver[][3] SENSOR_SIF1DMA_BUF ={
	{0x30,0x1a,0xf9},
	{0x48,0x05,0x03},
};

static const UINT8 cmd_stream_on_extra_setting_1_for_r2a_ver[][3] SENSOR_SIF1DMA_BUF ={
	{0x30,0x1a,0xf9},
};

static const UINT8 cmd_stream_on_extra_setting_2_for_r2a_ver[][3] SENSOR_SIF1DMA_BUF ={
	{0x30,0x1a,0xf1},
	{0x48,0x05,0x00},
	{0x30,0x1a,0xf0},
};

static const UINT8 cmd_extra_setting_1_for_r2a_ver[][3] SENSOR_SIF1DMA_BUF ={
	{0x30,0x3f,0x00},
	{0x36,0x03,0x00},
	{0x38,0x0e,0x12},
	{0x38,0x0f,0x36},
	{0x40,0x00,0x71},
	{0x45,0x03,0x02},
	{0x31,0x05,0x31},
	{0x30,0x1a,0xf9},
	{0x48,0x4b,0x05},
	{0x48,0x05,0x03},
	{0x36,0x01,0x01},
	{0x37,0x45,0xc0},
	{0x37,0x98,0x1b},
};

static const UINT8 cmd_extra_setting_2_for_r2a_ver[][3] SENSOR_SIF1DMA_BUF ={
	{0x31,0x05,0x11},
	{0x30,0x1a,0xf1},
	{0x48,0x05,0x00},
	{0x30,0x1a,0xf0},
	{0x32,0x08,0x00},
	{0x30,0x2a,0x00},
	{0x30,0x2a,0x00},
	{0x30,0x2a,0x00},
	{0x30,0x2a,0x00},
	{0x30,0x2a,0x00},
	{0x36,0x01,0x00},
	{0x36,0x38,0x00},
	{0x32,0x08,0x10},
	{0x32,0x08,0xa0},
};


static const UINT8 cmd_init_code[][3] SENSOR_SIF1DMA_BUF ={
{0x01,0x03,0x01},
{0x36,0x38,0x00},
{0x03,0x00,0x00},
{0x03,0x02,0x2a},
{0x03,0x03,0x00},
{0x03,0x04,0x03},
{0x03,0x0b,0x00},
{0x03,0x0d,0x1e},
{0x03,0x0e,0x04},
{0x03,0x0f,0x01},
{0x03,0x12,0x01},
{0x03,0x1e,0x00},
{0x30,0x00,0x20},
{0x30,0x02,0x00},
{0x30,0x18,0x72},
{0x30,0x20,0x93},
{0x30,0x21,0x03},
{0x30,0x22,0x01},
{0x30,0x31,0x0a},
{0x30,0x3f,0x0c},
{0x33,0x05,0xf1},
{0x33,0x07,0x04},
{0x33,0x09,0x29},
{0x35,0x00,0x00},
{0x35,0x01,0x60},
{0x35,0x02,0x00},
{0x35,0x03,0x04},
{0x35,0x04,0x00},
{0x35,0x05,0x00},
{0x35,0x06,0x00},
{0x35,0x07,0x00},
{0x35,0x08,0x00},
{0x35,0x09,0x80},
{0x35,0x0a,0x00},
{0x35,0x0b,0x00},
{0x35,0x0c,0x00},
{0x35,0x0d,0x00},
{0x35,0x0e,0x00},
{0x35,0x0f,0x80},
{0x35,0x10,0x00},
{0x35,0x11,0x00},
{0x35,0x12,0x00},
{0x35,0x13,0x00},
{0x35,0x14,0x00},
{0x35,0x15,0x80},
{0x35,0x16,0x00},
{0x35,0x17,0x00},
{0x35,0x18,0x00},
{0x35,0x19,0x00},
{0x35,0x1a,0x00},
{0x35,0x1b,0x80},
{0x35,0x1c,0x00},
{0x35,0x1d,0x00},
{0x35,0x1e,0x00},
{0x35,0x1f,0x00},
{0x35,0x20,0x00},
{0x35,0x21,0x80},
{0x35,0x22,0x08},
{0x35,0x24,0x08},
{0x35,0x26,0x08},
{0x35,0x28,0x08},
{0x35,0x2a,0x08},
{0x36,0x02,0x00},
{0x36,0x03,0x40},
{0x36,0x04,0x02},
{0x36,0x05,0x00},
{0x36,0x06,0x00},
{0x36,0x07,0x00},
{0x36,0x09,0x12},
{0x36,0x0a,0x40},
{0x36,0x0c,0x08},
{0x36,0x0f,0xe5},
{0x36,0x08,0x8f},
{0x36,0x11,0x00},
{0x36,0x13,0xf7},
{0x36,0x16,0x58},
{0x36,0x19,0x99},
{0x36,0x1b,0x60},
{0x36,0x1c,0x7a},
{0x36,0x1e,0x79},
{0x36,0x1f,0x02},
{0x36,0x32,0x00},
{0x36,0x33,0x10},
{0x36,0x34,0x10},
{0x36,0x35,0x10},
{0x36,0x36,0x15},
{0x36,0x46,0x86},
{0x36,0x4a,0x0b},
{0x37,0x00,0x17},
{0x37,0x01,0x22},
{0x37,0x03,0x10},
{0x37,0x0a,0x37},
{0x37,0x05,0x00},
{0x37,0x06,0x63},
{0x37,0x09,0x3c},
{0x37,0x0b,0x01},
{0x37,0x0c,0x30},
{0x37,0x10,0x24},
{0x37,0x11,0x0c},
{0x37,0x16,0x00},
{0x37,0x20,0x28},
{0x37,0x29,0x7b},
{0x37,0x2a,0x84},
{0x37,0x2b,0xbd},
{0x37,0x2c,0xbc},
{0x37,0x2e,0x52},
{0x37,0x3c,0x0e},
{0x37,0x3e,0x33},
{0x37,0x43,0x10},
{0x37,0x44,0x88},
{0x37,0x45,0xc0},
{0x37,0x4a,0x43},
{0x37,0x4c,0x00},
{0x37,0x4e,0x23},
{0x37,0x51,0x7b},
{0x37,0x52,0x84},
{0x37,0x53,0xbd},
{0x37,0x54,0xbc},
{0x37,0x56,0x52},
{0x37,0x5c,0x00},
{0x37,0x60,0x00},
{0x37,0x61,0x00},
{0x37,0x62,0x00},
{0x37,0x63,0x00},
{0x37,0x64,0x00},
{0x37,0x67,0x04},
{0x37,0x68,0x04},
{0x37,0x69,0x08},
{0x37,0x6a,0x08},
{0x37,0x6b,0x20},
{0x37,0x6c,0x00},
{0x37,0x6d,0x00},
{0x37,0x6e,0x00},
{0x37,0x73,0x00},
{0x37,0x74,0x51},
{0x37,0x76,0xbd},
{0x37,0x77,0xbd},
{0x37,0x81,0x18},
{0x37,0x83,0x25},
{0x37,0x98,0x1b},
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0a},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xfb},
{0x38,0x08,0x0a},
{0x38,0x09,0x80},
{0x38,0x0a,0x05},
{0x38,0x0b,0xf0},
{0x38,0x0c,0x03},
{0x38,0x0d,0x5c},
{0x38,0x0e,0x06},
{0x38,0x0f,0x12},
{0x38,0x10,0x00},
{0x38,0x11,0x08},
{0x38,0x12,0x00},
{0x38,0x13,0x04},
{0x38,0x14,0x01},
{0x38,0x15,0x01},
{0x38,0x19,0x01},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x00},
{0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x00},
{0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x06},
{0x38,0x21,0x06},
#else
{0x38,0x20,0x06},
{0x38,0x21,0x00},
#endif

{0x38,0x29,0x00},
{0x38,0x2a,0x01},
{0x38,0x2b,0x01},
{0x38,0x2d,0x7f},
{0x38,0x30,0x04},
{0x38,0x36,0x01},
{0x38,0x37,0x00},
{0x38,0x41,0x02},
{0x38,0x46,0x08},
{0x38,0x47,0x07},
{0x3d,0x85,0x36},
{0x3d,0x8c,0x71},
{0x3d,0x8d,0xcb},
{0x3f,0x0a,0x00},
{0x40,0x00,0xf1},
{0x40,0x01,0x40},
{0x40,0x02,0x04},
{0x40,0x03,0x14},
{0x40,0x0d,0x10},
{0x40,0x0e,0x00},
{0x40,0x11,0x00},
{0x40,0x1a,0x00},
{0x40,0x1b,0x00},
{0x40,0x1c,0x00},
{0x40,0x1d,0x00},
{0x40,0x1f,0x00},
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x07},
{0x40,0x23,0xcf},
{0x40,0x24,0x09},
{0x40,0x25,0x60},
{0x40,0x26,0x09},
{0x40,0x27,0x6f},
{0x40,0x28,0x00},
{0x40,0x29,0x02},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2c,0x02},
{0x40,0x2d,0x02},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},
{0x43,0x02,0xff},
{0x43,0x03,0xff},
{0x43,0x04,0x00},
{0x43,0x05,0x00},
{0x43,0x06,0x00},
{0x43,0x08,0x02},
{0x45,0x00,0x6c},
{0x45,0x01,0xc4},
{0x45,0x02,0x40},
{0x45,0x03,0x01},
{0x46,0x01,0xA7},
{0x48,0x00,0x04},
{0x48,0x13,0x08},

/*Add the data lane HS trail timing adjustment.*/
/*This adjustment could fix the issue that some SPCA6350M would have drop frame issue which results from the HD counter does not work correctly.*/
/*The adjustment could enlarge the time interval before entering the low power state and make the front HD counter work correctly again.*/
{0x48,0x1b,0x4C},/*default value is 0x3C, add extra 16 ns.*/

{0x48,0x1f,0x40},
{0x48,0x29,0x78},
{0x48,0x37,0x10},
{0x4b,0x00,0x2a},
{0x4b,0x0d,0x00},
{0x4d,0x00,0x04},
{0x4d,0x01,0x42},
{0x4d,0x02,0xd1},
{0x4d,0x03,0x93},
{0x4d,0x04,0xf5},
{0x4d,0x05,0xc1},
{0x50,0x00,0xf3},
{0x50,0x01,0x11},
{0x50,0x04,0x00},
{0x50,0x0a,0x00},
{0x50,0x0b,0x00},
{0x50,0x32,0x00},
{0x50,0x40,0x00},
{0x50,0x50,0x0c},
{0x55,0x00,0x00},
{0x55,0x01,0x10},
{0x55,0x02,0x01},
{0x55,0x03,0x0f},
{0x80,0x00,0x00},
{0x80,0x01,0x00},
{0x80,0x02,0x00},
{0x80,0x03,0x00},
{0x80,0x04,0x00},
{0x80,0x05,0x00},
{0x80,0x06,0x00},
{0x80,0x07,0x00},
{0x80,0x08,0x00},
{0x36,0x38,0x00},
/*{0x01,0x00,0x01},*/ /* Streaming   */
};

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
static const UINT8 cmd_default_exp_frm_code[][3] SENSOR_SIF1DMA_BUF ={
{0x35,0x00,0x00},
{0x35,0x01,0x10},
{0x35,0x02,0x00},
{0x38,0x0E,0x05},/*VTS=1486 for 2560x1440*/
{0x38,0x0F,0xce},
};
#else
static const UINT8 cmd_default_exp_frm_code[][3] SENSOR_SIF1DMA_BUF ={
{0x35,0x00,0x00},
{0x35,0x01,0x10},
{0x35,0x02,0x00},
{0x38,0x0E,0x03},
{0x38,0x0F,0x1d},
};
#endif

/*720p 60fps*/
static const UINT8 cmd_720P_60fps_code[][3] SENSOR_SIF1DMA_BUF ={

{0x01,0x00,0x00},
                 
{0x03,0x00,0x02},
{0x03,0x02,0x19},
{0x03,0x03,0x00},
{0x48,0x37,0x35},
                 
{0x36,0x32,0x05},
{0x37,0x4a,0x43},
{0x37,0x6b,0x40},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x10},
{0x38,0x21,0x07},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x10},
{0x38,0x21,0x01},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x16},
{0x38,0x21,0x07},
#else
{0x38,0x20,0x16},
{0x38,0x21,0x01},
#endif

{0x38,0x2a,0x03},
{0x38,0x30,0x08},
{0x38,0x36,0x02},
{0x40,0x01,0x50},
{0x40,0x03,0x14},
{0x45,0x02,0x44},
{0x50,0x50,0x3c},
                 
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0A},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xFB},
{0x38,0x08,0x05},
{0x38,0x09,0x40},
{0x38,0x0A,0x02},
{0x38,0x0B,0xf8},
{0x38,0x0C,0x13},
{0x38,0x0D,0x9a},
/*remove frame length
{0x38,0x0E,0x03},
{0x38,0x0F,0x1d},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x04},
{0x38,0x12,0x00},
{0x38,0x13,0x02},
                 
{0x38,0x14,0x03},
{0x38,0x15,0x01},
                 
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x03},
{0x40,0x23,0xe7},
{0x40,0x24,0x05},
{0x40,0x25,0x14},
{0x40,0x26,0x05},
{0x40,0x27,0x23},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},

{0x46,0x00,0x00},
{0x46,0x01,0x53},
                 
{0x01,0x00,0x01},/* Streaming   */
};
/*720p 120fps*/
static const UINT8 cmd_720P_120fps_code[][3] SENSOR_SIF1DMA_BUF ={
{0x01,0x00,0x00},
          
{0x03,0x00,0x02},
{0x03,0x02,0x1e},
{0x03,0x03,0x00},
{0x48,0x37,0x2c},
          
{0x36,0x32,0x05},
{0x37,0x4a,0x43},
{0x37,0x6b,0x40},
          
#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x10},
{0x38,0x21,0x07},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x10},
{0x38,0x21,0x01},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x16},
{0x38,0x21,0x07},
#else
{0x38,0x20,0x16},
{0x38,0x21,0x01},
#endif

{0x38,0x2a,0x03},
{0x38,0x30,0x08},
{0x38,0x36,0x02},
{0x40,0x01,0x50},
{0x40,0x03,0x14},
{0x45,0x02,0x44},
{0x50,0x50,0x3c},
          
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0A},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xFB},
{0x38,0x08,0x05},
{0x38,0x09,0x40},
{0x38,0x0A,0x02},
{0x38,0x0B,0xf8},
{0x38,0x0C,0x04},
{0x38,0x0D,0xe4},
/*remove frame length
{0x38,0x0E,0x03},
{0x38,0x0F,0x1d},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x04},
{0x38,0x12,0x00},
{0x38,0x13,0x02},
          
{0x38,0x14,0x03},
{0x38,0x15,0x01},
          
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x03},
{0x40,0x23,0xe7},
{0x40,0x24,0x05},
{0x40,0x25,0x14},
{0x40,0x26,0x05},
{0x40,0x27,0x23},

{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},

{0x46,0x00,0x00},
{0x46,0x01,0x53},
          
{0x01,0x00,0x01},/* Streaming   */
};

/*720p 120fps*/
static const UINT8 cmd_720P_120fps_hts_modify_code[][3] SENSOR_SIF1DMA_BUF ={
{0x01,0x00,0x00},
          
{0x03,0x00,0x02},
{0x03,0x02,0x1e},
{0x03,0x03,0x00},
{0x48,0x37,0x2c},
          
{0x36,0x32,0x05},
{0x37,0x4a,0x43},
{0x37,0x6b,0x40},
          
#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x10},
{0x38,0x21,0x07},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x10},
{0x38,0x21,0x01},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x16},
{0x38,0x21,0x07},
#else
{0x38,0x20,0x16},
{0x38,0x21,0x01},
#endif

{0x38,0x2a,0x03},
{0x38,0x30,0x08},
{0x38,0x36,0x02},
{0x40,0x01,0x50},
{0x40,0x03,0x14},
{0x45,0x02,0x44},
{0x50,0x50,0x3c},
          
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0A},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xFB},
{0x38,0x08,0x05},
{0x38,0x09,0x40},
{0x38,0x0A,0x02},
{0x38,0x0B,0xf8},
{0x38,0x0C,0x04},
{0x38,0x0D,0xde},
/*remove frame length
{0x38,0x0E,0x03},
{0x38,0x0F,0x1d},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x04},
{0x38,0x12,0x00},
{0x38,0x13,0x02},
          
{0x38,0x14,0x03},
{0x38,0x15,0x01},
          
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x03},
{0x40,0x23,0xe7},
{0x40,0x24,0x05},
{0x40,0x25,0x14},
{0x40,0x26,0x05},
{0x40,0x27,0x23},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},

{0x46,0x00,0x00},
{0x46,0x01,0x53},
          
{0x01,0x00,0x01},/* Streaming   */
};


static const UINT8 cmd_QUARTER_code[][3] SENSOR_SIF1DMA_BUF ={
{0x01,0x00,0x00},
					 
{0x03,0x00,0x02},
{0x03,0x02,0x19},
{0x03,0x03,0x00},
{0x48,0x37,0x35},
					 
{0x36,0x32,0x05},
{0x37,0x4a,0x43},
{0x37,0x6b,0x40},
					 
#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x10},
{0x38,0x21,0x07},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x10},
{0x38,0x21,0x01},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x16},
{0x38,0x21,0x07},
#else
{0x38,0x20,0x16},
{0x38,0x21,0x01},
#endif

{0x38,0x2a,0x03},
{0x38,0x30,0x08},
{0x38,0x36,0x02},
{0x40,0x01,0x50},
{0x40,0x03,0x14},
{0x45,0x02,0x44},
{0x50,0x50,0x3c},
					 
{0x38,0x00,0x01},
{0x38,0x01,0x88},
{0x38,0x02,0x00},
{0x38,0x03,0xe0},
{0x38,0x04,0x09},
{0x38,0x05,0x17},
{0x38,0x06,0x05},
{0x38,0x07,0x1f},
{0x38,0x08,0x03},
{0x38,0x09,0xc0},
{0x38,0x0A,0x02},
{0x38,0x0B,0x1c},
{0x38,0x0C,0x13},
{0x38,0x0D,0x9a},
/*remove frame length
{0x38,0x0E,0x03},
{0x38,0x0F,0x1d},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x04},
{0x38,0x12,0x00},
{0x38,0x13,0x02},
					 
{0x38,0x14,0x03},
{0x38,0x15,0x01},
					 
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x02},
{0x40,0x23,0x53},
{0x40,0x24,0x03},
{0x40,0x25,0x80},
{0x40,0x26,0x03},
{0x40,0x27,0x90},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},

{0x46,0x00,0x00},
{0x46,0x01,0x3b},
					 
{0x01,0x00,0x01},/* Streaming   */
};

static const UINT8 cmd_1080P_code[][3] SENSOR_SIF1DMA_BUF ={

 {0x01,0x00,0x00},
				  
 {0x03,0x00,0x00},
 {0x03,0x02,0x10},
 {0x03,0x03,0x00}, 
 {0x48,0x37,0x2a},
				  
 {0x36,0x32,0x00},
 {0x37,0x4a,0x43}, 
 {0x37,0x6b,0x20},
				  
#if (MIRROR_FLIP_EN==0)
 {0x38,0x20,0x00},
 {0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
 {0x38,0x20,0x00},
 {0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
 {0x38,0x20,0x06},
 {0x38,0x21,0x06},
#else
 {0x38,0x20,0x06},
 {0x38,0x21,0x00},
#endif


 {0x38,0x2a,0x01},
 {0x38,0x30,0x04},
 {0x38,0x36,0x01},
 {0x40,0x01,0x40},
 {0x40,0x03,0x14}, 
 {0x45,0x02,0x40},
 {0x50,0x50,0x0c},
				  
 {0x38,0x00,0x01},
 {0x38,0x01,0x88},
 {0x38,0x02,0x00},
 {0x38,0x03,0xe0},
 {0x38,0x04,0x09},
 {0x38,0x05,0x17},
 {0x38,0x06,0x05},
 {0x38,0x07,0x1f},
 {0x38,0x08,0x07},
 {0x38,0x09,0x80},
 {0x38,0x0A,0x04},
 {0x38,0x0B,0x38},
 {0x38,0x0C,0x06},
 {0x38,0x0D,0x5a},
 /*remove frame length
 {0x38,0x0E,0x04},
 {0x38,0x0F,0xce},
 */
 {0x38,0x10,0x00},
 {0x38,0x11,0x08},
 {0x38,0x12,0x00},
 {0x38,0x13,0x04},
				  
 {0x38,0x14,0x01},
 {0x38,0x15,0x01},
				  
 {0x40,0x20,0x00},
 {0x40,0x21,0x10},
 {0x40,0x22,0x06},
 {0x40,0x23,0x13},
 {0x40,0x24,0x07},
 {0x40,0x25,0x40},
 {0x40,0x26,0x07},
 {0x40,0x27,0x50},
 {0x40,0x2a,0x06},
 {0x40,0x2b,0x04},
 {0x40,0x2e,0x0e},
 {0x40,0x2f,0x04},

 {0x46,0x00,0x00},
 {0x46,0x01,0x77},
				  
 {0x01,0x00,0x01},/* Streaming on */	
};

static const UINT8 cmd_quarter_vbin_hfull_code[][3] SENSOR_SIF1DMA_BUF ={
/*----------------------------------------------
; Resolution	: 2688 x 760
; MCLK				: 24 Mhz
; MIPI				: 4 lane
; SCLK				: 120 Mhz
; MIPIDataRate: 720 Mbps/Lane
; FPS	        : 120
; HTS					: 1248 (R380c:R380d)
; VTS					: 801 (R380e:R380f)
; Tline 			: 10.4 us
;---------------------------------------------*/
{0x01,0x00,0x00},
{0x03,0x00,0x00},
{0x03,0x02,0x1e},
{0x03,0x03,0x00},
{0x48,0x37,0x16},

/*Default exposure time*/
/*
{0x35,0x00,0x00},
{0x35,0x01,0x31},
{0x35,0x02,0x00},
*/
{0x36,0x32,0x05},
{0x37,0x4a,0x43},
{0x37,0x6b,0x40},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x10},
{0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x10},
{0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x16},
{0x38,0x21,0x06},
#else
{0x38,0x20,0x16},
{0x38,0x21,0x00},
#endif

{0x38,0x2a,0x03},
{0x38,0x30,0x08},
{0x38,0x36,0x02},
{0x40,0x01,0x40},
{0x40,0x03,0x14},
{0x45,0x02,0x40},
{0x50,0x50,0x3c},
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0A},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xFB},
{0x38,0x08,0x0A},
{0x38,0x09,0x80},
{0x38,0x0A,0x02},
{0x38,0x0B,0xF8},

/*Change HTS to 1248.*/
{0x38,0x0C,0x04},
{0x38,0x0D,0xe0},

/*Original HTS=0x4e8 =1256*/
/*
{0x38,0x0C,0x04},
{0x38,0x0D,0xe8},
*/
/*Default frame total*/
/* 
{0x38,0x0E,0x03},
{0x38,0x0F,0x1d},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x08},
{0x38,0x12,0x00},
{0x38,0x13,0x02},
{0x38,0x14,0x01},
{0x38,0x15,0x01},
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x09},
{0x40,0x23,0x13},
{0x40,0x24,0x0A},
{0x40,0x25,0x40},
{0x40,0x26,0x0A},
{0x40,0x27,0x50},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},

{0x46,0x00,0x00},
{0x46,0x01,0xA7},
{0x01,0x00,0x01},
};


#if (1)
/*sensor output size 2560x1440*/
static const UINT8 cmd_full_hd_60fps_2lane_code[][3] SENSOR_SIF1DMA_BUF ={

/*{0x01,0x03,0x01},*/
{0x01,0x00,0x00},

{0x36,0x38,0x00},
{0x03,0x00,0x00},
{0x03,0x02,0x22},
{0x03,0x03,0x00},
{0x03,0x04,0x03},
{0x03,0x0b,0x00},
{0x03,0x0d,0x1e},
{0x03,0x0e,0x04},
{0x03,0x0f,0x01},
{0x03,0x12,0x01},
{0x03,0x1e,0x00},
{0x30,0x00,0x20},
{0x30,0x02,0x00},
{0x30,0x18,0x32},
{0x30,0x19,0x0c},
{0x30,0x20,0x93},
{0x30,0x21,0x03},
{0x30,0x22,0x01},
{0x30,0x31,0x0a},
{0x30,0x3f,0x0c},
{0x33,0x05,0xf1},
{0x33,0x07,0x04},
{0x33,0x09,0x29},
/*
{0x35,0x00,0x00},
{0x35,0x01,0x46},
{0x35,0x02,0x80},
*/
{0x35,0x03,0x04},
{0x35,0x04,0x00},
{0x35,0x05,0x00},
{0x35,0x06,0x00},
{0x35,0x07,0x00},
/*
{0x35,0x08,0x07},
{0x35,0x09,0x78},
*/
{0x35,0x0a,0x00},
{0x35,0x0b,0x00},
{0x35,0x0c,0x00},
{0x35,0x0d,0x00},
{0x35,0x0e,0x00},
{0x35,0x0f,0x80},
{0x35,0x10,0x00},
{0x35,0x11,0x00},
{0x35,0x12,0x00},
{0x35,0x13,0x00},
{0x35,0x14,0x00},
{0x35,0x15,0x80},
{0x35,0x16,0x00},
{0x35,0x17,0x00},
{0x35,0x18,0x00},
{0x35,0x19,0x00},
{0x35,0x1a,0x00},
{0x35,0x1b,0x80},
{0x35,0x1c,0x00},
{0x35,0x1d,0x00},
{0x35,0x1e,0x00},
{0x35,0x1f,0x00},
{0x35,0x20,0x00},
{0x35,0x21,0x80},
{0x35,0x22,0x08},
{0x35,0x24,0x08},
{0x35,0x26,0x08},
{0x35,0x28,0x08},
{0x35,0x2a,0x08},
{0x36,0x02,0x00},
{0x36,0x03,0x40},
{0x36,0x04,0x02},
{0x36,0x05,0x00},
{0x36,0x06,0x00},
{0x36,0x07,0x00},
{0x36,0x09,0x12},
{0x36,0x0a,0x40},
{0x36,0x0c,0x08},
{0x36,0x0f,0xe5},
{0x36,0x08,0x8f},
{0x36,0x11,0x00},
{0x36,0x13,0xf7},
{0x36,0x16,0x58},
{0x36,0x19,0x99},
{0x36,0x1b,0x60},
{0x36,0x1c,0x7a},
{0x36,0x1e,0x79},
{0x36,0x1f,0x02},
{0x36,0x32,0x00},
{0x36,0x33,0x10},
{0x36,0x34,0x10},
{0x36,0x35,0x10},
{0x36,0x36,0x15},
{0x36,0x46,0x86},
{0x36,0x4a,0x0b},
{0x37,0x00,0x17},
{0x37,0x01,0x22},
{0x37,0x03,0x10},
{0x37,0x0a,0x37},
{0x37,0x05,0x00},
{0x37,0x06,0x63},
{0x37,0x09,0x3c},
{0x37,0x0b,0x01},
{0x37,0x0c,0x30},
{0x37,0x10,0x24},
{0x37,0x11,0x0c},
{0x37,0x16,0x00},
{0x37,0x20,0x28},
{0x37,0x29,0x7b},
{0x37,0x2a,0x84},
{0x37,0x2b,0xbd},
{0x37,0x2c,0xbc},
{0x37,0x2e,0x52},
{0x37,0x3c,0x0e},
{0x37,0x3e,0x33},
{0x37,0x43,0x10},
{0x37,0x44,0x88},
{0x37,0x45,0xc0},
{0x37,0x4a,0x43},
{0x37,0x4c,0x00},
{0x37,0x4e,0x23},
{0x37,0x51,0x7b},
{0x37,0x52,0x84},
{0x37,0x53,0xbd},
{0x37,0x54,0xbc},
{0x37,0x56,0x52},
{0x37,0x5c,0x00},
{0x37,0x60,0x00},
{0x37,0x61,0x00},
{0x37,0x62,0x00},
{0x37,0x63,0x00},
{0x37,0x64,0x00},
{0x37,0x67,0x04},
{0x37,0x68,0x04},
{0x37,0x69,0x08},
{0x37,0x6a,0x08},
{0x37,0x6b,0x20},
{0x37,0x6c,0x00},
{0x37,0x6d,0x00},
{0x37,0x6e,0x00},
{0x37,0x73,0x00},
{0x37,0x74,0x51},
{0x37,0x76,0xbd},
{0x37,0x77,0xbd},
{0x37,0x81,0x18},
{0x37,0x83,0x25},
{0x37,0x98,0x1b},
{0x38,0x00,0x00},
{0x38,0x01,0x48},
{0x38,0x02,0x00},
{0x38,0x03,0x2C},
{0x38,0x04,0x0A},
{0x38,0x05,0x57},
{0x38,0x06,0x05},
{0x38,0x07,0xD3},
{0x38,0x08,0x0A},
{0x38,0x09,0x00},
{0x38,0x0a,0x05},
{0x38,0x0b,0xA0},

{0x38,0x0c,0x0A},/*HTS:2688*/
{0x38,0x0d,0x80},

#if 0 /*Setting by AE table*/
{0x38,0x0e,0x06},/*VTS:1558*/
{0x38,0x0f,0x16},
#endif

{0x38,0x10,0x00},
{0x38,0x11,0x08},
{0x38,0x12,0x00},
{0x38,0x13,0x04},
{0x38,0x14,0x01},
{0x38,0x15,0x01},
{0x38,0x19,0x01},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x00},
{0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x00},
{0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x06},
{0x38,0x21,0x06},
#else
{0x38,0x20,0x06},
{0x38,0x21,0x00},
#endif

{0x38,0x29,0x00},
{0x38,0x2a,0x01},
{0x38,0x2b,0x01},
{0x38,0x2d,0x7f},
{0x38,0x30,0x04},
{0x38,0x36,0x01},
{0x38,0x37,0x00},
{0x38,0x41,0x02},
{0x38,0x46,0x08},
{0x38,0x47,0x07},
{0x3d,0x85,0x36},
{0x3d,0x8c,0x71},
{0x3d,0x8d,0xcb},
{0x3f,0x0a,0x00},
{0x40,0x00,0x71},
{0x40,0x01,0x40},
{0x40,0x02,0x04},
{0x40,0x03,0x14},
{0x40,0x0d,0x10},/*Config the BLC target to 16 (10bit).*/
{0x40,0x0e,0x00},
{0x40,0x11,0x00},
{0x40,0x1a,0x00},
{0x40,0x1b,0x00},
{0x40,0x1c,0x00},
{0x40,0x1d,0x00},
{0x40,0x1f,0x00},
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x08},
{0x40,0x23,0x93},
{0x40,0x24,0x09},
{0x40,0x25,0xC0},
{0x40,0x26,0x09},
{0x40,0x27,0xD0},
{0x40,0x28,0x00},
{0x40,0x29,0x02},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2c,0x02},
{0x40,0x2d,0x02},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},
{0x43,0x02,0xff},
{0x43,0x03,0xff},
{0x43,0x04,0x00},
{0x43,0x05,0x00},
{0x43,0x06,0x00},
{0x43,0x08,0x02},
{0x45,0x00,0x6c},
{0x45,0x01,0xc4},
{0x45,0x02,0x40},
{0x45,0x03,0x01},
{0x46,0x00,0x00},
{0x46,0x01,0x9F},
{0x48,0x00,0x04},
{0x48,0x13,0x08},
{0x48,0x1f,0x40},
{0x48,0x29,0x78},
{0x48,0x37,0x14},
{0x4b,0x00,0x2a},
{0x4b,0x0d,0x00},
{0x4d,0x00,0x04},
{0x4d,0x01,0x42},
{0x4d,0x02,0xd1},
{0x4d,0x03,0x93},
{0x4d,0x04,0xf5},
{0x4d,0x05,0xc1},
{0x50,0x00,0xf3},
{0x50,0x01,0x11},
{0x50,0x04,0x00},
{0x50,0x0a,0x00},
{0x50,0x0b,0x00},
{0x50,0x32,0x00},
{0x50,0x40,0x00},
{0x50,0x50,0x0c},
{0x55,0x00,0x00},
{0x55,0x01,0x10},
{0x55,0x02,0x01},
{0x55,0x03,0x0f},
{0x80,0x00,0x00},
{0x80,0x01,0x00},
{0x80,0x02,0x00},
{0x80,0x03,0x00},
{0x80,0x04,0x00},
{0x80,0x05,0x00},
{0x80,0x06,0x00},
{0x80,0x07,0x00},
{0x80,0x08,0x00},
{0x36,0x38,0x00},
{0x01,0x00,0x01},

};

#else

/*sensor output size 1920x1080*/
static const UINT8 cmd_full_hd_60fps_2lane_code[][3] SENSOR_SIF1DMA_BUF ={

/*{0x01,0x03,0x01},*/
{0x36,0x38,0x00},
{0x03,0x00,0x00},
{0x03,0x02,0x22},
{0x03,0x03,0x00},
{0x03,0x04,0x03},
{0x03,0x0b,0x00},
{0x03,0x0d,0x1e},
/*
{0x03,0x0e,0x04},
{0x03,0x0f,0x01},
*/
{0x03,0x12,0x01},
{0x03,0x1e,0x00},
{0x30,0x00,0x20},
{0x30,0x02,0x00},
{0x30,0x18,0x32},
{0x30,0x19,0x0c},
{0x30,0x20,0x93},
{0x30,0x21,0x03},
{0x30,0x22,0x01},
{0x30,0x31,0x0a},
{0x30,0x3f,0x0c},
{0x33,0x05,0xf1},
{0x33,0x07,0x04},
{0x33,0x09,0x29},
/*
{0x35,0x00,0x00},
{0x35,0x01,0x46},
{0x35,0x02,0x80},
*/
{0x35,0x03,0x04},
{0x35,0x04,0x00},
{0x35,0x05,0x00},
{0x35,0x06,0x00},
{0x35,0x07,0x00},
/*
{0x35,0x08,0x07},
{0x35,0x09,0x78},
*/
{0x35,0x0a,0x00},
{0x35,0x0b,0x00},
{0x35,0x0c,0x00},
{0x35,0x0d,0x00},
{0x35,0x0e,0x00},
{0x35,0x0f,0x80},
{0x35,0x10,0x00},
{0x35,0x11,0x00},
{0x35,0x12,0x00},
{0x35,0x13,0x00},
{0x35,0x14,0x00},
{0x35,0x15,0x80},
{0x35,0x16,0x00},
{0x35,0x17,0x00},
{0x35,0x18,0x00},
{0x35,0x19,0x00},
{0x35,0x1a,0x00},
{0x35,0x1b,0x80},
{0x35,0x1c,0x00},
{0x35,0x1d,0x00},
{0x35,0x1e,0x00},
{0x35,0x1f,0x00},
{0x35,0x20,0x00},
{0x35,0x21,0x80},
{0x35,0x22,0x08},
{0x35,0x24,0x08},
{0x35,0x26,0x08},
{0x35,0x28,0x08},
{0x35,0x2a,0x08},
{0x36,0x02,0x00},
{0x36,0x03,0x40},
{0x36,0x04,0x02},
{0x36,0x05,0x00},
{0x36,0x06,0x00},
{0x36,0x07,0x00},
{0x36,0x09,0x12},
{0x36,0x0a,0x40},
{0x36,0x0c,0x08},
{0x36,0x0f,0xe5},
{0x36,0x08,0x8f},
{0x36,0x11,0x00},
{0x36,0x13,0xf7},
{0x36,0x16,0x58},
{0x36,0x19,0x99},
{0x36,0x1b,0x60},
{0x36,0x1c,0x7a},
{0x36,0x1e,0x79},
{0x36,0x1f,0x02},
{0x36,0x32,0x00},
{0x36,0x33,0x10},
{0x36,0x34,0x10},
{0x36,0x35,0x10},
{0x36,0x36,0x15},
{0x36,0x46,0x86},
{0x36,0x4a,0x0b},
{0x37,0x00,0x17},
{0x37,0x01,0x22},
{0x37,0x03,0x10},
{0x37,0x0a,0x37},
{0x37,0x05,0x00},
{0x37,0x06,0x63},
{0x37,0x09,0x3c},
{0x37,0x0b,0x01},
{0x37,0x0c,0x30},
{0x37,0x10,0x24},
{0x37,0x11,0x0c},
{0x37,0x16,0x00},
{0x37,0x20,0x28},
{0x37,0x29,0x7b},
{0x37,0x2a,0x84},
{0x37,0x2b,0xbd},
{0x37,0x2c,0xbc},
{0x37,0x2e,0x52},
{0x37,0x3c,0x0e},
{0x37,0x3e,0x33},
{0x37,0x43,0x10},
{0x37,0x44,0x88},
{0x37,0x45,0xc0},
{0x37,0x4a,0x43},
{0x37,0x4c,0x00},
{0x37,0x4e,0x23},
{0x37,0x51,0x7b},
{0x37,0x52,0x84},
{0x37,0x53,0xbd},
{0x37,0x54,0xbc},
{0x37,0x56,0x52},
{0x37,0x5c,0x00},
{0x37,0x60,0x00},
{0x37,0x61,0x00},
{0x37,0x62,0x00},
{0x37,0x63,0x00},
{0x37,0x64,0x00},
{0x37,0x67,0x04},
{0x37,0x68,0x04},
{0x37,0x69,0x08},
{0x37,0x6a,0x08},
{0x37,0x6b,0x20},
{0x37,0x6c,0x00},
{0x37,0x6d,0x00},
{0x37,0x6e,0x00},
{0x37,0x73,0x00},
{0x37,0x74,0x51},
{0x37,0x76,0xbd},
{0x37,0x77,0xbd},
{0x37,0x81,0x18},
{0x37,0x83,0x25},
{0x37,0x98,0x1b},
{0x38,0x00,0x01},
{0x38,0x01,0x88},
{0x38,0x02,0x00},
{0x38,0x03,0xe0},
{0x38,0x04,0x09},
{0x38,0x05,0x17},
{0x38,0x06,0x05},
{0x38,0x07,0x1F},
{0x38,0x08,0x07},
{0x38,0x09,0x80},
{0x38,0x0a,0x04},
{0x38,0x0b,0x38},
{0x38,0x0c,0x06},
{0x38,0x0d,0xe0},
{0x38,0x0e,0x04},
{0x38,0x0f,0x70},
{0x38,0x10,0x00},
{0x38,0x11,0x08},
{0x38,0x12,0x00},
{0x38,0x13,0x04},
{0x38,0x14,0x01},
{0x38,0x15,0x01},
{0x38,0x19,0x01},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x00},
{0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x00},
{0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x06},
{0x38,0x21,0x06},
#else
{0x38,0x20,0x06},
{0x38,0x21,0x00},
#endif

{0x38,0x29,0x00},
{0x38,0x2a,0x01},
{0x38,0x2b,0x01},
{0x38,0x2d,0x7f},
{0x38,0x30,0x04},
{0x38,0x36,0x01},
{0x38,0x37,0x00},
{0x38,0x41,0x02},
{0x38,0x46,0x08},
{0x38,0x47,0x07},
{0x3d,0x85,0x36},
{0x3d,0x8c,0x71},
{0x3d,0x8d,0xcb},
{0x3f,0x0a,0x00},
{0x40,0x00,0x71},
{0x40,0x01,0x40},
{0x40,0x02,0x04},
{0x40,0x03,0x14},
{0x40,0x0d,0x10},/*Config the BLC target to 16 (10bit).*/
{0x40,0x0e,0x00},
{0x40,0x11,0x00},
{0x40,0x1a,0x00},
{0x40,0x1b,0x00},
{0x40,0x1c,0x00},
{0x40,0x1d,0x00},
{0x40,0x1f,0x00},
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x06},
{0x40,0x23,0x13},
{0x40,0x24,0x07},
{0x40,0x25,0x40},
{0x40,0x26,0x07},
{0x40,0x27,0x50},
{0x40,0x28,0x00},
{0x40,0x29,0x02},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2c,0x02},
{0x40,0x2d,0x02},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},
{0x43,0x02,0xff},
{0x43,0x03,0xff},
{0x43,0x04,0x00},
{0x43,0x05,0x00},
{0x43,0x06,0x00},
{0x43,0x08,0x02},
{0x45,0x00,0x6c},
{0x45,0x01,0xc4},
{0x45,0x02,0x40},
{0x45,0x03,0x01},
{0x46,0x00,0x00},
{0x46,0x01,0x77},
{0x48,0x00,0x04},
{0x48,0x13,0x08},
{0x48,0x1f,0x40},
{0x48,0x29,0x78},
{0x48,0x37,0x14},
{0x4b,0x00,0x2a},
{0x4b,0x0d,0x00},
{0x4d,0x00,0x04},
{0x4d,0x01,0x42},
{0x4d,0x02,0xd1},
{0x4d,0x03,0x93},
{0x4d,0x04,0xf5},
{0x4d,0x05,0xc1},
{0x50,0x00,0xf3},
{0x50,0x01,0x11},
{0x50,0x04,0x00},
{0x50,0x0a,0x00},
{0x50,0x0b,0x00},
{0x50,0x32,0x00},
{0x50,0x40,0x00},
{0x50,0x50,0x0c},
{0x55,0x00,0x00},
{0x55,0x01,0x10},
{0x55,0x02,0x01},
{0x55,0x03,0x0f},
{0x80,0x00,0x00},
{0x80,0x01,0x00},
{0x80,0x02,0x00},
{0x80,0x03,0x00},
{0x80,0x04,0x00},
{0x80,0x05,0x00},
{0x80,0x06,0x00},
{0x80,0x07,0x00},
{0x80,0x08,0x00},
{0x36,0x38,0x00},
{0x01,0x00,0x01},

};

#endif /*select 2 lane output size is 1920x1080 or 2560x1440.*/

static const UINT8 cmd_full_60fps_code[][3] SENSOR_SIF1DMA_BUF ={
{0x01,0x00,0x00},
          
{0x03,0x00,0x00},
{0x03,0x02,0x1e},
{0x03,0x03,0x00},
{0x48,0x37,0x16},
          
{0x36,0x32,0x00},
{0x37,0x4a,0x43},
{0x37,0x6b,0x20},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x00},
{0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x00},
{0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x06},
{0x38,0x21,0x06},
#else
{0x38,0x20,0x06},
{0x38,0x21,0x00},
#endif

{0x38,0x2a,0x01},
{0x38,0x30,0x04},
{0x38,0x36,0x01},
{0x40,0x01,0x40},
{0x40,0x03,0x14},
{0x45,0x02,0x40},
{0x50,0x50,0x0c},
          
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0A},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xFB},
{0x38,0x08,0x0A},
{0x38,0x09,0x80},
{0x38,0x0A,0x05},
{0x38,0x0B,0xF0},
{0x38,0x0C,0x05},
{0x38,0x0D,0x04},
/*remove frame length
{0x38,0x0E,0x06},
{0x38,0x0F,0x12},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x08},
{0x38,0x12,0x00},
{0x38,0x13,0x04},
          
{0x38,0x14,0x01},
{0x38,0x15,0x01},
          
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x09},
{0x40,0x23,0x13},
{0x40,0x24,0x0A},
{0x40,0x25,0x40},
{0x40,0x26,0x0A},
{0x40,0x27,0x50},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},

{0x46,0x00,0x00},
{0x46,0x01,0xA7},
          
{0x01,0x00,0x01},/* Streaming   */
};

static const UINT8 cmd_full_60fps_hts_modify_code[][3] SENSOR_SIF1DMA_BUF ={
{0x01,0x00,0x00},
          
{0x03,0x00,0x00},
{0x03,0x02,0x1e},
{0x03,0x03,0x00},
{0x48,0x37,0x16},
          
{0x36,0x32,0x00},
{0x37,0x4a,0x43},
{0x37,0x6b,0x20},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x00},
{0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x00},
{0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x06},
{0x38,0x21,0x06},
#else
{0x38,0x20,0x06},
{0x38,0x21,0x00},
#endif

{0x38,0x2a,0x01},
{0x38,0x30,0x04},
{0x38,0x36,0x01},
{0x40,0x01,0x40},
{0x40,0x03,0x14},
{0x45,0x02,0x40},
{0x50,0x50,0x0c},
          
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0A},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xFB},
{0x38,0x08,0x0A},
{0x38,0x09,0x80},
{0x38,0x0A,0x05},
{0x38,0x0B,0xF0},
{0x38,0x0C,0x05},
{0x38,0x0D,0x02},
/*remove frame length
{0x38,0x0E,0x06},
{0x38,0x0F,0x12},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x08},
{0x38,0x12,0x00},
{0x38,0x13,0x04},
          
{0x38,0x14,0x01},
{0x38,0x15,0x01},
          
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x09},
{0x40,0x23,0x13},
{0x40,0x24,0x0A},
{0x40,0x25,0x40},
{0x40,0x26,0x0A},
{0x40,0x27,0x50},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},

{0x46,0x00,0x00},
{0x46,0x01,0xA7},
          
{0x01,0x00,0x01},/* Streaming   */
};

static const UINT8 cmd_still_code[][3] SENSOR_SIF1DMA_BUF ={

{0x01,0x00,0x00},
                 
{0x03,0x00,0x00},
{0x03,0x02,0x24},
{0x03,0x03,0x00},
{0x48,0x37,0x10},
                 
{0x36,0x32,0x00},
{0x37,0x4a,0x43},
{0x37,0x6b,0x20},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x00},
{0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x00},
{0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x06},
{0x38,0x21,0x06},
#else
{0x38,0x20,0x06},
{0x38,0x21,0x00},
#endif

{0x38,0x2a,0x01},
{0x38,0x30,0x04},
{0x38,0x36,0x01},
{0x40,0x01,0x40},
{0x40,0x03,0x14},
{0x45,0x02,0x40},
{0x50,0x50,0x0c},
                 
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0A},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xFB},
{0x38,0x08,0x0A},
{0x38,0x09,0x80},
{0x38,0x0A,0x05},
{0x38,0x0B,0xF0},
{0x38,0x0C,0x04},
{0x38,0x0D,0x08},
/* remove frame length
{0x38,0x0E,0x06},
{0x38,0x0F,0x26},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x08},
{0x38,0x12,0x00},
{0x38,0x13,0x04},
                 
{0x38,0x14,0x01},
{0x38,0x15,0x01},
                 
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x09},
{0x40,0x23,0x13},
{0x40,0x24,0x0A},
{0x40,0x25,0x40},
{0x40,0x26,0x0A},
{0x40,0x27,0x50},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},

{0x46,0x00,0x00},
{0x46,0x01,0xA7},
                 
{0x01,0x00,0x01},/* Streaming   */
};
#if HDR_test
static const UINT8 cmd_HDR720p_code[][3] SENSOR_SIF1DMA_BUF ={
	/*{0x01,0x03,0x01},*/
	{0x36,0x38,0x00},
	{0x03,0x00,0x02},
	{0x03,0x02,0x32},
	{0x03,0x03,0x00},
	{0x03,0x04,0x03},
	{0x03,0x0b,0x00},
	{0x03,0x0d,0x1e},
	{0x03,0x0e,0x04},
	{0x03,0x0f,0x01},
	{0x03,0x12,0x01},
	{0x03,0x1e,0x00},
	{0x30,0x00,0x20},
	{0x30,0x02,0x00},
	{0x30,0x18,0x72},
	{0x30,0x20,0x93},
	{0x30,0x21,0x03},
	{0x30,0x22,0x01},
	{0x30,0x31,0x0a},
	{0x30,0x3f,0x0c},
	{0x33,0x05,0xf1},
	{0x33,0x07,0x04},
	{0x33,0x09,0x29},
	/*L Exp
	{0x35,0x00,0x00},
	{0x35,0x01,0x30},
	{0x35,0x02,0x00},
	*/
	{0x35,0x03,0x04},
	{0x35,0x04,0x00},
	{0x35,0x05,0x00},
	{0x35,0x06,0x00},
	{0x35,0x07,0x00},
	/*L Agc
	{0x35,0x08,0x00},
	{0x35,0x09,0x80},
	*/
	/*M Exp
	{0x35,0x0a,0x00},
	{0x35,0x0b,0x30},
	{0x35,0x0c,0x00},
	*/
	{0x35,0x0d,0x00},
	/*M Agc
	{0x35,0x0e,0x00},
	{0x35,0x0f,0x80},
	*/
	/*S Exp
	{0x35,0x10,0x00},
	{0x35,0x11,0x10},
	{0x35,0x12,0x00},
	*/
	{0x35,0x13,0x00},
	/*S Agc
	{0x35,0x14,0x00},
	{0x35,0x15,0x80},
	*/
	{0x35,0x16,0x00},
	{0x35,0x17,0x08},
	{0x35,0x18,0x00},
	{0x35,0x19,0x00},
	{0x35,0x1a,0x00},
	{0x35,0x1b,0x80},
	{0x35,0x1c,0x00},
	{0x35,0x1d,0x02},
	{0x35,0x1e,0x00},
	{0x35,0x1f,0x00},
	{0x35,0x20,0x00},
	{0x35,0x21,0x80},
	{0x35,0x22,0x08},
	{0x35,0x24,0x08},
	{0x35,0x26,0x08},
	{0x35,0x28,0x08},
	{0x35,0x2a,0x08},
	{0x36,0x02,0x00},
	{0x36,0x03,0x40},
	{0x36,0x04,0x02},
	{0x36,0x05,0x00},
	{0x36,0x06,0x00},
	{0x36,0x07,0x00},
	{0x36,0x09,0x12},
	{0x36,0x0a,0x40},
	{0x36,0x0c,0x08},
	{0x36,0x0f,0xe5},
	{0x36,0x08,0x8f},
	{0x36,0x11,0x00},
	{0x36,0x13,0xf7},
	{0x36,0x16,0x58},
	{0x36,0x19,0x99},
	{0x36,0x1b,0x60},
	{0x36,0x1c,0x7a},
	{0x36,0x1e,0x79},
	{0x36,0x1f,0x02},
	{0x36,0x32,0x05},
	{0x36,0x33,0x10},
	{0x36,0x34,0x10},
	{0x36,0x35,0x10},
	{0x36,0x36,0x15},
	{0x36,0x46,0x86},
	{0x36,0x4a,0x0b},
	{0x37,0x00,0x17},
	{0x37,0x01,0x22},
	{0x37,0x03,0x10},
	{0x37,0x0a,0x37},
	{0x37,0x05,0x00},
	{0x37,0x06,0x63},
	{0x37,0x09,0x3c},
	{0x37,0x0b,0x01},
	{0x37,0x0c,0x30},
	{0x37,0x10,0x24},
	{0x37,0x11,0x0c},
	{0x37,0x16,0x00},
	{0x37,0x20,0x28},
	{0x37,0x29,0x7b},
	{0x37,0x2a,0x84},
	{0x37,0x2b,0xbd},
	{0x37,0x2c,0xbc},
	{0x37,0x2e,0x52},
	{0x37,0x3c,0x0e},
	{0x37,0x3e,0x33},
	{0x37,0x43,0x10},
	{0x37,0x44,0x88},
	{0x37,0x45,0xc0},
	{0x37,0x4a,0x43},
	{0x37,0x4c,0x00},
	{0x37,0x4e,0x23},
	{0x37,0x51,0x7b},
	{0x37,0x52,0x84},
	{0x37,0x53,0xbd},
	{0x37,0x54,0xbc},
	{0x37,0x56,0x52},
	{0x37,0x5c,0x00},
	{0x37,0x60,0x00},
	{0x37,0x61,0x00},
	{0x37,0x62,0x00},
	{0x37,0x63,0x00},
	{0x37,0x64,0x00},
	{0x37,0x67,0x04},
	{0x37,0x68,0x04},
	{0x37,0x69,0x08},
	{0x37,0x6a,0x08},
	{0x37,0x6b,0x40},
	{0x37,0x6c,0x00},
	{0x37,0x6d,0x00},
	{0x37,0x6e,0x00},
	{0x37,0x73,0x00},
	{0x37,0x74,0x51},
	{0x37,0x76,0xbd},
	{0x37,0x77,0xbd},
	{0x37,0x81,0x18},
	{0x37,0x83,0x25},
	{0x37,0x98,0x1b},
	{0x38,0x00,0x00},
	{0x38,0x01,0x48},
	{0x38,0x02,0x00},
	{0x38,0x03,0x2C},
	{0x38,0x04,0x0a},
	{0x38,0x05,0x57},
	{0x38,0x06,0x05},
	{0x38,0x07,0xD3},
	{0x38,0x08,0x05},
	{0x38,0x09,0x00},
	{0x38,0x0a,0x02},
	{0x38,0x0b,0xD0},
	{0x38,0x0c,0x06},
	{0x38,0x0d,0xb8},
	{0x38,0x0e,0x03},
	{0x38,0x0f,0x05},
	{0x38,0x10,0x00},
	{0x38,0x11,0x04},
	{0x38,0x12,0x00},
	{0x38,0x13,0x02},
	{0x38,0x14,0x03},
	{0x38,0x15,0x01},
	{0x38,0x19,0x01},
	
#if (MIRROR_FLIP_EN==0)
	{0x38,0x20,0x10},
	{0x38,0x21,0x07},
#elif (MIRROR_FLIP_EN==1)
	{0x38,0x20,0x10},
	{0x38,0x21,0x01},
#elif (MIRROR_FLIP_EN==2)
	{0x38,0x20,0x16},
	{0x38,0x21,0x07},
#else
	{0x38,0x20,0x16},
	{0x38,0x21,0x01},
#endif

	{0x38,0x29,0x00},
	{0x38,0x2a,0x03},
	{0x38,0x2b,0x01},
	{0x38,0x2d,0x7f},
	{0x38,0x30,0x08},
	{0x38,0x36,0x02},
	{0x38,0x37,0x00},
	{0x38,0x41,0x02},
	{0x38,0x46,0x0a},
	{0x38,0x47,0x07},
	{0x3d,0x85,0x36},
	{0x3d,0x8c,0x71},
	{0x3d,0x8d,0xcb},
	{0x3f,0x0a,0x00},
	{0x40,0x00,0xf1},
	{0x40,0x01,0x50},
	{0x40,0x02,0x04},
	{0x40,0x03,0x14},
	{0x40,0x0d,0x10},
	{0x40,0x0e,0x00},
	{0x40,0x11,0x00},
	{0x40,0x1a,0x00},
	{0x40,0x1b,0x00},
	{0x40,0x1c,0x00},
	{0x40,0x1d,0x00},
	{0x40,0x1f,0x00},
	{0x40,0x20,0x00},
	{0x40,0x21,0x10},
	{0x40,0x22,0x03},
	{0x40,0x23,0x93},
	{0x40,0x24,0x04},
	{0x40,0x25,0xC0},
	{0x40,0x26,0x04},
	{0x40,0x27,0xD0},
	{0x40,0x28,0x00},
	{0x40,0x29,0x02},
	{0x40,0x2a,0x06},
	{0x40,0x2b,0x04},
	{0x40,0x2c,0x02},
	{0x40,0x2d,0x02},
	{0x40,0x2e,0x0e},
	{0x40,0x2f,0x04},
	{0x43,0x02,0xff},
	{0x43,0x03,0xff},
	{0x43,0x04,0x00},
	{0x43,0x05,0x00},
	{0x43,0x06,0x00},
	{0x43,0x08,0x02},
	{0x45,0x00,0x6c},
	{0x45,0x01,0xc4},
	{0x45,0x02,0x44},
	{0x45,0x03,0x01},
	{0x46,0x01,0x4F},
	{0x48,0x00,0x04},
	{0x48,0x13,0x08},
	{0x48,0x1f,0x40},
	{0x48,0x29,0x78},
	{0x48,0x37,0x1b},
	{0x4b,0x00,0x2a},
	{0x4b,0x0d,0x00},
	{0x4d,0x00,0x04},
	{0x4d,0x01,0x42},
	{0x4d,0x02,0xd1},
	{0x4d,0x03,0x93},
	{0x4d,0x04,0xf5},
	{0x4d,0x05,0xc1},
	{0x50,0x00,0xf3},
	{0x50,0x01,0x11},
	{0x50,0x04,0x00},
	{0x50,0x0a,0x00},
	{0x50,0x0b,0x00},
	{0x50,0x32,0x00},
	{0x50,0x40,0x00},
	{0x50,0x50,0x3c},
	{0x55,0x00,0x00},
	{0x55,0x01,0x10},
	{0x55,0x02,0x01},
	{0x55,0x03,0x0f},
	{0x80,0x00,0x00},
	{0x80,0x01,0x00},
	{0x80,0x02,0x00},
	{0x80,0x03,0x00},
	{0x80,0x04,0x00},
	{0x80,0x05,0x00},
	{0x80,0x06,0x00},
	{0x80,0x07,0x00},
	{0x80,0x08,0x00},
	{0x36,0x38,0x00},
	{0x01,0x00,0x01},
};
#endif

static const UINT8 cmd_HDR_code[][3] SENSOR_SIF1DMA_BUF ={	
/*{0x01,0x03,0x01},*/
{0x01,0x00,0x00},
{0x36,0x38,0x00},
{0x03,0x00,0x00},
{0x03,0x02,0x22},
{0x03,0x03,0x00},
{0x03,0x04,0x03},
{0x03,0x0b,0x00},
{0x03,0x0d,0x1e},
{0x03,0x0e,0x04},
{0x03,0x0f,0x01},
{0x03,0x12,0x01},
{0x03,0x1e,0x00},
{0x30,0x00,0x20},
{0x30,0x02,0x00},
{0x30,0x18,0x72},
{0x30,0x20,0x93},
{0x30,0x21,0x03},
{0x30,0x22,0x01},
{0x30,0x31,0x0a},
{0x30,0x3f,0x0c},
{0x33,0x05,0xf1},
{0x33,0x07,0x04},
{0x33,0x09,0x29},
/*L Exp
{0x35,0x00,0x00},
{0x35,0x01,0x60},
{0x35,0x02,0x00},
*/
{0x35,0x03,0x04},
{0x35,0x04,0x00},
{0x35,0x05,0x00},
{0x35,0x06,0x00},
{0x35,0x07,0x00},
/*L Agc
{0x35,0x08,0x00},
{0x35,0x09,0x80},
*/
/*M Exp
{0x35,0x0a,0x00},
{0x35,0x0b,0x30},
{0x35,0x0c,0x00},
*/
{0x35,0x0d,0x00},
/*M Agc
{0x35,0x0e,0x00},
{0x35,0x0f,0x80},
*/
/*S Exp
{0x35,0x10,0x00},
{0x35,0x11,0x18},
{0x35,0x12,0x00},
*/
{0x35,0x13,0x00},
/*S Agc
{0x35,0x14,0x00},
{0x35,0x15,0x80},
*/
{0x35,0x16,0x00},
{0x35,0x17,0x08},
{0x35,0x18,0x00},
{0x35,0x19,0x00},
{0x35,0x1a,0x00},
{0x35,0x1b,0x80},
{0x35,0x1c,0x00},
{0x35,0x1d,0x02},
{0x35,0x1e,0x00},
{0x35,0x1f,0x00},
{0x35,0x20,0x00},
{0x35,0x21,0x80},
{0x35,0x22,0x08},
{0x35,0x24,0x08},
{0x35,0x26,0x08},
{0x35,0x28,0x08},
{0x35,0x2a,0x08},
{0x36,0x02,0x00},
{0x36,0x03,0x40},
{0x36,0x04,0x02},
{0x36,0x05,0x00},
{0x36,0x06,0x00},
{0x36,0x07,0x00},
{0x36,0x09,0x12},
{0x36,0x0a,0x40},
{0x36,0x0c,0x08},
{0x36,0x0f,0xe5},
{0x36,0x08,0x8f},
{0x36,0x11,0x00},
{0x36,0x13,0xf7},
{0x36,0x16,0x58},
{0x36,0x19,0x99},
{0x36,0x1b,0x60},
{0x36,0x1c,0x7a},
{0x36,0x1e,0x79},
{0x36,0x1f,0x02},
{0x36,0x32,0x00},
{0x36,0x33,0x10},
{0x36,0x34,0x10},
{0x36,0x35,0x10},
{0x36,0x36,0x15},
{0x36,0x46,0x86},
{0x36,0x4a,0x0b},
{0x37,0x00,0x17},
{0x37,0x01,0x22},
{0x37,0x03,0x10},
{0x37,0x0a,0x37},
{0x37,0x05,0x00},
{0x37,0x06,0x63},
{0x37,0x09,0x3c},
{0x37,0x0b,0x01},
{0x37,0x0c,0x30},
{0x37,0x10,0x24},
{0x37,0x11,0x0c},
{0x37,0x16,0x00},
{0x37,0x20,0x28},
{0x37,0x29,0x7b},
{0x37,0x2a,0x84},
{0x37,0x2b,0xbd},
{0x37,0x2c,0xbc},
{0x37,0x2e,0x52},
{0x37,0x3c,0x0e},
{0x37,0x3e,0x33},
{0x37,0x43,0x10},
{0x37,0x44,0x88},
{0x37,0x45,0xc0},
{0x37,0x4a,0x43},
{0x37,0x4c,0x00},
{0x37,0x4e,0x23},
{0x37,0x51,0x7b},
{0x37,0x52,0x84},
{0x37,0x53,0xbd},
{0x37,0x54,0xbc},
{0x37,0x56,0x52},
{0x37,0x5c,0x00},
{0x37,0x60,0x00},
{0x37,0x61,0x00},
{0x37,0x62,0x00},
{0x37,0x63,0x00},
{0x37,0x64,0x00},
{0x37,0x67,0x04},
{0x37,0x68,0x04},
{0x37,0x69,0x08},
{0x37,0x6a,0x08},
{0x37,0x6b,0x20},
{0x37,0x6c,0x00},
{0x37,0x6d,0x00},
{0x37,0x6e,0x00},
{0x37,0x73,0x00},
{0x37,0x74,0x51},
{0x37,0x76,0xbd},
{0x37,0x77,0xbd},
{0x37,0x81,0x18},
{0x37,0x83,0x25},
{0x37,0x98,0x1b},
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0a},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xfb},
{0x38,0x08,0x0a},
{0x38,0x09,0x80},
{0x38,0x0a,0x05},
{0x38,0x0b,0xf0},
{0x38,0x0c,0x04},
{0x38,0x0d,0x40},
{0x38,0x0e,0x06},
{0x38,0x0f,0x12},
{0x38,0x10,0x00},
{0x38,0x11,0x08},
{0x38,0x12,0x00},
{0x38,0x13,0x04},
{0x38,0x14,0x01},
{0x38,0x15,0x01},
{0x38,0x19,0x01},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x00},
{0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x00},
{0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x06},
{0x38,0x21,0x06},
#else
{0x38,0x20,0x06},
{0x38,0x21,0x00},
#endif

{0x38,0x29,0x00},
{0x38,0x2a,0x01},
{0x38,0x2b,0x01},
{0x38,0x2d,0x7f},
{0x38,0x30,0x04},
{0x38,0x36,0x01},
{0x38,0x37,0x00},
{0x38,0x41,0x02},
{0x38,0x46,0x0a},
{0x38,0x47,0x07},
{0x3d,0x85,0x36},
{0x3d,0x8c,0x71},
{0x3d,0x8d,0xcb},
{0x3f,0x0a,0x00},
{0x40,0x00,0xf1},
{0x40,0x01,0x40},
{0x40,0x02,0x04},
{0x40,0x03,0x14},
{0x40,0x0d,0x10},
{0x40,0x0e,0x00},
{0x40,0x11,0x00},
{0x40,0x1a,0x00},
{0x40,0x1b,0x00},
{0x40,0x1c,0x00},
{0x40,0x1d,0x00},
{0x40,0x1f,0x00},
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x07},
{0x40,0x23,0xcf},
{0x40,0x24,0x09},
{0x40,0x25,0x60},
{0x40,0x26,0x09},
{0x40,0x27,0x6f},
{0x40,0x28,0x00},
{0x40,0x29,0x02},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2c,0x02},
{0x40,0x2d,0x02},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},
{0x43,0x02,0xff},
{0x43,0x03,0xff},
{0x43,0x04,0x00},
{0x43,0x05,0x00},
{0x43,0x06,0x00},
{0x43,0x08,0x02},
{0x45,0x00,0x6c},
{0x45,0x01,0xc4},
{0x45,0x02,0x40},
{0x45,0x03,0x01},
{0x46,0x01,0xA7},
{0x48,0x00,0x04},
{0x48,0x13,0x08},
{0x48,0x1f,0x40},
{0x48,0x29,0x78},
{0x48,0x37,0x14},
{0x4b,0x00,0x2a},
{0x4b,0x0d,0x00},
{0x4d,0x00,0x04},
{0x4d,0x01,0x42},
{0x4d,0x02,0xd1},
{0x4d,0x03,0x93},
{0x4d,0x04,0xf5},
{0x4d,0x05,0xc1},
{0x50,0x00,0xf3},
{0x50,0x01,0x11},
{0x50,0x04,0x00},
{0x50,0x0a,0x00},
{0x50,0x0b,0x00},
{0x50,0x32,0x00},
{0x50,0x40,0x00},
{0x50,0x50,0x0c},
{0x55,0x00,0x00},
{0x55,0x01,0x10},
{0x55,0x02,0x01},
{0x55,0x03,0x0f},
{0x80,0x00,0x00},
{0x80,0x01,0x00},
{0x80,0x02,0x00},
{0x80,0x03,0x00},
{0x80,0x04,0x00},
{0x80,0x05,0x00},
{0x80,0x06,0x00},
{0x80,0x07,0x00},
{0x80,0x08,0x00},
{0x36,0x38,0x00},
{0x01,0x00,0x01},
};

static const UINT8 cmd_HDR2mode_code[][3] SENSOR_SIF1DMA_BUF ={
/*{0x01,0x03,0x01},*/
{0x36,0x38,0x00},
{0x03,0x00,0x00},
/*{0x03,0x02,0x22},*/
{0x03,0x02,0x1E},/*Change from 0x22 to 0x1E to slow down MIPI to be 720Mbps/lane*/
{0x03,0x03,0x00},
{0x03,0x04,0x03},
{0x03,0x0b,0x00},
{0x03,0x0d,0x1e},
{0x03,0x0e,0x04},
{0x03,0x0f,0x01},
{0x03,0x12,0x01},
{0x03,0x1e,0x00},
{0x30,0x00,0x20},
{0x30,0x02,0x00},
{0x30,0x18,0x72},
{0x30,0x20,0x93},
{0x30,0x21,0x03},
{0x30,0x22,0x01},
{0x30,0x31,0x0a},
{0x30,0x3f,0x0c},
{0x33,0x05,0xf1},
{0x33,0x07,0x04},
{0x33,0x09,0x29},
/*L Exp
{0x35,0x00,0x00},
{0x35,0x01,0x60},
{0x35,0x02,0x00},
*/
{0x35,0x03,0x04},
{0x35,0x04,0x00},
{0x35,0x05,0x00},
{0x35,0x06,0x00},
{0x35,0x07,0x00},
/*L Agc
{0x35,0x08,0x00},
{0x35,0x09,0x80},
*/
/*M Exp
{0x35,0x0a,0x00},
{0x35,0x0b,0x30},
{0x35,0x0c,0x00},
*/
{0x35,0x0d,0x00},
/*M Agc
{0x35,0x0e,0x00},
{0x35,0x0f,0x80},
*/
/*S Exp
{0x35,0x10,0x00},
{0x35,0x11,0x18},
{0x35,0x12,0x00},
*/
{0x35,0x13,0x00},
/*S Agc
{0x35,0x14,0x00},
{0x35,0x15,0x80},
*/
{0x35,0x16,0x00},
{0x35,0x17,0x08},
{0x35,0x18,0x00},
{0x35,0x19,0x00},
{0x35,0x1a,0x00},
{0x35,0x1b,0x80},
{0x35,0x1c,0x00},
{0x35,0x1d,0x02},
{0x35,0x1e,0x00},
{0x35,0x1f,0x00},
{0x35,0x20,0x00},
{0x35,0x21,0x80},
{0x35,0x22,0x08},
{0x35,0x24,0x08},
{0x35,0x26,0x08},
{0x35,0x28,0x08},
{0x35,0x2a,0x08},
{0x36,0x02,0x00},
{0x36,0x03,0x40},
{0x36,0x04,0x02},
{0x36,0x05,0x00},
{0x36,0x06,0x00},
{0x36,0x07,0x00},
{0x36,0x09,0x12},
{0x36,0x0a,0x40},
{0x36,0x0c,0x08},
{0x36,0x0f,0xe5},
{0x36,0x08,0x8f},
{0x36,0x11,0x00},
{0x36,0x13,0xf7},
{0x36,0x16,0x58},
{0x36,0x19,0x99},
{0x36,0x1b,0x60},
{0x36,0x1c,0x7a},
{0x36,0x1e,0x79},
{0x36,0x1f,0x02},
{0x36,0x32,0x00},
{0x36,0x33,0x10},
{0x36,0x34,0x10},
{0x36,0x35,0x10},
{0x36,0x36,0x15},
{0x36,0x46,0x86},
{0x36,0x4a,0x0b},
{0x37,0x00,0x17},
{0x37,0x01,0x22},
{0x37,0x03,0x10},
{0x37,0x0a,0x37},
{0x37,0x05,0x00},
{0x37,0x06,0x63},
{0x37,0x09,0x3c},
{0x37,0x0b,0x01},
{0x37,0x0c,0x30},
{0x37,0x10,0x24},
{0x37,0x11,0x0c},
{0x37,0x16,0x00},
{0x37,0x20,0x28},
{0x37,0x29,0x7b},
{0x37,0x2a,0x84},
{0x37,0x2b,0xbd},
{0x37,0x2c,0xbc},
{0x37,0x2e,0x52},
{0x37,0x3c,0x0e},
{0x37,0x3e,0x33},
{0x37,0x43,0x10},
{0x37,0x44,0x88},
{0x37,0x45,0xc0},
{0x37,0x4a,0x43},
{0x37,0x4c,0x00},
{0x37,0x4e,0x23},
{0x37,0x51,0x7b},
{0x37,0x52,0x84},
{0x37,0x53,0xbd},
{0x37,0x54,0xbc},
{0x37,0x56,0x52},
{0x37,0x5c,0x00},
{0x37,0x60,0x00},
{0x37,0x61,0x00},
{0x37,0x62,0x00},
{0x37,0x63,0x00},
{0x37,0x64,0x00},
{0x37,0x67,0x04},
{0x37,0x68,0x04},
{0x37,0x69,0x08},
{0x37,0x6a,0x08},
{0x37,0x6b,0x20},
{0x37,0x6c,0x00},
{0x37,0x6d,0x00},
{0x37,0x6e,0x00},
{0x37,0x73,0x00},
{0x37,0x74,0x51},
{0x37,0x76,0xbd},
{0x37,0x77,0xbd},
{0x37,0x81,0x18},
{0x37,0x83,0x25},
{0x37,0x98,0x1b},
{0x38,0x00,0x00},
{0x38,0x01,0x08},
{0x38,0x02,0x00},
{0x38,0x03,0x04},
{0x38,0x04,0x0a},
{0x38,0x05,0x97},
{0x38,0x06,0x05},
{0x38,0x07,0xfb},
{0x38,0x08,0x0a},
{0x38,0x09,0x80},
{0x38,0x0a,0x05},
{0x38,0x0b,0xe8},/*{0x38,0x0b,0xf0} size:1512*/
/*
{0x38,0x0c,0x04},
{0x38,0x0d,0x40},
*/

/*Change the line total to be 0x502=1282*/
{0x38,0x0c,0x05},
{0x38,0x0d,0x02},

/*Remove the frame length*/
/*
{0x38,0x0e,0x07},
{0x38,0x0f,0x2e},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x08},
{0x38,0x12,0x00},
{0x38,0x13,0x08},/*{0x38,0x13,0x04},4(original)+4, more offset:4*/
{0x38,0x14,0x01},
{0x38,0x15,0x01},
{0x38,0x19,0x01},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x00},
{0x38,0x21,0x06},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x00},
{0x38,0x21,0x00},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x06},
{0x38,0x21,0x06},
#else
{0x38,0x20,0x06},
{0x38,0x21,0x00},
#endif

{0x38,0x29,0x00},
{0x38,0x2a,0x01},
{0x38,0x2b,0x01},
{0x38,0x2d,0x7f},
{0x38,0x30,0x04},
{0x38,0x36,0x01},
{0x38,0x37,0x00},
{0x38,0x41,0x02},
{0x38,0x46,0x09},
{0x38,0x47,0x07},
{0x3d,0x85,0x36},
{0x3d,0x8c,0x71},
{0x3d,0x8d,0xcb},
{0x3f,0x0a,0x00},
{0x40,0x00,0xf1},
{0x40,0x01,0x40},
{0x40,0x02,0x04},
{0x40,0x03,0x14},
{0x40,0x0e,0x00},
{0x40,0x0d,0x10},
{0x40,0x11,0x00},
{0x40,0x1a,0x00},
{0x40,0x1b,0x00},
{0x40,0x1c,0x00},
{0x40,0x1d,0x00},
{0x40,0x1f,0x00},
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x07},
{0x40,0x23,0xcf},
{0x40,0x24,0x09},
{0x40,0x25,0x60},
{0x40,0x26,0x09},
{0x40,0x27,0x6f},
{0x40,0x28,0x00},
{0x40,0x29,0x02},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2c,0x02},
{0x40,0x2d,0x02},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},
{0x43,0x02,0xff},
{0x43,0x03,0xff},
{0x43,0x04,0x00},
{0x43,0x05,0x00},
{0x43,0x06,0x00},
{0x43,0x08,0x02},
{0x45,0x00,0x6c},
{0x45,0x01,0xc4},
{0x45,0x02,0x40},
{0x45,0x03,0x01},
{0x46,0x01,0xA7},
{0x48,0x00,0x04},
{0x48,0x13,0x08},
{0x48,0x1f,0x40},
{0x48,0x29,0x78},

{0x48,0x37,0x16},/*Change from 0x14 to 0x16 (MIPI speed =720Mbps/lane)*/
/*{0x48,0x37,0x14},*/

{0x4b,0x00,0x2a},
{0x4b,0x0d,0x00},
{0x4d,0x00,0x04},
{0x4d,0x01,0x42},
{0x4d,0x02,0xd1},
{0x4d,0x03,0x93},
{0x4d,0x04,0xf5},
{0x4d,0x05,0xc1},
{0x50,0x00,0xf3},
{0x50,0x01,0x11},
{0x50,0x04,0x00},
{0x50,0x0a,0x00},
{0x50,0x0b,0x00},
{0x50,0x32,0x00},
{0x50,0x40,0x00},
{0x50,0x50,0x0c},
{0x55,0x00,0x00},
{0x55,0x01,0x10},
{0x55,0x02,0x01},
{0x55,0x03,0x0f},
{0x80,0x00,0x00},
{0x80,0x01,0x00},
{0x80,0x02,0x00},
{0x80,0x03,0x00},
{0x80,0x04,0x00},
{0x80,0x05,0x00},
{0x80,0x06,0x00},
{0x80,0x07,0x00},
{0x80,0x08,0x00},
{0x36,0x38,0x00},
{0x01,0x00,0x01},
};

/*The necessary settings from HDR mode back to normal preview mode.*/
static const UINT8 cmd_HDRBackToNormal_code[][3] SENSOR_SIF1DMA_BUF ={ 
	{0x35,0x0a,0x00},
	{0x35,0x0b,0x00},
	{0x35,0x0c,0x00},
	{0x35,0x0d,0x00},
	{0x35,0x0e,0x00},
	{0x35,0x0f,0x80},
	{0x35,0x10,0x00},
	{0x35,0x11,0x00},
	{0x35,0x12,0x00},
	{0x35,0x13,0x00},
	{0x35,0x14,0x00},
	{0x35,0x15,0x80},
	{0x35,0x16,0x00},
	{0x35,0x17,0x00},
	{0x35,0x18,0x00},
	{0x35,0x19,0x00},
	{0x35,0x1a,0x00},
	{0x35,0x1b,0x80},
	{0x35,0x1c,0x00},
	{0x35,0x1d,0x00},
	{0x35,0x1e,0x00},
	{0x35,0x1f,0x00},
	{0x35,0x20,0x00},
	{0x35,0x21,0x80},

	{0x38,0x46,0x08},

	{0x48,0x37,0x10},
};


/*cropped quarter 240fps*/
static const UINT8 cmd_cropped_quarter_code[][3] SENSOR_SIF1DMA_BUF ={

{0x01,0x00,0x00},
                 
{0x03,0x00,0x02},
{0x03,0x02,0x32},
{0x03,0x03,0x01},
{0x48,0x37,0x36},
                 
{0x36,0x32,0x05},
{0x37,0x4a,0x43},
{0x37,0x6b,0x40},
                 
#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x10},
{0x38,0x21,0x07},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x10},
{0x38,0x21,0x01},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x16},
{0x38,0x21,0x07},
#else
{0x38,0x20,0x16},
{0x38,0x21,0x01},
#endif

{0x38,0x2a,0x03},
{0x38,0x30,0x08},
{0x38,0x36,0x02},
{0x40,0x01,0x50},
{0x40,0x03,0x14},
{0x45,0x02,0x44},
{0x50,0x50,0x3c},
                 
{0x38,0x00,0x02},
{0x38,0x01,0x78},
{0x38,0x02,0x00},
{0x38,0x03,0xe0},
{0x38,0x04,0x08},
{0x38,0x05,0x27},
{0x38,0x06,0x05},
{0x38,0x07,0x27},/*Add 8  to V end*/
{0x38,0x08,0x02},
{0x38,0x09,0xd0},
{0x38,0x0A,0x02},
/*{0x38,0x0B,0x1C},*/
{0x38,0x0B,0x20},/*Add 4 more lines*/


{0x38,0x0C,0x03},/*Modify HTS to be 843*/
{0x38,0x0D,0x4B},

/*872 is the original HTS value*/
/*
{0x38,0x0C,0x03}, 
{0x38,0x0D,0x68},
*/

/*remove frame length
{0x38,0x0E,0x02},
{0x38,0x0F,0x3d},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x04},
{0x38,0x12,0x00},
{0x38,0x13,0x02},
                 
{0x38,0x14,0x03},
{0x38,0x15,0x01},
                 
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x01},
{0x40,0x23,0x63},
{0x40,0x24,0x02},
{0x40,0x25,0x90},
{0x40,0x26,0x02},
{0x40,0x27,0xa0},
{0x40,0x2a,0x06},
{0x40,0x2b,0x04},
{0x40,0x2e,0x0e},
{0x40,0x2f,0x04},

{0x46,0x00,0x00},
{0x46,0x01,0x2C},
                 
{0x01,0x00,0x01},/* Streaming   */
};

/*4x4 binning 240fps*/
static const UINT8 cmd_bin4_code[][3] SENSOR_SIF1DMA_BUF ={

{0x01,0x00,0x00},
                 
{0x03,0x00,0x02},
{0x03,0x02,0x32},
{0x03,0x03,0x01},
{0x48,0x37,0x35},
                 
{0x36,0x32,0x06},
{0x37,0x4a,0x47},
{0x37,0x6b,0x40},

#if (MIRROR_FLIP_EN==0)
{0x38,0x20,0x20},
{0x38,0x21,0x0f},
#elif (MIRROR_FLIP_EN==1)
{0x38,0x20,0x20},
{0x38,0x21,0x09},
#elif (MIRROR_FLIP_EN==2)
{0x38,0x20,0x26},
{0x38,0x21,0x0f},
#else
{0x38,0x20,0x26},
{0x38,0x21,0x09},
#endif
                 
{0x38,0x2a,0x07},
{0x38,0x30,0x08},
{0x38,0x36,0x02},
{0x40,0x01,0x60},
{0x40,0x03,0x0c},
{0x45,0x02,0x48},
{0x50,0x50,0x3c},
                 
{0x38,0x00,0x00},
{0x38,0x01,0x00},
{0x38,0x02,0x00},
{0x38,0x03,0x00},
{0x38,0x04,0x0a},
{0x38,0x05,0x9f},
{0x38,0x06,0x05},
{0x38,0x07,0xff},
{0x38,0x08,0x02},
{0x38,0x09,0xa0},
{0x38,0x0A,0x01},
{0x38,0x0B,0x7c},

{0x38,0x0C,0x03},/*Modify HTS to be 860*/
{0x38,0x0D,0x5c},

/*remove frame length
{0x38,0x0E,0x01},
{0x38,0x0F,0xa5},
*/
{0x38,0x10,0x00},
{0x38,0x11,0x04},
{0x38,0x12,0x00},
{0x38,0x13,0x02},
                 
{0x38,0x14,0x07},
{0x38,0x15,0x01},
                 
{0x40,0x20,0x00},
{0x40,0x21,0x10},
{0x40,0x22,0x01},
{0x40,0x23,0x2b},
{0x40,0x24,0x02},
{0x40,0x25,0x58},
{0x40,0x26,0x02},
{0x40,0x27,0x67},

{0x40,0x2a,0x04},
{0x40,0x2b,0x02},
{0x40,0x2e,0x08},
{0x40,0x2f,0x02},

{0x46,0x00,0x00},
{0x46,0x01,0x29},
                 
{0x01,0x00,0x01},/* Streaming   */
};


/*sensor mipi cfg*/
static const sensorLVDSCfg_t sensorLVDSCfg =     
{   
	.t = FRONT_RX_BIAS_10UA,
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
	/*.settle = 18,*/
};                                               

static const sensorLVDSCfg_t sensorLVDSCfg2 =     
{  	
	.t = FRONT_RX_BIAS_10UA,
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

static const sensorLVDSCfg_t sensorLVDSCfg_HDR =     
{          
	.t = FRONT_RX_BIAS_10UA,
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
	.Lvdseofsel = 0x01,		/*count EOL*/
	.Ctrlword02_sol=0xD60, 	/*virtual channel 2*/
	.Ctrlword03_sol=0xD50, 	/*virtual channel 3*/
	.settle = 8,/*1UI=1000/720=1.3888ns, 115+8*UI=126.1ns, 126.1*(720/8)M/1000=11.349=>12,  12-4(fixed) = 8*/
}; 

/*for solving V35 auto decod word count erro*/
static const sensorLVDSCfg_t sensorLVDSCfg_VGA =     
{   
	.t = FRONT_RX_BIAS_10UA,
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
	.Lvdseofsel = 0x01,/*count EOL*/
	.settle = 2,/*1UI=1000/300=3.333ns, 115+8*UI=141.664ns, 141.664*(300/8)M/1000=5.3124 =>6,  6-4(fixed) = 2*/
};   

/*for solving V35 auto decod word count erro*/
static const sensorLVDSCfg_t sensorLVDSCfg_FULLHD_60FPS_2LANE =     
{  
	.t = FRONT_RX_BIAS_10UA,
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
	.SenPinSelNum =2,
	.SenPinSel={0x0,0x1},
	.Lvdseofsel = 0x01,/*count EOL*/
};  


#if 0
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
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_VGA_CFAPATTERN,
	.hratio = 1600, 
	.vratio = 900,
	.fps_mhz = (PREVIEW_VGA_FRAMERATE_KFPS*1000),
	.sensor_pclk = 120000000,
	.sensor_htotal = PREVIEW_VGA_SENSOR_LINETOTAL,			
	.pix_100khz =PREVIEW_VGA_CDSPCLK,
	.bayerhs_size = PREVIEW_VGA_BAYERHS_SIZE,
};
#endif
static const sensorTgPreviewCfg_t sensorTgPreview720P_30fps_SCALECfg = {
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
		.xSize = PREVIEW_720P_30fps_SCALE_HSIZE,
		.ySize = PREVIEW_720P_30fps_SCALE_VSIZE,
	},
	.hsize =					PREVIEW_720P_30fps_SCALE_HSIZE,
	.vsize = 					PREVIEW_720P_30fps_SCALE_VSIZE,  
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
    .model_info =               SENSORMODEL_DUAL_MIPI_RAW,
#else
    .model_info =               SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_720P_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_720P_30fps_SCALE_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_720P_30fps_SCALE_PCLK1X,
	.sensor_htotal = PREVIEW_720P_30fps_SCALE_LINETOTAL, 		
	.pix_100khz =PREVIEW_720P_30fps_SCALE_CDSPCLK,
	.bayerhs_size = PREVIEW_720P_30fps_SCALE_BAYERHS_SIZE,	
};
static const sensorTgPreviewCfg_t sensorTgPreview720P_60fps_SCALECfg = {
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
		.xSize = PREVIEW_720P_60fps_SCALE_HSIZE,
		.ySize = PREVIEW_720P_60fps_SCALE_VSIZE,
	},
	.hsize =					PREVIEW_720P_60fps_SCALE_HSIZE,
	.vsize = 					PREVIEW_720P_60fps_SCALE_VSIZE,  
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
    .model_info =               SENSORMODEL_DUAL_MIPI_RAW,
#else
    .model_info =               SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_720P_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_720P_60fps_SCALE_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_720P_60fps_SCALE_PCLK1X,
	.sensor_htotal = PREVIEW_720P_60fps_SCALE_LINETOTAL, 		
	.pix_100khz =PREVIEW_720P_60fps_SCALE_CDSPCLK,
	.bayerhs_size = PREVIEW_720P_60fps_SCALE_BAYERHS_SIZE,
};


static const sensorTgPreviewCfg_t sensorTgPreview720P_SCALECfg = {
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
		.xSize = PREVIEW_720P_120fps_SCALE_HSIZE,
		.ySize = PREVIEW_720P_120fps_SCALE_VSIZE,
	},
	.hsize =					PREVIEW_720P_120fps_SCALE_HSIZE,
	.vsize = 					PREVIEW_720P_120fps_SCALE_VSIZE,  
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
    .model_info =               SENSORMODEL_DUAL_MIPI_RAW,
#else
    .model_info =               SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_720P_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_720P_120fps_SCALE_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_720P_120fps_SCALE_PCLK1X,
	.sensor_htotal = PREVIEW_720P_120fps_SCALE_LINETOTAL, 		
	.pix_100khz =PREVIEW_720P_120fps_SCALE_CDSPCLK,
	.bayerhs_size = PREVIEW_720P_120fps_SCALE_BAYERHS_SIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreview720P_SCALE_BINSUMCfg = {
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
		.xSize = PREVIEW_720P_SCALE_BINSUM_HSIZE,
		.ySize = PREVIEW_720P_SCALE_BINSUM_VSIZE,
	},
	.hsize =					PREVIEW_720P_SCALE_BINSUM_HSIZE,
	.vsize = 					PREVIEW_720P_SCALE_BINSUM_VSIZE,  
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
    .model_info =               SENSORMODEL_DUAL_MIPI_RAW,
#else
    .model_info =               SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_720P_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_720P_SCALE_BINSUM_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_720P_SCALE_BINSUM_PCLK1X,
	.sensor_htotal = PREVIEW_720P_SCALE_BINSUM_LINETOTAL, 		
	.pix_100khz =PREVIEW_720P_SCALE_BINSUM_CDSPCLK,
	.bayerhs_size = PREVIEW_720P_SCALE_BINSUM_BAYERHS_SIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreview720P_CROPCfg = {
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
		.xSize = PREVIEW_720P_CROP_HSIZE,
		.ySize = PREVIEW_720P_CROP_VSIZE,
	},
	.hsize =					PREVIEW_720P_CROP_HSIZE,
	.vsize = 					PREVIEW_720P_CROP_VSIZE,  
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
    .model_info =               SENSORMODEL_DUAL_MIPI_RAW,
#else
    .model_info =               SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_720P_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_720P_CROP_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_720P_CROP_PCLK1X,
	.sensor_htotal = PREVIEW_720P_CROP_LINETOTAL, 		
	.pix_100khz =PREVIEW_720P_CROP_CDSPCLK,
	.bayerhs_size = PREVIEW_720P_CROP_BAYERHS_SIZE,
};
static const sensorTgPreviewCfg_t sensorTgPreviewQUARTERCfg = {
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
		.xSize = PREVIEW_QUARTER_HSIZE,
		.ySize = PREVIEW_QUARTER_VSIZE,
	},
	.hsize =					PREVIEW_QUARTER_HSIZE,
	.vsize =					PREVIEW_QUARTER_VSIZE,
	.sensor_model =				SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif

	.cfa_pattern =				PREVIEW_QUARTER_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_QUARTER_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_QUARTER_PCLK1X,
	.sensor_htotal = PREVIEW_QUARTER_LINETOTAL,	
	.pix_100khz =PREVIEW_QUARTER_CDSPCLK,
	.bayerhs_size = PREVIEW_QUARTER_BAYERHS_SIZE,
};
static const sensorTgPreviewCfg_t sensorTgPreviewQUARTER1440P30FPSCfg = {
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
		.xSize = PREVIEW_QUARTER1440P30_HSIZE,
		.ySize = PREVIEW_QUARTER1440P30_VSIZE,
	},
	.hsize = 					PREVIEW_QUARTER1440P30_HSIZE,
	.vsize =                    PREVIEW_QUARTER1440P30_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_QUARTER1440P_CFAPATTERN,
	.hratio = 400,
	.vratio = 300,
	.fps_mhz = (PREVIEW_QUARTER1440P30_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_QUARTER1440P30_PCLK1X,
	.sensor_htotal = PREVIEW_QUARTER1440P30_LINETOTAL, 		
	.pix_100khz =PREVIEW_QUARTER1440P30_CDSPCLK,
	.bayerhs_size = PREVIEW_QUARTER1440P30_BAYERHS_SIZE,
};

static const sensorTgPreviewCfg_t sensorTgPreviewQUARTER1440P60FPSCfg = {
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
		.xSize = PREVIEW_QUARTER1440P60_HSIZE,
		.ySize = PREVIEW_QUARTER1440P60_VSIZE,
	},
	.hsize = 					PREVIEW_QUARTER1440P60_HSIZE,
	.vsize =                    PREVIEW_QUARTER1440P60_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_QUARTER1440P_CFAPATTERN,
	.hratio = 400,
	.vratio = 300,
	.fps_mhz = (PREVIEW_QUARTER1440P60_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_QUARTER1440P60_PCLK1X,
	.sensor_htotal = PREVIEW_QUARTER1440P60_LINETOTAL, 			
	.pix_100khz =PREVIEW_QUARTER1440P60_CDSPCLK,
	.bayerhs_size = PREVIEW_QUARTER1440P60_BAYERHS_SIZE,	
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
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif

	.cfa_pattern =				PREVIEW_1080P_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_1080P_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_1080P_PCLK1X,
	.sensor_htotal = PREVIEW_1080P_LINETOTAL, 		
	.pix_100khz =PREVIEW_1080P_CDSPCLK,
	.bayerhs_size = PREVIEW_1080P_BAYERHS_SIZE,	
};
static const sensorTgPreviewCfg_t sensorTgPreview1440P30FPSCfg = {
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
		.xSize = PREVIEW_1440P30_HSIZE,
		.ySize = PREVIEW_1440P30_VSIZE,
	},
	.hsize = 					PREVIEW_1440P30_HSIZE,
	.vsize =                    PREVIEW_1440P30_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 400,
	.vratio = 300,
	.fps_mhz = (PREVIEW_1440P30_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_1440P30_PCLK1X,
	.sensor_htotal = PREVIEW_1440P30_LINETOTAL,			
	.pix_100khz =PREVIEW_1440P30_CDSPCLK,
	.bayerhs_size = PREVIEW_1440P30_BAYERHS_SIZE,		
};

static const sensorTgPreviewCfg_t sensorTgPreview1440P60FPSCfg = {
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
		.xSize = PREVIEW_1440P60_HSIZE,
		.ySize = PREVIEW_1440P60_VSIZE,
	},
	.hsize = 					PREVIEW_1440P60_HSIZE,
	.vsize =                    PREVIEW_1440P60_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 400,
	.vratio = 300,
	.fps_mhz = (PREVIEW_1440P60_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_1440P60_PCLK1X,
	.sensor_htotal = PREVIEW_1440P60_LINETOTAL,			
	.pix_100khz =PREVIEW_1440P60_CDSPCLK,
	.bayerhs_size = PREVIEW_1440P60_BAYERHS_SIZE,			
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
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =				PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_FULL_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_FULL_PCLK1X,
	.sensor_htotal = PREVIEW_FULL_LINETOTAL, 		
	.pix_100khz =PREVIEW_FULL_CDSPCLK,
	.bayerhs_size = PREVIEW_FULL_BAYERHS_SIZE,	

};

static const sensorTgPreviewCfg_t sensorTgPreviewFull30FPSCfg = {
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
		.xSize = PREVIEW_Full30_HSIZE,
		.ySize = PREVIEW_Full30_VSIZE,
	},
	.hsize = 					PREVIEW_Full30_HSIZE,
	.vsize =                    PREVIEW_Full30_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_Full30_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_Full30_PCLK1X,
	.sensor_htotal = PREVIEW_Full30_LINETOTAL,			
	.pix_100khz =PREVIEW_Full30_CDSPCLK,
	.bayerhs_size = PREVIEW_Full30_BAYERHS_SIZE,		
};

static const sensorTgPreviewCfg_t sensorTgPreviewFull30FPS_CROPCfg = {
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
		.xSize = PREVIEW_Full30_CROP_HSIZE,
		.ySize = PREVIEW_Full30_CROP_VSIZE,
	},
	.hsize = 					PREVIEW_Full30_CROP_HSIZE,
	.vsize =                    PREVIEW_Full30_CROP_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_Full30_CROP_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_Full30_CROP_PCLK1X,
	.sensor_htotal = PREVIEW_Full30_CROP_LINETOTAL,			
	.pix_100khz =PREVIEW_Full30_CROP_CDSPCLK,
	.bayerhs_size = PREVIEW_Full30_CROP_BAYERHS_SIZE,		
};

static const sensorTgPreviewCfg_t sensorTgPreviewFull27p5FPSCfg = {
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
		.xSize = PREVIEW_Full27p5_HSIZE,
		.ySize = PREVIEW_Full27p5_VSIZE,
	},
	.hsize = 					PREVIEW_Full27p5_HSIZE,
	.vsize =                    PREVIEW_Full27p5_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_Full27p5_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_Full27p5_PCLK1X,
	.sensor_htotal = PREVIEW_Full27p5_LINETOTAL,			
	.pix_100khz =PREVIEW_Full27p5_CDSPCLK,
	.bayerhs_size = PREVIEW_Full27p5_BAYERHS_SIZE,			
};

static const sensorTgPreviewCfg_t sensorTgPreviewFull55FPSCfg = {
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
		.xSize = PREVIEW_Full55_HSIZE,
		.ySize = PREVIEW_Full55_VSIZE,
	},
	.hsize = 					PREVIEW_Full55_HSIZE,
	.vsize =                    PREVIEW_Full55_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_Full55_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_Full55_PCLK1X,
	.sensor_htotal = PREVIEW_Full55_LINETOTAL,			
	.pix_100khz =PREVIEW_Full55_CDSPCLK,
	.bayerhs_size = PREVIEW_Full55_BAYERHS_SIZE,				
};


static const sensorTgPreviewCfg_t sensorTgPreviewFull60FPSCfg = {
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
		.xSize = PREVIEW_Full60_HSIZE,
		.ySize = PREVIEW_Full60_VSIZE,
	},
	.hsize = 					PREVIEW_Full60_HSIZE,
	.vsize =                    PREVIEW_Full60_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_Full60_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_Full60_PCLK1X,
	.sensor_htotal = PREVIEW_Full60_LINETOTAL,			
	.pix_100khz =PREVIEW_Full60_CDSPCLK,
	.bayerhs_size = PREVIEW_Full60_BAYERHS_SIZE,					
};

static const sensorTgPreviewCfg_t sensorTgPreviewQUARTER_VBin_HFullCfg = {
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
		.xSize = PREVIEW_QUARTER_VBIN_HFULL_HSIZE,
		.ySize = PREVIEW_QUARTER_VBIN_HFULL_VSIZE,
	},
	.hsize = 					PREVIEW_QUARTER_VBIN_HFULL_HSIZE,
	.vsize =                    PREVIEW_QUARTER_VBIN_HFULL_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_QUARTER_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_QUARTER_VBIN_HFULL_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_QUARTER_VBIN_HFULL_PCLK1X,
	.sensor_htotal = PREVIEW_QUARTER_VBIN_HFULL_LINETOTAL,			
	.pix_100khz =PREVIEW_QUARTER_VBIN_HFULL_CDSPCLK,
	.bayerhs_size = PREVIEW_QUARTER_VBIN_HFULL_BAYERHS_SIZE,						
};

static const sensorTgPreviewCfg_t sensorTgPreviewFullHD_60FPS_2LANECfg = {
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
		.xSize = PREVIEW_FullHD60_2LANE_HSIZE,
		.ySize = PREVIEW_FullHD60_2LANE_VSIZE,
	},
	.hsize = 					PREVIEW_FullHD60_2LANE_HSIZE,
	.vsize =                    PREVIEW_FullHD60_2LANE_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_FullHD60_2LANE_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_FullHD60_2LANE_PCLK1X,
	.sensor_htotal = PREVIEW_FullHD60_2LANE_LINETOTAL,			
	.pix_100khz =PREVIEW_FullHD60_2LANE_CDSPCLK,
	.bayerhs_size = PREVIEW_FullHD60_2LANE_BAYERHS_SIZE,							
};

static const sensorTgPreviewCfg_t sensorTgPreviewQUARTER_CROP_240Cfg = {
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
		.xSize = PREVIEW_QUARTER_CROP_240_HSIZE,
		.ySize = PREVIEW_QUARTER_CROP_240_VSIZE,
	},
	.hsize = 					PREVIEW_QUARTER_CROP_240_HSIZE,
	.vsize =                    PREVIEW_QUARTER_CROP_240_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_QUARTER_CFAPATTERN,
	.hratio = 400,
	.vratio = 300,
	.fps_mhz = (PREVIEW_QUARTER_CROP_240_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_QUARTER_CROP_240_PCLK1X,
	.sensor_htotal = PREVIEW_QUARTER_CROP_240_LINETOTAL,			
	.pix_100khz =PREVIEW_QUARTER_CROP_240_CDSPCLK,
	.bayerhs_size = PREVIEW_QUARTER_CROP_240_BAYERHS_SIZE,							
};

static const sensorTgPreviewCfg_t sensorTgPreviewBIN4Cfg = {
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
		.xSize = PREVIEW_BIN4_HSIZE,
		.ySize = PREVIEW_BIN4_VSIZE,
	},
	.hsize = 					PREVIEW_BIN4_HSIZE,
	.vsize =                    PREVIEW_BIN4_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_BIN4_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (PREVIEW_BIN4_FRAMERATE_KFPS*1000),
	.sensor_pclk = PREVIEW_BIN4_PCLK1X,
	.sensor_htotal = PREVIEW_BIN4_LINETOTAL,			
	.pix_100khz =PREVIEW_BIN4_CDSPCLK,
	.bayerhs_size = PREVIEW_BIN4_BAYERHS_SIZE,							
};

#if HDR_test
static const sensorTgPreviewCfg_t sensorTgHDR720pCfg = {
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
		.xSize = HDR720p_HSIZE,
		.ySize = HDR720p_VSIZE,
	},
	.hsize = 					HDR720p_HSIZE,
	.vsize =                    HDR720p_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (HDR720p_FRAMERATE_KFPS*1000),
	.sensor_pclk = HDR720p_PCLK1X,
	.sensor_htotal = HDR720p_LINETOTAL,			
	.pix_100khz =HDR720p_CDSPCLK,
	.bayerhs_size	=HDR720p_BAYERHS_SIZE, 
};
static const sensorTgPreviewCfg_t sensorTgHDRCfg = {
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
		.xSize = HDR_HSIZE,
		.ySize = HDR_VSIZE,
	},
	.hsize = 					HDR_HSIZE,
	.vsize =                    HDR_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 900,
	.fps_mhz = (HDR_FRAMERATE_KFPS*1000),
	.sensor_pclk = HDR_PCLK1X,
	.sensor_htotal = HDR_LINETOTAL,			
	.pix_100khz =HDR_CDSPCLK,
	.bayerhs_size	=HDR_BAYERHS_SIZE, 
};
#endif
static const sensorTgPreviewCfg_t sensorTgHDR2modeCfg = {
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
		.xSize = HDR2mode_HSIZE,
		.ySize = HDR2mode_VSIZE,
	},
	.hsize = 					HDR2mode_HSIZE,
	.vsize =                    HDR2mode_VSIZE,
	.sensor_model =             SENSORMODEL,
#if DUAL_RAW
	.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
	.model_info =				SENSORMODEL_MIPI_RAW,
#endif
	.cfa_pattern =              PREVIEW_FULL_CFAPATTERN,
	.hratio = 1600,
	.vratio = 1800,
	.fps_mhz = (HDR2mode_FRAMERATE_KFPS*1000),
	.sensor_pclk = HDR2mode_PCLK1X,
	.sensor_htotal = HDR2mode_LINETOTAL,			
	.pix_100khz =HDR2mode_CDSPCLK,
	.bayerhs_size	=HDR2mode_BAYERHS_SIZE, 
};

#if 0
static const sensorTgPreviewCfg_t *sensorTgPreviewCfgTbl[] = {
#if 0
    &sensorTgPreviewFull60FPSCfg,            /* bin2-UXGA30FPS(320Mbps)           0x30 */          
	&sensorTgPreviewFull60FPSCfg, 		   /* bin2-UXGA30FPS(320Mbps)			0x30 */ 		 
	&sensorTgPreviewFull60FPSCfg, 		   /* bin2-UXGA30FPS(320Mbps)			0x30 */ 		 
	&sensorTgPreviewFull60FPSCfg, 		   /* bin2-UXGA30FPS(320Mbps)			0x30 */ 		 

    &sensorTgPreviewFull60FPSCfg,            /* bin2-UXGA30FPS(320Mbps)           0x30 */          
#endif		 
#if 1
    &sensorTgPreviewVGACfg,            		/* bin2	-VGA@30fps          0x30 */          
    &sensorTgPreview720P_SCALECfg,     		/* scale	-720p@120fps        0x31 */          
    &sensorTgPreview720P_CROPCfg,      		/* crop	-720p@120fps        0x32 */          
    &sensorTgPreviewFull60FPSCfg,      		/* scale	-1080P@60fps       0x33 */          
    &sensorTgPreview1080PCfg,      	   		/* crop 	-1080P@60fps       0x34 */    
    &sensorTgHDRCfg,				   		/* HDR       				0x35 */
    &sensorTgPreviewFull30FPSCfg,      		/* scale	-1080P@30fps       0x36 */ 
    &sensorTgPreview720P_30fps_SCALECfg,    /* scale	-720p@30fps        0x37 */ 
    &sensorTgPreview720P_60fps_SCALECfg,    /* scale	-720p@60fps        0x38 */ 
 
#endif	
};  
#endif
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
		.model_info =				SENSORMODEL_MIPI_RAW,
#endif
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
		.pix_100khz =CAPTURE_CDSPCLK,
		.sensor_htotal = CAPTURE_LINETOTAL,
		.sensor_pclk = CAPTURE_PCLK1X,
};

#if 0
static  sensorTgSnapCfg_t sensorTgHDRRollCfg = {
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
 		.hsize =                   HDR_HSIZE,
		.vsize =				   HDR_VSIZE,
		.sensor_model =			   SENSORMODEL,/*TYPE_HSCMOS,*/
#if DUAL_RAW
		.model_info =				SENSORMODEL_DUAL_MIPI_RAW,
#else
		.model_info =				SENSORMODEL_MIPI_RAW,
#endif
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
		.pix_100khz =HDR_CDSPCLK,
};

#endif
#if 0
static const sensorTgSnapCfg_t *sensorTgSnapCfgTbl[] = {
	&sensorTgSnapRollCfg, 
	&sensorTgHDRRollCfg,
};
#endif
#if 0
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
	.ext_hdnum		 = 32,
	.pLVDScfg		 = &sensorLVDSCfg_VGA,
};
#endif
static const sensorSyncSigCfg_t sensorSyncSigPreview720P_30fps_SCALECfg = {
	.linetotal       = PREVIEW_720P_30fps_SCALE_LINETOTAL,
	.frametotal      = PREVIEW_720P_30fps_SCALE_FRAMETOTAL,
	.hoffset         = PREVIEW_720P_30fps_SCALE_HOFFSET,
	.voffset         = PREVIEW_720P_30fps_SCALE_VOFFSET,
	.hsize           = PREVIEW_720P_30fps_SCALE_HSIZE,
	.vsize           = PREVIEW_720P_30fps_SCALE_VSIZE,
	.lastlinepix     = PREVIEW_720P_30fps_SCALE_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_720P_30fps_SCALE_PCLKNX,
	.pclk1x 		 = PREVIEW_720P_30fps_SCALE_PCLK1X,
	.ext_hdnum		 = PREVIEW_720P_30fps_SCALE_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg_VGA,
	.lvds_hsize		 = PREVIEW_720P_30fps_SCALE_SENSOR_MIPIOUT_HSIZE,
	.lvds_vsize		 = PREVIEW_720P_30fps_SCALE_SENSOR_MIPIOUT_VSIZE,	
};
static const sensorSyncSigCfg_t sensorSyncSigPreview720P_60fps_SCALECfg = {
	.linetotal       = PREVIEW_720P_60fps_SCALE_LINETOTAL,
	.frametotal      = PREVIEW_720P_60fps_SCALE_FRAMETOTAL,
	.hoffset         = PREVIEW_720P_60fps_SCALE_HOFFSET,
	.voffset         = PREVIEW_720P_60fps_SCALE_VOFFSET,
	.hsize           = PREVIEW_720P_60fps_SCALE_HSIZE,
	.vsize           = PREVIEW_720P_60fps_SCALE_VSIZE,
	.lastlinepix     = PREVIEW_720P_60fps_SCALE_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_720P_60fps_SCALE_PCLKNX,
	.pclk1x 		 = PREVIEW_720P_60fps_SCALE_PCLK1X,
	.ext_hdnum		 = PREVIEW_720P_60fps_SCALE_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};
static const sensorSyncSigCfg_t sensorSyncSigPreview720P_SCALECfg = {
	.linetotal       = PREVIEW_720P_120fps_SCALE_LINETOTAL,
	.frametotal      = PREVIEW_720P_120fps_SCALE_FRAMETOTAL,
	.hoffset         = PREVIEW_720P_120fps_SCALE_HOFFSET,
	.voffset         = PREVIEW_720P_120fps_SCALE_VOFFSET,
	.hsize           = PREVIEW_720P_120fps_SCALE_HSIZE,
	.vsize           = PREVIEW_720P_120fps_SCALE_VSIZE,
	.lastlinepix     = PREVIEW_720P_120fps_SCALE_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_720P_120fps_SCALE_PCLKNX,
	.pclk1x 		 = PREVIEW_720P_120fps_SCALE_PCLK1X,
	.ext_hdnum		 = PREVIEW_720P_120fps_SCALE_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};
static const sensorSyncSigCfg_t sensorSyncSigPreview720P_SCALE_BINSUMCfg = {
	.linetotal       = PREVIEW_720P_SCALE_BINSUM_LINETOTAL,
	.frametotal      = PREVIEW_720P_SCALE_BINSUM_FRAMETOTAL,
	.hoffset         = PREVIEW_720P_SCALE_BINSUM_HOFFSET,
	.voffset         = PREVIEW_720P_SCALE_BINSUM_VOFFSET,
	.hsize           = PREVIEW_720P_SCALE_BINSUM_HSIZE,
	.vsize           = PREVIEW_720P_SCALE_BINSUM_VSIZE,
	.lastlinepix     = PREVIEW_720P_SCALE_BINSUM_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_720P_SCALE_BINSUM_PCLKNX,
	.pclk1x 		 = PREVIEW_720P_SCALE_BINSUM_PCLK1X,
	.ext_hdnum		 = PREVIEW_720P_SCALE_BINSUM_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};
static const sensorSyncSigCfg_t sensorSyncSigPreview720P_CROPCfg = {
	.linetotal       = PREVIEW_720P_CROP_LINETOTAL,
	.frametotal      = PREVIEW_720P_CROP_FRAMETOTAL,
	.hoffset         = PREVIEW_720P_CROP_HOFFSET,
	.voffset         = PREVIEW_720P_CROP_VOFFSET,
	.hsize           = PREVIEW_720P_CROP_HSIZE,
	.vsize           = PREVIEW_720P_CROP_VSIZE,
	.lastlinepix     = PREVIEW_720P_CROP_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_720P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_720P_CROP_PCLKNX,
	.pclk1x 		 = PREVIEW_720P_CROP_PCLK1X,
	.ext_hdnum		 = PREVIEW_720P_CROP_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewQUARTERPCfg = {
	.linetotal       = PREVIEW_QUARTER_LINETOTAL,
	.frametotal      = PREVIEW_QUARTER_FRAMETOTAL,
	.hoffset         = PREVIEW_QUARTER_HOFFSET,
	.voffset         = PREVIEW_QUARTER_VOFFSET,
	.hsize           = PREVIEW_QUARTER_HSIZE,
	.vsize           = PREVIEW_QUARTER_VSIZE,
	.lastlinepix     = PREVIEW_QUARTER_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_QUARTER_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_QUARTER_PCLKNX,
	.pclk1x 		 = PREVIEW_QUARTER_PCLK1X,
	.ext_hdnum		 = PREVIEW_QUARTER_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg_VGA,
	.lvds_hsize		 = PREVIEW_QUARTER_SENSOR_MIPIOUT_HSIZE,
	.lvds_vsize		 = PREVIEW_QUARTER_SENSOR_MIPIOUT_VSIZE,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewQUARTER1440P30fpsCfg = {
	.linetotal       = PREVIEW_QUARTER1440P30_LINETOTAL,
	.frametotal      = PREVIEW_QUARTER1440P30_FRAMETOTAL,
	.hoffset         = PREVIEW_QUARTER1440P30_HOFFSET,
	.voffset         = PREVIEW_QUARTER1440P30_VOFFSET,
	.hsize           = PREVIEW_QUARTER1440P30_HSIZE,
	.vsize           = PREVIEW_QUARTER1440P30_VSIZE,
	.lastlinepix     = PREVIEW_QUARTER1440P30_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_QUARTER1440P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_QUARTER1440P30_PCLKNX,
	.pclk1x 		 = PREVIEW_QUARTER1440P30_PCLK1X,
	.ext_hdnum		 = PREVIEW_QUARTER1440P30_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewQUARTER1440P60fpsCfg = {
	.linetotal       = PREVIEW_QUARTER1440P60_LINETOTAL,
	.frametotal      = PREVIEW_QUARTER1440P60_FRAMETOTAL,
	.hoffset         = PREVIEW_QUARTER1440P60_HOFFSET,
	.voffset         = PREVIEW_QUARTER1440P60_VOFFSET,
	.hsize           = PREVIEW_QUARTER1440P60_HSIZE,
	.vsize           = PREVIEW_QUARTER1440P60_VSIZE,
	.lastlinepix     = PREVIEW_QUARTER1440P60_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_QUARTER1440P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_QUARTER1440P60_PCLKNX,
	.pclk1x 		 = PREVIEW_QUARTER1440P60_PCLK1X,
	.ext_hdnum		 = PREVIEW_QUARTER1440P60_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
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
	.ext_hdnum		 = PREVIEW_1080P_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreview1440P30fpsCfg = {
	.linetotal       = PREVIEW_1440P30_LINETOTAL,
	.frametotal      = PREVIEW_1440P30_FRAMETOTAL,
	.hoffset         = PREVIEW_1440P30_HOFFSET,
	.voffset         = PREVIEW_1440P30_VOFFSET,
	.hsize           = PREVIEW_1440P30_HSIZE,
	.vsize           = PREVIEW_1440P30_VSIZE,
	.lastlinepix     = PREVIEW_1440P30_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1440P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_1440P30_PCLKNX,
	.pclk1x 		 = PREVIEW_1440P30_PCLK1X,
	.ext_hdnum		 = PREVIEW_1440P30_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};


static const sensorSyncSigCfg_t sensorSyncSigPreview1440P60fpsCfg = {
	.linetotal       = PREVIEW_1440P60_LINETOTAL,
	.frametotal      = PREVIEW_1440P60_FRAMETOTAL,
	.hoffset         = PREVIEW_1440P60_HOFFSET,
	.voffset         = PREVIEW_1440P60_VOFFSET,
	.hsize           = PREVIEW_1440P60_HSIZE,
	.vsize           = PREVIEW_1440P60_VSIZE,
	.lastlinepix     = PREVIEW_1440P60_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_1440P_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_1440P60_PCLKNX,
	.pclk1x 		 = PREVIEW_1440P60_PCLK1X,
	.ext_hdnum		 = PREVIEW_1440P60_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
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
	.ext_hdnum		 = EXT_HD_LINE,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFull30fpsCfg = {
	.linetotal       = PREVIEW_Full30_LINETOTAL,
	.frametotal      = PREVIEW_Full30_FRAMETOTAL,
	.hoffset         = PREVIEW_Full30_HOFFSET,
	.voffset         = PREVIEW_Full30_VOFFSET,
	.hsize           = PREVIEW_Full30_HSIZE,
	.vsize           = PREVIEW_Full30_VSIZE,
	.lastlinepix     = PREVIEW_Full30_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_FULL_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_Full30_PCLKNX,
	.pclk1x 		 = PREVIEW_Full30_PCLK1X,
	.ext_hdnum		 = PREVIEW_Full30_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFull30fps_CropCfg = {
	.linetotal       = PREVIEW_Full30_CROP_LINETOTAL,
	.frametotal      = PREVIEW_Full30_CROP_FRAMETOTAL,
	.hoffset         = PREVIEW_Full30_CROP_HOFFSET,
	.voffset         = PREVIEW_Full30_CROP_VOFFSET,
	.hsize           = PREVIEW_Full30_CROP_HSIZE,
	.vsize           = PREVIEW_Full30_CROP_VSIZE,
	.lastlinepix     = PREVIEW_Full30_CROP_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_FULL_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_Full30_CROP_PCLKNX,
	.pclk1x 		 = PREVIEW_Full30_CROP_PCLK1X,
	.ext_hdnum		 = PREVIEW_Full30_CROP_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFull27p5fpsCfg = {
	.linetotal       = PREVIEW_Full27p5_LINETOTAL,
	.frametotal      = PREVIEW_Full27p5_FRAMETOTAL,
	.hoffset         = PREVIEW_Full27p5_HOFFSET,
	.voffset         = PREVIEW_Full27p5_VOFFSET,
	.hsize           = PREVIEW_Full27p5_HSIZE,
	.vsize           = PREVIEW_Full27p5_VSIZE,
	.lastlinepix     = PREVIEW_Full27p5_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_FULL_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_Full27p5_PCLKNX,
	.pclk1x 		 = PREVIEW_Full27p5_PCLK1X,
	.ext_hdnum		 = PREVIEW_Full27p5_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFull55fpsCfg = {
	.linetotal       = PREVIEW_Full55_LINETOTAL,
	.frametotal      = PREVIEW_Full55_FRAMETOTAL,
	.hoffset         = PREVIEW_Full55_HOFFSET,
	.voffset         = PREVIEW_Full55_VOFFSET,
	.hsize           = PREVIEW_Full55_HSIZE,
	.vsize           = PREVIEW_Full55_VSIZE,
	.lastlinepix     = PREVIEW_Full55_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_FULL_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_Full55_PCLKNX,
	.pclk1x 		 = PREVIEW_Full55_PCLK1X,
	.ext_hdnum		 = PREVIEW_Full55_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};


static const sensorSyncSigCfg_t sensorSyncSigPreviewFull60fpsCfg = {
	.linetotal       = PREVIEW_Full60_LINETOTAL,
	.frametotal      = PREVIEW_Full60_FRAMETOTAL,
	.hoffset         = PREVIEW_Full60_HOFFSET,
	.voffset         = PREVIEW_Full60_VOFFSET,
	.hsize           = PREVIEW_Full60_HSIZE,
	.vsize           = PREVIEW_Full60_VSIZE,
	.lastlinepix     = PREVIEW_Full60_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_FULL_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_Full60_PCLKNX,
	.pclk1x 		 = PREVIEW_Full60_PCLK1X,
	.ext_hdnum		 = PREVIEW_Full60_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewQUARTER_VBin_HFullCfg = {
	.linetotal       = PREVIEW_QUARTER_VBIN_HFULL_LINETOTAL,
	.frametotal      = PREVIEW_QUARTER_VBIN_HFULL_FRAMETOTAL,
	.hoffset         = PREVIEW_QUARTER_VBIN_HFULL_HOFFSET,
	.voffset         = PREVIEW_QUARTER_VBIN_HFULL_VOFFSET,
	.hsize           = PREVIEW_QUARTER_VBIN_HFULL_HSIZE,
	.vsize           = PREVIEW_QUARTER_VBIN_HFULL_VSIZE,
	.lastlinepix     = PREVIEW_QUARTER_VBIN_HFULL_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_QUARTER_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_QUARTER_VBIN_HFULL_PCLKNX,
	.pclk1x 		 = PREVIEW_QUARTER_VBIN_HFULL_PCLK1X,
	.ext_hdnum		 = PREVIEW_QUARTER_VBIN_HFULL_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewFullHD_60FPS_2LANECfg = {
	.linetotal       = PREVIEW_FullHD60_2LANE_LINETOTAL,
	.frametotal      = PREVIEW_FullHD60_2LANE_FRAMETOTAL,
	.hoffset         = PREVIEW_FullHD60_2LANE_HOFFSET,
	.voffset         = PREVIEW_FullHD60_2LANE_VOFFSET,
	.hsize           = PREVIEW_FullHD60_2LANE_HSIZE,
	.vsize           = PREVIEW_FullHD60_2LANE_VSIZE,
	.lastlinepix     = PREVIEW_FullHD60_2LANE_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_FULL_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_FullHD60_2LANE_PCLKNX,
	.pclk1x 		 = PREVIEW_FullHD60_2LANE_PCLK1X,
	.ext_hdnum		 = PREVIEW_FullHD60_2LANE_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg_FULLHD_60FPS_2LANE,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewQUARTER_CROP_240Cfg = {
	.linetotal       = PREVIEW_QUARTER_CROP_240_LINETOTAL,
	.frametotal      = PREVIEW_QUARTER_CROP_240_FRAMETOTAL,
	.hoffset         = PREVIEW_QUARTER_CROP_240_HOFFSET,
	.voffset         = PREVIEW_QUARTER_CROP_240_VOFFSET,
	.hsize           = PREVIEW_QUARTER_CROP_240_HSIZE,
	.vsize           = PREVIEW_QUARTER_CROP_240_VSIZE,
	.lastlinepix     = PREVIEW_QUARTER_CROP_240_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_QUARTER_CROP_240_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_QUARTER_CROP_240_PCLKNX,
	.pclk1x 		 = PREVIEW_QUARTER_CROP_240_PCLK1X,
	.ext_hdnum		 = PREVIEW_QUARTER_CROP_240_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

static const sensorSyncSigCfg_t sensorSyncSigPreviewBIN4Cfg = {
	.linetotal       = PREVIEW_BIN4_LINETOTAL,
	.frametotal      = PREVIEW_BIN4_FRAMETOTAL,
	.hoffset         = PREVIEW_BIN4_HOFFSET,
	.voffset         = PREVIEW_BIN4_VOFFSET,
	.hsize           = PREVIEW_BIN4_HSIZE,
	.vsize           = PREVIEW_BIN4_VSIZE,
	.lastlinepix     = PREVIEW_BIN4_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = PREVIEW_BIN4_HDNUM_CMDPORT,
	.fpclk			 = PREVIEW_BIN4_PCLKNX,
	.pclk1x 		 = PREVIEW_BIN4_PCLK1X,
	.ext_hdnum		 = PREVIEW_BIN4_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg,
};

#if HDR_test
static  sensorSyncSigCfg_t sensorSyncSigPreviewHDR720pCfg = {
	.linetotal       = HDR720p_LINETOTAL,
	.frametotal      = HDR720p_FRAMETOTAL,
	.hoffset         = HDR720p_HOFFSET,
	.voffset         = HDR720p_VOFFSET,
	.hsize           = HDR720p_HSIZE,
	.vsize           = HDR720p_VSIZE,
	.lastlinepix     = HDR720p_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
	.fpclk			 = HDR720p_PCLKNX,
	.pclk1x 		 = HDR720p_PCLK1X,
	.ext_hdnum		 = EXT_HD_LINE,
	.pLVDScfg		 = &sensorLVDSCfg_HDR,
};
static  sensorSyncSigCfg_t sensorSyncSigPreviewHDRCfg = {
	.linetotal       = HDR_LINETOTAL,
	.frametotal      = HDR_FRAMETOTAL,
	.hoffset         = HDR_HOFFSET,
	.voffset         = HDR_VOFFSET,
	.hsize           = HDR_HSIZE,
	.vsize           = HDR_VSIZE,
	.lastlinepix     = HDR_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
	.fpclk			 = HDR_PCLKNX,
	.pclk1x 		 = HDR_PCLK1X,
	.ext_hdnum		 = EXT_HD_LINE,
	.pLVDScfg		 = &sensorLVDSCfg_HDR,
};
#endif

#if(!SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
static  sensorSyncSigCfg_t sensorSyncSigPreviewHDR2modeCfg = {
	.linetotal       = HDR2mode_LINETOTAL,
	.frametotal      = HDR2mode_FRAMETOTAL,
	.hoffset         = HDR2mode_HOFFSET,
	.voffset         = HDR2mode_VOFFSET,
	.hsize           = HDR2mode_HSIZE,
	.vsize           = HDR2mode_VSIZE,
	.lastlinepix     = HDR2mode_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
	.fpclk			 = HDR2mode_PCLKNX,
	.pclk1x 		 = HDR2mode_PCLK1X,
	.ext_hdnum		 = HDR2mode_EXT_LINES,
	.pLVDScfg		 = &sensorLVDSCfg_HDR,
	.lvds_hsize		 = HDR2mode_SENSOR_MIPIOUT_HSIZE,
	.lvds_vsize		 = HDR2mode_SENSOR_MIPIOUT_VSIZE,
};
#endif

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

#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	.pLVDScfg        = &sensorLVDSCfg_FULLHD_60FPS_2LANE	
#else	
	.pLVDScfg		 = &sensorLVDSCfg,
#endif	
};
#if 0
static  sensorSyncSigCfg_t sensorSyncSigSnapHDRCfg = {
	.linetotal       = HDR_LINETOTAL,
	.frametotal      = HDR_FRAMETOTAL,
	.hoffset         = HDR_HOFFSET,
	.voffset         = HDR_VOFFSET,
	.hsize           = HDR_HSIZE,
	.vsize           = HDR_VSIZE,
	.lastlinepix     = HDR_HSIZE,
	.conf_hdpol      = 0,
	.conf_vdpol      = 0,
	.hdnum_cmdport   = CAPTURE_HDNUM_CMDPORT,
	.fpclk			 = HDR_PCLKNX,
	.pclk1x 		 = HDR_PCLK1X,
	.ext_hdnum		 = EXT_HD_LINE,
	.pLVDScfg		 = &sensorLVDSCfg_HDR,
};
#endif
typedef struct {
	const sensorTgPreviewCfg_t *pstpPrev;
	const sensorSyncSigCfg_t *ssPrevCfg;
	const UINT8 *pCmdSet;
	const UINT16 CmdLen;
	const UINT8  ucModeInfo; /*Bit 0:  binning mode =>1,  non binning mode =>0*/
						 	 /*Bit 1:  HDR mode =>1,  HDR mode =>0*/
							 /*Bit 2:  binning mode use binning sum =>1,  binning mode use binning average =>0*/
} sensorPrevModeSet_t;

typedef struct {
	const sensorTgSnapCfg_t *pstpSnap;
	const sensorSyncSigCfg_t *ssSnapCfg;
	const UINT8 *pCmdSet;
	const UINT16 CmdLen;
} sensorSnapModeSet_t;


#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
static sensorPrevModeSet_t sensorPrevModeTbl[]=
{
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x00]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x01]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x02]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x03]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x04]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x05]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x06]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x07]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x08]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x09]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x0A]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x0B]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x0C]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x0D]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x0E]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
	/*Full HD 60fps 2 lane only apply to the case that ov4689 is connected to the serdes.*/
	[0x0F]={&sensorTgPreviewFullHD_60FPS_2LANECfg	,&sensorSyncSigPreviewFullHD_60FPS_2LANECfg ,cmd_full_hd_60fps_2lane_code[0]	,CMD_LEN(cmd_full_hd_60fps_2lane_code), 0},
};

static sensorSnapModeSet_t sensorSnapModeTbl[]=
{
	[0x00]={&sensorTgSnapRollCfg 		,&sensorSyncSigSnapCfg		,cmd_full_hd_60fps_2lane_code[0]		,CMD_LEN(cmd_full_hd_60fps_2lane_code)},
};

#else
static sensorPrevModeSet_t sensorPrevModeTbl[]=
{
	[0x00]={&DEF_PREV_MODE_CFG, &DEF_SYNC_SIG_CFG, DEF_INIT_AFE_CODE[0], CMD_LEN(DEF_INIT_AFE_CODE), 1},

	[0x01]={&sensorTgPreview720P_SCALECfg			,&sensorSyncSigPreview720P_SCALECfg			,cmd_720P_120fps_hts_modify_code[0]	,CMD_LEN(cmd_720P_120fps_hts_modify_code), 1},
	/*[0x02]={&sensorTgPreview720P_CROPCfg       		,&sensorSyncSigPreview720P_CROPCfg			,cmd_720P_120fps_code[0]			,CMD_LEN(cmd_720P_120fps_code), 1},*/
	[0x02]={&sensorTgPreview720P_SCALE_BINSUMCfg	,&sensorSyncSigPreview720P_SCALE_BINSUMCfg	,cmd_720P_120fps_hts_modify_code[0] ,CMD_LEN(cmd_720P_120fps_hts_modify_code), 5},
	[0x03]={&sensorTgPreviewFull60FPSCfg      		,&sensorSyncSigPreviewFull60fpsCfg			,cmd_full_60fps_hts_modify_code[0]	,CMD_LEN(cmd_full_60fps_hts_modify_code), 0},
	[0x04]={&sensorTgPreview1080PCfg      		 	,&sensorSyncSigPreview1080PCfg				,cmd_1080P_code[0]					,CMD_LEN(cmd_1080P_code), 0},
	[0x05]={&sensorTgPreviewFull27p5FPSCfg			,&sensorSyncSigPreviewFull27p5fpsCfg		,cmd_full_60fps_code[0] 			,CMD_LEN(cmd_full_60fps_code), 0},   
	[0x06]={&sensorTgPreviewFull30FPSCfg			,&sensorSyncSigPreviewFull30fpsCfg			,cmd_full_60fps_code[0] 			,CMD_LEN(cmd_full_60fps_code), 0},
	[0x07]={&sensorTgPreviewFull30FPS_CROPCfg		,&sensorSyncSigPreviewFull30fps_CropCfg		,cmd_full_60fps_code[0] 			,CMD_LEN(cmd_full_60fps_code), 0}, 
	[0x08]={&sensorTgPreview720P_60fps_SCALECfg		,&sensorSyncSigPreview720P_60fps_SCALECfg	,cmd_720P_120fps_code[0] 			,CMD_LEN(cmd_720P_120fps_code), 1},   
	[0x09]={&sensorTgPreviewFull55FPSCfg			,&sensorSyncSigPreviewFull55fpsCfg			,cmd_full_60fps_code[0] 			,CMD_LEN(cmd_full_60fps_code), 0},   
	[0x0A]={&sensorTgHDR2modeCfg					,&sensorSyncSigPreviewHDR2modeCfg			,cmd_HDR2mode_code[0] 				,CMD_LEN(cmd_HDR2mode_code), 2},
	[0x0B]={&sensorTgPreviewQUARTER_CROP_240Cfg		,&sensorSyncSigPreviewQUARTER_CROP_240Cfg   ,cmd_cropped_quarter_code[0] 		,CMD_LEN(cmd_cropped_quarter_code), 1},
	[0x0C]={&sensorTgPreviewBIN4Cfg					,&sensorSyncSigPreviewBIN4Cfg 			,cmd_bin4_code[0] 			,CMD_LEN(cmd_bin4_code), 1}, 

	[0x0D]={&sensorTgPreview1440P60FPSCfg			,&sensorSyncSigPreview1440P60fpsCfg			,cmd_full_60fps_hts_modify_code[0] 	,CMD_LEN(cmd_full_60fps_hts_modify_code), 0},   
	[0x0E]={&sensorTgPreviewQUARTER1440P60FPSCfg	,&sensorSyncSigPreviewQUARTER1440P60fpsCfg	,cmd_720P_120fps_code[0] 			,CMD_LEN(cmd_720P_120fps_code), 1},   

	/*quarter which V is binning and H keeps full, 120fps.*/
	[0x0F]={&sensorTgPreviewQUARTER_VBin_HFullCfg	,&sensorSyncSigPreviewQUARTER_VBin_HFullCfg	,cmd_quarter_vbin_hfull_code[0]	,CMD_LEN(cmd_quarter_vbin_hfull_code), 1},

};

static sensorSnapModeSet_t sensorSnapModeTbl[]=
{
	[0x00]={&sensorTgSnapRollCfg 		,&sensorSyncSigSnapCfg		,cmd_full_60fps_code[0]		,CMD_LEN(cmd_full_60fps_code)},
	/*[0x01]={&sensorTgHDRRollCfg 		,&sensorSyncSigSnapHDRCfg	,cmd_HDR_code[0] 			,CMD_LEN(cmd_HDR_code)},*/
};

#endif

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
	#if (HTC_FAST_CAPTURE)
		.init = 		sensorDoFuncFastCapInit,
	#else
		.init =			sensorDoFuncInit,
	#endif
		.stop =			sensorDoFuncStop, /****TODO***/
	#if 0/*(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)*/
		.pwrsave  = 	NULL,
	#else
		.pwrsave  =     sensorDoFuncPowerSave,
	#endif
		.expwrite =		sensor_cmdport_exp_write,
		.expvtbldo =	sensor_cmdport_exp_vtbl_do,

		/*If using frmwrite function, disable expwriteRedo.*/
		.expwriteRedo=	NULL,
		.frmwrite= 		sensor_cmdport_frame_write,
		.cmdportBegin = sensor_cmdport_begin,
		.cmdportEnd =   sensor_cmdport_end,
		#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)		
		.senstschk = sensorStsCheck,
		.connect = NULL,/*sensorConnectProcess,*/
		#endif
		.crop =			NULL, /****TODO***/
	},
#if USEFUL

	.previewCfg =		sensorTgPreviewCfgTbl,
	.snapCfg =			sensorTgSnapCfgTbl,
	.defSyncSigCfg  =	&sensorSyncSigPreviewVGACfg,
};
#endif
	.verStr =			MODEL_VERSION,
};

UINT32 
sensorOVRead(UINT16 addr)
{
	UINT32 cmdportid;
	static UINT8 regread[3];
	cmdportid = sensorTgCfg.cmdportid_xtg;
	regread[0]=addr>>8;
	regread[1]=addr&0xFF;
	
	SENSOR_PROF_LOG_ADD("sensorOVRead++");
	sensorSerialBlockRead(cmdportid,1,regread,0);
	while((READ8(0xb0009105)&0x01)==0x01);/*sif1  CPU  trigger busy*/
    /*printf("\nREAD [0x%x] = [0x%x]\n",addr,regread[2]);*/
	SENSOR_PROF_LOG_PRINT("READ [%x] = [%x]",addr,regread[2]);
	SENSOR_PROF_LOG_ADD("sensorOVRead--");
	return regread[2];
}
void
sensorOVReadAll(UINT16 none)
{
	UINT16 i = 0,addr;
	printf("================init================\n");
	for(i=0;i<sizeof(cmd_init_code)/3;i++)
	{
		addr= cmd_init_code[i][0]<<8|cmd_init_code[i][1];
		printf("%x %x\n",addr,sensorOVRead(addr));
	}	
	printf("================Preview================\n");
	for(i=0;i<sizeof(cmd_720P_60fps_code)/3;i++)
	{
		addr= cmd_720P_60fps_code[i][0]<<8|cmd_720P_60fps_code[i][1];
		printf("%x %x\n",addr,sensorOVRead(addr));
	}
	printf("================Full================\n");
	for(i=0;i<sizeof(cmd_still_code)/3;i++)
	{
		addr= cmd_still_code[i][0]<<8|cmd_still_code[i][1];
		printf("%x %x\n",addr,sensorOVRead(addr));
	}	
}

void
sensorOVWrite(UINT16 addr, UINT8 data)
{

	UINT32 cmdportid;

	cmdportid = sensorTgCfg.cmdportid_xtg;
	static UINT8 regdata1[3] ;
	regdata1[0]=  ( UINT8 )( addr>>8);	
	regdata1[1]=  ( UINT8 )( addr & 0x000000ff );
	regdata1[2]=  ( UINT8 )( data & 0x000000ff );
	sensorSerialDirectBufSnoop(regdata1, sizeof(regdata1));
	sensorSerialBulkWrite( cmdportid, 1, regdata1, 0);	
 
}

void
sensorTg_ov4689_Install()
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
	sensorTgDev.defSyncSigCfg=sensorPrevModeTbl[DEF_SENSOR_MODE&0x0F].ssPrevCfg;/*default syncsig */

	/*uncomment this if SSG supports both master and slave modes*/
	/*sensorTgCfg.tg_type |= sensorSyncSigMasterCfgGet();*/
	/*FIXME*/ /****TODO*SSG master switch*/
	sensorTgDevInstall(SENSOR_DEV_ID_SENSOR,&sensorTgDev);
}

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
void 
serdes_ov4689_i2c_write(UINT8 a_ucSerdesSel,UINT32 a_udwAddr, UINT32 a_udwData)
{
	UINT32 cmdportid;
	static UINT8 SENSOR_SIF1DMA_VARBUF regdata[3];
	if (a_ucSerdesSel == SERDES_S100)
		cmdportid = g_Cmdportids100;
	else if (a_ucSerdesSel == SERDES_S101)
		cmdportid = g_Cmdportids101;
	else
		return;

	/*profLogPrintf(3,"@!@I2CWrite S[%d]0x%x=0x%x",a_ucSerdesSel,a_udwAddr,a_udwData);*/
	
	regdata[0]=a_udwAddr>>8;
	regdata[1]=a_udwAddr&0xFF;
	regdata[2]=a_udwData&0xFF;
	sensorSerialDirectBufSnoop(regdata, sizeof(regdata));
	sensorSerialBulkWrite(cmdportid,1,regdata,0);

	/*profLogPrintf(3,"@!@I2CWrite E");*/	
}

UINT32
serdes_ov4689_i2c_read(UINT8 a_ucSerdesSel, UINT32 a_udwAddr)
{
	UINT32 cmdportid;
	static UINT8 SENSOR_SIF1DMA_VARBUF regread[3];
	
	if (a_ucSerdesSel == SERDES_S100)
		cmdportid = g_Cmdportids100;
	else if (a_ucSerdesSel == SERDES_S101)
		cmdportid = g_Cmdportids101;
	else
		return 0xFF;

	/*profLogPrintf(3,"@!@I2CRead S");*/
	regread[0]=a_udwAddr>>8;
	regread[1]=a_udwAddr&0xFF;
	regread[2]=0x00;/*clear the data before getting the data*/
	sensorSerialDirectBufSnoop(regread, sizeof(regread));
	sensorSerialBlockRead(cmdportid,1,regread,0);

	/*profLogPrintf(3,"@!@I2CRead E[%d]0x%x=0x%x",a_ucSerdesSel,a_udwAddr,regread[2]);*/
	/*SENSOR_PROF_LOG_PRINT("\nREAD [%x] = [%x]\n",a_udwAddr,regread[2]);*/
	return regread[2];
}

static void serdes_spca101_PwrOn_Seq(UINT32 sel)
{	
	profLogPrintf(3,"@#@ SPCA101 PwrOn Seq S!!!");
	/*Use the sensor power up case 3.*/
	UINT32 sensel=sel?SENSOR_DEV_POWER_ID_CMOS_SEC_SEL:0;

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_RST_OFF);  /* RESET Low */
	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWDN_OFF);  /* PWDN Low */

    tmrUsWait(20000); /*20ms*/
   
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_SYS_ON);   

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_ON);  /*2.8v on */
	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_IO_ON);	 /*1.8v on */

	tmrUsWait(1000); /*1ms*/

	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_ON);	 /*1.2v on */    
    tmrUsWait(1000); /*1ms*/ 


    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_RST_ON);  /* RESET High */
	/*Wait 10ms to make sure SPCA101 internal reset done.(need 8ms)*/
	tmrUsWait(10000); /*10ms*/

	profLogPrintf(3,"@#@ SPCA101 PwrOn Seq E!!!");	
}

static void serdes_spca101_PwrOff_Seq(UINT32 sel)
{
	UINT32 sensel=sel?SENSOR_DEV_POWER_ID_CMOS_SEC_SEL:0;

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_RST_OFF);
	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWDN_OFF);  /* PWDN Low */	
    tmrUsWait(1000);

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_OFF); /*1.2v off */
    tmrUsWait(1000);

	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_IO_OFF);   /*1.8v off */
	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_OFF);  /*2.8v off */
    tmrUsWait(1000);
	
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_SYS_OFF);  
    tmrUsWait(1000); /*1ms*/

}

static void ov4689_serdes_io_trap_chk(void)
{
	UINT8 ucTmrCnt,ucIOTrapVal;
	ucIOTrapVal = 0;
	ucTmrCnt = 0;

	/*Adjust the order of disabling I2C redirect function.*/
	serdes_i2c_redirect_en_set(SERDES_S101,I2C_REDIRECT_DISABLE);
	serdes_i2c_redirect_en_set(SERDES_S100,I2C_REDIRECT_DISABLE);

	while (1)
	{
		ucIOTrapVal=serdes_io_trap_val_chk(SERDES_S100);

		if ((ucIOTrapVal==SERDES_S100_UNIDIRECTION_IO_TRAP)||(ucTmrCnt == IOTRAP_POLLINGCNT))/*Check MIPI clock lane is in low power state or not.*/
		{
			if (ucTmrCnt == IOTRAP_POLLINGCNT)
			{
				SENSOR_PROF_LOG_ADD("#@# SPCA100 IOTRAP Check timeout!");
			    /*printf("@#@ Error SPCA100 IOTRAP check timeout!\n");*/
			}
			
			break;
		}
	
		tmrUsWait(IOTRAP_POLLINGTIME);
		ucTmrCnt++;			
	}

	SENSOR_PROF_LOG_PRINT("#@#S100 IOTRAP Chk Done! TRAP=0x%x,Cnt[%d]",ucIOTrapVal,ucTmrCnt);
	
	ucIOTrapVal=serdes_io_trap_val_chk(SERDES_S101);

	SENSOR_PROF_LOG_PRINT("!@!S101 IOTRAP Chk Done! TRAP=0x%x",ucIOTrapVal);

}

#else

static void ov4689_PwrOn_Seq(UINT32 sel)
{
	/*Use the sensor power up case 1.*/
	UINT32 sensel=sel?SENSOR_DEV_POWER_ID_CMOS_SEC_SEL:0;
		
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_SYS_ON);  
    tmrUsWait(1000); /*1ms*/

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_ON);  /*2.8v on */
    tmrUsWait(1000); /*1ms*/

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWDN_ON);  /* PWDN High */
   
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_IO_ON);   /*1.8v on */
    tmrUsWait(1000); /*1ms*/
    
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_ON); /*1.2v on */
    tmrUsWait(1000); /*1ms*/

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_RST_ON);  /* RESET High */
    tmrUsWait(1000); /*1ms*/

}  

static void ov4689_PwrDown_Seq(UINT32 sel)
{
	UINT32 sensel=sel?SENSOR_DEV_POWER_ID_CMOS_SEC_SEL:0;
	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWDN_OFF);/*Only make the PWDN pin to low to make OV4689 enter power down mode.*/
	tmrUsWait(1000);
}

static void ov4689_PwrOff_Seq(UINT32 sel)
{
	UINT32 sensel=sel?SENSOR_DEV_POWER_ID_CMOS_SEC_SEL:0;

	sensorDoFuncSensorMIPIClkLaneChk();/*Check the MIPI clock lane in on low power state or not.*/
	                                   /*Data and clock must be at low power stat before turning off power.*/
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_RST_OFF); 
    tmrUsWait(1000);

    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_OFF); /*1.2v off */
    tmrUsWait(1000);

	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_IO_OFF);   /*1.8v off */
	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWDN_OFF);  /* PWDN Low */
    tmrUsWait(1000); /*1ms*/

	sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_OFF);  /*2.8v off */
    tmrUsWait(1000);
	
    sensorDevPowerCustomCb(sensel|SENSOR_DEV_POWER_ID_CMOS_SYS_OFF);  
    tmrUsWait(1000); /*1ms*/	
}
#endif

static void OV4689_SensorInfoCfgSet(void)
{
	sensorInfoSet(SENSOR_MODE_PREVIEW,SENSOR_INFO_OB,OV4689_BLC_VAL,sizeof(sensorPrevModeTbl)/sizeof(sensorPrevModeTbl[0]),0/*opt*/);/*0x30 ~ 0x3f use 64*/
	sensorInfoSet(SENSOR_MODE_SNAP,SENSOR_INFO_OB,OV4689_BLC_VAL,sizeof(sensorSnapModeTbl)/sizeof(sensorSnapModeTbl[0]),0/*opt*/);   /*0x40 use 64*/
	sensorInfoSet(SENSOR_MODE_PREVIEW,SENSOR_INFO_SENSITIVITY_STS,(UINT32)PrevSensitivityTbl,sizeof(PrevSensitivityTbl)/sizeof(PrevSensitivityTbl[0]),1/*opt*/);
	sensorInfoSet(SENSOR_MODE_SNAP,SENSOR_INFO_SENSITIVITY_STS,(UINT32)SnapSensitivityTbl,sizeof(SnapSensitivityTbl)/sizeof(SnapSensitivityTbl[0]),1/*opt*/);	 
}

static UINT32 ov4689_otp_value_read(void)
{
	static UINT8 ucSensorOtpProductId[][3] SENSOR_SIF1DMA_VARBUF ={
		{0x70,0x00,0x00},
		{0x70,0x01,0x00},
		{0x70,0x02,0x00},		
		{0x70,0x0E,0x00},
	};
	
	/*sensorSerialDirectBufSnoop((UINT8 *)ucSensorOtpProductId,sizeof(ucSensorOtpProductId));
	sensorSerialBlockRead(cmdportid,CMD_LEN(ucSensorOtpProductId),ucSensorOtpProductId[0],0);*/
	/*ucSensorOtpProductId[0][2] = sensorOVRead((ucSensorOtpProductId[0][0]<<8)|ucSensorOtpProductId[0][1]);*/
	ucSensorOtpProductId[1][2] = sensorOVRead((ucSensorOtpProductId[1][0]<<8)|ucSensorOtpProductId[1][1]);
	ucSensorOtpProductId[2][2] = sensorOVRead((ucSensorOtpProductId[2][0]<<8)|ucSensorOtpProductId[2][1]);
	ucSensorOtpProductId[3][2] = sensorOVRead((ucSensorOtpProductId[3][0]<<8)|ucSensorOtpProductId[3][1]);
	
	profLogPrintf(3,"!@!ov4689 productid=[0x%x 0x%x 0x%x 0x%x]",ucSensorOtpProductId[0][2],ucSensorOtpProductId[1][2],\
																ucSensorOtpProductId[2][2],ucSensorOtpProductId[3][2]);
	
	return ((ucSensorOtpProductId[0][2])|(ucSensorOtpProductId[1][2]<<8)|(ucSensorOtpProductId[2][2]<<16)|(ucSensorOtpProductId[3][2]<<24));
}

static void ov4689_product_id_chk(void)
{
	UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;

	if (g_ucR2aVerFlag == 0xff)/*if sensor product id is not check before. Check it.*/
	{
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_init_code_StreamOn)/sizeof(cmd_init_code_StreamOn[0]), cmd_init_code_StreamOn[0],0);
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_sensor_otp_memory_read)/sizeof(cmd_sensor_otp_memory_read[0]), cmd_sensor_otp_memory_read[0],0);		
		tmrUsWait(1000);	
		
		UINT32 udwOTPVal;

		udwOTPVal = ov4689_otp_value_read();

		if ((udwOTPVal&0x00ffff00)!=0x00894600)
		{
			sensorSerialBulkWrite(cmdportid,sizeof(cmd_extra_setting_1_for_r2a_ver)/sizeof(cmd_extra_setting_1_for_r2a_ver[0]), cmd_extra_setting_1_for_r2a_ver[0],0);

			profLogPrintf(3,"!@!Retry R2A extra 1 !");
			
			tmrUsWait(10000);

			sensorSerialBulkWrite(cmdportid,sizeof(cmd_extra_setting_2_for_r2a_ver)/sizeof(cmd_extra_setting_2_for_r2a_ver[0]), cmd_extra_setting_2_for_r2a_ver[0],0);

			profLogPrintf(3,"!@!Retry R2A extra 2 !");

			sensorSerialBulkWrite(cmdportid,sizeof(cmd_sensor_otp_memory_read)/sizeof(cmd_sensor_otp_memory_read[0]), cmd_sensor_otp_memory_read[0],0);
			tmrUsWait(1000);			

			udwOTPVal = ov4689_otp_value_read();
		}

		if (((udwOTPVal>>24)&0xff)==0x75)
		{
			g_ucR2aVerFlag=1;/*R2A version.*/
		}
		else
		{
			g_ucR2aVerFlag=0;/*Old version.*/
		}
	}

}

static void ov4689_r2a_extra_setting_2_cfg_set(void)
{
	if (g_ucR2aVerFlag == 1)
	{
		UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;

		/*following step is only for R2A version.*/
		tmrUsWait(10000);
		/*for R2A version.*/	
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_extra_setting_2_for_r2a_ver)/sizeof(cmd_extra_setting_2_for_r2a_ver[0]), cmd_extra_setting_2_for_r2a_ver[0],0);
		profLogPrintf(3,"!@! Extra 2 for R2A version!");
	}
}

static void ov4689_r2a_stream_off_extra_setting(void)
{
	if (g_ucR2aVerFlag == 1)
	{
	
		UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_stream_off_extra_setting_1_for_r2a_ver)/sizeof(cmd_stream_off_extra_setting_1_for_r2a_ver[0]), cmd_stream_off_extra_setting_1_for_r2a_ver[0],0);

		profLogPrintf(3,"!@! Extra stream off setting for R2A version!");
	}
}

static void ov4689_r2a_stream_on_extra_setting(void)
{
	if (g_ucR2aVerFlag == 1)
	{
		UINT32 cmdportid = sensorTgCfg.cmdportid_xtg;
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_stream_on_extra_setting_1_for_r2a_ver)/sizeof(cmd_stream_on_extra_setting_1_for_r2a_ver[0]), cmd_stream_on_extra_setting_1_for_r2a_ver[0],0);
		tmrUsWait(10000);
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_stream_on_extra_setting_2_for_r2a_ver)/sizeof(cmd_stream_on_extra_setting_2_for_r2a_ver[0]), cmd_stream_on_extra_setting_2_for_r2a_ver[0],0);
		profLogPrintf(3,"!@! Extra stream on setting for R2A version!");
	}
}

#if (HTC_FAST_CAPTURE)

static void
sensorDoFuncFastCapInit()
{
	
	UINT32 cmdportid;
	UINT32 initMode=0x40;
		
	SENSOR_PROF_LOG_ADD("OV4689 sensorDoFuncFastCapInit++ 6350 ");
	SENSOR_PROF_LOG_PRINT("_Sensor driver ver <<%s>>", MODEL_VERSION);
	SENSOR_PROF_LOG_PRINT("_Sensor driver build time <<%s>>", BUILD_TIME);
	SENSOR_PROF_LOG_ADD("sensorDoFuncFastCapInit()_ov4689_enter_....");

	sensorTgPllClkCfgSet(SENSOR_TGPLL_CFG_DRIVING,4);/*40uA*/
	sensorTgPllClkExtSet(24000000);/*24MHz*/
	ov4689_PwrOn_Seq(0);

#ifdef SEN_REMAP_0
		sensorModelApiCfgSet(SENSOR_API_PHY0_LANE_REMAP,SEN_REMAP_0,0xFFFFFFFF);
		profLogPrintf(0,"Use IO file pin remap....");
#else
        #pragma message("Use Default Pin ReMap....")
		sensorModelApiCfgSet(SENSOR_API_PHY0_LANE_REMAP,0xFFFFBA10,0xFFFFFFFF);
#endif

	cmdportid = sensorTgCfg.cmdportid_xtg;
	/*tmrUsWait(10000);*//*Remove the unnecessary time delay.*/
	profLogPrintf(3,"sensor DoFunInit model-ID =\n ");
	UINT16 id[2];
	UINT16 i;
	id[0]=sensorOVRead(0x300a); /*sensor ID-MSB */
	id[1]=sensorOVRead(0x300b); /*sensor ID-LSB  */
	g_uwSensorID = (id[0]<<8)|id[1];
	profLogPrintf(3,"sensor id =%x %x\n",id[0],id[1]);
	for(i=0;i<3;i++)
	{
		if((id[0]==0x46)&&(id[1]==0x88)) 
			break;
		else		
		{
			id[0]=sensorOVRead(0x300a); /*sensor ID-MSB */
			id[1]=sensorOVRead(0x300b); /*sensor ID-LSB  */
			g_uwSensorID = (id[0]<<8)|id[1];
			profLogPrintf(3,"retry read sensor ID\n");
			profLogPrintf(3,"sensor id =%x %x\n",id[0],id[1]);
		}
	}

	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_init_code)/sizeof(cmd_init_code[0]), cmd_init_code[0]);
	sensorSerialFlush(cmdportid);

	ov4689_product_id_chk();

	if (g_ucR2aVerFlag == 1)
	{
		profLogPrintf(3,"!@! Extra setting for R2A version!");
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_extra_setting_1_for_r2a_ver)/sizeof(cmd_extra_setting_1_for_r2a_ver[0]), cmd_extra_setting_1_for_r2a_ver[0],0);

		ov4689_r2a_extra_setting_2_cfg_set();
	}
	
	sensorSerialAsyncWrite(cmdportid,1, cmd_init_code_StreamOff[0]);
	sensorSerialFlush(cmdportid);

	ov4689_r2a_stream_off_extra_setting();
	
	/*following setting should not include stream on command.*/
	sensorSerialAsyncWrite(cmdportid,(sensorSnapModeTbl[initMode&0xF].CmdLen-1), sensorSnapModeTbl[initMode&0xF].pCmdSet); 
	sensorSerialFlush(cmdportid);

	SINT32 sdwExpIdx;
	UINT32 sdwAgcIdx;
	sensorExpAgcPreCfgGet(&sdwExpIdx,&sdwAgcIdx);
	sensorExpAgcPreSet(initMode,sdwExpIdx,sdwAgcIdx);

	sensorSyncSigInit(sensorSnapModeTbl[initMode&0xf].ssSnapCfg);

	halPllCdspClkSet(300000);/*cdsp max 300MHz*/
	UINT32 cdspclk,mclk;
	halCdspMasterClockGet(&cdspclk,&mclk);
	SENSOR_PROF_LOG_PRINT("cdspclk=%d , mclk= %d",cdspclk,mclk);
    /* Reset front and cdsp*/ 
	
	sensorSerialBulkWrite(cmdportid,1, cmd_init_code_StreamOn[0],0);

	ov4689_r2a_stream_on_extra_setting();

	fpvsusp = 0;
	g_ucBoot1st = 1;
	g_ucPreSenMode = initMode;		
	SENSOR_PROF_LOG_ADD("sensorDoFuncInit()___done");
	SENSOR_PROF_LOG_ADD("OV4689 sensorDoFuncInit--");
	/*printf("sensorDoFuncInit()___done \n"); */

}

#else

static void
sensorDoFuncInit()
{
	
	UINT32 cmdportid;
		
	SENSOR_PROF_LOG_ADD("OV4689 sensorDoFuncInit++ 6350 ");
	SENSOR_PROF_LOG_PRINT("________Sensor driver ver <<%s>>", MODEL_VERSION);
	SENSOR_PROF_LOG_PRINT("________Sensor driver build time <<%s>>", BUILD_TIME);
	SENSOR_PROF_LOG_ADD("sensorDoFuncInit()_ov4689_enter_....");

	#if(!SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
		sensorTgPllClkCfgSet(SENSOR_TGPLL_CFG_DRIVING,4);/*40uA*/
		sensorTgPllClkExtSet(24000000);/*24MHz*/
		ov4689_PwrOn_Seq(0);
		#if DUAL_RAW
			ov4689_PwrOn_Seq(1);
		#endif		
	#else	
		/*sensorTgPllClkCtrl(SENSOR_TGPLL_CTRL_EN,0);*//*disable tgpll, the needed clock of serdes is supplied by oscillator.*/
												   /*The timing of disabling tgpll must before serdes initial. This might influence the
													  serdes IO trap.*/	

		sensorTgPllClkCfgSet(SENSOR_TGPLL_CFG_DRIVING,4);/*40uA*/
		sensorTgPllClkExtSet(27000000);/*27MHz*/
		serdes_spca101_PwrOn_Seq(0);
	#endif

#ifdef SEN_REMAP_0
#if DUAL_RAW		
		sensorModelApiCfgSet(SENSOR_API_PHY0_LANE_REMAP,SEN_REMAP_0,0xFFFFFFFF);
#else		
		sensorModelApiCfgSet(SENSOR_API_PHY0_LANE_REMAP,SEN_REMAP_0,0xFFFFFFFF);
#endif
		profLogPrintf(0,"Use IO file pin remap....");
#else
        #pragma message("Use Default Pin ReMap....")
		sensorModelApiCfgSet(SENSOR_API_PHY0_LANE_REMAP,0xFFFFBA10,0xFFFFFFFF);
#endif

	/*
	WRITE8(0xb0009030,0x70);
	WRITE8(0xb0009034,0x30); 
	*/
#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	profLogPrintf(3,"@#@ Serdes Initilization S");
	ov4689_serdes_io_trap_chk();
	serdes_initial(SERDES_UNIDIRECTION_RAW10_CASE);
	serdes_sensor_mode_change_cfg_set(SERDES_S100,(sensorTgDev.defSyncSigCfg->hsize)+(sensorTgDev.defSyncSigCfg->hoffset*2),(sensorTgDev.defSyncSigCfg->vsize)+(sensorTgDev.defSyncSigCfg->voffset*2),STREAM_DATA_RAW10);		 
	serdes_sensor_mode_change_cfg_set(SERDES_S101,(sensorTgDev.defSyncSigCfg->hsize)+(sensorTgDev.defSyncSigCfg->hoffset*2),(sensorTgDev.defSyncSigCfg->vsize)+(sensorTgDev.defSyncSigCfg->voffset*2),STREAM_DATA_RAW10);
	serdes_lvds_tx_transmission_point_set(SERDES_S100,LVDS_TX_LINE_TRANSMISSION_USE_ORI_HD1_FALLING);
	serdes_stream_on_set(SERDES_S100_S101_BOTH);
	profLogPrintf(3,"@#@ Serdes Initilization E");	
#endif

	cmdportid = sensorTgCfg.cmdportid_xtg;
	/*tmrUsWait(10000);*//*Remove the unnecessary time delay.*/
	profLogPrintf(3,"sensor DoFunInit model-ID =\n ");
	UINT16 id[2];
	UINT16 i;
	id[0]=sensorOVRead(0x300a); /*sensor ID-MSB */
	id[1]=sensorOVRead(0x300b); /*sensor ID-LSB  */
	g_uwSensorID = (id[0]<<8)|id[1];
	profLogPrintf(3,"sensor id =%x %x\n",id[0],id[1]);
	for(i=0;i<3;i++)
	{
		if((id[0]==0x46)&&(id[1]==0x88)) 
			break;
		else		
		{
			id[0]=sensorOVRead(0x300a); /*sensor ID-MSB */
			id[1]=sensorOVRead(0x300b); /*sensor ID-LSB  */
			g_uwSensorID = (id[0]<<8)|id[1];
			profLogPrintf(3,"retry read sensor ID\n");
			profLogPrintf(3,"sensor id =%x %x\n",id[0],id[1]);
		}
	}
	
	/*set gain valid time delay 1vd*/
	
	/*sensorSerialBulkWrite(cmdportid,sizeof(cmd_init_code)/sizeof(cmd_init_code[0]), cmd_init_code[0],0);*/
	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_init_code)/sizeof(cmd_init_code[0]), cmd_init_code[0]);
	sensorSerialFlush(cmdportid);
	
	ov4689_product_id_chk();

	if (g_ucR2aVerFlag == 1)
	{
		profLogPrintf(3,"!@! Extra setting for R2A version!");
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_extra_setting_1_for_r2a_ver)/sizeof(cmd_extra_setting_1_for_r2a_ver[0]), cmd_extra_setting_1_for_r2a_ver[0],0);
		
		ov4689_r2a_extra_setting_2_cfg_set();
	}

	/*sensorOVWrite(0x3503,0x1c);*//*cancel for group setting, OV: 0x3503[4:5] immediately 0x3503[0:1] not immediately in group function*/
	/*tmrUsWait(30000);*//*30ms*/
	
	/*sensorSerialBulkWrite(cmdportid,1, cmd_init_code_StreamOff[0],0); */
	sensorSerialAsyncWrite(cmdportid,1, cmd_init_code_StreamOff[0]);
	sensorSerialFlush(cmdportid);

	ov4689_r2a_stream_off_extra_setting();
	
	/*sensorSerialBulkWrite(cmdportid,sizeof(cmd_default_exp_frm_code)/sizeof(cmd_default_exp_frm_code[0]), cmd_default_exp_frm_code[0],0);	*/
	sensorSerialAsyncWrite(cmdportid,sizeof(cmd_default_exp_frm_code)/sizeof(cmd_default_exp_frm_code[0]), cmd_default_exp_frm_code[0]);	
	sensorSerialFlush(cmdportid);

	
	/*sensorSerialBulkWrite(cmdportid,sizeof(DEF_INIT_AFE_CODE)/sizeof(DEF_INIT_AFE_CODE[0]), DEF_INIT_AFE_CODE[0],0);*/
	
	/*sensorSerialBulkWrite(cmdportid,sensorPrevModeTbl[DEF_SENSOR_MODE&0xF].CmdLen, sensorPrevModeTbl[DEF_SENSOR_MODE&0xF].pCmdSet,0); */
	sensorSerialAsyncWrite(cmdportid,sensorPrevModeTbl[DEF_SENSOR_MODE&0xF].CmdLen, sensorPrevModeTbl[DEF_SENSOR_MODE&0xF].pCmdSet); 
	sensorSerialFlush(cmdportid);


	/*tmrUsWait(30000);*//*30ms*/
	#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE) 
	/*Use this bulkwrite function to avoid the I2C command of cmdportid does not finish sending when entering previewbegin.*/
	/*If I2C of cmdportid does not finish before using by other cmdportid , the I2C of S101 and S100 would be blocked because serial could not be done correctly.*/
	sensorSerialBulkWrite(cmdportid,(sizeof(cmd_init_code_StreamOn)/sizeof(cmd_init_code_StreamOn[0])), cmd_init_code_StreamOn[0],0);
	#endif

	#if 0
	/*s0lvds_vsize  set 760*/
	/*
	WRITE8(0xb0009822,0xF8);
	WRITE8(0xb0009823,0x02);
	*/
    /*TG PLL=MCLK out=24MHz*/
    sensorLVDSCfgSet(&sensorLVDSCfg_VGA);
	#endif

#if DUAL_RAW	
    sensorLVDSCfgSet(&sensorLVDSCfg2);
#endif

#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	if(!g_ucRetryInit)
#endif
	{
    	sensorSyncSigInit(sensorTgDev.defSyncSigCfg);
	}

	ov4689_r2a_stream_on_extra_setting();

	/*halFrontLvdsSet(1);*/
	/*tmrUsWait(30000);*//*30ms*/


#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	sensorStsCfg(SENSOR_SYS_CFG_VD_WATCHDOG_PRI_EN,VD_WATCHDOG_EN);
#endif
	
	fpvsusp = 0;
	g_ucBoot1st = 1;
	g_ucPreSenMode = DEF_SENSOR_MODE;		
	SENSOR_PROF_LOG_ADD("sensorDoFuncInit()___done");
	SENSOR_PROF_LOG_ADD("OV4689 sensorDoFuncInit--");
	/*printf("sensorDoFuncInit()___done \n"); */

}

#endif

static void
sensorDoFuncStop()
{
  UINT32 cmdportid;
  cmdportid = sensorTgCfg.cmdportid_xtg; 

  /* Set sensor to stream off mode (EVB has NOT Power IC) */
  sensorSerialBulkWrite(cmdportid,(sizeof(cmd_init_code_StreamOff)/sizeof(cmd_init_code_StreamOff[0])), cmd_init_code_StreamOff[0],0);

#if (!SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
			
  ov4689_PwrOff_Seq(0);
			
#if DUAL_RAW
  ov4689_PwrOff_Seq(1);
#endif

#endif

  /* turn off MCK, XHS, XVS */
  sensorTgPllClkCtrl(SENSOR_TGPLL_CTRL_EN,0); 	 /*disable tgpll*/ 
  halFrontSyncSigCfgSet(FRONT_SYNC_SIGGEN_EN,0,0); /*sigen disable*/
  ucSenModeRec = 0;/*Reset the sensor mode record to avoid sending group hold/end in mode switch.*/
}

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
/*
static UINT32
sensorConnectProcess(void)
{
	profLogPrintf(3,"@ SensorConnectProcessCB!");

	return STATE_SENSOR_VWD_IDLE;
}
*/
static UINT32 
sensorStsCheck(void)
{
	profLogPrintf(3,"@#@ SensorStsCheck!!!");
	g_ucRetryInit = 1;
	sensorSerialFlush(sensorTgCfg.cmdportid_xtg);
	tmrUsWait(10000);/*Wait AE Setting flush done to avoid different tasks use to send I2C settings.*/
	sensorDoFuncInit();

	g_ucRetryInit = 0;/*Disable RetryInit*/

	if (g_uwSensorID == 0x4688)
	{
		profLogPrintf(3,"@#@ SensorStsCheck Done!!!");
		return SUCCESS;
	}
	else
	{	
		profLogPrintf(3,"@#@ SensorStsCheck Fail!!!");

		tmrUsWait(66000);

		return FAIL;
	}
}
#endif

static void
sensorDoFuncSensorMIPIClkLaneChk()
{
#if (!SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)

	UINT8 ucTmrCnt;

	ucTmrCnt = 0;
	while (1)
	{
		if (((READ8(0xb00098b8)&MIPICLKLANE_LPSTATE_MASK) == MIPICLKLANE_LPSTATE)||(ucTmrCnt == MIPICLKLANE_POLLINGCNT))/*Check MIPI clock lane is in low power state or not.*/
		{
			if (ucTmrCnt == MIPICLKLANE_POLLINGCNT)
			{
			  	SENSOR_PROF_LOG_ADD("#@# MIPI Clk lane Check timeout!");
				  printf("@#@ Error MIPI Clk lane LP state check timeout!");
			}
				
			break;
		}
		
		tmrUsWait(MIPICLKLANE_POLLINGTIME);
		ucTmrCnt++;
		
	  	SENSOR_PROF_LOG_PRINT("#@# MIPI Clk lane Check![%d]",ucTmrCnt);
	}

  	SENSOR_PROF_LOG_ADD("#@# MIPI Clk lane Check Done!");

#endif
}

static void
sensorDoFuncPreviewBegin()
{
	UINT32 cmdportid, smode;

#if(!SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	static UINT8 ucIsFormerHDRmode=0;
#endif

	cmdportid = sensorTgCfg.cmdportid_xtg; 
  	smode=sensorModeGet();

  	SENSOR_PROF_LOG_ADD("preview begin"); 

#if(!SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	if (g_ucBoot1st)
	{
		g_ucBoot1st = 0;
		if(sensorModeGet() == g_ucPreSenMode)
		{
  			SENSOR_PROF_LOG_ADD("@#@ preview begin return!");                                                             				
			return;
		}
	}
#endif

	if (g_ucIsSnap2Pv)
	{
		g_ucIsSnap2Pv = 0;
		/*Move the waiting Vd event here. If putting this event waiting to snap next callback function,
		it might cause the capture performance(returning to preview time) to become worse than current status.*/ 
		sensorEventVdWait(1);		
	}

#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	serdes_stream_off_set(SERDES_S100_S101_BOTH);
#endif

	sensorSerialBulkWrite(cmdportid,1, cmd_init_code_StreamOff[0],0);

#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	serdes_sensor_mode_change_cfg_set(SERDES_S100,(sensorPrevModeTbl[smode&0xF].ssPrevCfg->hsize)+(sensorPrevModeTbl[smode&0xF].ssPrevCfg->hoffset*2),(sensorPrevModeTbl[smode&0xF].ssPrevCfg->vsize)+(sensorPrevModeTbl[smode&0xF].ssPrevCfg->voffset*2),STREAM_DATA_RAW10);
	serdes_sensor_mode_change_cfg_set(SERDES_S101,(sensorPrevModeTbl[smode&0xF].ssPrevCfg->hsize)+(sensorPrevModeTbl[smode&0xF].ssPrevCfg->hoffset*2),(sensorPrevModeTbl[smode&0xF].ssPrevCfg->vsize)+(sensorPrevModeTbl[smode&0xF].ssPrevCfg->voffset*2),STREAM_DATA_RAW10);
	serdes_lvds_tx_transmission_point_set(SERDES_S100,LVDS_TX_LINE_TRANSMISSION_USE_ORI_HD1_FALLING);
#endif

	ae_preview_evset_write();
	sensorSerialFlush(cmdportid);

	sensorDoFuncSensorMIPIClkLaneChk();

	ov4689_r2a_stream_off_extra_setting();
	
  	SENSOR_PROF_LOG_ADD("preview cmd done wait done..");
	
	SENSOR_PROF_LOG_PRINT("mode %x i2c send +++..",smode);	

	#if 0
	sensorSerialBulkWrite(cmdportid, sizeof(cmd_720P_60fps_code)/sizeof(cmd_720P_60fps_code[0]), cmd_720P_60fps_code[0],0);
	sensorSyncSigInit(&sensorSyncSigPreviewVGACfg); 	
	#endif

	#if 0
	sensorSerialBulkWrite(cmdportid, sizeof(cmd_full_60fps_code)/sizeof(cmd_full_60fps_code[0]), cmd_full_60fps_code[0],0);
	sensorSyncSigInit(&sensorSyncSigSnapCfg);     
	#endif
	#if 0 /*HDR test*/
	if(smode==0x35){
		/*s0lvds_vsize  set 4560*/
		WRITE8(0xb0009822,0xD0);
		WRITE8(0xb0009823,0x11);
	}
	else if(smode==0x39){
		/*s0lvds_vsize  set 2160*/
		WRITE8(0xb0009822,0x70);
		WRITE8(0xb0009823,0x08);
	}
	#endif

#if 0 /*Remove the unnecessary lvds size configuration.*/
	if(smode==0x3A){
		/*s0lvds_vsize  set 3040*/
		WRITE8(0xb0009822,0xE0);
		WRITE8(0xb0009823,0x0B);
	}
#endif

#if 1


#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	serdes_stream_on_set(SERDES_S100_S101_BOTH);
#else
	/*IF former sensor mode is HDR mode and current sensro mode is non HDR mode(normal preview).*/
	if ((ucIsFormerHDRmode==1)&&(((sensorPrevModeTbl[smode&0xF].ucModeInfo&0x2)>>1)==0))
	{
		SENSOR_PROF_LOG_PRINT("#$# HDR Back to normal preview!!!");
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_HDRBackToNormal_code)/sizeof(cmd_HDRBackToNormal_code[0]),cmd_HDRBackToNormal_code[0],0); 
	}


	if ((sensorPrevModeTbl[smode&0xF].ucModeInfo&0x5)==0x5)/*Means binning mode use binning sum*/
	{
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_bin_sum_en)/sizeof(cmd_bin_sum_en[0]),cmd_bin_sum_en[0],0); 
		SENSOR_PROF_LOG_PRINT("@ bin sum..");
	}
	else /*Other cases, return to use default value.*/
	{
		sensorSerialBulkWrite(cmdportid,sizeof(cmd_bin_avg_en)/sizeof(cmd_bin_avg_en[0]),cmd_bin_avg_en[0],0);
		SENSOR_PROF_LOG_PRINT("@ bin avg..");
	}

	ucIsFormerHDRmode = (sensorPrevModeTbl[smode&0xF].ucModeInfo&0x2)>>1;
	
#endif
	sensorSerialBulkWrite(cmdportid,sensorPrevModeTbl[smode&0xF].CmdLen, sensorPrevModeTbl[smode&0xF].pCmdSet,0); 

	sensorSyncSigInit(sensorPrevModeTbl[smode&0xF].ssPrevCfg);

	ov4689_r2a_stream_on_extra_setting();
	
#else
  	if(smode==0x30){     
		/*s0lvds_vsize  set 760*/
		WRITE8(0xb0009822,0xF8);
		WRITE8(0xb0009823,0x02);
  		/* bin2	-preview 1344x760@30fps          0x30 */  
  		sensorSerialBulkWrite(cmdportid, sizeof(cmd_720P_60fps_code)/sizeof(cmd_720P_60fps_code[0]), cmd_720P_60fps_code[0],0);  
  		sensorSyncSigInit(&sensorSyncSigPreviewVGACfg);
    }else if(smode==0x31){
    	/* scale	-720p@120fps        0x31 */ 
    	sensorSerialBulkWrite(cmdportid, sizeof(cmd_720P_120fps_code)/sizeof(cmd_720P_120fps_code[0]), cmd_720P_120fps_code[0],0);  
    	sensorSyncSigInit(&sensorSyncSigPreview720P_SCALECfg);                                                           
	}else if(smode==0x32){
		/* crop	-720p@120fps        0x32 */ 
		sensorSerialBulkWrite(cmdportid, sizeof(cmd_720P_120fps_code)/sizeof(cmd_720P_120fps_code[0]), cmd_720P_120fps_code[0],0);
		sensorSyncSigInit(&sensorSyncSigPreview720P_CROPCfg);														   
	}else if(smode==0x33){
		 /* scale -1080P@60fps       0x33 */
 		sensorSerialBulkWrite(cmdportid, sizeof(cmd_full_60fps_code)/sizeof(cmd_full_60fps_code[0]), cmd_full_60fps_code[0],0);
		sensorSyncSigInit(&sensorSyncSigPreviewFull60fpsCfg);  
	}
	else if(smode==0x34){
		/* crop	-1080P@60fps       0x34 */
		sensorSerialBulkWrite(cmdportid, sizeof(cmd_1080P_code)/sizeof(cmd_1080P_code[0]), cmd_1080P_code[0],0);
		sensorSyncSigInit(&sensorSyncSigPreview1080PCfg); 
	}
	else if(smode==0x35){
		/*s0lvds_vsize  set 4560*/
		WRITE8(0xb0009822,0xD0);
		WRITE8(0xb0009823,0x11);
		/* HDR	-2688x1520@71fps       0x35 */
		sensorSerialBulkWrite(cmdportid, sizeof(cmd_HDR_code)/sizeof(cmd_HDR_code[0]), cmd_HDR_code[0],0);
		sensorSyncSigInit(&sensorSyncSigPreviewHDRCfg); 
	}
	else if(smode==0x36){
		/* scale 	-1080P@30fps       0x36 */
		sensorSerialBulkWrite(cmdportid, sizeof(cmd_full_60fps_code)/sizeof(cmd_full_60fps_code[0]), cmd_full_60fps_code[0],0);
		sensorSyncSigInit(&sensorSyncSigPreviewFull30fpsCfg); 
	}
	else if(smode==0x37){
		/*s0lvds_vsize  set 760*/
		WRITE8(0xb0009822,0xF8);
		WRITE8(0xb0009823,0x02);
    	/* scale	-720p@30fps        0x37 */ 
    	sensorSerialBulkWrite(cmdportid, sizeof(cmd_720P_60fps_code)/sizeof(cmd_720P_60fps_code[0]), cmd_720P_60fps_code[0],0);  
    	sensorSyncSigInit(&sensorSyncSigPreview720P_30fps_SCALECfg);                                                           
	}
	else if(smode==0x38){
    	/* scale	-720p@60fps        0x38 */ 
    	sensorSerialBulkWrite(cmdportid, sizeof(cmd_720P_120fps_code)/sizeof(cmd_720P_120fps_code[0]), cmd_720P_120fps_code[0],0);  
    	sensorSyncSigInit(&sensorSyncSigPreview720P_60fps_SCALECfg);                                                           
	}
#endif	

	SENSOR_PROF_LOG_PRINT("mode %x i2c send ---..",smode);	

	g_ucPreSenMode = smode;
    fpvsusp = 1;
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
	UINT32 cmdportid;
	UINT32 smode;
    SENSOR_PROF_LOG_ADD("SnapExpBegin s");  
	cmdportid = sensorTgCfg.cmdportid_xtg;

	smode=sensorModeGet();

	/*profLogInit(2,0x8000);*/

	g_ucIsSnap2Pv = 1;
	
	#if 1
	sensorSerialFlush(cmdportid);

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	serdes_stream_off_set(SERDES_S100_S101_BOTH);
#endif

	sensorSerialBulkWrite(cmdportid,1, cmd_init_code_StreamOff[0],0); /*Stream off must be sent 1stly to make sensor could stop stream after vvalid falling.*/

	sensorDoFuncSensorMIPIClkLaneChk();/*Use this function to check the MIPI clock lane is in low power state or not.  Avoid using vvalid falling wait. */	

	ov4689_r2a_stream_off_extra_setting();

#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	serdes_sensor_mode_change_cfg_set(SERDES_S101,(sensorSnapModeTbl[smode&0xF].ssSnapCfg->hsize)+(sensorSnapModeTbl[smode&0xF].ssSnapCfg->hoffset*2),(sensorSnapModeTbl[smode&0xF].ssSnapCfg->vsize)+(sensorSnapModeTbl[smode&0xF].ssSnapCfg->voffset*2),STREAM_DATA_RAW10);
	serdes_lvds_tx_transmission_point_set(SERDES_S100,LVDS_TX_LINE_TRANSMISSION_USE_ORI_HD1_FALLING);
#endif

	sensorSerialBulkWrite(cmdportid,sensorSnapModeTbl[smode&0xF].CmdLen, sensorSnapModeTbl[smode&0xF].pCmdSet,0); 

#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	serdes_stream_on_set(SERDES_S100_S101_BOTH);
	sensorSerialBulkWrite(cmdportid,1, cmd_init_code_StreamOn[0],0); /*Stream on.*/
#endif

	sensorSyncSigInit(sensorSnapModeTbl[smode&0xF].ssSnapCfg);

	ov4689_r2a_stream_on_extra_setting();

	#else
	if(sensorModeGet()==0x41){
		/*HDR mode*/
		/*s0lvds_vsize  set 4560*/
		WRITE8(0xb0009822,0xD0);
		WRITE8(0xb0009823,0x11);
		sensorSerialBulkWrite(cmdportid, sizeof(cmd_HDR_code)/sizeof(cmd_HDR_code[0]), cmd_HDR_code[0],0);
		sensorSyncSigInit(&sensorSyncSigSnapHDRCfg);
	}
	else{	
		/*normal capture  0x40*/
		sensorSerialBulkWrite(cmdportid, sizeof(cmd_full_60fps_code)/sizeof(cmd_full_60fps_code[0]), cmd_full_60fps_code[0],0);
		sensorSyncSigInit(&sensorSyncSigSnapCfg);
	}
	#endif
	g_ucPreSenMode = smode;
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
		.speed_khz = TG_SPI_I2C_FREQ_KHz,
	};

#if(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
static const sensorCmdport_t s100_cmdport_xtg_cfg = {
	.mode		  = (0x45<<1),
	.format2	  = TG_FORMAT2,
	.format3	  = TG_FORMAT3,
	.speed		  = TG_SPEED,
	.nbit_regdata = TG_NBIT_REGDATA,
	.nbit_regaddr = TG_NBIT_REGADDR,
	.speed_khz = TG_SPI_I2C_FREQ_KHz,		
};
static const sensorCmdport_t s101_cmdport_xtg_cfg = {
	.mode		  = (0x63<<1),
	.format2	  = TG_FORMAT2,
	.format3	  = TG_FORMAT3,
	.speed		  = TG_SPEED,
	.nbit_regdata = TG_NBIT_REGDATA,
	.nbit_regaddr = TG_NBIT_REGADDR,
	.speed_khz = TG_SPI_I2C_FREQ_KHz,		
};
#endif

	ptcfg->cmdportid_exp =
	ptcfg->cmdportid_xtg = sensorSerialCreate(&sensor_cmdport_xtg_cfg);

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)	
	g_Cmdportids100 = sensorSerialCreate(&s100_cmdport_xtg_cfg);
	profLogPrintf(3,"@#@S100 cmdportID=%d",g_Cmdportids100);
	g_Cmdportids101 = sensorSerialCreate(&s101_cmdport_xtg_cfg);
	profLogPrintf(3,"@#@S101 cmdportID=%d",g_Cmdportids101);	
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

	OV4689_SensorInfoCfgSet();

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
sensor_pregain_delay_decision(UINT32 nreg2, const UINT8 *pdata2)
{
	static UINT32 frmlinecnt_record = 0;
	UINT32 frmlinecnt = 0;
	#if 0
	UINT32 frmlinecnt_limit[9]={
		797,  /*exp line of mode 0x30 at 1/60*/
		1605, /*exp line of mode 0x31 at 1/60*/
		1605, /*exp line of mode 0x32 at 1/60*/
		1565, /*exp line of mode 0x33 at 1/60*/
		1238, /*exp line of mode 0x34 at 1/60*/
		3108, /*exp line of mode 0x36 at 1/60*/
		797,  /*exp line of mode 0x37 at 1/60*/
		1605,  /*exp line of mode 0x38 at 1/60*/
		797,   /*exp line of mode 0x3B at 1/60*/
	};
	#endif
	
	frmlinecnt =  pdata2[11]+(pdata2[14]<<8);
	if(frmlinecnt_record == 0){
		frmlinecnt_record = frmlinecnt;
	}
	
	#if 0
	switch(sensorModeGet()){
			case 0x30:
					/*if(frmlinecnt>frmlinecnt_limit[0] && frmlinecnt!=frmlinecnt_record)*/
					if(frmlinecnt!=frmlinecnt_record)
						sensor_PregainDelay(0);
					else
						sensor_PregainDelay(1);
				break;
			case 0x31:
					/*if(frmlinecnt>frmlinecnt_limit[1] && frmlinecnt!=frmlinecnt_record)*/
					if(frmlinecnt!=frmlinecnt_record)
						sensor_PregainDelay(0);
					else
						sensor_PregainDelay(1);
				break;
			case 0x32:
					/*if(frmlinecnt>frmlinecnt_limit[2] && frmlinecnt!=frmlinecnt_record)*/
					if(frmlinecnt!=frmlinecnt_record)
						sensor_PregainDelay(0);
					else
						sensor_PregainDelay(1);
				break;
			case 0x33:
					/*if(frmlinecnt>frmlinecnt_limit[3] && frmlinecnt!=frmlinecnt_record)*/
					if(frmlinecnt!=frmlinecnt_record)
						sensor_PregainDelay(0);
					else
						sensor_PregainDelay(1);
				break;
			case 0x34:
					/*if(frmlinecnt>frmlinecnt_limit[4] && frmlinecnt!=frmlinecnt_record)*/
					if(frmlinecnt!=frmlinecnt_record)
						sensor_PregainDelay(0);
					else
						sensor_PregainDelay(1);
				break;
			case 0x36:
					/*if(frmlinecnt>frmlinecnt_limit[5] && frmlinecnt!=frmlinecnt_record)*/
					if(frmlinecnt!=frmlinecnt_record)
						sensor_PregainDelay(0);
					else
						sensor_PregainDelay(1);
				break;
			case 0x37:
					/*if(frmlinecnt>frmlinecnt_limit[6] && frmlinecnt!=frmlinecnt_record)*/
					if(frmlinecnt!=frmlinecnt_record)
						sensor_PregainDelay(0);
					else
						sensor_PregainDelay(1);
				break;
			case 0x38:
					/*if(frmlinecnt>frmlinecnt_limit[7] && frmlinecnt!=frmlinecnt_record)*/
					if(frmlinecnt!=frmlinecnt_record)
						sensor_PregainDelay(0);
					else
						sensor_PregainDelay(1);
				break;
			case 0x3B:
					/*if(frmlinecnt>frmlinecnt_limit[8] && frmlinecnt!=frmlinecnt_record)*/
					if(frmlinecnt!=frmlinecnt_record)
						sensor_PregainDelay(0);
					else
						sensor_PregainDelay(1);
				break;
			default:
					sensor_PregainDelay(1);
				break;
		}
	#else
		if(frmlinecnt!=frmlinecnt_record){
			sensor_PregainDelay(0);
		}
		else{
			sensor_PregainDelay(1);
		}
	#endif
	frmlinecnt_record = frmlinecnt;
}

static void
sensor_cmdport_frame_write(UINT32 smode,UINT32 kfps_max,UINT32 kfps_min,UINT32 nreg,const UINT8 *pdata)
{
/*
15fps
[0x35,0x00,0x00]
[0x35,0x01,0x61]
[0x35,0x02,0x50]
[0x38,0x0f,0x3A]
[0x38,0x0e,0x0C]
*/
	static UINT32 frmlinecnt_record;
	UINT32 frm_org_length,frm_org_kfps ;
	UINT32 frm_new_length,udwCurExpLineCnt;
	UINT8 ucExpLineOverFrmBound=0;
	static UINT8 regData[][3] SENSOR_SIF1DMA_VARBUF = {
		{0x38,0x0E,0x0},
		{0x38,0x0F,0x0},
	};

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	if (g_ucRetryInit)
	{
		return;
	}
#endif


	udwCurExpLineCnt = (pdata[2]<<12)|(pdata[5]<<4)|(pdata[8]>>4);
	/*profLogPrintf(3,"@#@exp=0x%x,[0x%x 0x%x 0x%x][0x%x 0x%x 0x%x][0x%x 0x%x 0x%x]",udwCurExpLineCnt,pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7],pdata[8]);*/

	/*profLogPrintf(3,"[MODE 0x%x FPS MAX=%d FPS MIN=%d NREG=%d]",smode,kfps_max,kfps_min,nreg);*/

	if(!kfps_max){

		/*Do not use the following method to send frame length settings. Because the array pointer address must be 4 bytes alignment.*/
		/*(pdata+3*3) is not 4 bytes alignment.*/
		/*sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,2,(pdata+3*3));*/

		regData[1][2] = pdata[11];
		regData[0][2] = pdata[14];
		sensorSerialDirectBufSnoop(regData[0], sizeof(regData));
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,sizeof(regData)/sizeof(regData[0]),regData[0]);

		frm_new_length = pdata[11]+(pdata[14]<<8);

		goto End;
		/*profLogPrintf(3,"@#@ Maximum frame rate control disabled!");*/
		/*return;*/ /*use exposure table frame rate, ,pdata[9]~pdata[15]*/
	}


	/*UINT32 i;*/
	/*
	for(i=0;i<nreg;i++)
		profLogPrintf(3,"[%x %x %x]",pdata[i*3+0],pdata[i*3+1],pdata[i*3+2]);
	*/
	if ((smode&0x30)==0x30)
	{
		frm_org_length = sensorPrevModeTbl[smode&0xF].ssPrevCfg->frametotal;
		frm_org_kfps = sensorPrevModeTbl[smode&0xF].pstpPrev->fps_mhz/1000;
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

	if(kfps_max<frm_org_kfps)
	{			
		frm_new_length = frm_org_kfps*frm_org_length/kfps_max;
		/*profLogPrintf(3,"# Calculated new frm=%d",frm_new_length);*/
		if (udwCurExpLineCnt > (frm_new_length-8))
		{
			frm_new_length = udwCurExpLineCnt+8;
			ucExpLineOverFrmBound = 1;
			/*profLogPrintf(3,"# Exp Over frm, new frm=%d",frm_new_length);*/
		}
		
		/*The sensor mode is binning mode. Change the new frame length to be 4N+1.*/
		if ((sensorPrevModeTbl[smode&0xF].ucModeInfo&0x1) == 1) 
		{
			UINT8 ucMod;
			ucMod = frm_new_length%4;
			/*profLogPrintf(3,"#$# frm_new_length before Chk=%d",frm_new_length);*/
			if (ucExpLineOverFrmBound)
			{
				/*If exp line is over the calculated new frm length, change to take the nearest 4N+1 which is equal or greater than
				   new frm length.*/
				if (ucMod == 0)
				{
					frm_new_length = (frm_new_length+1);
				}
				else if (ucMod != 1)
				{
					frm_new_length = (((frm_new_length>>2)+1)<<2)+1;
				}
			}
			else
			{
				/*If exp line is not over the calculated new frm length, change to take the nearest 4N+1 which is smaller than
				   new frm length.*/
				if (ucMod == 0)
				{
					frm_new_length = (((frm_new_length>>2)-1)<<2)+1;
				}
				else if (ucMod != 1)
				{
					frm_new_length = (((frm_new_length>>2))<<2)+1;
				}
			}
			
			if (frm_new_length < frm_org_length)
				frm_new_length = frm_org_length;
		}

		/*profLogPrintf(3,"@#@ frm_org=%d,kfps_org=%d,frm_new=%d",frm_org_length,frm_org_kfps,frm_new_length);*/
	
		regData[0][2] = (frm_new_length>>8)&0xff;
		regData[1][2] = (frm_new_length)&0xff;
		sensorSerialDirectBufSnoop(regData[0], sizeof(regData));
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,sizeof(regData)/sizeof(regData[0]),regData[0]);
	}
	else /*kfps_max >=frm_org_kfps*/ /* setting maximum frame rate  > original maximum frame rate,  use the orignal frame length.*/
	{

		/*Do not use the following method to send frame length settings. Because the array pointer address must be 4 bytes alignment.*/
		/*(pdata+3*3) is not 4 bytes alignment.*/
		/*sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,2,(pdata+3*3));*/

		regData[1][2] = pdata[11];
		regData[0][2] = pdata[14];
		frm_new_length = pdata[11]+(pdata[14]<<8);
		sensorSerialDirectBufSnoop(regData[0], sizeof(regData));
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,sizeof(regData)/sizeof(regData[0]),regData[0]);

		/*profLogPrintf(3,"#$# Over original maximum, use original maximum frame rate!");*/
	}


End:
	if(frm_new_length!=frmlinecnt_record){
		sensor_PregainDelay(0);
		/*profLogPrintf(3,"@#@ PreGainDelay Set 0");*/
	}
	else{
		sensor_PregainDelay(1);
		/*profLogPrintf(3,"@#@ PreGainDelay Set 1");*/
	}

	frmlinecnt_record = frm_new_length;

}

static void 
sensor_cmdport_begin(UINT32 smode,UINT32 expIdx,UINT32 agcIdx,UINT32 exp2Idx,UINT32 agc2Idx)
{
	/*group hold*/
	static UINT8 regdata1[1][3] SENSOR_SIF1DMA_VARBUF ={
		{0x32,0x08,0x00},
	} ;

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	if (g_ucRetryInit)
	{
		return;
	}
#endif

	if (ucSenModeRec != sensorModeGet())/*in mode switch, skip group hold*/
		return;
	
	sensorSerialDirectBufSnoop(regdata1[0], sizeof(regdata1));
	sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 1, regdata1[0]);
	/*profLogPrintf(3,"@#@ Group hold");*/
}

extern UINT8  preGainDelay;
static void 
sensor_cmdport_end(void)
{
	/*group end and launch */
	/*if(preGainDelay || sensorModeGet()==0x41 || sensorModeGet()==0x35 ||	sensorModeGet()==0x39 || sensorModeGet()==0x3A)*/
	UINT8 ucCurSenMode;
	ucCurSenMode = sensorModeGet();

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	if (g_ucRetryInit)
	{
		goto End;
	}
#endif	

	if (ucCurSenMode != ucSenModeRec)/*in mode switch, skip group release*/
		goto End;
	
	if(preGainDelay || ucCurSenMode==0x3A)
	{
		static UINT8 regdata1[2][3] SENSOR_SIF1DMA_VARBUF ={
			{0x32,0x08,0x10},
			{0x32,0x08,0xA0},
		};
		sensorSerialDirectBufSnoop(regdata1[0], sizeof(regdata1));
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 2, regdata1[0]);
	}
	else
	{
		static UINT8 regdata1[4][3] SENSOR_SIF1DMA_VARBUF={
			{0x38,0x23,0x01},/*timming re-set on*/
			{0x38,0x23,0x00},/*timming re-set off*/
			{0x32,0x08,0x10},
			{0x32,0x08,0xA0},
		};
		sensorSerialDirectBufSnoop(regdata1[0], sizeof(regdata1));
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp, 4, regdata1[0]);
	}

End:
	ucSenModeRec = ucCurSenMode;
	/*profLogPrintf(3,"@#@ Group End");*/

	return;	

}


static void
sensor_cmdport_exp_write(UINT32 nreg, const UINT8 *pdata)
{
	/*
	UINT32 i;
	printf("\ndata:%d %d %d \n%d %d %d \n%d %d %d \n%d %d %d \n%d %d %d \n",*(pdata+0),*(pdata+1),*(pdata+2),
									 								*(pdata+3),*(pdata+4),*(pdata+5),
									 								*(pdata+6),*(pdata+7),*(pdata+8),
									 								*(pdata+9),*(pdata+10),*(pdata+11),
									 								*(pdata+12),*(pdata+13),*(pdata+14));
	*/

	UINT8 ucCurSenMode;

	ucCurSenMode = sensorModeGet();

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	if (g_ucRetryInit)
	{	
		return;
	}/*g_ucRetryInit.*/
#endif

	#if 1


		if (sensorTgDev.tgSvc->frmwrite != NULL)
		{
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,(nreg-2), pdata);
			/*
			profLogPrintf(3,"@#@ [0x%x 0x%x 0x%x] [0x%x 0x%x 0x%x] [0x%x 0x%x 0x%x]",(pdata+0),*(pdata+1),*(pdata+2), \
		 								*(pdata+3),*(pdata+4),*(pdata+5), \
		 								*(pdata+6),*(pdata+7),*(pdata+8));
			*/
		}
		else
		{
			sensor_pregain_delay_decision(nreg,pdata);
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,nreg, pdata);
			/*profLogPrintf(3,"$#$ Without frmwrite function!!!");*/
		}		
	#endif

	if(ucCurSenMode==0x41 ||ucCurSenMode==0x35 || ucCurSenMode==0x39 || ucCurSenMode==0x3A)
	{
		gpdata = pdata;		

		if (ucCurSenMode == 0x3A)
		{
			sensor_HDR_EXP_do();
			sensorSerialDirectBufSnoop(g_ucMiddleExpData[0], sizeof(g_ucMiddleExpData));
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,sizeof(g_ucMiddleExpData)/sizeof(g_ucMiddleExpData[0]), g_ucMiddleExpData[0]);
			sensorSerialDirectBufSnoop(g_ucShortExpData[0], sizeof(g_ucShortExpData));
			sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,sizeof(g_ucShortExpData)/sizeof(g_ucShortExpData[0]), g_ucShortExpData[0]);
			/*profLogPrintf(3,"#@# Send HDR Middle/Short Exp Settings!!! ");*/
		}		
	}

}

#if 0
static void 
sensor_exp_write_redo(UINT32 smode,SINT32 *expIdx,UINT32 *vcount,UINT8 *vtbl_type,UINT32 *int_us)
{
	UINT32 snap_frm_org_length,snap_frm_org_fps	;
	UINT32 snap_frm_new_length,snap_frm_new_fps;
	UINT32 max_intv_ms;
	UINT32 interval_ms;

	if(*int_us==0)
		return;
	
	if(smode==0x40){
		snap_frm_org_length =CAPTURE_FRAMETOTAL;
		snap_frm_org_fps	=60;	
	}
	else{ /*0x41*/
		snap_frm_org_length =HDR_FRAMETOTAL;
		snap_frm_org_fps	=30;	
	}

	/*max frm rate, sensor register limitation */
	max_intv_ms=(1000*0xFFFF)/(snap_frm_org_fps*snap_frm_org_length);
	interval_ms=*int_us/1000;
	if(interval_ms > max_intv_ms)
	{
		printf("interval exceed max(%d us)",max_intv_ms*1000);
		interval_ms=max_intv_ms;
	}
	snap_frm_new_length=interval_ms*snap_frm_org_fps*snap_frm_org_length/1000;
	snap_frm_new_fps=1000*100/(interval_ms);
	snap_frm_new_length= (snap_frm_new_length>0xFFFF)?0xFFFF:snap_frm_new_length; /*must <=0xFFFF*/

    if(snap_frm_new_length<snap_frm_org_length)
	    snap_frm_new_length=snap_frm_org_length;

	profLogPrintf(3,"interval delay us=%d, exp idx= %d, new frmlength =%d new frm rate= %d.%02d FPS",*int_us,*expIdx,snap_frm_new_length,snap_frm_new_fps/100,snap_frm_new_fps%100);

	/*
	static UINT8 regData[][4] SENSOR_SIF1DMA_VARBUF = {
		MI_REG(0x300a,0x0000),
	};
	*/
	static UINT8 regData[][3] SENSOR_SIF1DMA_VARBUF = {
		{0x38,0x0E,0},
		{0x38,0x0F,0},
	};

	regData[0][2] = (snap_frm_new_length>>8)&0xff;
	regData[1][2] = (snap_frm_new_length)&0xff;

	profLogPrintf(3,"[%x %x %x]",regData[1][0],regData[1][1],regData[1][2]);
	profLogPrintf(3,"[%x %x %x]",regData[0][0],regData[0][1],regData[0][2]);
	sensorSerialDirectBufSnoop(regData[0], sizeof(regData));
	sensorSerialBulkWrite(0,sizeof(regData)/sizeof(regData[0]),regData[0],0);
}
#endif

static void
sensor_cmdport_exp_vtbl_do(UINT32 vtbl_type, UINT32 vcount)
{
}

void ov4689_sensorCmdPortExpLineSend(UINT16 expLine)
{
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
}

#if SP5K_SENSOR_OV4689
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	ov4689_sensorCmdPortExpLineSend(expLine);
}
#endif

static void
sensorDoFuncPowerSave(UINT32 senmode)
{
	UINT32 cmdportid;                                                                                                                             
	cmdportid = sensorTgCfg.cmdportid_xtg;  

    if(senmode==SENSOR_MODE_POWERSAVE){
		profLogPrintf(3,"@#@ OV4689 power save function S!");

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
		sensor_vd_watchdog_stop(1);
        serdes_spca101_PwrOff_Seq(0);
#else		
        /* Set sensor to stream off mode (EVB has NOT Power IC) */
        sensorSerialBulkWrite(cmdportid,(sizeof(cmd_init_code_StreamOff)/sizeof(cmd_init_code_StreamOff[0])), cmd_init_code_StreamOff[0],0);

        ov4689_PwrDown_Seq(0);
#endif
		
#if DUAL_RAW
		ov4689_PwrDown_Seq(1);
#endif
		/* turn off MCK, XHS, XVS */
		sensorTgPllClkCtrl(SENSOR_TGPLL_CTRL_EN,0); 	 /*disable tgpll*/ 
		halFrontSyncSigCfgSet(FRONT_SYNC_SIGGEN_EN,0,0); /*sigen disable*/

		ucSenModeRec = 0;/*Reset the sensor mode record to avoid sending group hold/end in mode switch.*/
		g_ucPreSenMode = 0x00;
		profLogPrintf(3,"@#@ OV4689 power save function E!");		
    }else{
	    profLogPrintf(3,"@#@ OV4689 power resume function S!"); 
		sensorSerialFlush(cmdportid);    /*avoid ae not flush*/

		#if (HTC_FAST_CAPTURE)
			sensorDoFuncFastCapInit();
		#else
	        sensorDoFuncInit();
		#endif
		
	#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
		sensor_vd_watchdog_start(1);
	#endif
	
		profLogPrintf(3,"@#@ OV4689 power resume function E!");
    }
}

void
sensor_HDR_ev_set(UINT8 ev)
{
	static UINT8 ev_rec=0;
	if(ev_rec!=ev)
	{
		ev_rec=ev;
		SENSOR_PROF_LOG_PRINT("set HDR_EV:%d.%d\n",ev_rec/10,ev_rec%10);
	}
	HDR_ev = ev;
}
void
sensor_HDR_EXP_do(void)
{
	#define Max_exp_line 27573  /* AE_idx 33(1/4 sec):check exp table exp line*/
	#define Min_exp_line 5		/* AE_idx 225:check exp table exp line*/
	#define HDR_debug 0
	
	/*UINT8  exp_data[3];*/
	
	#if HDR_debug
	UINT32 frmlinecnt = 0;
	UINT32 u1_EV_diff = 0, u2_EV_diff = 0;
	#endif

	/*2^0,2^0.1~2^2*/
	const UINT32 gwPwrTblx1000[31] =
	{
		1000, 1072, 1149, 1231, 1320, 1414, 1516, 1625, 1741, 1866,
		2000, 2144, 2297, 2462, 2639, 2828, 3031, 3249, 3482, 3732,
		4000, 4287, 4595, 4925, 5278, 5657, 6063, 6498, 6964, 7464,
		8000
	};
	
	/*HDR mode, Middel & Short Exp set*/
	if(sensorModeGet()==0x41 || sensorModeGet()==0x35 || sensorModeGet()==0x39|| sensorModeGet()==0x3A)
	{
		UINT32 explinecnt = 0; 
		UINT32 u1_EV_lincnt = 0, u2_EV_lincnt,EV_diff_temp = 0;
		explinecnt = (gpdata[2]<<16)+(gpdata[5]<<8)+gpdata[8];
		explinecnt = (explinecnt>>4);
		
		#if HDR_debug
		frmlinecnt = gpdata[11]+(gpdata[14]<<8);
		printf("%d %d \n",explinecnt,frmlinecnt);
		#endif

		
		EV_diff_temp = (explinecnt*1000)/(gwPwrTblx1000[HDR_ev]);  /*prevent over flow, (explinecnt*100000000)/(gwPwrTblx1000[HDR_ev])^2*/
		EV_diff_temp = (EV_diff_temp*1000)/(gwPwrTblx1000[HDR_ev]);/*prevent over flow, (explinecnt*100000000)/(gwPwrTblx1000[HDR_ev])^2*/
		

		#if HDR_debug
		u1_EV_diff = (explinecnt - (explinecnt*1000)/gwPwrTblx1000[HDR_ev]); 	/*-1 EV exp line diff*/
		u2_EV_diff = ((explinecnt*1000)/gwPwrTblx1000[HDR_ev] - EV_diff_temp); /*-2 EV exp line diff*/
		printf("ev1_linecnt:%d ev2_linecnt%d \n",u1_EV_diff,u2_EV_diff);	
		#endif
		
		#if 1
		/*middle exp*/
		/*u1_EV_lincnt = explinecnt - HDR_ev*16*(u1_EV_diff*100/16)/1000;*/ /*explinecnt - HDR_ev*16*(u1_EV_diff/16)/10*/
		u1_EV_lincnt = (explinecnt*1000)/gwPwrTblx1000[HDR_ev];
		/*boundary check*/
		if(u1_EV_lincnt >Max_exp_line)
		{
			u1_EV_lincnt = Max_exp_line;		
		}
		else if (u1_EV_lincnt < Min_exp_line)
		{
			u1_EV_lincnt = Min_exp_line;		
		}

		/*profLogPrintf(3,"@#@u1_EV_lincnt:%d",u1_EV_lincnt);*/
		#if HDR_debug
		printf("u1_EV_lincnt:%d\n",u1_EV_lincnt);
		#endif
		/*
		u1_EV_lincnt = u1_EV_lincnt<<4;
		
		exp_data[0] = u1_EV_lincnt>>16;
		exp_data[1]	= u1_EV_lincnt>>8;
		exp_data[2] = u1_EV_lincnt;*/

		/*
		exp_data[2] = (u1_EV_lincnt%16)*16;
		exp_data[1]	= ((u1_EV_lincnt-(u1_EV_lincnt%16))/16)%256;
		exp_data[0] = (u1_EV_lincnt-exp_data[2]/16-exp_data[1]*16)/256/16;
		*/

		g_ucMiddleExpData[0][2] = (u1_EV_lincnt>>12)&0x0f;
		g_ucMiddleExpData[1][2] = (u1_EV_lincnt>>4)&0xff;
		g_ucMiddleExpData[2][2] = (u1_EV_lincnt&0x0f)<<4;

		/*
		profLogPrintf(3,"@#@Reg1:%d %d %d",exp_data[0],exp_data[1],exp_data[2]);		
		profLogPrintf(3,"@#@Reg2:%d %d %d",g_ucMiddleExpData[0][2],g_ucMiddleExpData[1][2],g_ucMiddleExpData[2][2]);		
		*/
		#if HDR_debug
		printf("%d %d %d\n",exp_data[0],exp_data[1],exp_data[2]);
		printf("%d %d %d\n",g_ucMiddleExpData[0][2],g_ucMiddleExpData[1][2],g_ucMiddleExpData[2][2]);		
		#endif

		/*
		sensorOVWrite(0x350A,exp_data[0]);
		sensorOVWrite(0x350B,exp_data[1]);
		sensorOVWrite(0x350C,exp_data[2]);
		*/
		#if 0
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,0x350A,&exp_data[0]);
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,0x350B,&exp_data[1]);
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,0x350C,&exp_data[2]);
		#endif
		
		/*short exp*/
		/*u2_EV_lincnt = (explinecnt/2) - HDR_ev*16*(u2_EV_diff*100/16)/1000; *//* HDR_ev*16*(u2_EV_diff/16)/10*/
		u2_EV_lincnt = EV_diff_temp;
		/*boundary check*/
		if(u2_EV_lincnt >Max_exp_line)
		{  		
			u2_EV_lincnt = Max_exp_line;		
		}
		else if (u2_EV_lincnt < Min_exp_line)
		{	
			u2_EV_lincnt = Min_exp_line;		
		}

		/*profLogPrintf(3,"@#@u2_EV_lincnt:%d",u2_EV_lincnt);*/		
		#if HDR_debug
		printf("u2_EV_lincnt:%d\n",u2_EV_lincnt);
		#endif
		/*
		u2_EV_lincnt = u2_EV_lincnt<<4;
		exp_data[0] = u2_EV_lincnt>>16;
		exp_data[1]	= u2_EV_lincnt>>8;
		exp_data[2] = u2_EV_lincnt;*/
		/*
		exp_data[2] = (u2_EV_lincnt%16)*16;
		exp_data[1]	= ((u2_EV_lincnt-(u2_EV_lincnt%16))/16)%256;
		exp_data[0] = (u2_EV_lincnt-exp_data[2]/16-exp_data[1]*16)/256/16;
		*/
		g_ucShortExpData[0][2] = (u2_EV_lincnt>>12)&0x0f;
		g_ucShortExpData[1][2] = (u2_EV_lincnt>>4)&0xff;
		g_ucShortExpData[2][2] = (u2_EV_lincnt&0x0f)<<4;

		/*
		profLogPrintf(3,"@#@Reg1:%d %d %d",exp_data[0],exp_data[1],exp_data[2]);		
		profLogPrintf(3,"@#@Reg2:%d %d %d",g_ucShortExpData[0][2],g_ucShortExpData[1][2],g_ucShortExpData[2][2]);		
		*/
		#if HDR_debug
		printf("%d %d %d\n",exp_data[0],exp_data[1],exp_data[2]);		
		printf("%d %d %d\n",g_ucShortExpData[0][2],g_ucShortExpData[1][2],g_ucShortExpData[2][2]);		
		#endif
		
		/*
		sensorOVWrite(0x3510,exp_data[0]);
		sensorOVWrite(0x3511,exp_data[1]);
		sensorOVWrite(0x3512,exp_data[2]);
		*/
		#if 0
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,0x3510,&exp_data[0]);
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,0x3511,&exp_data[1]);
		sensorSerialAsyncWrite(sensorTgCfg.cmdportid_exp,0x3512,&exp_data[2]);
		#endif
		
		#endif
	}
}
