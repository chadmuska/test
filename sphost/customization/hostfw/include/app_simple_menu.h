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
#ifndef _APP_SIMPLE_MENU_H_
#define _APP_SIMPLE_MENU_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

#include "app_osd.h"
#include "app_osdfont.h"
#include "app_osd_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define SMENU_ITEM_NUM_TOTAL		7

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	/*-- Dialog Box config */
	UINT8	*pszTitleText;
	UINT32	numItems;
	UINT8	*pszItemText[SMENU_ITEM_NUM_TOTAL];
	UINT32	lastActIndex;
	UINT32	actIndex;

	/*-- Control data */
	UINT32 boxX;
	UINT32 boxY;
	UINT32 boxW;
	UINT32 boxH;
	SINT32 boxY_Offset;

	UINT32 markerIconW;
	//UINT32 optionH;

	UINT32 titleX;
	UINT32 titleY;

	UINT32 itemX[SMENU_ITEM_NUM_TOTAL];
	UINT32 itemY[SMENU_ITEM_NUM_TOTAL];

} simpleMenuInfo_t;

typedef enum {
	SMENU_SEL_YES,
	SMENU_SEL_NO,
} simpleMenuYesNoSelect_t;

typedef enum {
	START_TESTING_BAR_POSITION,
	SCAN_SETTING_BAR_POSITION,
	NORMAL_POSITION,
} testAFSetBarControl_t;

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void stext_Clear(UINT32 x, UINT32 y, UINT8 *pszText);
void stext_Print(UINT32 x, UINT32 y, 	const char *format, ...);
void dbgOsd_Print(UINT32 row, const char *format, ...);

UINT8 SMENU_SelectGet(void);
void SMENU_Scroll(BOOL isUp);
void SMENU_Clear(void);
void SMENU_Show(void);
void SMENU_Config(UINT8 *pszTitle, UINT32 numItems, UINT32 *pszOptText, UINT32 actIndex, SINT32 yOffset);

void SMENU_YesNoConfig(UINT8 *pszTitle);
void SMENU_YesNoShow(void);
void SMENU_YesNoClear(void);
UINT32 SMENU_SelScan(void);

#endif  /* _APP_SIMPLE_MENU_H_ */

