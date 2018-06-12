/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/

#include "app_com_def.h"
#include "app_com_api.h"

#include "app_menu.h"
#include "app_menu_event.h"
#include "app_menu_core.h"
#include "app_menu_tab.h"
#include "app_menu_disp.h"
#include "app_menu_disp1.h"
#include "app_ui_para.h"
#include "app_osd_api.h"
#include "app_icon_def.h"
#include "app_res_def.h"
#include "app_pal.h"
#include "gpio_custom.h"
#include "sp5k_gfx_api.h"
#include "sp5k_pip_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_playback_osd.h"
#include "../app_ver.h"
#if FUNC_HOST_TOUCH_PANEL
#include "app_ts_util.h"
#endif
#define ROUND_DOWN_TO_DIVISIBLE(num,div) \
	( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) \
	ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#if HDMI_MENU_BY_JPG
typedef struct menuSrcHDMI_s{
	UINT8* fileName;
	UINT32 pos[4];  /* X pos,Y pos, width ,height  */
}menuSrcHDMI_t;

enum{
	HDMI_PIP_MAIN		=	SP5K_GFX_PAGE_PIP_MAIN,
	HDMI_MENU_LAYER0	=	SP5K_GFX_PAGE_PIP_BASE+0,
	HDMI_MENU_LAYER1	=	SP5K_GFX_PAGE_PIP_BASE+1,
	HDMI_MENU_LAYER2	=	SP5K_GFX_PAGE_PIP_BASE+2,
	HDMI_MENU_LAYER3	=	SP5K_GFX_PAGE_PIP_BASE+3,
	HDMI_MENU_LAYER4	=	SP5K_GFX_PAGE_PIP_BASE+4
};

enum {
	/* reference the order of ID_ICONFILE_MODE_HDMI -- s */
	MENU_STILL_HDMI,
	MENU_VIDEO_HDMI,
	MENU_AUDIO_HDMI,
	MENU_PLAYBACK_HDMI,
	MENU_SETUP_HDMI,
	MENU_PRINTER_HDMI,
	/* reference the order of ID_ICONFILE_MODE_HDMI -- e */
	MENU_SCENE_HDMI,

	MENU_TITLE_POS1_HDMI,
	MENU_TITLE_POS2_HDMI,
	MENU_ADJUST_LEFT_HDMI,
	MENU_ADJUST_RIGHT_HDMI,
	MENU_BUTTON_OK_HDMI,
	MENU_BUTTON_MENU_HDMI,

	MENU_BODY_HDMI,
	MENU_FIVE_LINES_BAR_HDMI,
	MENU_FOUR_LINES_BODY_HDMI,
	MENU_FOUR_LINES_BAR_HDMI,
	MENU_THREE_LINES_BODY_HDMI,
	MENU_THREE_LINES_BAR_HDMI,
	MENU_TWO_LINES_BODY_HDMI,

	MENU_FIVE_LINES_SCROLL_HDMI,
	MENU_FOUR_LINES_SCROLL_HDMI,

	MENU_ADJUST_DIALOG_HDMI,
	MENU_SOUND_ADJUST_1_HDMI,
	MENU_SOUND_ADJUST_2_HDMI
};

static menuSrcHDMI_t menuSrcFileInfo[] = {
	[MENU_STILL_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\STILL.JPG",			{140,25,166,150}},
	[MENU_VIDEO_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\VIDEO.JPG",			{140,25,166,150}},
	[MENU_AUDIO_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\AUDIO.JPG",			{140,25,166,150}},
	[MENU_PLAYBACK_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\PLAYBACK.JPG",		{140,25,166,150}},
	[MENU_SETUP_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\SETUP.JPG",			{420,25,166,150}},
	[MENU_PRINTER_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\PRINTER.JPG",		{140,25,166,150}},
	[MENU_SCENE_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\SCENE.JPG",			{140,25,166,150}},

	[MENU_TITLE_POS1_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\TITLE.JPG",			{20,5,412,220}},
	[MENU_TITLE_POS2_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\TITLE.JPG",			{300,5,412,220}},
	[MENU_ADJUST_LEFT_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\ADJUST_LEFT.JPG",	{1200,325,42,76}},
	[MENU_ADJUST_RIGHT_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\ADJUST_RIGHT.JPG",	{1456,325,42,76}},
	[MENU_BUTTON_OK_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\BUTTON_OK.JPG",		{150,985,197,80}},
	[MENU_BUTTON_MENU_HDMI]			=	{"A:\\RO_RES\\UI\\JPG\\BUTTON_MENU.JPG",	{1400,985,198,80}},

	[MENU_BODY_HDMI]				=	{"A:\\RO_RES\\UI\\JPG\\MENU_BODY.JPG",		{0,0,1920,1080}},
	[MENU_FIVE_LINES_BAR_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\FIVE_LINES_BAR.JPG",	{0,220,1858,140}},
	[MENU_FOUR_LINES_BODY_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\FOUR_LINES_BODY.JPG",{200,295,1618,592}},
	[MENU_FOUR_LINES_BAR_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\FOUR_LINES_BAR.JPG",	{214,307,1534,134}},
	[MENU_THREE_LINES_BODY_HDMI]	=	{"A:\\RO_RES\\UI\\JPG\\THREE_LINES_BODY.JPG",{180,360,1616,588}},
	[MENU_THREE_LINES_BAR_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\THREE_LINES_BAR.JPG",{0,0,1593,138}},
	[MENU_TWO_LINES_BODY_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\TWO_LINES_BODY.JPG",	{150,510,1618,448}},

	[MENU_FIVE_LINES_SCROLL_HDMI]	=	{"A:\\RO_RES\\UI\\JPG\\FIVE_LINES_SCROLL.JPG",{0,0,22,90}},
	[MENU_FOUR_LINES_SCROLL_HDMI]	=	{"A:\\RO_RES\\UI\\JPG\\FOUR_LINES_SCROLL.JPG",{0,0,21,90}},

	[MENU_ADJUST_DIALOG_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\ADJUST_DIALOG.JPG",	{1244,320,208,86}},
	[MENU_SOUND_ADJUST_1_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\SOUND_ADJUST_1.JPG",	{0,0,324,26}},
	[MENU_SOUND_ADJUST_2_HDMI]		=	{"A:\\RO_RES\\UI\\JPG\\SOUND_ADJUST_2.JPG",	{0,0,38,64}},
};

enum{
	PRELOAD_MODE_STILL			=	0,
	PRELOAD_MODE_VIDEO			=	1,
	PRELOAD_MODE_AUDIO			=	2,
	PRELOAD_MODE_PLAYBACK		=	3,
	PRELOAD_MODE_SETUP			=	4,
	PRELOAD_MODE_PRINTER		=	5,

	PRELOAD_MENU_BODY			=	6,
	PRELOAD_FIVE_LINES_BAR		=	7,
	PRELOAD_FOUR_LINES_BODY		=	8,
	PRELOAD_FOUR_LINES_BAR		=	9,

	PRELOAD_BUTTON_OK			=	10,
	PRELOAD_BUTTON_MENU			=	11,
	PRELOAD_MENU_TITLE			=	12
};
#endif

typedef enum {
	ICON_ID_STILL = 0,
	ICON_ID_VIDEO = 1,
	ICON_ID_AUDIO = 2,
	ICON_ID_PLAYBACK,
	ICON_ID_SETUP,
	ICON_ID_DPS
} modeIconID_t;

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define WIDTH_ICON_MUSUSBAR			256
#define WIDTH_ICON_MUSBAR			237
#define WIDTH_ICON_MUDLGBAR			296

#define WIDTH_ICON_MODE				40
#define HEIGHT_ICON_MODE			32

#define WIDTH_ICON_MUSUSBAR_HDMI	WIDTH_ICON_MUSUSBAR
#define WIDTH_ICON_MUBAR_HDMI		WIDTH_ICON_MUSBAR

static UINT16 MENU_HEADER_TAB1_X;
static UINT16 MENU_HEADER_TAB2_X;
static UINT16 MENU_HEADER_TAB_Y;
static UINT16 MENU_HEADER_MODE_X;
static UINT16 MENU_HEADER_MODE_Y;
static UINT16 MENU_HEADER_SET_X;
static UINT16 MENU_HEADER_SET_Y;

//#define MENU_MODE_X_ODDSET
static UINT16 MENU_MODE_Y_OFFSET;// 4

UINT16 MENU_CHAR_WIDTH;//  16
UINT16 MENU_CHAR_HEIGHT;// 30//31

static UINT16 MENU_LAYER1_STR_X_OFFSET;// 32
static UINT16 MENU_LAYER1_STR_Y_OFFSET;// 		(43+MENU_MODE_Y_OFFSET)//44
static SINT16 MENU_LAYER1_ICON_X_OFFSET;// (-16)
static UINT16 MENU_LAYER1_ICON_Y_OFFSET;// (43+MENU_MODE_Y_OFFSET)//44
static UINT16 MENU_LAYER2_STR_X_OFFSET;// 56
static UINT16 MENU_LAYER2_STR_Y_OFFSET;// 60
static UINT16 MENU_LAYER2_ICON_X_OFFSET;// 4
static UINT16 MENU_LAYER2_ICON_Y_OFFSET;// 60

static UINT16 MENU_BAR_LAYER1_X_OFFSET;// 24
static UINT16 MENU_BAR_LAYER1_Y_OFFSET;// (43+MENU_MODE_Y_OFFSET-3)//44
static UINT16 MENU_BAR_LAYER2_X_OFFSET;// 48
static UINT16 MENU_BAR_LAYER2_Y_OFFSET;// 61-3
static UINT16 MENU_BAR_LAYER3_X_OFFSET;// 32
static UINT16 MENU_BAR_LAYER3_Y_OFFSET;// 44

static UINT16 MENU_BODY_LAYER1_X_OFFSET;// 16
static UINT16 MENU_BODY_LAYER1_Y_OFFSET;// (33+MENU_MODE_Y_OFFSET)//34
static UINT16 MENU_BODY_LAYER2_X_OFFSET;// 40
static UINT16 MENU_BODY_LAYER2_Y_OFFSET;// 47//49
static UINT16 MENU_BODY_LAYER3_X_OFFSET;// 14
static UINT16 MENU_BODY_LAYER3_Y_OFFSET;// 96-3

static UINT16 MENU_OFFSET;
static UINT16 MENU_MU2BARSL_WIDTH;
static UINT16 MENU_HEADER_OFFSET;
static UINT16 MENU_BACK_ON_OFFSET;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 menuPosBackup;
UINT16 LCD_WIDTH;// ((UINT16)320)
UINT16 LCD_HEIGHT;// ((UINT16)240)
#if HDMI_MENU_BY_JPG
static UINT32 dlgUpdateFlag = 0;
extern UINT32 pipHDMIMenuFlag;
#endif
#if CAM_TYPE_CVR||CAM_TYPE_SPC||CAM_TYPE_DUAL||CAM_TYPE_SBC  /*add for system state*/
static UINT32 strId_Reset[]={ID_STR_YES,ID_STR_NO};
#endif
#if CAM_TYPE_CVR||CAM_TYPE_SPC||CAM_TYPE_DUAL||CAM_TYPE_SBC   /*add for system state*/
UINT32 RTCTimer=TIMER_NULL;
UINT32 GSENSORTimercount;
#endif
extern UINT8 fileTypeSelect;
extern UINT8 USBModeSet;
#if CAM_TYPE_SPC||CAM_TYPE_CVR||CAM_TYPE_DUAL||CAM_TYPE_SBC
UINT8 verStr1[16];
UINT8 verStr2[16];
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//"YYYY/MM/DD",
//"MM/DD/YYYY",
//"DD/MM/YYYY",
UINT8 rtc_disp[3][6]={{0, 1, 2, 3, 4, 5}, {1, 2, 0, 3, 4, 5}, {2, 1, 0, 3, 4, 5}};
//UINT8 code exposure[]="\x7e\x7f\x7f\x7e\x7f\x7f\x7e\x7f\x7f\x7e\x7f\x7f\x7e";

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void _setupMenuFunctionResetAll(void);
#if GPS_SUPPORT
#if CAM_TYPE_CVR||CAM_TYPE_DUAL||CAM_TYPE_SBC  /*add for system state*/
extern void appGpsGsvInfoUpdate(void);
#endif
#endif

/**
 * @fn        void appOsdLayoutAttrGet(UINT8 *hNum, UINT8 *vNum)
 * @brief     appOsdLayoutAttrGet
 * @param     [in] hNum
 * @param     [in] vNum
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-21
 * @todo      N/A
 * @bug       N/A
*/
void
appOsdLayoutAttrGet(
	UINT8 *hNum,
	UINT8 *vNum
)
{
	if (hNum) *hNum = 20;
	if (vNum) *vNum = 7;
}

/**
 * @fn        void appOsdFontSizeGet(UINT16 *w, UINT16 *h)
 * @brief     appOsdFontSizeGet
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-21
 * @todo      N/A
 * @bug       N/A
*/
void
appOsdFontSizeGet(
	UINT32 *w,
	UINT32 *h
)
{
	/* TODO */
#if 0
	sp5kGfxFontSizeGet(1, w, h);
	printf("w:%d,h:%d\n", *w, *h);
#else
	if (w) *w = 36;
	if (h) *h = 32;
#endif
}

UINT16 appMenuDispParaGet(UINT32 paraIdx)
{
	UINT16 menuPara;
	switch(paraIdx)
	{
		case IDX_MENU_CHAR_WIDTH:
			menuPara = MENU_CHAR_WIDTH;
			break;
		case IDX_MENU_CHAR_HEIGHT:
			menuPara = MENU_CHAR_HEIGHT;
			break;
		case IDX_MENU_BAR_LAYER2_X_OFFSET:
			menuPara = MENU_BAR_LAYER2_X_OFFSET;
			break;
		case IDX_MENU_BAR_LAYER2_Y_OFFSET:
			menuPara = MENU_BAR_LAYER2_Y_OFFSET;
			break;
		case IDX_MENU_MU2BARSL_WIDTH:
			menuPara = MENU_MU2BARSL_WIDTH;
			break;
		default:
			menuPara = MENU_CHAR_WIDTH;
			break;
	}
	return menuPara;
}

void menuDispParaInit_TV(UINT32 charw,UINT32 charh)
{
    switch(charh){
    case 32:
        LCD_WIDTH = 320;
        LCD_HEIGHT = 240;
        //printf("font 32\n");

        MENU_HEADER_TAB1_X = 0;
        MENU_HEADER_TAB2_X = 60;
        MENU_HEADER_TAB_Y = 0;
        MENU_HEADER_MODE_X = 20;
        MENU_HEADER_MODE_Y = 6;
        MENU_HEADER_SET_X = 80;
        MENU_HEADER_SET_Y = 6;

        MENU_MODE_Y_OFFSET = 4;

        MENU_CHAR_WIDTH = 16;
        MENU_CHAR_HEIGHT = 34;

        MENU_LAYER1_STR_X_OFFSET = 32;
        MENU_LAYER1_STR_Y_OFFSET = (37+MENU_MODE_Y_OFFSET);
	#if FUNC_HOST_TOUCH_PANEL
        MENU_LAYER1_ICON_X_OFFSET =0 -MENU_CHAR_WIDTH*2;
	#else
        MENU_LAYER1_ICON_X_OFFSET = 0;
	#endif
        MENU_LAYER1_ICON_Y_OFFSET = (43+MENU_MODE_Y_OFFSET);
        MENU_LAYER2_STR_X_OFFSET = 56;
        MENU_LAYER2_STR_Y_OFFSET = 59;
        MENU_LAYER2_ICON_X_OFFSET = 4;
        MENU_LAYER2_ICON_Y_OFFSET = 63;

        MENU_BAR_LAYER1_X_OFFSET = 24;
        MENU_BAR_LAYER1_Y_OFFSET = (40+MENU_MODE_Y_OFFSET-3);//44
        MENU_BAR_LAYER2_X_OFFSET = 45;
        MENU_BAR_LAYER2_Y_OFFSET = 60;
        MENU_BAR_LAYER3_X_OFFSET = 32;
        MENU_BAR_LAYER3_Y_OFFSET = 44;

        MENU_BODY_LAYER1_X_OFFSET = 16;
        MENU_BODY_LAYER1_Y_OFFSET = (33+MENU_MODE_Y_OFFSET);//34
        MENU_BODY_LAYER2_X_OFFSET = 40;
        MENU_BODY_LAYER2_Y_OFFSET = 57;//49
        MENU_BODY_LAYER3_X_OFFSET = 14;
        MENU_BODY_LAYER3_Y_OFFSET = 96-3;

        MENU_OFFSET = 12;
        MENU_MU2BARSL_WIDTH = 66;
        MENU_HEADER_OFFSET = 6;
        MENU_BACK_ON_OFFSET = 3;
        break;
    case 24:
        LCD_WIDTH = 220;
        LCD_HEIGHT = 176;
        //printf("font 24\n");

        MENU_HEADER_TAB1_X = 0;
        MENU_HEADER_TAB2_X = 60;
        MENU_HEADER_TAB_Y = 0;
        MENU_HEADER_MODE_X = 20;
        MENU_HEADER_MODE_Y = 6;
        MENU_HEADER_SET_X = 80;
        MENU_HEADER_SET_Y = 6;

        MENU_MODE_Y_OFFSET = 2;

        MENU_CHAR_WIDTH = 12;
        MENU_CHAR_HEIGHT = 24;//31

        MENU_LAYER1_STR_X_OFFSET = 24;
        MENU_LAYER1_STR_Y_OFFSET = (28+MENU_MODE_Y_OFFSET);
	#if FUNC_HOST_TOUCH_PANEL
        MENU_LAYER1_ICON_X_OFFSET = (-8)-MENU_CHAR_WIDTH*2;
	#else
        MENU_LAYER1_ICON_X_OFFSET = (-8);
	#endif
        MENU_LAYER1_ICON_Y_OFFSET = (30+MENU_MODE_Y_OFFSET);
        MENU_LAYER2_STR_X_OFFSET = 41;
        MENU_LAYER2_STR_Y_OFFSET = 46;
        MENU_LAYER2_ICON_X_OFFSET = 0;
        MENU_LAYER2_ICON_Y_OFFSET = 47;

        MENU_BAR_LAYER1_X_OFFSET = 16;
        MENU_BAR_LAYER1_Y_OFFSET = (32+MENU_MODE_Y_OFFSET-3);
        MENU_BAR_LAYER2_X_OFFSET = 33;
        MENU_BAR_LAYER2_Y_OFFSET = 46;
        MENU_BAR_LAYER3_X_OFFSET = 21;
        MENU_BAR_LAYER3_Y_OFFSET = 35;

        MENU_BODY_LAYER1_X_OFFSET = 8;
        MENU_BODY_LAYER1_Y_OFFSET = (23+MENU_MODE_Y_OFFSET);
        MENU_BODY_LAYER2_X_OFFSET = 25;
        MENU_BODY_LAYER2_Y_OFFSET = 37;
        MENU_BODY_LAYER3_X_OFFSET = 10;
        MENU_BODY_LAYER3_Y_OFFSET = 73;

        MENU_OFFSET = 0;
        MENU_MU2BARSL_WIDTH = 43;
        MENU_HEADER_OFFSET = 6;
        MENU_BACK_ON_OFFSET = 2;
        break;
    case 16:
        LCD_WIDTH = 160;
        LCD_HEIGHT = 128;
        //printf("font 16\n");

        MENU_HEADER_TAB1_X = 0;
        MENU_HEADER_TAB2_X = 60;
        MENU_HEADER_TAB_Y = 0;
        MENU_HEADER_MODE_X = 20;
        MENU_HEADER_MODE_Y = 6;
        MENU_HEADER_SET_X = 80;
        MENU_HEADER_SET_Y = 6;

        MENU_MODE_Y_OFFSET = 4;

        MENU_CHAR_WIDTH = 8;
        MENU_CHAR_HEIGHT = 16;

        MENU_LAYER1_STR_X_OFFSET = 22;
        MENU_LAYER1_STR_Y_OFFSET = (21+MENU_MODE_Y_OFFSET);
	#if FUNC_HOST_TOUCH_PANEL
        MENU_LAYER1_ICON_X_OFFSET = 4-MENU_CHAR_WIDTH*2;
	#else
        MENU_LAYER1_ICON_X_OFFSET = 4;
	#endif
        MENU_LAYER1_ICON_Y_OFFSET = (21+MENU_MODE_Y_OFFSET);
        MENU_LAYER2_STR_X_OFFSET = 37;
        MENU_LAYER2_STR_Y_OFFSET = 32;
        MENU_LAYER2_ICON_X_OFFSET = 4;
        MENU_LAYER2_ICON_Y_OFFSET = 32;

        MENU_BAR_LAYER1_X_OFFSET = 16;
        MENU_BAR_LAYER1_Y_OFFSET = (24+MENU_MODE_Y_OFFSET-3);
        MENU_BAR_LAYER2_X_OFFSET = 28;
        MENU_BAR_LAYER2_Y_OFFSET = 32;
        MENU_BAR_LAYER3_X_OFFSET = 20;
        MENU_BAR_LAYER3_Y_OFFSET = 27;

        MENU_BODY_LAYER1_X_OFFSET = 10;
        MENU_BODY_LAYER1_Y_OFFSET = (16+MENU_MODE_Y_OFFSET);
        MENU_BODY_LAYER2_X_OFFSET = 20;
        MENU_BODY_LAYER2_Y_OFFSET = 24;
        MENU_BODY_LAYER3_X_OFFSET = 12;
        MENU_BODY_LAYER3_Y_OFFSET = 33;

        MENU_OFFSET = 10;
        MENU_MU2BARSL_WIDTH = 38;
        MENU_HEADER_OFFSET = 3;
        MENU_BACK_ON_OFFSET = 2;
        break;
    }

}

void menuDispParaInit_HDMI(UINT32 charw,UINT32 charh)
{
    LCD_WIDTH = 320;
    LCD_HEIGHT = 240;

    MENU_HEADER_TAB1_X = 0;
    MENU_HEADER_TAB2_X = 60;
    MENU_HEADER_TAB_Y = 0;
    MENU_HEADER_MODE_X = 20;
    MENU_HEADER_MODE_Y = 6;
    MENU_HEADER_SET_X = 80;
    MENU_HEADER_SET_Y = 6;

    MENU_MODE_Y_OFFSET = 4;

    MENU_CHAR_WIDTH = 16;
    MENU_CHAR_HEIGHT = 34;

    MENU_LAYER1_STR_X_OFFSET = 32;
    MENU_LAYER1_STR_Y_OFFSET = 46;

#if FUNC_HOST_TOUCH_PANEL
    MENU_LAYER1_ICON_X_OFFSET = 0-MENU_CHAR_WIDTH*2;
#else
    MENU_LAYER1_ICON_X_OFFSET = 0;
#endif
    MENU_LAYER1_ICON_Y_OFFSET = 46;
    MENU_LAYER2_STR_X_OFFSET = 56;
    MENU_LAYER2_STR_Y_OFFSET = 59;
    MENU_LAYER2_ICON_X_OFFSET = 4;
    MENU_LAYER2_ICON_Y_OFFSET = 63;

    MENU_BAR_LAYER1_X_OFFSET = 24;       /*the first start position*/
    MENU_BAR_LAYER1_Y_OFFSET = 46;       /*the first start position*/
    MENU_BAR_LAYER2_X_OFFSET = 35;
    MENU_BAR_LAYER2_Y_OFFSET = 60;
    MENU_BAR_LAYER3_X_OFFSET = 16;
    MENU_BAR_LAYER3_Y_OFFSET = 33;

    MENU_BODY_LAYER1_X_OFFSET = 16;
    MENU_BODY_LAYER1_Y_OFFSET = (33+MENU_MODE_Y_OFFSET);
    MENU_BODY_LAYER2_X_OFFSET = 33;
    MENU_BODY_LAYER2_Y_OFFSET = 57;
    MENU_BODY_LAYER3_X_OFFSET = 14;
    MENU_BODY_LAYER3_Y_OFFSET = 96-3;

    MENU_OFFSET = 6;
    MENU_MU2BARSL_WIDTH = 66;
    MENU_HEADER_OFFSET = 6;
    MENU_BACK_ON_OFFSET = 3;

}

void menuDispParaInit_ELSE(UINT32 charw,UINT32 charh)
{
        LCD_WIDTH = 320;
        LCD_HEIGHT = 240;

        MENU_HEADER_TAB1_X = 0;
        MENU_HEADER_TAB2_X = 60;
        MENU_HEADER_TAB_Y = 0;
        MENU_HEADER_MODE_X = 20;
        MENU_HEADER_MODE_Y = 6;
        MENU_HEADER_SET_X = 80;
        MENU_HEADER_SET_Y = 6;

        MENU_MODE_Y_OFFSET = 4;

        MENU_CHAR_WIDTH = 16;
        MENU_CHAR_HEIGHT = 34;

        MENU_LAYER1_STR_X_OFFSET = 32;
        MENU_LAYER1_STR_Y_OFFSET = 41;

#if FUNC_HOST_TOUCH_PANEL
        MENU_LAYER1_ICON_X_OFFSET = 0-MENU_CHAR_WIDTH*2;
#else
        MENU_LAYER1_ICON_X_OFFSET = 0;
#endif
        MENU_LAYER1_ICON_Y_OFFSET = 45;
        MENU_LAYER2_STR_X_OFFSET = 56;
        MENU_LAYER2_STR_Y_OFFSET = 59;
        MENU_LAYER2_ICON_X_OFFSET = 4;
        MENU_LAYER2_ICON_Y_OFFSET = 63;

        MENU_BAR_LAYER1_X_OFFSET = 24;
        MENU_BAR_LAYER1_Y_OFFSET = 41;
        MENU_BAR_LAYER2_X_OFFSET = 35;
        MENU_BAR_LAYER2_Y_OFFSET = 60;
        MENU_BAR_LAYER3_X_OFFSET = 16;
        MENU_BAR_LAYER3_Y_OFFSET = 33;

        MENU_BODY_LAYER1_X_OFFSET = 16;
        MENU_BODY_LAYER1_Y_OFFSET = (33+MENU_MODE_Y_OFFSET);
        MENU_BODY_LAYER2_X_OFFSET = 33;
        MENU_BODY_LAYER2_Y_OFFSET = 57;
        MENU_BODY_LAYER3_X_OFFSET = 14;
        MENU_BODY_LAYER3_Y_OFFSET = 96-3;

        MENU_OFFSET = 6;
        MENU_MU2BARSL_WIDTH = 66;
        MENU_HEADER_OFFSET = 6;
        MENU_BACK_ON_OFFSET = 3;

}

void menuDispParaInit(void)
{
	UINT32 charw,charh;

	appOsdFontSizeGet(&charw,&charh);
#if (KIT_WITH_HDMI)
    printf("[menu]IS_HDMI_IN=%d\n",IS_HDMI_IN);
#endif

	if(IS_TV_IN){
        menuDispParaInit_TV(charw,charh);
	}
#if (KIT_WITH_HDMI)
	else if(IS_HDMI_IN){
        menuDispParaInit_HDMI(charw,charh);
    }
#endif
    else{
        menuDispParaInit_ELSE(charw,charh);
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuLanguageGet                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuLanguageGet(void)
{
	uiPara_t* puiPara = appUiParaGet();

	return puiPara->Language;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuDateItemConv                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuDateItemConv(UINT8 item)
{
	uiPara_t* puiPara = appUiParaGet();

	return rtc_disp[puiPara->DateStyle][item];
}



/**************************************************************************
 *                                                                        *
 *  Function Name: menuLayerSizeGet                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *0
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuLayerSizeGet(UINT8 layer, UINT8* line, UINT8* col)
{
	UINT8 OsdMaxX, OsdMaxY;
	UINT32 cw,ch;

	appOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	appOsdFontSizeGet(&cw, &ch);
	if(ch <= 16)OsdMaxY-=1;

	if (layer != 2)
	{
		*col = OsdMaxX - 2;
		*line = OsdMaxY - 2;
	}
	else
	{
		*col = OsdMaxX-5;
		*line = OsdMaxY-3;
	}

	if(menuProcCurrRootGet() == MENU_MARK_SCENEMODE)
	{
		*line = 1;
	}
}

void appOsdColorSet(UINT8 attr)
{
#if 0
	attr = attr;
#else
	COLOR_DEF enColor;
	COLOR_DEF enShadowColor;
	COLOR_DEF enBkColor;
	BLEND_TYPE enBkBlend;

	switch (attr)
	{
		case MENU_STR_GET_FOCUS:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_NOCOLOR;
			enColor = PAL_WHITE;
			break;
		case MENU_STR_LOST_FOCUS:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_BLACK;
			enColor = PAL_WHITE;
			break;
		case SUB_MENU_STR_GET_FOCUS:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_NOCOLOR;
			enColor = PAL_BLACK;
			break;
		case SUB_MENU_STR_LOST_FOCUS:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_NOCOLOR;
			enColor = PAL_BLACK;
			break;
		case MENU_STR_DISABLE:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_NOCOLOR;////PAL_GRAY_2_OLD;
			enColor = PAL_GRAY_4;//PAL_GRAY_5;
			break;
		default:
			enBkBlend = PAL_BLEND_100;
			enBkColor = PAL_NOCOLOR;
			enShadowColor = PAL_BLACK;
			enColor = PAL_WHITE;
			break;
	}

	appOsd_ColorDrawTextColorSet(enColor, enShadowColor, enBkColor, enBkBlend);
#endif
}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuStrDisp                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuStrDisp(UINT8 x, UINT8 y, UINT8* str, UINT8 blendOpt, UINT8 layer)
{
	UINT16 offset = x*MENU_CHAR_WIDTH;

//	blendOpt = blendOpt;

	if(layer == 2)
	{
        appOsdLib_TextPIPDraw(offset+MENU_LAYER2_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, str);
	}
	else
	{
       appOsdLib_TextPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, str);
	}

	return strlen((char *)str);
}

UINT32 menuStrDraw(UINT32 x, UINT32 y, UINT32 strIdx, UINT32 blendOpt, UINT32 layer)
{
    UINT32 offset = x*MENU_CHAR_WIDTH;

//    blendOpt = blendOpt;

    if(layer == 2)
    {
        #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
        if(IS_HDMI_IN){
            appOsdLib_TextIDPIPDraw(offset+MENU_LAYER2_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
        }
        else
		#endif
		{

            if (menuProcCurrRootGet() == MENU_MARK_CALIBRATION)
            {
                appOsdLib_TextIDPIPDraw(offset-20, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
            }
            else
            {
                appOsdLib_TextIDPIPDraw(offset+MENU_LAYER2_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
            }
        }
    }
    else
    {
        #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
        if(IS_HDMI_IN){
            if(layer == 1){
                appOsdLib_TextIDPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
            }
            else{
                appOsdLib_TextIDPIPDraw(offset+MENU_LAYER2_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET-40, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
            }
        }
        else
        #endif
        {

            #if CAM_TYPE_SPC  /*modify by YJ*/
                if(USBModeSet)
                {
                    appOsdLib_TextIDDraw(165, 120, SP5K_GFX_ALIGN_CENTER, ID_STR_USB_MODE);
                    appOsdLib_TextIDPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET+65, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);

                }
                else if(fileTypeSelect == 1)
                {


                    appOsdLib_TextIDPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET-20, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
                }
            #else

                if((fileTypeSelect == 1) || USBModeSet )
                {
/*modify by Isaac*/
					#if	defined(PRJ_JETDVR) || defined(PRJ_PDVR)
                    			appOsdLib_TextIDPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
					#else
					appOsdLib_TextIDPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET-20, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
					#endif
                }
            #endif
                else
                {
                    appOsdLib_TextIDPIPDraw(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
                }
        }
    }

	return SUCCESS;
}

void menuStrDispExt(UINT16 x, UINT16 y, UINT8* str, UINT8 blendOpt)
{
	appOsdLib_TextDraw(x, y, SP5K_GFX_ALIGN_TOP_LEFT, str);
}

void menuBarDisp(UINT8 line, UINT8 layer)
{
	UINT8 attr = MENU_BAR_GET_FOCUS;
	UINT8 *pIconFile = NULL;
	SINT32 x = 0, y = 0;
	UINT8 nodeItem = 0;
	#if HDMI_MENU_BY_JPG
	//UINT32 p[4];
	#endif

	nodeItem = menuProcNodeItemGet(layer, 0);
	if ((layer == 1) && (menuProcLayerGet() == 2)) {
		attr = MENU_BAR_DISABLE;
	}

	appOsdColorSet(attr);

	#if KIT_WITH_HDMI && HDMI_MENU_BY_JPG
	if(IS_HDMI_IN){
		#if 0
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
		appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+0, 0, 220,1920,955);
		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
		#endif
	}
	#endif

	if(layer == 2){
		#if 0
		if( menuProcSubItemGet(layer,menuProcScrollGet(layer))){
			pIconFile = ID_ICONFILE_MUSUSBAR;
		} else {
			pIconFile = ID_ICONFILE_MUSBAR;
		}
		#endif

		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			#if HDMI_MENU_BY_JPG
			UINT32 p[4];
			p[0] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[0];
			p[1] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[1]+line*145;
			p[2] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2];
			p[3] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3];

			sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BAR, HDMI_MENU_LAYER1, p, 0);
			#else
			x = MENU_BAR_LAYER2_X_OFFSET;
			y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET;
			pIconFile = ID_ICONFILE_MUSUSBAR_HDMI;
			#endif
		}
		else
        #endif
        {

			x = MENU_BAR_LAYER2_X_OFFSET;
			y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET;
			pIconFile = (UINT8 *)ID_ICONFILE_MUSUSBAR;

		}
	}
	else if(layer == 3)
	{
		if(nodeItem>2)
		{
		    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN)
			{
				#if HDMI_MENU_BY_JPG
				UINT32 p[4];
				p[0] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[0];
				p[1] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[1]+line*145;
				p[2] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2];
				p[3] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3];

				sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BAR, HDMI_MENU_LAYER1, p, 0);
				#else
#if 0
				x = MENU_BAR_LAYER2_X_OFFSET*2;
				y = (UINT16)line*(MENU_CHAR_HEIGHT+14)+MENU_BAR_LAYER2_Y_OFFSET+13;
#else
				if((fileTypeSelect == 1) || USBModeSet)
				{
					x = MENU_BAR_LAYER2_X_OFFSET-5;
					y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET-40;
				}
				else
				{
					x = MENU_BAR_LAYER2_X_OFFSET;
					y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET-17;
				}
#endif
				pIconFile = ID_ICONFILE_MUSUSBAR_HDMI;
				#endif
			}
			else
            #endif
			{
				if((fileTypeSelect == 1) || USBModeSet)
				{
					x = MENU_BAR_LAYER2_X_OFFSET-5;
					y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET-40;
				}
				else
				{
					x = MENU_BAR_LAYER2_X_OFFSET;
					y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET-17;
				}
				pIconFile = (UINT8 *)ID_ICONFILE_MUSUSBAR;

			}
		}
		else
		{
			pIconFile = ID_ICONFILE_MUDLGBAR;

		#if CAM_TYPE_SPC
			if( USBModeSet)
			{
				x = MENU_BAR_LAYER2_X_OFFSET-18;
				y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET+42;

			}
			else
			{
				x = MENU_BAR_LAYER3_X_OFFSET;
				y = (UINT16)line*MENU_CHAR_HEIGHT+5+MENU_BAR_LAYER3_Y_OFFSET;
			}

		#else
				x = MENU_BAR_LAYER3_X_OFFSET;
				y = (UINT16)line*MENU_CHAR_HEIGHT+5+MENU_BAR_LAYER3_Y_OFFSET;
		#endif

		}
	}
	else{

	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN)
		{
			#if HDMI_MENU_BY_JPG
			UINT32 p[4];
			p[0] = menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[0];
			p[1] = menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[1]+line*147;
			p[2] = menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[2];
			p[3] = menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[3];

			sp5kRsvPreloadDraw(PRELOAD_FIVE_LINES_BAR, HDMI_MENU_LAYER3, p, 0);
			#else
			x = 0;
			y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER1_Y_OFFSET;
			pIconFile = ID_ICONFILE_MUBAR_HDMI;
			#endif
		}
		else
    #endif
		{
			x = 0;
			y = (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER1_Y_OFFSET+1;
			pIconFile = ID_ICONFILE_MUBAR;
		}
	}

	if(menuProcCurrRootGet() == MENU_MARK_SCENEMODE)
	{
		pIconFile = NULL;
	}

	#if HDMI_MENU_BY_JPG
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(!IS_HDMI_IN){
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, x, y, SP5K_GFX_ALIGN_TOP_LEFT, pIconFile, 0x0000);
	}
	#endif
	#else
	if(pIconFile == (UINT8 *)ID_ICONFILE_MUSUSBAR)
	{
		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_3);
		appOsdLib_PIPLayerIconDraw(x, y, SP5K_GFX_ALIGN_TOP_LEFT, 0x0000);
	}
	else
	{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, x, y, SP5K_GFX_ALIGN_TOP_LEFT, pIconFile, 0x0000);
	}
	#endif

}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuIconDisp                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuIconDisp(UINT8 x, UINT8 y, UINT16 icon, UINT8 blendOpt, UINT8 layer)
{
	SINT32 posX, posY;

	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
	if (icon == ID_ICON_NONE) {
		printf("icon none\n");
		return;
	}

//	blendOpt = blendOpt;

	if(layer == 2)
	{
        posX = (UINT16)x*MENU_CHAR_WIDTH+MENU_LAYER2_ICON_X_OFFSET;
		posY = (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_ICON_Y_OFFSET;
	}
	else
	{
        posX = (UINT16)(x-1)*MENU_CHAR_WIDTH+MENU_LAYER1_ICON_X_OFFSET;
        posY = (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_ICON_Y_OFFSET;
	}
	printf("ID:0x%x\n",icon);
	appOsdLib_PIPLayerIconDraw(posX, posY, SP5K_GFX_ALIGN_TOP_LEFT, icon);

}

void menuIconDispExt(UINT16 x, UINT16 y, UINT16 icon, UINT8 blendOpt)
{
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);

//	blendOpt = blendOpt;
	appOsdLib_PIPLayerIconDraw(x, y, SP5K_GFX_ALIGN_TOP_LEFT, icon);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuHeaderDisp                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuHeaderDisp(UINT8 layer)
{
	UINT32 iconId;
	/*when you delete a mode in menu,please delete the icon id of this mode*/
	UINT32 icon_ID[6] = {ICON_ID_STILL,ICON_ID_VIDEO,/*ICON_ID_AUDIO,*/ICON_ID_PLAYBACK,ICON_ID_SETUP,ICON_ID_DPS};
	if((layer == 0xff) || (menuProcCurrRootGet() == MENU_MARK_SCENEMODE)\
		||(menuProcCurrRootGet() == MENU_MARK_CALIBRATION))
	{
		return;
	}

	iconId =icon_ID[(UINT32)((menuPosBackup>>24) & 0xff) - 1];

	if (menuProcCurrRootGet() == MENU_MARK_SETUP)
	{
	    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN)
		{
			#if HDMI_MENU_BY_JPG
				#if 1
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER3, menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[0], menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[1],
					menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[0]+menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[2],menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				#endif

				sp5kRsvPreloadDraw(iconId,HDMI_MENU_LAYER2,menuSrcFileInfo[iconId].pos,0);

				sp5kRsvPreloadDraw(PRELOAD_MODE_SETUP,HDMI_MENU_LAYER2,menuSrcFileInfo[MENU_SETUP_HDMI].pos,0);

				sp5kRsvPreloadDraw(PRELOAD_MENU_TITLE,HDMI_MENU_LAYER3,menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos,0);

			#else
                /*main menu tab2: HDMI*/
                appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_TAB2_X, MENU_HEADER_TAB_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MENUTAB_HDMI, 0x0000);
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_MODE_X, MENU_HEADER_MODE_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE_HDMI, iconId);
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_SET_X, MENU_HEADER_SET_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE_HDMI, ICON_ID_SETUP);
			#endif
		}
		else
        #endif
		{
    		/*main menu tab2: not HDMI*/
            appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_TAB2_X, MENU_HEADER_TAB_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MENUTAB, 0x0000);
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_MODE_X, MENU_HEADER_MODE_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE, iconId);
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_SET_X, MENU_HEADER_SET_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE, ICON_ID_SETUP);
			#if FUNC_HOST_TOUCH_PANEL
			appGuiObjNew_EvtMsgReg(20,6,\
			WIDTH_ICON_MODE,HEIGHT_ICON_MODE,\
			APP_KEY_PRESS_TPMODE,0);
			#endif
		}

	}
	else
	{
	    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN)
		{
			#if HDMI_MENU_BY_JPG
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER3, menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[0], menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[1],
					menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[0]+menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[2],menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[1]+menuSrcFileInfo[MENU_TITLE_POS2_HDMI].pos[3]);

				sp5kRsvPreloadDraw(iconId,HDMI_MENU_LAYER2,menuSrcFileInfo[iconId].pos,0);
				#if 0//((!CAM_TYPE_CVR)&&(!CAM_TYPE_SPC)&&(!CAM_TYPE_DUAL)&&(!CAM_TYPE_SBC))
				if((menuPosBackup&0x0f000000) != MENU_MARK_DPS)
				#endif
				{
					sp5kRsvPreloadDraw(PRELOAD_MODE_SETUP,HDMI_MENU_LAYER2,menuSrcFileInfo[MENU_SETUP_HDMI].pos,0);
				}

				sp5kRsvPreloadDraw(PRELOAD_MENU_TITLE,HDMI_MENU_LAYER3,menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos,0);
			#else
                /*main menu tab1: HDMI*/
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_TAB1_X, MENU_HEADER_TAB_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MENUTAB_HDMI, 0x0000);
                appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_MODE_X, MENU_HEADER_MODE_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE_HDMI, iconId);
                appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_SET_X, MENU_HEADER_SET_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE_HDMI, ICON_ID_SETUP);
			#endif
		}
		else
        #endif
		{
		    /*main menu tab1: not HDMI*/
    		printf("[menu.Header]%s %d\n",__FUNCTION__,__LINE__);
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_TAB1_X, MENU_HEADER_TAB_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MENUTAB, 0x0000);
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_MODE_X, MENU_HEADER_MODE_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE, iconId);

			#if 0//((!CAM_TYPE_CVR)&&(!CAM_TYPE_SPC)&&(!CAM_TYPE_DUAL)&&(!CAM_TYPE_SBC))
			if((menuPosBackup&0x0f000000) != MENU_MARK_DPS)
			#endif
			{
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_HEADER_SET_X, MENU_HEADER_SET_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MODE, ICON_ID_SETUP);
			}
			#if FUNC_HOST_TOUCH_PANEL
			#if 0//((!CAM_TYPE_CVR)&&(!CAM_TYPE_SPC)&&(!CAM_TYPE_DUAL)&&(!CAM_TYPE_SBC))
			if((menuPosBackup&0x0f000000) != MENU_MARK_DPS)
			#endif
			{
				appGuiObjNew_EvtMsgReg(80, 6,\
				WIDTH_ICON_MODE,HEIGHT_ICON_MODE,\
				APP_KEY_PRESS_TPMODE,0);
			}
			#endif
		}
	}

	#if 0
	if (layer == 0)
	{
		if (menuProcCurrRootGet() == MENU_MARK_SETUP)
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);

			appOsdColorSet(MENU_ICON_DISABLE);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);

			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			appOsdColorSet(MENU_TAB_GET_FOCUS);
/*			appOsdLib_OsdLayIconDraw(5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_STR_GET_FOCUS);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
		else
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);

			appOsdColorSet(MENU_TAB_GET_FOCUS);
/*			appOsdLib_OsdLayIconDraw(2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_STR_GET_FOCUS);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);
			appOsdColorSet(MENU_ICON_DISABLE);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
	}
	else if (layer == 1)
	{
		if (menuProcCurrRootGet() == MENU_MARK_SETUP)
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
			appOsdColorSet(MENU_ICON_DISABLE);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);

			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			appOsdColorSet(MENU_TAB_LOST_TFOCUS);
/*			appOsdLib_OsdLayIconDraw(5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_TAB_GET_FOCUS);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
		else
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);

			appOsdColorSet(MENU_TAB_LOST_TFOCUS);
/*			appOsdLib_OsdLayIconDraw(2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);

			appOsdColorSet(MENU_ICON_DISABLE);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
	}
	else if (layer == 2)
	{
		if (menuProcCurrRootGet() == MENU_MARK_SETUP)
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);

			appOsdColorSet(MENU_ICON_DISABLE);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);

			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			appOsdColorSet(MENU_TAB_DISABLE);
/*			appOsdLib_OsdLayIconDraw(5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_ICON_DISABLE);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
		else
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);

			appOsdColorSet(MENU_TAB_DISABLE);
/*			appOsdLib_OsdLayIconDraw(2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET, MENU_MODE_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_MUTAB);*/
			appOsdColorSet(MENU_ICON_DISABLE);
			appOsdLib_OsdLayIconDraw(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT,icon);

			appOsdColorSet(MENU_ICON_DISABLE);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);

			appOsdLib_OsdLayIconDraw(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, SP5K_GFX_ALIGN_TOP_LEFT, icon);
		}
	}
	#if FUNC_HOST_TOUCH_PANEL
	menuProcButIconDisp(0xff,2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET,MENU_MODE_Y_OFFSET,\
		3*MENU_CHAR_WIDTH,MENU_CHAR_HEIGHT,\
		MENU_OBJ_MODE_NOR,ID_ICON_NONE,MENU_FUNC_MENUMODE,0x00,SP1K_MSG_NULL,0);

	menuProcButIconDisp(0xff,5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET,MENU_MODE_Y_OFFSET,\
		3*MENU_CHAR_WIDTH,MENU_CHAR_HEIGHT,\
		MENU_OBJ_MODE_SETUP,ID_ICON_NONE,MENU_FUNC_MENUMODE,0x01,SP1K_MSG_NULL,0);
	#endif
	#endif

}

void menuBodyDisp(UINT8 layer)
{
	UINT8 attr = MENU_BODY_GET_FOCUS;
	#if  1/*FUNC_HOST_TOUCH_PANEL*/
	UINT8 line,col;
	UINT8 nodeItem;
	#if HDMI_MENU_BY_JPG
/* 	UINT32 ratioX = 1920/640;
	UINT32 ratioY = 1080/360; */
	#endif

	if(layer)
	{
		menuLayerSizeGet(layer, &line, &col);
		nodeItem = menuProcNodeItemGet(layer, 0);
	}
	else
	{
		menuLayerSizeGet(1, &line, &col);
		nodeItem = menuProcNodeItemGet(1, 0);
	}
	#endif
	if(menuProcCurrRootGet() == MENU_MARK_SCENEMODE)
	{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, 14, 4, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSCENE, 0x0000);
		return;
	}

	if ((layer == 1) && (menuProcLayerGet() == 2))
	{
		attr = MENU_BODY_DISABLE;
	}
	else if (layer == 3)
	{
		attr = MENU_DLG_GET_FOCUS;
	}

	appOsdColorSet(attr);
	if(layer == 2)
	{
	    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN)
		{
			#if HDMI_MENU_BY_JPG
				UINT32 ratioX = 1920/640;
				UINT32 ratioY = 1080/360;
				appOsd_ColorDrawShapeColorSet(0x00000000, PAL_BLEND_100, 0x00000000, PAL_BLEND_100);
				appOsdLib_PIPLayerShapeDraw(APP_OSD_SHAPE_RECT,menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]/ratioX, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]/ratioY - 2,
				menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2]/ratioX, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]/ratioY, 0, 0);

				#if 0
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
				appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+1, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
					menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_BASE+0, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
					menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				#endif

				sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BODY, HDMI_MENU_LAYER2, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos, 0);

			#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (MENU_BODY_LAYER2_X_OFFSET), (MENU_BODY_LAYER2_Y_OFFSET), SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC_HDMI, 0x0000);
			#endif
        }
        else
		#endif
		{
            appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_4);
            appOsdLib_PIPLayerIconDraw(MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, 0x0000);
			//appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER2_X_OFFSET,MENU_BODY_LAYER2_Y_OFFSET , SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC, 0x0000);
        }
	}
	else if (layer == 3)
	{
		if(nodeItem > 2)/*item more than 2*/
		{
		    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN)
			{
				#if HDMI_MENU_BY_JPG
				#if 0
				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
					menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				//appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
				//	menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);
				#endif
				sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BODY, HDMI_MENU_LAYER2, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos, 0);

				#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (MENU_BODY_LAYER2_X_OFFSET), (MENU_BODY_LAYER2_Y_OFFSET), SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC_HDMI, 0x0000);
				#endif
			}
			else
			{
				if((fileTypeSelect == 1) || USBModeSet)
				{
					appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_4);
					appOsdLib_PIPLayerIconDraw(MENU_BODY_LAYER2_X_OFFSET-6, MENU_BODY_LAYER2_Y_OFFSET-6, SP5K_GFX_ALIGN_TOP_LEFT, 0x0000);
					//appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER2_X_OFFSET-6, MENU_BODY_LAYER2_Y_OFFSET-6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC, 0x0000);
				}
				else
				{
					appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_4);
					appOsdLib_PIPLayerIconDraw(MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET+20, SP5K_GFX_ALIGN_TOP_LEFT, 0x0000);
					//appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET+20, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUBSC, 0x0000);
				}
			}
			#endif

		}
		else
		{
		     #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN)
			{
				#if HDMI_MENU_BY_JPG
				sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,
			 		menuSrcFileInfo[MENU_TWO_LINES_BODY_HDMI].fileName, HDMI_MENU_LAYER1, menuSrcFileInfo[MENU_TWO_LINES_BODY_HDMI].pos, 0);

				#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER3_X_OFFSET<<1, MENU_BODY_LAYER3_Y_OFFSET<<1, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUDLG_HDMI, 0x0000);
				#endif
			}
			else
			{
				if(fileTypeSelect == 1) 
				{
				/*modify by Isaac*/
					#if	defined(PRJ_JETDVR) || defined(PRJ_PDVR)
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER3_X_OFFSET, 28, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUDLG, 0x0000);
					#else
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER3_X_OFFSET, MENU_BODY_LAYER3_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUDLG, 0x0000);
					#endif
				}
				else
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, MENU_BODY_LAYER3_X_OFFSET, MENU_BODY_LAYER3_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUDLG, 0x0000);
			}
			#endif
		}
	}
	else
	{
	     #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			#if HDMI_MENU_BY_JPG
			//sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG | SP5K_RES_SUBTYPE_JPG_MAIN,
			// menuSrcFileInfo[MENU_BODY_HDMI].fileName, SP5K_GFX_PAGE_PIP_MAIN, menuSrcFileInfo[MENU_BODY_HDMI].pos, 0);
			sp5kRsvPreloadDraw(PRELOAD_MENU_BODY, HDMI_MENU_LAYER4, menuSrcFileInfo[MENU_BODY_HDMI].pos, 0);
			#else
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUBODY_HDMI, 0x0000);
			#endif
		}
		else{
			appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_2);
			appOsdLib_PIPLayerIconDraw(0, 0, SP5K_GFX_ALIGN_TOP_LEFT, 0x0000);
			//appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUBODY, 0x0000);
		}
		#endif
	}

}

#if HDMI_MENU_BY_JPG
void menuHDMIRsvPreload(void)
{
	UINT32 ret;
#if 0
	UINT32 width,height;

	sp5kRsvPreloadDrawCfgSet(SP5K_RES_PRELOAD_NUM,7);

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BODY_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BODY_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_BODY_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BODY_HDMI].fileName, PRELOAD_MENU_BODY, width, height, 0);
	printf("MENU_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[2],menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].fileName, PRELOAD_FIVE_LINES_BAR, width, height, 0);
	printf("MENU_FIVE_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].fileName, PRELOAD_FOUR_LINES_BODY, width, height, 0);
	printf("MENU_FOUR_LINES_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].fileName, PRELOAD_FOUR_LINES_BAR, width, height, 0);
	printf("MENU_FOUR_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_SETUP_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_SETUP_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_SETUP_HDMI].pos[2],menuSrcFileInfo[MENU_SETUP_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_SETUP_HDMI].fileName, PRELOAD_MODE_SETUP, width, height, 0);
	printf("MENU_SETUP_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[2],menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].fileName, PRELOAD_BUTTON_OK, width, height, 0);
	printf("MENU_BUTTON_OK_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	width  = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[2],16);
	height = ROUND_UP_TO_DIVISIBLE(menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[3],16);
	printf("%d  %d   width:%d height:%d\n",menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[2],menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[3],
		width,height);
	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].fileName, PRELOAD_BUTTON_MENU, width, height, 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");
#elif 0
	sp5kRsvPreloadDrawCfgSet(SP5K_RES_PRELOAD_NUM,13);

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BODY_HDMI].fileName, PRELOAD_MENU_BODY, menuSrcFileInfo[MENU_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_BODY_HDMI].pos[3], 0);
	printf("MENU_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].fileName, PRELOAD_FIVE_LINES_BAR, menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[2],menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].pos[3], 0);
	printf("MENU_FIVE_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].fileName, PRELOAD_FOUR_LINES_BODY, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3], 0);
	printf("MENU_FOUR_LINES_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].fileName, PRELOAD_FOUR_LINES_BAR, menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3], 0);
	printf("MENU_FOUR_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_SETUP_HDMI].fileName, PRELOAD_MODE_SETUP, menuSrcFileInfo[MENU_SETUP_HDMI].pos[2],menuSrcFileInfo[MENU_SETUP_HDMI].pos[3], 0);
	printf("MENU_SETUP_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].fileName, PRELOAD_BUTTON_OK, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[2],menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos[3], 0);
	printf("MENU_BUTTON_OK_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].fileName, PRELOAD_BUTTON_MENU, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[2],menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_TITLE_POS1_HDMI].fileName, PRELOAD_MENU_TITLE, menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[2],menuSrcFileInfo[MENU_TITLE_POS1_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_STILL_HDMI].fileName, PRELOAD_MODE_STILL, menuSrcFileInfo[MENU_STILL_HDMI].pos[2],menuSrcFileInfo[MENU_STILL_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_VIDEO_HDMI].fileName, PRELOAD_MODE_VIDEO, menuSrcFileInfo[MENU_VIDEO_HDMI].pos[2],menuSrcFileInfo[MENU_VIDEO_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_AUDIO_HDMI].fileName, PRELOAD_MODE_AUDIO, menuSrcFileInfo[MENU_AUDIO_HDMI].pos[2],menuSrcFileInfo[MENU_AUDIO_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_PLAYBACK_HDMI].fileName, PRELOAD_MODE_PLAYBACK, menuSrcFileInfo[MENU_PLAYBACK_HDMI].pos[2],menuSrcFileInfo[MENU_PLAYBACK_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_PRINTER_HDMI].fileName, PRELOAD_MODE_PRINTER, menuSrcFileInfo[MENU_PRINTER_HDMI].pos[2],menuSrcFileInfo[MENU_PRINTER_HDMI].pos[3], 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

#else
	sp5kRsvPreloadDrawCfgSet(SP5K_RES_PRELOAD_NUM,13);

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BODY_HDMI].fileName, PRELOAD_MENU_BODY, 0,0, 0);
	printf("MENU_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FIVE_LINES_BAR_HDMI].fileName, PRELOAD_FIVE_LINES_BAR, 0,0, 0);
	printf("MENU_FIVE_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].fileName, PRELOAD_FOUR_LINES_BODY, 0,0, 0);
	printf("MENU_FOUR_LINES_BODY_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].fileName, PRELOAD_FOUR_LINES_BAR, 0,0, 0);
	printf("MENU_FOUR_LINES_BAR_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_SETUP_HDMI].fileName, PRELOAD_MODE_SETUP, 0,0, 0);
	printf("MENU_SETUP_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].fileName, PRELOAD_BUTTON_OK,0,0, 0);
	printf("MENU_BUTTON_OK_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].fileName, PRELOAD_BUTTON_MENU, 0,0, 0);
	printf("MENU_BUTTON_MENU_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_TITLE_POS1_HDMI].fileName, PRELOAD_MENU_TITLE, 0,0, 0);
	printf("MENU_TITLE_POS1_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_STILL_HDMI].fileName, PRELOAD_MODE_STILL, 0,0, 0);
	printf("MENU_STILL_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_VIDEO_HDMI].fileName, PRELOAD_MODE_VIDEO, 0,0, 0);
	printf("MENU_VIDEO_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_AUDIO_HDMI].fileName, PRELOAD_MODE_AUDIO, 0,0, 0);
	printf("MENU_AUDIO_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_PLAYBACK_HDMI].fileName, PRELOAD_MODE_PLAYBACK, 0,0, 0);
	printf("MENU_PLAYBACK_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");

	ret = sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG, menuSrcFileInfo[MENU_PRINTER_HDMI].fileName, PRELOAD_MODE_PRINTER, 0,0, 0);
	printf("MENU_PRINTER_HDMI preload -- %s\n",(ret==FAIL)?"FAIL":"SUCCESS");
#endif
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: menuPromptDisp                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuPromptDisp(UINT16 attr)
{
	UINT8 line;
	//UINT8 col;
	UINT8 OsdMaxX, OsdMaxY;
	UINT32 cw,ch;

	appOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	appOsdFontSizeGet(&cw, &ch);
	if(ch <= 16)OsdMaxY-=1;

	line = OsdMaxY;
	//col = OsdMaxX;

	appOsdColorSet(MENU_PROMPT_GET_FOCUS);
	if (attr & MENU_PROMPT_OK_ON)
	{
		#if HDMI_MENU_BY_JPG
		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
		}
		#endif
		#endif

		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			#if HDMI_MENU_BY_JPG
			//sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,
		 	//	menuSrcFileInfo[MENU_BUTTON_OK_HDMI].fileName, SP5K_GFX_PAGE_PIP_BASE+0, menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos, 0);
			sp5kRsvPreloadDraw(PRELOAD_BUTTON_OK,HDMI_MENU_LAYER1,menuSrcFileInfo[MENU_BUTTON_OK_HDMI].pos,0);
			appOsd_GfxStringIDDraw((MENU_BACK_ON_OFFSET+5)*MENU_CHAR_WIDTH, (line+2)*MENU_CHAR_HEIGHT+MENU_OFFSET+5, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_ENTER);
			#else
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,90,320,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICONFILE_BUTTON_HDMI,0x0001);
			appOsd_GfxStringIDDraw((MENU_BACK_ON_OFFSET+7)*MENU_CHAR_WIDTH, (line+2)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_ENTER);
			#endif
		}
		else
		#endif
		{
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,30,214,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICONFILE_BUTTON,0x0001);
			if(IS_TV_IN){
				appOsd_GfxStringIDDraw((MENU_BACK_ON_OFFSET+2)*MENU_CHAR_WIDTH+2, (line-1)*MENU_CHAR_HEIGHT+3, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_ENTER);
			}else{
				appOsd_GfxStringIDDraw((MENU_BACK_ON_OFFSET+2)*MENU_CHAR_WIDTH+2, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_ENTER);
			}
		}
	}

	if (attr & MENU_PROMPT_MENU_ON)
	{
		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			#if HDMI_MENU_BY_JPG
			//sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,
		 	//	menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].fileName, SP5K_GFX_PAGE_PIP_BASE+0, menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos, 0);
			sp5kRsvPreloadDraw(PRELOAD_BUTTON_MENU,HDMI_MENU_LAYER1,menuSrcFileInfo[MENU_BUTTON_MENU_HDMI].pos,0);
			appOsd_GfxStringIDDraw((MENU_BACK_ON_OFFSET+31)*MENU_CHAR_WIDTH, (line+2)*MENU_CHAR_HEIGHT+MENU_OFFSET+5, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
			#else
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,410,320,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICONFILE_BUTTON_HDMI,0x0000);
			appOsd_GfxStringIDDraw((MENU_BACK_ON_OFFSET+27)*MENU_CHAR_WIDTH, (line+2)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
			#endif
		}
		else
		#endif
		{
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,190,214,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICONFILE_BUTTON,0x0000);
			if(IS_TV_IN){
				appOsd_GfxStringIDDraw((MENU_BACK_ON_OFFSET+12)*MENU_CHAR_WIDTH+2, (line-1)*MENU_CHAR_HEIGHT+3, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
			}else{
				appOsd_GfxStringIDDraw((MENU_BACK_ON_OFFSET+12)*MENU_CHAR_WIDTH+2, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
			}
		}
	}
#if FUNC_HOST_TOUCH_PANEL
	if (attr & MENU_PROMPT_PAGEUPDN_ON)
	{

		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			menuIconDispExt(MENU_BODY_LAYER2_X_OFFSET + 250-MENU_CHAR_WIDTH,9, ID_ICON_TBUTTON_UP,0);/*SHOW UP BUTTON*/
			menuIconDispExt(MENU_BODY_LAYER2_X_OFFSET + 410-MENU_CHAR_WIDTH,9, ID_ICON_TBUTTON_DOWN,0);/*SHOW DOWN BUTTON*/
		}
		else
		#endif
		{
			menuIconDispExt(MENU_BODY_LAYER2_X_OFFSET + 110-MENU_CHAR_WIDTH,9, ID_ICON_TBUTTON_UP,0);/*SHOW UP BUTTON*/
			menuIconDispExt(MENU_BODY_LAYER2_X_OFFSET + 170-MENU_CHAR_WIDTH,9, ID_ICON_TBUTTON_DOWN,0);/*SHOW DOWN BUTTON*/

		}
	}
#endif

}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuPromptDispReg                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
#if FUNC_HOST_TOUCH_PANEL
void menuPromptDispReg(UINT16 attr)
{
	UINT8	line;
	UINT8 OsdMaxX, OsdMaxY;
	UINT32 cw,ch;

	appOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	appOsdFontSizeGet(&cw, &ch);
	if(ch <= 16)OsdMaxY-=1;

	line = OsdMaxY;
	if(attr & MENU_PROMPT_OK_ON)
	{
	appGuiObjNew_EvtMsgReg((MENU_BACK_ON_OFFSET+2)*MENU_CHAR_WIDTH-2-45, (line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET,\
		(4*MENU_CHAR_WIDTH+2+appOsdLib_FontGetStringWidthbyId(ID_STR_BACK)),MENU_CHAR_HEIGHT-2,\
		APP_KEY_PRESS_TPSET,0);
	}
	if(attr & MENU_PROMPT_MENU_ON)
	{
	appGuiObjNew_EvtMsgReg((MENU_BACK_ON_OFFSET+8)*MENU_CHAR_WIDTH-2,(line-1)*MENU_CHAR_HEIGHT+MENU_OFFSET,\
		(4*MENU_CHAR_WIDTH+2+appOsdLib_FontGetStringWidthbyId(ID_STR_BACK)),MENU_CHAR_HEIGHT-2,\
		APP_KEY_PRESS_TPMENU,0);
	}
	if(attr & MENU_PROMPT_PAGEUPDN_ON)
	{
	appGuiObjNew_EvtMsgReg(MENU_BODY_LAYER2_X_OFFSET + 110-MENU_CHAR_WIDTH,9,\
		MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT-4,\
		APP_KEY_PRESS_TPUP,0);
	appGuiObjNew_EvtMsgReg(MENU_BODY_LAYER2_X_OFFSET + 170-MENU_CHAR_WIDTH,9,\
		MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT-4,\
		APP_KEY_PRESS_TPDOWN,0);
	}
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: menuFileTypeSelectExitDispAndReg                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:show PB icon and register icon for touchpanel                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 #if FUNC_HOST_TOUCH_PANEL
void menuFileTypeSelectExitDispAndReg(void)
{
	#if KIT_WITH_HDMI
	if(0/*IS_HDMI_IN*/){
		appOsdIconModeSet(APP_ICON_MODE_PLAYBACK_HDMI);
	}else{
		appOsdIconModeSet(APP_ICON_MODE_PLAYBACK);
	}
	#else
	appOsdIconModeSet(APP_ICON_MODE_PLAYBACK);
	#endif
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
	appIconShow(ID_ICON_MODE_PLAYBACK, ICON_SHOW, SP5K_GFX_ALIGN_TOP_LEFT);
	iconGuiReg(ID_ICON_MODE_PLAYBACK, APP_KEY_PRESS_TPMODE, 0);
}
 #endif
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcDisp                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuProcDisp (void)
{
	UINT16	icon;
	UINT8	nodeItem = 0;
	UINT8	subItem=0;
	UINT8	selItem;
	UINT8	layer = 0;
	UINT8	item = 0;
	UINT8	line, col;
	UINT16	i;
	UINT32	menuRoot;
	UINT8 OsdMaxX, OsdMaxY;
	UINT32 strIdx;
	#if FUNC_HOST_TOUCH_PANEL
	UINT32 promptParm;
	#endif

	appOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	menuRoot = menuProcCurrRootGet();
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	#if FUNC_HOST_TOUCH_PANEL
	appGuiObjClr(-1);
	#endif

	appOsd_ColorBlendingSet(PAL_GREEN_2,PAL_BLEND_0);
	appOsd_ColorBlendingSet(PAL_GREEN_3,PAL_BLEND_0);
	appOsd_ColorBlendingSet(PAL_GREEN_4,PAL_BLEND_0);

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		#if !HDMI_MENU_BY_JPG
		appOsd_ColorBlendingSet(PAL_GREEN_2,PAL_BLEND_0);
		appOsd_ColorBlendingSet(PAL_GREEN_3,PAL_BLEND_0);
		appOsd_ColorBlendingSet(PAL_GREEN_4,PAL_BLEND_0);
		#else
		/* init PIP layer */
		if(!pipHDMIMenuFlag){
			menuHDMIPIPInit();
		}else{
			sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
			sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

			sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
			if (menuProcLayerGet() == 1 || menuProcLayerGet() == 0){
				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER3, 0, 220,1920,955);
			}
			appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER0, 0, 220,1920,955);
			appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER1, 0, 220,1920,955);
			appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, 0, 0,1920,1080);
			//appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, 0, 220,1920,955);
			//appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER4, 0, 0,1920,1080);
			//sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
		}

		dlgUpdateFlag = 0;
		#endif
	}
	#endif

	printf("@@@@@@ pos:0x%08x,layer =%d\n",menuProcCurrPosGet(),menuProcLayerGet());
	#if 1
	 /* Highlight the tab */
	if (menuProcLayerGet() == 0)
	{
		layer = 1;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);

		menuBodyDisp(0);
		menuHeaderDisp(0);

		nodeItem = menuProcNodeItemGet(layer, item);
		#if FUNC_HOST_TOUCH_PANEL
	     /*modify bug ICOM-759*/
		if (nodeItem > line)
		{
			promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON;

		}else
		{

			promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_OFF;

		}
		#endif
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer, item, &strIdx);
			icon = menuProcIconIDGet(layer, item);
			subItem = menuProcSubItemGet(layer, item);

			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}
			if (menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			#if FUNC_HOST_TOUCH_PANEL
			appGuiObjNew_EvtFcsCallBackReg(MENU_LAYER1_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER1_STR_Y_OFFSET,\
				WIDTH_ICON_MUSBAR,MENU_CHAR_HEIGHT-2,\
				 (pFunc)appMenuViewSelItemFuncGui,i,\
				 (pFunc)appMenuViewSelItemGetFocusFuncGui,i,\
				 (pFunc)appMenuViewSelItemLostFocusFuncGui,i\
				);
			#endif
		}
		#if FUNC_HOST_TOUCH_PANEL
	   /*modify bug ICOM-759*/
		menuPromptDisp(promptParm);
		menuPromptDispReg(promptParm);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_OFF);
		#endif
	}
	#endif
	#if 1 /* Highlight the setting menu */
	else if (menuProcLayerGet() == 1)
	{
		layer = 1;
		item = 0;

		menuLayerSizeGet(layer, &line, &col);
		#if HDMI_MENU_BY_JPG
		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, 0, 220,1920,955);
		}
		#endif
		#endif
		menuBodyDisp(layer);
		menuHeaderDisp(layer);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer,item, &strIdx);
			icon = menuProcIconIDGet(layer, item);
			subItem = menuProcSubItemGet(layer, item);

			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i, layer);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDraw(0, i, strIdx, 0, layer);
				appOsdColorSet(MENU_ICON_GET_FOCUS);
				menuIconDisp(col-2, i, icon, 1, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			#if FUNC_HOST_TOUCH_PANEL
			appGuiObjNew_EvtFcsCallBackReg(MENU_LAYER1_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER1_STR_Y_OFFSET,\
				WIDTH_ICON_MUSUSBAR,MENU_CHAR_HEIGHT-2,\
				 (pFunc)appMenuViewSelItemFuncGui,i,\
				 (pFunc)appMenuViewSelItemGetFocusFuncGui,i,\
				 (pFunc)appMenuViewSelItemLostFocusFuncGui,i\
				);
			#endif

		}

		#if FUNC_HOST_TOUCH_PANEL
		menuPromptDisp(MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON);
		menuPromptDispReg(MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_OFF);
		#endif
	}
	#endif
	#if 1 /* Highlight the option menu */
	else if (menuProcLayerGet() == 2)
	{
		layer = 1;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);
		#if HDMI_MENU_BY_JPG
		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(!IS_HDMI_IN){
			menuBodyDisp(layer);
		}
		#endif
		#else
			menuBodyDisp(layer);
		#endif

		menuHeaderDisp(layer);
		nodeItem = menuProcNodeItemGet(layer, item);
		#if FUNC_HOST_TOUCH_PANEL
		  /*modify bug ICOM-759*/
		if (nodeItem > line)
		{
			promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON;

		}else
		{

			promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_OFF;

		}
		#endif
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer,item, &strIdx);
			icon = menuProcIconIDGet(layer, item);
			subItem = menuProcSubItemGet(layer, item);

			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i, layer);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDraw(0, i, strIdx, 1, layer);
				menuIconDisp(col-2, i, icon, 1, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDraw(0, i, strIdx, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
		}


		layer = 2;
		item = 0;
		menuBodyDisp(layer);
		//menuHeaderDisp(layer);

		menuLayerSizeGet(layer, &line, &col);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);


		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer,item, &strIdx);
			icon = menuProcIconIDGet(layer, item);

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				if(menuRoot == MENU_MARK_PLAYBACK)
				{
					menuStrDraw(0, i, strIdx, 0, layer);
				}
				else
				{
					menuStrDraw(4, i, strIdx, 0, layer);
					menuIconDisp(4, i, icon, 0, layer);
				}
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i, layer);
				appOsdColorSet(SUB_MENU_STR_GET_FOCUS);

				if(menuRoot == MENU_MARK_PLAYBACK)
				{
					menuStrDraw(0, i, strIdx, 1, layer);
				}
				else
				{
					menuStrDraw(4, i, strIdx, 1, layer);
					appOsdColorSet(MENU_ICON_GET_FOCUS);
					menuIconDisp(4, i, icon, 1, layer);
				}
			}
			else
			{
				appOsdColorSet(SUB_MENU_STR_LOST_FOCUS);
				if(menuRoot == MENU_MARK_PLAYBACK)
				{
					menuStrDraw(0, i, strIdx, 0, layer);
				}
				else
				{
					menuStrDraw(4, i, strIdx, 0, layer);
					menuIconDisp(4, i, icon, 0, layer);
				}
			}
			#if FUNC_HOST_TOUCH_PANEL
			appGuiObjNew_EvtFcsCallBackReg(MENU_LAYER2_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER2_STR_Y_OFFSET,\
				WIDTH_ICON_MUSBAR,MENU_CHAR_HEIGHT-2,\
				 (pFunc)appMenuViewSelItemFuncGui,i,\
				 (pFunc)appMenuViewSelItemGetFocusFuncGui,i,\
				 (pFunc)appMenuViewSelItemLostFocusFuncGui,i\
				);
			#endif
		}
		#if FUNC_HOST_TOUCH_PANEL
	   /*modify bug ICOM-759*/
		menuPromptDisp(promptParm);
		menuPromptDispReg(promptParm);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_OFF);
		#endif
	}
	#endif
	#if 1 /* Highlight the option menu */
	else if (menuProcLayerGet() == 3)
	{
		layer = 3;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);

		menuBodyDisp(layer);
		//menuHeaderDisp(layer);

		nodeItem = menuProcNodeItemGet(layer, item);
		#if FUNC_HOST_TOUCH_PANEL
		 /*modify bug ICOM-759*/
		if (nodeItem > line)
		{
			promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_OFF|MENU_PROMPT_PAGEUPDN_ON;

		}else
		{

			promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_OFF|MENU_PROMPT_PAGEUPDN_OFF;

		}
		#endif
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		for(i=0; i<nodeItem; i++,item++)
		{
			menuProcStrGet(layer,item, &strIdx);
//			icon = menuProcIconIDGet(layer, item);

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				if((fileTypeSelect == 1) || USBModeSet)
				{
					menuStrDraw(1, i + 1, strIdx, 0, layer);
				}
				else
				{
					menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
				}
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				if((fileTypeSelect == 1) || USBModeSet)
				{
					menuBarDisp(i+ 1, layer);
					appOsdColorSet(MENU_STR_GET_FOCUS);
					menuStrDraw(1, i+ 1, strIdx, 1, layer);
				}
				else
				{
					menuBarDisp(i + (line - nodeItem), layer);
					appOsdColorSet(MENU_STR_GET_FOCUS);
					menuStrDraw(2, i + (line - nodeItem), strIdx, 1, layer);
				}
			}
			else
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				if((fileTypeSelect == 1) || USBModeSet)
				{
					menuStrDraw(1, i+ 1, strIdx, 0, layer);
				}
				else
				{
					menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
				}
			}

		#if FUNC_HOST_TOUCH_PANEL
				appGuiObjNew_EvtFcsCallBackReg(MENU_LAYER2_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER2_STR_Y_OFFSET,\
					WIDTH_ICON_MUSBAR,MENU_CHAR_HEIGHT-2,\
					 (pFunc)appMenuViewSelItemFuncGui,i,\
					 (pFunc)appMenuSelItemGetFocusFuncGui,i,\
					 (pFunc)appMenuSelItemLostFocusFuncGui,i\
					);
		#endif
		}

		item = menuProcCurrItemGet(layer-1);
		menuProcStrGet(layer-1,item, &strIdx);

		if((fileTypeSelect == 0) && (USBModeSet == 0))
		{
			menuBarDisp(line-nodeItem-1, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			appOsdLib_TextIDDraw(\
				(LCD_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, \
				(line-nodeItem)*MENU_CHAR_HEIGHT, \
				SP5K_GFX_ALIGN_TOP_LEFT, strIdx
			);
		}

		#if FUNC_HOST_TOUCH_PANEL
		if(fileTypeSelect){
			menuFileTypeSelectExitDispAndReg();
		}

	    /*modify bug ICOM-759*/
		menuPromptDisp(promptParm);
		menuPromptDispReg(promptParm);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_OFF|MENU_PROMPT_PAGEUPDN_OFF);
		#endif
	}
	#endif

	#if KIT_WITH_HDMI && HDMI_MENU_BY_JPG
	if(IS_HDMI_IN){
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER0, 10, 1, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER0, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER1, 10, 1, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER1, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER2, 10, 1, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER2, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER3, 10, 1, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER3, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER4, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER4, 1, 0, 0);

		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
	}
	#endif
	APP_OSD_REFRESH_ON;
}

void menuEVDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
/*	UINT8 icon = menuProcIconIDGet(layer, item);*/
	UINT8 line, col;
	//UINT8 nodeItem;
	UINT8 count=0;
	UINT16 xpos,ypos,width,heigth;
	UINT8 depth;
	//UINT8 color;
	UINT32 strIdx;

	menuProcStrGet(layer,item, &strIdx);
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;

	menuLayerSizeGet(layer, &line, &col);
	appOsdColorSet(MENU_STR_LOST_FOCUS);
	//nodeItem = menuProcNodeItemGet(layer, item);
	menuStrDisp((col-6)/2, line-2, (UINT8 *)"EV", 0, layer);
	menuStrDraw((col-6)/2+3, line-2, strIdx, 0, layer);
	//menuStrDisp((col-6)/2-2, line-1, exposure, 0, layer);
	xpos=MENU_CHAR_WIDTH*(col/4);
	ypos =(MENU_CHAR_HEIGHT<=16)?(line*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET): (LCD_HEIGHT-2*MENU_CHAR_HEIGHT);
	width=MENU_CHAR_WIDTH/2;
	heigth=MENU_CHAR_HEIGHT/4;
	depth=MENU_CHAR_WIDTH/4;

	for(count=0;count<13;count++)
	{
		xpos+=MENU_CHAR_WIDTH;
		//if(item == count)
		//{
		//	color=0x04;
		//}
		//else
		//{
		//	color=0x3e;
		//}
		if(count%3 ==0)
		{
			appOsdLib_OsdLayerFrameDraw(xpos,ypos-heigth,width,heigth*3,depth);
		}
		else
		{
			appOsdLib_OsdLayerFrameDraw(xpos,ypos,width,heigth*2,depth);
		}
	}

	//appOsdColorSet(MENU_TAB_GET_FOCUS);
	//osdCharDrawExt((((UINT16)col-6)/2 + menuXOff)*MENU_CHAR_WIDTH+item*fontW +8 , ((UINT16)line+menuYOff)*32 -10, exposure[item],1);
	#if FUNC_HOST_TOUCH_PANEL
	menuPromptDisp(MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON);
	menuPromptDispReg(MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON);
	#else
	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_OFF);
	#endif
	APP_OSD_REFRESH_ON;
}

#if 1
#if 0
void menuWIFIPasswordDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer-1);
	UINT32 i;
	UINT32  xpos=0,xStartPos=0, xOffSet=0,ypos=2;
	UINT8 str[5];
	UINT32 strIdx;

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_PIPLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);

	printf("layer = %d,item =%d\n",layer,item);
	if((LCD_WIDTH/ssid_PwCfg.pwTotalNum-MENU_CHAR_WIDTH-2)>=0)
	{
		xOffSet =(LCD_WIDTH/ssid_PwCfg.pwTotalNum-MENU_CHAR_WIDTH-2);
	}

	xStartPos =(LCD_WIDTH -(ssid_PwCfg.pwTotalNum-1) *(MENU_CHAR_WIDTH+xOffSet))/2;
	menuProcStrGet(layer-1, item , &strIdx);
	appOsdLib_TextIDDraw(LCD_WIDTH/2, MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_CENTER, strIdx);

	for(i=0;i<ssid_PwCfg.pwTotalNum; i++)
	{
		sprintf(str, "%c", ssid_PwCfg.wfifPw[i]);

		if (i == ssid_PwCfg.pwCurrNum){
			appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
			appOsd_GfxIconDraw((xStartPos+xpos*(MENU_CHAR_WIDTH+xOffSet)),ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET+20,SP5K_GFX_ALIGN_CENTER,ID_ICON_TBUTTON_DOWN);
			appOsd_GfxIconDraw((xStartPos+xpos*(MENU_CHAR_WIDTH+xOffSet)),ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET-15,SP5K_GFX_ALIGN_CENTER,ID_ICON_TBUTTON_UP);
		}else{
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		}
		appOsd_GfxStringDraw((xStartPos+xpos*(MENU_CHAR_WIDTH+xOffSet)), ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_CENTER,str);
		xpos += 1;

	}
	menuPromptDisp(MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON);

	APP_OSD_REFRESH_ON;
}
#endif
#else
void menuWIFIPasswordDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT8 i;
	UINT8 nodeItem, xpos=0, ypos=1;
	UINT8 str[32];
	uiPara_t* puiPara=appUiParaGet();
	UINT8 Data[8]={
		puiPara->WIFIPassword_7,
		puiPara->WIFIPassword_6,
		puiPara->WIFIPassword_5,
		puiPara->WIFIPassword_4,
		puiPara->WIFIPassword_3,
		puiPara->WIFIPassword_2,
		puiPara->WIFIPassword_1,
		puiPara->WIFIPassword_0
	};


	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	nodeItem = menuProcNodeItemGet(layer, item);

	for(i=0, item=0; i<nodeItem; i++,item++)
	{
		sprintf(str, "%d", Data[item]);

		if (menuProcCurrItemGet(layer) == item){
			appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		}else{

			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		}
		appOsd_GfxStringDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT,str);
		xpos += 2;

	}

	//xpos += 2;
	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);

	APP_OSD_REFRESH_ON;
}
#endif

#if FUNC_HOST_TOUCH_PANEL
void menuRTCDisp(void)
{
	UINT8 layer = menuProcLayerGet(),attr;
	UINT8 item = menuProcCurrItemGet(layer);
/*	UINT8 icon = menuProcIconIDGet(layer, item);*/
	UINT8 line, col, i;
	UINT8 nodeItem, xpos=0, ypos=1;
	UINT32 strIdx;
//	UINT16 iconXpos;
	UINT8 OsdMaxX, OsdMaxY;
	UINT32 cw,ch;

	appOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	appOsdFontSizeGet(&cw, &ch);
	if(ch <= 16)OsdMaxY-=1;
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
#if FUNC_HOST_TOUCH_PANEL
	appGuiObjClr(-1);
#endif
	menuLayerSizeGet(layer, &line, &col);

 	nodeItem = menuProcNodeItemGet(layer, item);

	for(i=0, item=0; i<nodeItem; i++,item++)
	{
		UINT32 strWidth;
		UINT8 selItem;
		selItem = menuProcParaGet (layer, menuDateItemConv(item));
		menuProcChildStrGet(layer, menuDateItemConv(item), selItem, &strIdx);
		if (menuProcCurrItemGet(layer) == menuDateItemConv(item)) {
			attr = MENU_STR_GET_FOCUS;
		} else {
			attr = MENU_STR_LOST_FOCUS;
		}
		appOsdColorSet(attr);
		strWidth = appOsdLib_FontGetStringWidthbyId(strIdx);

		appOsdLib_TextIDDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET-30, SP5K_GFX_ALIGN_TOP_LEFT,strIdx);
		if (menuProcCurrItemGet(layer) == menuDateItemConv(item))
		{
			menuProcButIconDisp(attr,(xpos+1)*MENU_CHAR_WIDTH+strWidth/2,(ypos-1)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET-30,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			ID_ICON_TBUTTON_UP,APP_KEY_PRESS_TPUP,0);
			menuProcButIconDisp(attr,(xpos+1)*MENU_CHAR_WIDTH+strWidth/2,(ypos+1)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET-26,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			ID_ICON_TBUTTON_DOWN,APP_KEY_PRESS_TPDOWN,0);
		}

		xpos += appOsdLib_FontGetStringWidthbyId(strIdx) / 13;

		if ((item == 0) || (item == 1))
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			appOsdLib_TextDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET-30, SP5K_GFX_ALIGN_TOP_LEFT, "/");
			xpos += strlen("/");
		}
		else if (item == 3)
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			appOsdLib_TextDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET-30, SP5K_GFX_ALIGN_TOP_LEFT, ":");
			xpos += strlen(":");
		}
		else if (item == 4)
		{
			if(MENU_CHAR_WIDTH<=8)
				xpos = 2;
			else
				xpos = 4;

			ypos += 2;
		}
		else
			xpos++;
	}
	menuProcButIconDisp(0,MENU_CHAR_WIDTH*3,ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET-26,\
	MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
	ID_ICON_TBUTTON_LEFT,APP_KEY_PRESS_TPLEFT,0);
	menuProcButIconDisp(0,MENU_CHAR_WIDTH*15,ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET-26,\
	MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
	ID_ICON_TBUTTON_RIGHT,APP_KEY_PRESS_TPRIGHT,0);
	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_OFF);

	appGuiObjNew_EvtMsgReg((MENU_BACK_ON_OFFSET+2)*MENU_CHAR_WIDTH-2-45, (OsdMaxY-1)*MENU_CHAR_HEIGHT+MENU_OFFSET,\
		(4*MENU_CHAR_WIDTH+2+appOsdLib_FontGetStringWidthbyId(ID_STR_BACK)),MENU_CHAR_HEIGHT-2,\
			APP_KEY_PRESS_TPSET,0);
	appGuiObjNew_EvtMsgReg((MENU_BACK_ON_OFFSET+8)*MENU_CHAR_WIDTH-2,(OsdMaxY-1)*MENU_CHAR_HEIGHT+MENU_OFFSET,\
			(4*MENU_CHAR_WIDTH+2+appOsdLib_FontGetStringWidthbyId(ID_STR_BACK)),MENU_CHAR_HEIGHT-2,\
			APP_KEY_PRESS_TPMENU,0);


	APP_OSD_REFRESH_ON;
}

void menuProcDisp2Layer(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT16 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, subItem;
	UINT32 strIdx;

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;

	layer--;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);


	menuBodyDisp(layer);
	menuHeaderDisp(layer);
	#if 0
	if(layer == 2)
	{
		osdIconDrawExt(MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET, ID_ICON_MUSUBSC,0);
	}
	#endif
	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);


	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(0, i, strIdx, 1, layer);
			menuIconDisp(col-2, i, icon, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
	}

	layer++;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);
	#if FUNC_HOST_TOUCH_PANEL
	appGuiObjClr(-1);
	#endif
	menuBodyDisp(layer);
 	//menuHeaderDisp(layer);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;

	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		subItem = menuProcSubItemGet(layer, item);
		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(col-6, i, " ", 0, layer);
			appOsdColorSet(MENU_STR_DISABLE);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(54-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-2, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
			}
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN){
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,MENU_BAR_LAYER2_X_OFFSET*2+3,(i*(MENU_CHAR_HEIGHT-2)+MENU_BAR_LAYER2_Y_OFFSET+6)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUSBAR_HDMI, 0x0000);
			}
			else
			#endif
			{
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,MENU_BAR_LAYER2_X_OFFSET,(i*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET), SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUSBAR, 0x0000);
			}
			menuStrDraw(0, i, strIdx, 1, layer);
			if (subItem)
			{
				appOsdColorSet(MENU_TAB_GET_FOCUS);
				#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
				if(IS_HDMI_IN){
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, ((col-2)*MENU_CHAR_WIDTH -9)<<1, ((i+2)*(MENU_CHAR_HEIGHT-2)+5)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MU2LYBAR_HDMI, 0x0000);
				}
				else
				#endif
				{
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (col-2)*MENU_CHAR_WIDTH - 5, (i+2)*MENU_CHAR_HEIGHT-4, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MU2LYBAR, 0x0000);
				}
				appGuiObjNew_EvtMsgReg((col-2)*MENU_CHAR_WIDTH - 5-13, (i+2)*MENU_CHAR_HEIGHT-4-5,40,40,APP_KEY_PRESS_TPLEFT,0);
				appGuiObjNew_EvtMsgReg((col-2)*MENU_CHAR_WIDTH - 5+76-27, (i+2)*MENU_CHAR_HEIGHT-4-5,40,40,APP_KEY_PRESS_TPRIGHT,0);
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
				if(IS_HDMI_IN){
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH*2+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, ((i+2)*(MENU_CHAR_HEIGHT-2))*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				else
				#endif
				{
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
			}
			else
			{
				appGuiObjNew_EvtMsgReg(MENU_BAR_LAYER2_X_OFFSET, (i*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET),WIDTH_ICON_MUSUSBAR,MENU_CHAR_HEIGHT-2,APP_KEY_PRESS_TPSET,0);
			}
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuStrDisp(col-6, i, " ", 0, layer);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
				if(IS_HDMI_IN){
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH*2+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, ((i+2)*(MENU_CHAR_HEIGHT-2))*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				else
				#endif
				{
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
			}

		}
		#if FUNC_HOST_TOUCH_PANEL
		appGuiObjNew_EvtFcsCallBackReg(MENU_BAR_LAYER2_X_OFFSET, (i*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET),\
			WIDTH_ICON_MUSUSBAR,MENU_CHAR_HEIGHT-2,\
			 (pFunc)appMenuDisp2LayerSelItemFuncGui,i,\
			 (pFunc)appMenuDisp2LayerSelItemGetFocusFuncGui,i,\
			 (pFunc)appMenuDisp2LayerSelItemLostFocusFuncGui,i\
			);
		#endif
	}

	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON);
	menuPromptDispReg(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON);

	APP_OSD_REFRESH_ON;
}

void menuProcDispYesNo(UINT16 idx)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
//	UINT16 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem;
	UINT32 strIdx;

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
#if FUNC_HOST_TOUCH_PANEL
	appGuiObjClr(-1);
#endif
	menuLayerSizeGet(layer, &line, &col);

	menuBodyDisp(3);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);;

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
//		icon = menuProcIconIDGet(layer, item);

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i + (line - nodeItem), layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
		}
		printf("disp yes no i=%d\n",i);
		appGuiObjNew_EvtFcsCallBackReg(MENU_BAR_LAYER3_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_BAR_LAYER3_Y_OFFSET,\
			WIDTH_ICON_MUSBAR,MENU_CHAR_HEIGHT-2,\
			(pFunc)appMenuViewSelItemFuncGui,i,\
			(pFunc)appMenuViewSelItemGetFocusFuncGui,i,\
			(pFunc)appMenuViewSelItemLostFocusFuncGui,i\
			);
	}

	appOsdColorSet(MENU_STR_GET_FOCUS);
	appOsd_GfxStringIDDraw((LCD_WIDTH-appOsdLib_FontGetStringWidthbyId((UINT32)idx))/2,  (line-nodeItem)*MENU_CHAR_HEIGHT, SP5K_GFX_ALIGN_TOP_LEFT, (UINT32)idx);
	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_OFF|MENU_PROMPT_PAGEUPDN_ON);
	menuPromptDispReg(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_OFF|MENU_PROMPT_PAGEUPDN_ON);
	APP_OSD_REFRESH_ON;
}
#else
void menuRTCDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
/*	UINT8 icon = menuProcIconIDGet(layer, item);*/
	UINT8 line, col, i;
	UINT8 nodeItem, xpos=0, ypos=1;
	UINT32 strIdx = 1;

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;

	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);/*add for mantis bug 0049188*/
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240 , 0, 0);
	menuLayerSizeGet(layer, &line, &col);

 	nodeItem = menuProcNodeItemGet(layer, item);

	for(i=0, item=0; i<nodeItem; i++,item++)
	{
		UINT32 strWidth;
		UINT8 selItem;
		selItem = menuProcParaGet (layer, menuDateItemConv(item));
		menuProcChildStrGet(layer, menuDateItemConv(item), selItem, &strIdx);
        strWidth = appOsdLib_FontGetStringWidthbyId(strIdx);

		if (menuProcCurrItemGet(layer) == menuDateItemConv(item))
		{
            appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
            /* draw icon when press left(right) key */
            if (IS_HDMI_IN) {
                appOsd_GfxIconDraw(((2*xpos+1)*MENU_CHAR_WIDTH+strWidth/2)-5, (3*(ypos-1)/2)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET+129,
                    SP5K_GFX_ALIGN_CENTER, ID_ICON_TBUTTON_DOWN);
                appOsd_GfxIconDraw(((2*xpos+1)*MENU_CHAR_WIDTH+strWidth/2)-5, (3*(ypos-1)/2)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET+76,
                    SP5K_GFX_ALIGN_CENTER, ID_ICON_TBUTTON_UP);
            }
            else {
                appOsdLib_OsdLayIconDraw((xpos+2)*MENU_CHAR_WIDTH, (ypos-1)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET+61,
                    SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_DOWN);
                appOsdLib_OsdLayIconDraw((xpos+2)*MENU_CHAR_WIDTH, (ypos-1)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET+15,
                    SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_UP);
            }
		}
		else
		{
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		}

		appOsdLib_TextIDDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT,strIdx);
		xpos += appOsdLib_FontGetStringWidthbyId(strIdx) / 13;

		if ((item == 0) || (item == 1))
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			appOsdLib_TextDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"/");
			xpos += strlen("/");
		}
		else if (item == 3)
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			appOsdLib_TextDraw((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)":");
			xpos += strlen(":");
		}
		else if (item == 4)
		{
			if(MENU_CHAR_WIDTH<=8)
				xpos = 2;
			else
				xpos = 3;
			ypos += 2;
		}
		else
			xpos++;
	}

	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);

	APP_OSD_REFRESH_ON;
}
void menuProcDisp2Layer(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT16 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, subItem;
	UINT32 strIdx;
	#if HDMI_MENU_BY_JPG
//	UINT32 p[4];
	#endif

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;

	layer--;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);

	menuBodyDisp(layer);
	menuHeaderDisp(layer);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(0, i, strIdx, 1, layer);
			menuIconDisp(col-2, i, icon, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
	}

	layer++;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);

	menuBodyDisp(layer);
 	//menuHeaderDisp(layer);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;

	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		subItem = menuProcSubItemGet(layer, item);
		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			appOsdColorSet(SUB_MENU_STR_LOST_FOCUS);
			menuStrDisp(col-6, i, (UINT8 *)" ", 0, layer);
			appOsdColorSet(MENU_STR_DISABLE);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(54-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-2, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
			}
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			appOsdColorSet(SUB_MENU_STR_LOST_FOCUS);
			#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN){
				#if HDMI_MENU_BY_JPG
				UINT32 p[4];
				p[0] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[0];
				p[1] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[1]+i*145;
				p[2] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[2];
				p[3] = menuSrcFileInfo[MENU_FOUR_LINES_BAR_HDMI].pos[3];

				sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);

				sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
				sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

				appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER1, menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0], menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1],
					menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[0]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[2],menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[1]+menuSrcFileInfo[MENU_FOUR_LINES_BODY_HDMI].pos[3]);

				sp5kRsvPreloadDraw(PRELOAD_FOUR_LINES_BAR, HDMI_MENU_LAYER1, p, 0);
				/*sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);*/
				#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,MENU_BAR_LAYER2_X_OFFSET*2+3-40,((i*(MENU_CHAR_HEIGHT-2)+MENU_BAR_LAYER2_Y_OFFSET+6)*3>>1)-39-(i*16), SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUSBAR_HDMI, 0x0000);
				#endif
			}
			else
			#endif
           		 {
       			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,MENU_BAR_LAYER2_X_OFFSET,(i*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET), SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)ID_ICONFILE_MUSUSBAR, 0x0000);
			}

			menuStrDraw(0, i, strIdx, 1, layer);
			if (subItem)
			{
				appOsdColorSet(MENU_TAB_GET_FOCUS);
				#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
				if(IS_HDMI_IN){
					#if HDMI_MENU_BY_JPG

					UINT32 p[4];
					sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);

					sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
					sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

					appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER0, menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[0], menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[1],
					menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[0]+menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[2],menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[1]+menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[3]+4*145);

					p[0] = menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[0];
					p[1] = menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[1]+i*145;
					p[2] = menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[2];
					p[3] = menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].pos[3];

					sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,
			 			menuSrcFileInfo[MENU_ADJUST_DIALOG_HDMI].fileName, HDMI_MENU_LAYER0, p, 0);

					p[0] = menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[0];
					p[1] = menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[1]+i*145;
					p[2] = menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[2];
					p[3] = menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].pos[3];

					sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,
			 			menuSrcFileInfo[MENU_ADJUST_LEFT_HDMI].fileName, HDMI_MENU_LAYER0, p, 0);

					p[0] = menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[0];
					p[1] = menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[1]+i*145;
					p[2] = menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[2];
					p[3] = menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].pos[3];

					sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,
			 			menuSrcFileInfo[MENU_ADJUST_RIGHT_HDMI].fileName, HDMI_MENU_LAYER0, p, 0);

					/*sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);*/
					#else
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, ((col-2)*MENU_CHAR_WIDTH -9)<<1, ((i+2)*(MENU_CHAR_HEIGHT-2)+5)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MU2LYBAR_HDMI, 0x0000);
					#endif
				}
				else
                #endif
                {
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (col-2)*MENU_CHAR_WIDTH - 5, (i+2)*MENU_CHAR_HEIGHT-4, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MU2LYBAR, 0x0000);
				}

				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);

				#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
				if(IS_HDMI_IN){
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH*2+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, ((i+2)*(MENU_CHAR_HEIGHT-2))*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				else
                #endif
                {
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}

			}
		}
		else
		{
			appOsdColorSet(SUB_MENU_STR_LOST_FOCUS);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuStrDisp(col-6, i, (UINT8 *)" ", 0, layer);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
				if(IS_HDMI_IN){
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH*2+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, ((i+2)*(MENU_CHAR_HEIGHT-2))*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				else
                #endif
                {
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
			}
		}
	}

	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);

	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN)
	{
		#if HDMI_MENU_BY_JPG
		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
		#endif
	}
	#endif
	APP_OSD_REFRESH_ON;
}

void menuProcDispYesNo(UINT16 idx)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	//UINT16 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem;
	UINT32 strIdx;

	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;

	menuLayerSizeGet(layer, &line, &col);

	menuBodyDisp(3);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuProcStrGet(layer,item, &strIdx);
		//icon = menuProcIconIDGet(layer, item);

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i + (line - nodeItem), layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(2, i + (line - nodeItem), strIdx, 0, layer);
		}
	}

	appOsdColorSet(MENU_STR_GET_FOCUS);
	appOsd_GfxStringIDDraw((LCD_WIDTH-appOsdLib_FontGetStringWidthbyId((UINT32)idx))/2,  (line-nodeItem)*MENU_CHAR_HEIGHT, SP5K_GFX_ALIGN_TOP_LEFT, (UINT32)idx);
	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_OFF);

	APP_OSD_REFRESH_ON;
}
#endif

#if HDMI_MENU_BY_JPG

UINT32 menuHDMIPIPInit(void)
{
	sp5kPipLayerCfg_t pipLayer;

	if(pipHDMIMenuFlag){
		return SUCCESS;
	}
	RPrintf("HDMI menu ---  create PIP layer\n");
	sp5kPipTerm();
	printf("\033[36m %s PIP term \033[0m\n",__FUNCTION__);
	appModeSet(SP5K_MODE_STANDBY);
	sp5kPipInitCfgSet(SP5K_PIP_INIT_MAIN_FRAME_SIZE, 1920, 1080, 0, 0);
	sp5kPipInit();
	printf("\033[36m %s PIP init \033[0m\n",__FUNCTION__);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);

	pipLayer.frmW = 1920;
	pipLayer.frmH = 1088;
	pipLayer.frmRoiW = 1920;
	pipLayer.frmRoiH = 1088;
	pipLayer.winX = 0;
	pipLayer.winY = 0;
	pipLayer.isShow = 0;
	pipLayer.alpha = 32;

	if (SUCCESS != sp5kPipLayerCreate(0, &pipLayer)) {
		printf("PIP layer 0 create err\n");
		return FAIL;
	}

	if (SUCCESS != sp5kPipLayerCreate(1, &pipLayer)) {
		printf("PIP layer 1 create err\n");
		return FAIL;
	}

	if (SUCCESS != sp5kPipLayerCreate(2, &pipLayer)) {
		printf("PIP layer 2 create err\n");
		return FAIL;
	}

	if (SUCCESS != sp5kPipLayerCreate(3, &pipLayer)) {
		printf("PIP layer 3 create err\n");
		return FAIL;
	}

	if (SUCCESS != sp5kPipLayerCreate(4, &pipLayer)) {
		printf("PIP layer 4 create err\n");
		return FAIL;
	}

	pipHDMIMenuFlag = 1;
	return SUCCESS;
}

void menuDlgShow(UINT32 num)
{
	UINT32 fileID = 0;

	switch(num){
		case 2:
			fileID = MENU_TWO_LINES_BODY_HDMI;
			break;
		case 3:
			fileID = MENU_THREE_LINES_BODY_HDMI;
			break;
		case 4:
			fileID = MENU_FOUR_LINES_BODY_HDMI;
			break;
	}

	if(!dlgUpdateFlag){
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

		appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER2, 0, 0,1920,1080);
	}
	dlgUpdateFlag = 1;

	sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,menuSrcFileInfo[fileID].fileName,HDMI_MENU_LAYER2,
		menuSrcFileInfo[fileID].pos,0);

	sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_COLOR_KEY, HDMI_MENU_LAYER2, 1, 1, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER2, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
}

void menuDlgBarShow(UINT32 dlgNum,UINT32 item)
{
	UINT32 p[4];
	UINT8* fileName = menuSrcFileInfo[MENU_THREE_LINES_BAR_HDMI].fileName;

	p[2] = menuSrcFileInfo[MENU_THREE_LINES_BAR_HDMI].pos[2];
	p[3] = menuSrcFileInfo[MENU_THREE_LINES_BAR_HDMI].pos[3];

	switch(dlgNum){
		case 2:
			if(item>1){
				break;
			}
			p[0] = 160;
			p[1] = 660 + item*146;
			break;
		case 3:
			if(item>2){
				break;
			}
			p[0] = 192;
			p[1] = 510 + item*146;
			break;
		case 4:
			if(item>3){
				break;
			}
			p[0] = 200;
			p[1] = 446 + item*146;
			break;
	}

	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);

	appOsd_GfxRectShapeDraw(HDMI_MENU_LAYER1, 0, 0,1920,1080);

	sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,fileName,HDMI_MENU_LAYER1,p,0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_LAYER_ACTIVE, HDMI_MENU_LAYER1, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
}

void menuHDMIClean(void)
{
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 0, 0, 0, 0);
	sp5kGfxPageClear(HDMI_MENU_LAYER2, PAL_NOCOLOR);
	sp5kGfxPageClear(HDMI_MENU_LAYER1, PAL_NOCOLOR);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0);
}
#endif

#if CAM_TYPE_CVR||CAM_TYPE_SPC||CAM_TYPE_DUAL||CAM_TYPE_SBC  /*add for system state*/
void appSystemCardInfoShow(void)
{
	unsigned char totalSizeStr[20],freeSizeStr[30],cardType[20];
	UINT32 tempa,tempb;
	appDiskInfo_t* pDiskInfo;

	if(!IS_CARD_EXIST)
	{
		appOsdLib_PIPLayerClearDraw();
		appOsdLib_OsdLayerClearDraw();
		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, 320, 240, 0, 0);
		APP_OSD_REFRESH_ON;
		appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
		appOsdLib_PIPLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);
		appErrMsgWithSound(ERR_MSG_NO_MEMORY_CARD,ERR_MSG_ONLY,ERR_MSG_SHOW_FOREVER);
		return;
	}

	if(!IS_CARD_EXIST)
	{
		appOsdLib_PIPLayerClearDraw();
		appOsdLib_OsdLayerClearDraw();
		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, 320, 240, 0, 0);
		APP_OSD_REFRESH_ON;
		appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
		appOsdLib_PIPLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);
		appErrMsgWithSound(ERR_MSG_NO_MEMORY_CARD,ERR_MSG_ONLY,ERR_MSG_SHOW_FOREVER);
		return;
	}

	memset(totalSizeStr,0,20);
	memset(freeSizeStr,0,30);
	memset(cardType,0,20);

	pDiskInfo = appCurDiskInfoGet();

	tempa = (UINT32)((float)(pDiskInfo->dskMbSz)/(float)1024);
	tempb = (UINT32)((((float)(pDiskInfo->dskMbSz)/(float)1024)-tempa)*100);
	printf("size:%f\n",(float)(pDiskInfo->dskMbSz)/(float)1024);
	sprintf(totalSizeStr,"Size: %02u.%02u GB",tempa,tempb);
	printf("%s\n",totalSizeStr);

	tempa = (UINT32)((float)pDiskInfo->freeSz/(1024*1024));
	tempb = (UINT32)(((float)pDiskInfo->freeSz/(1024*1024)-tempa)*100);
	/*sprintf(freeSizeStr,"Free: %02d.%02d GB (%02d%%)",tempa,tempb,(100*pDiskInfo->freeSz)/(pDiskInfo->dskMbSz*1024));*/
	/* resolve 64G TF card freesize overflow */
	sprintf(freeSizeStr,"Free: %02u.%02u GB (%02d%%)",tempa,tempb,(10*pDiskInfo->freeSz)/(pDiskInfo->dskMbSz*102));
	printf("%s\n",freeSizeStr);

	if(pDiskInfo->actDsk == SP5K_DISK_NAND){
		sprintf(cardType,"Name: NAND");
	}else{ /* if(pDiskInfo->actDsk == SP5K_DISK_SD){*/
		sprintf(cardType,"Name: SD");
	}

	printf("type:%s\n",cardType);

	//appModeSet(SP5K_MODE_STANDBY);
	//appJpgFileDraw(SP5K_RES_TYPE_JPG,RES_CARD_INFO,0,0,320,240);

	appOsdLib_PIPLayerClearDraw();
	appOsdLib_OsdLayerClearDraw();
	appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, 320, 240, 0, 0);
	APP_OSD_REFRESH_ON;
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(10,40+30,GFX_ALIGN_BOTTOM_LEFT,"Card Information");
	appOsdLib_TextDraw(10,LCD_SIZE_Y/2-35+30,GFX_ALIGN_BOTTOM_LEFT,cardType);
	appOsdLib_TextDraw(10,LCD_SIZE_Y/2-5+30,GFX_ALIGN_BOTTOM_LEFT,totalSizeStr);
	appOsdLib_TextDraw(10,LCD_SIZE_Y/2+25+30,GFX_ALIGN_BOTTOM_LEFT,freeSizeStr);
}


void appSystemMenuInit(void)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT8 buf[128]={0};

	RPrintf("@@-----system  menu \n");
	switch(puiPara->SystemMode){
		case FACTORY_RESET:
			appOsdLib_PIPLayerClearDraw();
			appOsdLib_OsdLayerClearDraw();
			APP_OSD_REFRESH_ON;

			appPlayOsd_DlgBoxConfig(ID_STR_FACTORY_RESET, 2,strId_Reset, 1, 40);
			appPlayOsd_DlgBoxShow();
			appPbOsd_AuxNote_OKSelect(TRUE);
			break;
		case RESET_SYSTEM_INFO:

			appOsdLib_PIPLayerClearDraw();
			appOsdLib_OsdLayerClearDraw();
			appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, 320, 240, 0, 0);
			APP_OSD_REFRESH_ON;
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);

		//	appOsdLib_TextDraw((LCD_SIZE_X-sizeof(appVersionGet()))/2,LCD_SIZE_Y/2,GFX_ALIGN_BOTTOM_CENTER,appVersionGet());

			#if CAM_TYPE_SPC|| CAM_TYPE_CVR||CAM_TYPE_DUAL||CAM_TYPE_SBC   /*modify bug ICOM-297*/
			sprintf(verStr1,"%s.%s", APP_VER, APP_SUN_VER);
			sprintf(verStr2,"VER:%05d", APP_REVISION);
			appOsdLib_TextDraw((LCD_SIZE_X-sizeof(verStr1))/2,LCD_SIZE_Y/2,GFX_ALIGN_BOTTOM_CENTER,verStr1);
			appOsdLib_TextDraw((LCD_SIZE_X-sizeof(verStr2))/2,(LCD_SIZE_Y/2)+25,GFX_ALIGN_BOTTOM_CENTER,verStr2);
			#else
			appOsdLib_TextDraw((LCD_SIZE_X-sizeof(appVersionGet()))/2,LCD_SIZE_Y/2,GFX_ALIGN_BOTTOM_CENTER,appVersionGet());
			#endif
			break;
		case RESET_CARD_INFO:
			appSystemCardInfoShow();
			break;
		case GPS_GSVINFO:
			APP_OSD_REFRESH_OFF;
			GSENSORTimercount=0;
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);

			sprintf(buf,"total:%2d ",0);

			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_TextDraw(20, 40, SP5K_GFX_ALIGN_TOP_LEFT, buf);
			APP_OSD_REFRESH_ON;

			if(RTCTimer == TIMER_NULL){
				RTCTimer = appTimerSet(1000,"RTCTimer");
			}
			break;
	}
}

void appSystemStOnKey(UINT32 msg,UINT32 param)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT32 mode ;
	sp5kModeGet(&mode);

	switch(msg){
		#if FUNC_HOST_TOUCH_PANEL
		case APP_KEY_PRESS_TPDOWN:
		case APP_KEY_PRESS_TPUP:
		#endif
		case APP_KEY_PRESS_DOWN:
		case APP_KEY_PRESS_UP:
			if(puiPara->SystemMode == FACTORY_RESET){
				appBeep(TRUE, FALSE);
				appPlayOsd_DlgBoxScroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
			}
			break;
		#if FUNC_HOST_TOUCH_PANEL
		case APP_KEY_PRESS_TPSET:
		#endif
		case APP_KEY_PRESS_SET:
			appBeep(TRUE, FALSE);
			menuProcCurrPosSet(MENU_MARK_SYSTEM);
			if(puiPara->SystemMode == FACTORY_RESET){
				if(appPlayOsd_DlgBoxSelectGet() == 0){

					if(mode == SP5K_MODE_VIDEO_RECORD)
					{
						menuProcVidRecAlwaysResetExcute();
						break;
					}
					else
					{
						_setupMenuFunctionResetAll();
					}
				}
				appOsdLib_OsdLayerClearDraw();
				//appPlayOsd_DlgBoxClear();
				appStateChange(APP_STATE_MENU,STATE_PARAM_MENU_POS_SET);
			}else{

				if(puiPara->SystemMode == GPS_GSVINFO)
				{
					appTimerClear(&RTCTimer);
					GSENSORTimercount=0;
				}
				appStateChange(APP_STATE_MENU,STATE_PARAM_MENU_POS_SET);
			}

			break;
	}
}

void appSystemStOnMsg(UINT32 msg,UINT32 param)
{
	uiPara_t* puiPara = appUiParaGet();
	 /*ez mark*/
	//appVideoRecAlwaysMenuMsgProc(msg,param);

	switch ( msg) {
	    case SP5K_MSG_TIMER:
		 if(param == RTCTimer){
			GSENSORTimercount++;
			if(GSENSORTimercount==2)
			{
				GSENSORTimercount=0;
				#if GPS_SUPPORT
				appGpsGsvInfoUpdate();
				#endif
			}
		}
		break;
	default:
		break;
	}
}
void appSystemState(UINT32 msg, UINT32 param)
{
	switch(msg){
		case APP_STATE_MSG_INIT:
			appSystemMenuInit();
			break;
		case APP_STATE_MSG_CLOSE:
			CLEAR_OSD_SRC;
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg))
			{
				appSystemStOnKey(msg,param);
			}
			else
			{
				appSystemStOnMsg(msg,param);
			}
			break;
	}
}
#endif

