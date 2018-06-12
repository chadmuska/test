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
 **************************************************************************/



#ifndef APP_CALIB_H
#define APP_CALIB_H
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TKTYPE_CALIBMENU_PERPAGE	 5

#define TKTYPE_CALIBMENU_MAIN_STR_BAR_X  23
#define TKTYPE_CALIBMENU_MAIN_STR_BAR_Y  42
#define TKTYPE_CALIBMENU_MAIN_BAR_Y_GAP  34

#define TKTYPE_CALIBMENU_MAIN_STR_X  25
#define TKTYPE_CALIBMENU_MAIN_STR_Y  45
#define TKTYPE_CALIBMENU_MAIN_STR_Y_GAP  34


#define TKTYPE_CALIBMENU_STATUS_STR_X  238



/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct appCalibmenu_s
{
	UINT8 vmTotalTabs;
	UINT8 vmCurTab;
	UINT8 vmLastTab;
	UINT8 vmRecentlyTab;
	UINT8 vmTab[25];
	UINT32 vmEnableTab;
} appCalibmenu_t, *PappCalibmenu_t;



enum CalibMenuItem_e
{
	CALIBMENU_CALIB_INFO			= (unsigned char) 0x00,
	CALIBMENU_OB_CLBT				,
	CALIBMENU_MSHUTTER_CLBT			,
	CALIBMENU_ISO_CLBT				,
	CALIBMENU_ISO_OB_CLBT			,
	CALIBMENU_WB_CLBT				,
	CALIBMENU_VIEW_DPC_CLBT			,
	CALIBMENU_CAPTURE_DPC_CLBT		,
	CALIBMENU_CAM_CONFIG 			, /* Keep TEST_BENCH as the last 2nd item !! */
	CALIBMENU_TEST_BENCH 			, /* Keep TEST_BENCH as the last item !! */
	CALIBMENU_USB_ISP             	,
	CALIBMENU_STAND_ALONE_LSC,			/**/
	CALIBMENU_PV_OB_CAL,
	CALIBMENU_VIDEO_DEBUG_DATESTAMP,
	CALIBMENU_DIGITALZOOM,
	CALIBMENU_DUAL_SENSOR,	
	CALIBMENU_TOTAL
};

typedef enum {
	USB_MODE_NORMAL,  //MSDC, DPS
	USB_MODE_API_TEST,
} appUsbModeSw_e;


typedef struct appCalibMenuItemSelect_s{
	unsigned short gClbtInfo[2];
	unsigned short gOBClbt[2];
	unsigned short gMShutterClbt[2];
	unsigned short gIsoClbt[2];
	unsigned short gIsoObClbt[2];
	unsigned short gWBClbt[2];
	unsigned short gLightSSClbt[2];
	unsigned short gDarkDPCClbt[2];
	unsigned short gCamCfg[2];
	unsigned short gTestBench[2]; /* TEST_BENCH, Keep it the last one !! */
	unsigned short gStandAloneLsc[2];
	unsigned short gPvOBCal[2];
	unsigned short gVideoRecLog[2];
	unsigned short gDzoomEnableLog[2];
	unsigned short gDualSensor[2];	
	unsigned short gNullClbt[2];
}appCalibMenuItemSelect_t, *PappCalibMenuItemSelect_s;


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/


/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/


/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

void appCalibMenu_TKSubItemDetailStr(unsigned char nCurTab, unsigned short **pStrId);
void appCalibMenu_MainMenuCreate(void);
void appCalibMenu_TKMainMenuDraw(void);




#endif  /* APP_CALIB_H */


