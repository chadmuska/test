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

#ifndef ___MENU_MICRO_H__
#define ___MENU_MICRO_H__
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* Preview Media Type		*/
typedef enum _PRV_MEDIATYPE
{
	PRV_TYPE_STILL			= (unsigned char) 0x00,
	PRV_TYPE_MOVIE			= (unsigned char) 0x01,
	PRV_TYPE_AUDIO			= (unsigned char) 0x02,
	PRV_TYPE_MAX			= (unsigned char) 0x03,
} PRV_MEDIATYPE;

/* Preview Snap Mode		*/
typedef enum _PRV_SNAPMODE
{
	PRV_SNAP_SINGLE			= (unsigned char) 0x00,
	PRV_SNAP_SELF_2S		= (unsigned char) 0x01,
	PRV_SNAP_SELF_10S		= (unsigned char) 0x02,
	PRV_SNAP_BURST			= (unsigned char) 0x03,
	PRV_SNAP_MAX			= (unsigned char) 0x04,
} PRV_SNAPMODE;

/* Preview Image size		*/
typedef enum _PRV_SIZE
{
	PRV_SIZE_START		= (unsigned char) 0x00,
	PRV_SIZE_9M			= (unsigned char) 0x00,			/*	3456x2592	*/
	PRV_SIZE_7M_16_9	= (unsigned char) 0x01,			/*	3648x2048(16:9)	*/
	PRV_SIZE_5M			= (unsigned char) 0x02,			/* 	2592x1944	*/
	PRV_SIZE_3M			= (unsigned char) 0x03, 		/*	2048x1536	*/
	PRV_SIZE_2M			= (unsigned char) 0x04,  		/*	1600x1200	*/
	PRV_SIZE_2M_16_9	= (unsigned char) 0x05,			/*	1920x1080(16:9)	*/
	PRV_SIZE_1M			= (unsigned char) 0x06, 		/*	1280x1024	*/
	PRV_SIZE_VGA		= (unsigned char) 0x07,
	PRV_SIZE_MAX		= (unsigned char) 0x08,

	PRV_SIZE_QVGA	,
	PRV_SIZE_4M		, 		/*	2272x1704	*/
	PRV_SIZE_4M_P	, 		/*	2272x1504	*/
	PRV_SIZE_5M_P	,
	PRV_SIZE_6M		,
	PRV_SIZE_6M_P	,
} PRV_SIZE;


// Quality Items
typedef enum _PRV_QUALITY
{
	PRV_QUAL_SUPER		= (unsigned char) 0x00,
	PRV_QUAL_FINE		= (unsigned char) 0x01,
	PRV_QUAL_NORMAL		= (unsigned char) 0x02,
	PRV_QUAL_MAX		= (unsigned char) 0x03,
} PRV_QUALITY;
//Stamp Items
typedef enum _PRV_STAMP
{
	PRV_STAMP_OFF 		= (unsigned char) 0x00,
	PRV_STAMP_DATE		= (unsigned char) 0x01,
	PRV_STAMP_DATETIME	= (unsigned char) 0x02,
	PRV_STAMP_MAX		= (unsigned char) 0x03,
} PRV_STAMP;


/*	Preview Edge		*/
typedef enum _PRV_SHARPNESS
{
	PRV_EDGE_HARD				= (unsigned char) 0x00,
	PRV_EDGE_NORMAL				= (unsigned char) 0x01,
	PRV_EDGE_SOFT				= (unsigned char) 0x02,
	PRV_EDGE_MAX				= (unsigned char) 0x03,
} PRV_SHARPNESS;

typedef enum _PRV_FLASH
{
	PRV_STROBE_OFF			= (unsigned char) 0x00,
	PRV_STROBE_AUTO			= (unsigned char) 0x01,	
	PRV_STROBE_ON			= (unsigned char) 0x02,
	PRV_STROBE_MAX			= (unsigned char) 0x03,
} PRV_FLASH;


/*	Preview	ISO		*/
typedef enum _PRV_ISO
{
	PRV_ISO_AUTO			= (unsigned char) 0x00,
	PRV_ISO_100				= (unsigned char) 0x01,
	PRV_ISO_200				= (unsigned char) 0x02,
	PRV_ISO_400				= (unsigned char) 0x03,
	PRV_ISO_800				= (unsigned char) 0x04,
	PRV_ISO_MAX				= (unsigned char) 0x05,

} PRV_ISO;

/*	Preview	FREQ	*/
typedef enum _PRV_FREQ
{
	PRV_FREQ_50			= (unsigned char) 0x00,
	PRV_FREQ_60			= (unsigned char) 0x01,
	PRV_FREQ_AUTO		= (unsigned char) 0x02,
	PRV_FREQ_MAX		= (unsigned char) 0x03,
	
} PRV_FREQ;

/*	Preview	WB Mode		*/
typedef enum _PRV_WB
{
	PRV_WB_AUTO				= (unsigned char) 0x00,
	PRV_WB_DAYLIGHT			= (unsigned char) 0x01,
	PRV_WB_CLOUDY			= (unsigned char) 0x02,
	PRV_WB_FLUORESCENT_H		= (unsigned char) 0x03,
	PRV_WB_TUNGSTEN		= (unsigned char) 0x04,
	/*PRV_WB_FLUORESCENT_L	= (unsigned char) 0x05,*/
	PRV_WB_MAX				= (unsigned char) 0x05,
} PRV_WB;
/*	Preview	Effect		*/

typedef enum _PRV_EFFECT
{
	PRV_EFFECT_NORNAL 		= (unsigned char) 0x00,
	PRV_EFFECT_BLACKWHITE	= (unsigned char) 0x01,
	PRV_EFFECT_SEPIA		= (unsigned char) 0x02,
	PRV_EFFECT_NEGATIVE		= (unsigned char) 0x03,
	PRV_EFFECT_RED			= (unsigned char) 0x04,
	PRV_EFFECT_GREEN		= (unsigned char) 0x05,
	PRV_EFFECT_BLUE			= (unsigned char) 0x06,
	PRV_EFFECT_MAX			= (unsigned char) 0x07,
} PRV_EFFECT;


/*	Preview	AE Metering		*/
typedef enum _PRV_METER
{
	PRV_METER_CENTER		= (unsigned char) 0x00,
	PRV_METER_MULTI			= (unsigned char) 0x01,
	PRV_METER_SPOT			= (unsigned char) 0x02,
	PRV_METER_MAX 			= (unsigned char) 0x04,
	PRV_METER_FP			= (unsigned char) 0x03,//Face Priority
} PRV_METER;


/*	Preview	AF Mode		*/
typedef enum _PRV_FOCUS
{
	PRV_AF_CENTER			= (unsigned char) 0x00,
	PRV_AF_SPOT				= (unsigned char) 0x01,
	PRV_AF_MAX,
	PRV_AF_SUPER_MACRO,
	PRV_AF_MANUAL,
} PRV_FOCUS;

/*	Preview	AF Mode		*/

typedef enum _PRV_SCENE
{	
	PRV_SCENE_AUTO				= (unsigned char) 0x00,
	PRV_SCENE_LANDSCAPE			= (unsigned char) 0x01,
	PRV_SCENE_NIGHTSCENERY		= (unsigned char) 0x02,
	PRV_SCENE_PORTRAIT			= (unsigned char) 0x03,
	PRV_SCENE_NIGHTPORTRAIT		= (unsigned char) 0x04,
	PRV_SCENE_BEACH				= (unsigned char) 0x05,
	PRV_SCENE_SPORT				= (unsigned char) 0x06,
	PRV_SCENE_DINNER				= (unsigned char) 0x07,
	PRV_SCENE_MAX					= (unsigned char) 0x08,
} PRV_SCENE;
/*	Preview	AF Mode		*/


typedef enum _FD_AE_
{	
	FD_AE_ON				= (unsigned char) 0x00,
	FD_AE_OFF			= (unsigned char) 0x01,
	FD_AE_MAX					= (unsigned char) 0x08,
} _FD_AE;




typedef enum _PRV_EV
{		
	PRV_COMP_N20EV		= (unsigned char) 0x00, 		
	PRV_COMP_N17EV		= (unsigned char) 0x01, 
	PRV_COMP_N13EV		= (unsigned char) 0x02, 		
	PRV_COMP_N10EV		= (unsigned char) 0x03, 		
	PRV_COMP_N07EV		= (unsigned char) 0x04, 
	PRV_COMP_N03EV		= (unsigned char) 0x05, 		
	PRV_COMP_0EV			= (unsigned char) 0x06, 		
	PRV_COMP_P03EV		= (unsigned char) 0x07, 
	PRV_COMP_P07EV		= (unsigned char) 0x08, 		
	PRV_COMP_P10EV		= (unsigned char) 0x09, 		
	PRV_COMP_P13EV		= (unsigned char) 0x0A, 
	PRV_COMP_P17EV		= (unsigned char) 0x0B, 		
	PRV_COMP_P20EV		= (unsigned char) 0x0C, 		
	PRV_COMP_MAX		= (unsigned char) 0x0D,           
} PRV_EV;

// contrast Items
typedef enum _PRV_CONTRAST
{
	PRV_CONTRAST_HIGH			= (unsigned char) 0x00,
	PRV_CONTRAST_MIDDLE			= (unsigned char) 0x01,
	PRV_CONTRAST_LOW			= (unsigned char) 0x02,
	PRV_CONTRAST_MAX			= (unsigned char) 0x03,
	

} PRV_CONTRAST;
// saturation Items
typedef enum _PRV_SATURATION
{
	PRV_SATURATION_HIGH			= (unsigned char) 0x00,
	PRV_SATURATION_MIDDLE		= (unsigned char) 0x01,
	PRV_SATURATION_LOW			= (unsigned char) 0x02,
	PRV_SATURATION_MAX			= (unsigned char) 0x03,
} PRV_SATURATION;


// Self Timer Items
typedef enum _PRV_SELF_TIMER
{
	SELF_TIMER_OFF			= (unsigned char) 0x00,
	SELF_TIMER_2SEC			= (unsigned char) 0x01,
	SELF_TIMER_10SEC		= (unsigned char) 0x02,
	SELF_TIMER_DOUBLE		= (unsigned char) 0x03,
	SELF_TIMER_MAX			= (unsigned char) 0x04,
	SELF_TIMER_BURST_SHOOT	= (unsigned char) 0x05,
} PRV_SELF_TIMER;


typedef enum _PRV_PHOTO_FRAME
{
	PRV_PHOTO_FRAME_OFF		= (unsigned char) 0x00,
	PRV_PHOTO_FRAME_A		= (unsigned char) 0x01,
	PRV_PHOTO_FRAME_B		= (unsigned char) 0x02,
	PRV_PHOTO_FRAME_C		= (unsigned char) 0x03,
	PRV_PHOTO_FRAME_D		= (unsigned char) 0x04,
	PRV_PHOTO_FRAME_MAX		= (unsigned char) 0x05,
} PRV_PHOTO_FRAME;

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* Preview Video size		*/
typedef enum _PRV_VIDEOFORMAT
{
	MOV_FORMAT_AVI		= (unsigned char) 0x00,
	MOV_FORMAT_ASF		= (unsigned char) 0x01,
	MOV_FORMAT_MP3		= (unsigned char) 0x02,
	MOV_FORMAT_MAX		= (unsigned char) 0x03,
} PRV_VIDEOFORMAT;

/* Preview Video Record Mode	*/
typedef enum _PRV_VIDEORECORDMODE
{
	MOV_MODE_NORMAL		= (unsigned char) 0x00,
	MOV_MODE_SILENT		= (unsigned char) 0x01,
	MOV_MODE_VOICE		= (unsigned char) 0x02,
} PRV_VIDEORECORDMODE;
// SIZE Items

typedef enum _PRV_VIDEOSIZE
{
	MOV_SIZE_1080P		= (unsigned char) 0x00,
	MOV_SIZE_720P_30	= (unsigned char) 0x01,
	MOV_SIZE_720P_60	= (unsigned char) 0x02,
	MOV_SIZE_640		= (unsigned char) 0x03,
	MOV_SIZE_320		= (unsigned char) 0x04,
	MOV_SIZE_MAX		= (unsigned char) 0x05,
	MOV_SIZE_QVGA_HFR	= (unsigned char) 0x06,
	
	//MOV_SIZE_160		= (unsigned char) 0x04,
} PRV_VIDEOSIZE;

// Quality Items
typedef enum _MOV_QUALITY
{
	MOV_QUAL_SUPER	= (unsigned char) 0x00,
	MOV_QUAL_FINE		= (unsigned char) 0x01,
	MOV_QUAL_NORMAL	= (unsigned char) 0x02,
	MOV_QUAL_MAX		= (unsigned char) 0x03,
} MOV_QUALITY;

// Seamless Items
typedef enum _MOV_SEAMLESS
{
	MOV_SEAMLESS_OFF		= (unsigned char) 0x00,
	MOV_SEAMLESS_1MIN	= (unsigned char) 0x01,
	MOV_SEAMLESS_3MIN	= (unsigned char) 0x02,
	MOV_SEAMLESS_5MIN	= (unsigned char) 0x03,
	MOV_SEAMLESS_MAX		= (unsigned char) 0x04,
}MOV_SEAMLESS_OFF;

// Frame Items
typedef enum _MOV_FRMRATE
{
	MOV_FRM_HIGH		= (unsigned char) 0x00,
	MOV_FRM_MIDDLE	= (unsigned char) 0x01,
	MOV_FRM_LOW		= (unsigned char) 0x02,
	MOV_FRM_MAX		= (unsigned char) 0x03,	
} MOV_FRMRATE;

// mic Items
typedef enum _MOV_MIC
{
	PRV_MIC_ON			= (unsigned char) 0x00,
	PRV_MIC_OFF		= (unsigned char) 0x01,
	PRV_MIC_MAX		= (unsigned char) 0x02,
} MOV_MIC;
// stabilizer
typedef enum _MOV_STABILIZER
{
	PRV_STABILIZER_ON		= (unsigned char) 0x00,
	PRV_STABILIZER_OFF	= (unsigned char) 0x01,
	PRV_STABILIZER_MAX	= (unsigned char) 0x02,
} MOV_STABILIZER;

// Rolling Shutter
typedef enum _MOV_ROLLING_SHUTTER
{
	MOV_ROLLINGSHUTTER_ON	= (unsigned char) 0x00,
	MOV_ROLLINGSHUTTER_OFF	= (unsigned char) 0x01,
	MOV_ROLLINGSHUTTER_MAX	= (unsigned char) 0x02,
}MOV_ROLLING_SHUTER;

/*	AE Metering		*/
typedef enum _MOV_METER
{
	MOV_METER_CENTER		= (unsigned char) 0x00,
	MOV_METER_MULTI		= (unsigned char) 0x01,
	MOV_METER_SPOT		= (unsigned char) 0x02,
	MOV_METER_MAX 		= (unsigned char) 0x03,
} MOV_METER;

/*	Preview	WB Mode		*/
typedef enum _MOV_WB
{
	MOV_WB_AUTO				= (unsigned char) 0x00,
	MOV_WB_DAYLIGHT			= (unsigned char) 0x01,
	MOV_WB_CLOUDY			= (unsigned char) 0x02,
	MOV_WB_TUNGSTEN			= (unsigned char) 0x03,
	MOV_WB_FLUORESCENT_H	= (unsigned char) 0x04,
	MOV_WB_FLUORESCENT_L	= (unsigned char) 0x05,
	MOV_WB_CUSTOM			= (unsigned char) 0x06,
	MOV_WB_MAX				= (unsigned char) 0x07,
} MOV_WB;

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*effect item*/
typedef enum _PB_EFFECT
{
	PB_EFFECT_BLACK_WHITE 	= (unsigned char) 0x00,
	PB_EFFECT_SEPIA			= (unsigned char) 0x01,
	PB_EFFECT_NEGA 			= (unsigned char) 0x02,
	PB_EFFECT_RED 			= (unsigned char) 0x03,
	PB_EFFECT_GREEN 		= (unsigned char) 0x04,
	PB_EFFECT_BLUE 			= (unsigned char) 0x05,
	PB_EFFECT_BINARIZE 	= (unsigned char) 0x06,
	PB_EFFECT_SUNSET			= (unsigned char) 0x07,
	PB_EFFECT_WARM 			= (unsigned char) 0x08,
	PB_EFFECT_COLD			= (unsigned char) 0x09,
	PB_EFFECT_CONTRAST		= (unsigned char) 0x0A,
	PB_EFFECT_MAX 			= (unsigned char) 0x0B,
} PB_EFFECT;

// storage Items
typedef enum _PRV_OPENPIC
{
	PB_OPENPIC_OFF 			= (unsigned char) 0x00,
	PB_OPENPIC_DEFAULT		= (unsigned char) 0x01,
	PB_OPENPIC_USER_DEF		= (unsigned char) 0x02,
	PB_OPENPIC_MAX			= (unsigned char) 0x03,
} PRV_OPENPIC;

// slide interval Items
typedef enum _PB_INTERVAL
{
	PB_INTERVAL_1S 		= (unsigned char) 0x00,
	PB_INTERVAL_2S 		= (unsigned char) 0x01,
	PB_INTERVAL_3S 		= (unsigned char) 0x02,
	PB_INTERVAL_4S 		= (unsigned char) 0x03,
	PB_INTERVAL_5S 		= (unsigned char) 0x04,
	PB_INTERVAL_6S 		= (unsigned char) 0x05,
	PB_INTERVAL_7S 		= (unsigned char) 0x06,
	PB_INTERVAL_8S 		= (unsigned char) 0x07,
	PB_INTERVAL_9S 		= (unsigned char) 0x08,
	PB_INTERVAL_10S 	= (unsigned char) 0x09,
	PB_INTERVAL_MAX 	= (unsigned char) 0x0a,
} PB_INTERVAL;

// slide repeate Items
typedef enum _PB_REPEATE
{
	PB_REPEATE_YES 		= (unsigned char) 0x00,
	PB_REPEATE_NO 		= (unsigned char) 0x01,
	PB_REPEATE_MAX 		= (unsigned char) 0x02,
} PB_PB_REPEATE;

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*media type*/
typedef enum _PRV_MEDIA_TYPE{
	MEDIA_IN_MEM	= (unsigned char) 0x00,
	MEDIA_SD		= (unsigned char) 0x01,
	MEDIA_MAX		= (unsigned char) 0x02,
} PRV_MEDIA_TYPE;

/*	Setup Menu QUICK VIEW		*/
typedef enum _QUICKVIEW
{
	QUICKVIEW_OFF		= (unsigned char) 0x00,
	QUICKVIEW_1S		= (unsigned char) 0x01,
	QUICKVIEW_3S		= (unsigned char) 0x02,
	QUICKVIEW_MAX		= (unsigned char) 0x03,
} QUICKVIEW;

typedef enum _BEEPSOUND
{
	BEEP_ON		= (unsigned char) 0x00,
	BEEP_OFF			= (unsigned char) 0x01,
	BEEP_MAX		= (unsigned char) 0x02,
} BEEPSOUND;

typedef enum _BLINKMODE
{
	BLINKMODE_ON		= (unsigned char) 0x00,
	BLINKMODE_OFF			= (unsigned char) 0x01,
	BLINKMODE_MAX		= (unsigned char) 0x02,
} BLINKMODE;


typedef enum _SMILEMODE
{
	SMILEMODE_ON		= (unsigned char) 0x00,
	SMILEMODE_OFF			= (unsigned char) 0x01,
	SMILEMODE_MAX		= (unsigned char) 0x02,
} SMILEMODE;

typedef enum _REDEYEPB
{
	REDEYEPB_ON		= (unsigned char) 0x00,
	REDEYEPB_OFF		= (unsigned char) 0x01,
	REDEYEPB_SAVE		= (unsigned char) 0x02,
	REDEYEPB_MAX		= (unsigned char) 0x03,
} REDEYEPB;
typedef enum _WDRMODE
{
	WDRMODE_ON		= (unsigned char) 0x00,
	WDRMODE_OFF			= (unsigned char) 0x01,
	WDRMODE_MAX		= (unsigned char) 0x02,
} WDRMODE;

typedef enum _FDMODE
{
	FDMODE_ON		= (unsigned char) 0x00,
	FDMODE_OFF			= (unsigned char) 0x01,
	FDMODE_MAX		= (unsigned char) 0x02,
} FDMODE;


typedef enum _DISMODE
{
	DISMODE_ON		= (unsigned char) 0x00,
	DISMODE_OFF			= (unsigned char) 0x01,
	DISMODE_MAX		= (unsigned char) 0x02,
} DISMODE;


typedef enum _STARTSOUND
{
	STARTSOUND_NONE		= (unsigned char) 0x00,
	STARTSOUND_1		= (unsigned char) 0x01,
	STARTSOUND_2		= (unsigned char) 0x02,
	STARTSOUND_3		= (unsigned char) 0x03,
	STARTSOUND_MAX		= (unsigned char) 0x04,
} STARTSOUND;

typedef enum _SHUTTERSOUND
{
	SHUTTER_ON		= (unsigned char) 0x00,
	SHUTTER_OFF		= (unsigned char) 0x01,
	SHUTTER_MAX		= (unsigned char) 0x02,
} SHUTTERSOUND;

typedef enum _VOLUME
{
	VOLUME_0		= (unsigned char) 0x00,
	VOLUME_1		= (unsigned char) 0x01,
	VOLUME_2		= (unsigned char) 0x02,
	VOLUME_3		= (unsigned char) 0x03,
	VOLUME_MAX		= (unsigned char) 0x04,
} VOLUME;

typedef enum _TVMODE
{
	TVMODE_NTSC		= (unsigned char) 0x00,
	TVMODE_PAL		= (unsigned char) 0x01,
	TVMODE_MAX		= (unsigned char) 0x02,
} TVMODE;

typedef enum _DATESTYLE
{
	DATESTYLE_MMDDYYYY	= (unsigned char) 0x00,
	DATESTYLE_DDMMYYYY	= (unsigned char) 0x01,
	DATESTYLE_YYYYMMDD	= (unsigned char) 0x02,
	DATESTYLE_MAX		= (unsigned char) 0x03,
} DATESTYLE;

typedef enum _STARTIMAGE
{
	STARTIMAGE_OFF		= (unsigned char) 0x00,
	STARTIMAGE_SYS		= (unsigned char) 0x01,
	STARTIMAGE_DEF		= (unsigned char) 0x02,
	STARTIMAGE_MAX		= (unsigned char) 0x04,
} STARTIMAGE;

typedef enum _LANGUAGE
{
	ENGLISH				= (unsigned char) 0x00,
	S_CHINESE			= (unsigned char) 0x01,
	T_CHINESE			= (unsigned char) 0x02,
	LANGUAGE_MAX		= (unsigned char) 0x03,
} LANGUAGE;

typedef enum _SLEEPTIME
{
	POWER_SAVE_1MIN		= (unsigned char) 0x00,
	POWER_SAVE_3MIN		= (unsigned char) 0x01,		
	POWER_SAVE_5MIN		= (unsigned char) 0x02,	
	POWER_SAVE_OFF		= (unsigned char) 0x03,
	POWER_SAVE_MAX		= (unsigned char) 0x04,
	//POWER_SAVE_OFF
} SLEEPTIEM;

typedef enum _USBMODE
{
	USB_MASS		= (unsigned char) 0x00,
	USB_PCCAM		= (unsigned char) 0x01,
	USB_SIDC		= (unsigned char) 0x02,
	USB_MAX			= (unsigned char) 0x03,	
} USBMODE;

typedef enum _LCD_BRIGHTNESS
{
	LCD_BRIGHT_DARK		= (unsigned char) 0x00,
	LCD_BRIGHT_NORMAL	= (unsigned char) 0x01,
	LCD_BRIGHT_LIGHT	= (unsigned char) 0x02,
	LCD_BRIGHT_MAX		= (unsigned char) 0x03,
} LCD_BRIGHTNESS;

/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */

typedef enum _DPS_PRINT
{
	DPS_TO_CON_PRINT 		= (unsigned char) 0x00,
	DPS_PRINT_CANCEL		= (unsigned char) 0x01,
	DPS_PRINT_CON_OK		= (unsigned char) 0x02,
	DPS_PRINT_DOING			= (unsigned char) 0x03,
	DPS_PRINT_ABORT			= (unsigned char) 0x04,
	
} DPS_PRINT;
typedef enum _DPS_IMAGEMODE
{
	DPS_IMAGE_SIGNLE        = (unsigned char) 0x00,
	DPS_IMAGE_ALL           = (unsigned char) 0x01,
	DPS_IMAGE_CHOOSE		= (unsigned char) 0x02,

}DPS_IMAGEMODE;
/*typedef enum _DPS_PRINTMODE
{
	DPS_PRINTMODE_STANDARD		= ( unsigned char ) 0x00,
	DPS_PRINTMODE_INDEX		= ( unsigned char ) 0x01,
} DPS_PRINTMODE;*/

typedef enum _DPS_QUALITY
{
	DPS_QUALITY_AUTO		= ( unsigned char ) 0x00,
	DPS_QUALITY_FINE		= ( unsigned char ) 0x01,
	DPS_QUALITY_NORMAL	= ( unsigned char ) 0x02,
	DPS_QUALITY_DRAFT		= ( unsigned char ) 0x03,
} DPS_QUALITY;

typedef enum _DPS_LAYOUT
{
	DPS_LAYOUT_AUTO		= ( unsigned char ) 0x00,
	DPS_LAYOUT_1		= ( unsigned char ) 0x01,
	DPS_LAYOUT_2		= ( unsigned char ) 0x02,
	DPS_LAYOUT_4		= ( unsigned char ) 0x03,
	DPS_LAYOUT_6		= ( unsigned char ) 0x04,
	DPS_LAYOUT_8		= ( unsigned char ) 0x05,
	DPS_LAYOUT_FULL		= ( unsigned char ) 0x06,
	DPS_LAYOUT_INDEX    = ( unsigned char ) 0x07, 
} DPS_LAYOUT;

typedef enum _DPS_PAPERSIZE
{
	DPS_PAPERSIZE_AUTO		= ( unsigned char ) 0x00,
	DPS_PAPERSIZE_L		= ( unsigned char ) 0x01,
	DPS_PAPERSIZE_2L		= ( unsigned char ) 0x02,
	DPS_PAPERSIZE_POSTCARD		= ( unsigned char ) 0x03,
	DPS_PAPERSIZE_100_150_MM		= ( unsigned char ) 0x04,
	DPS_PAPERSIZE_4_6_INCH		= ( unsigned char ) 0x05,
	DPS_PAPERSIZE_8_10_INCH		= ( unsigned char ) 0x06,
	DPS_PAPERSIZE_LETTER		= ( unsigned char ) 0x07,
	DPS_PAPERSIZE_A4		= ( unsigned char ) 0x08,
	DPS_PAPERSIZE_A3		= ( unsigned char ) 0x09,
} DPS_PAPERSIZE;

typedef enum _DPS_PAPERTYPE
{
	DPS_PAPERTYPE_AUTO		= ( unsigned char ) 0x00,
	DPS_PAPERTYPE_PLAIN		= ( unsigned char ) 0x01,
	DPS_PAPERTYPE_PHOTO		= ( unsigned char ) 0x02,
	DPS_PAPERTYPE_F_PHOTO		= ( unsigned char ) 0x03,
} DPS_PAPERTYPE;

typedef enum _DPS_DATE
{
	DPS_DATE_AUTO		= ( unsigned char ) 0x00,
	DPS_DATE_OFF		= ( unsigned char ) 0x01,
	DPS_DATE_ON	= ( unsigned char ) 0x02,
} DPS_DATE;

typedef enum _DPS_FILENAME
{
	DPS_FILENAME_AUTO		= ( unsigned char ) 0x00,
	DPS_FILENAME_OFF		= ( unsigned char ) 0x01,
	DPS_FILENAME_ON	= ( unsigned char ) 0x02,
} DPS_FILENAME;
typedef enum _DPS_DATETYPE
{
	DPS_DATESTYPE_YYMMDD		= ( unsigned char ) 0x00,
	DPS_DATESTYPE_DDMMYY		= ( unsigned char ) 0x01,
	DPS_DATESTYPE_MMDDYY	= ( unsigned char ) 0x02,
} DPS_DATETYPE;

typedef enum _SPEEDYCAPTURE
{
	SPEEDY_CAPTURE_ON		= ( unsigned char ) 0x00,
	SPEEDY_CAPTURE_OFF	= ( unsigned char ) 0x01,
	SPEEDY_CAPTURE_MAX	= (unsigned char) 0x02,

} SPEEDYCAPTURE;



typedef enum _PRV_ANTISHAKE             
{
	PRV_ANTISHAKE_ON 		= (unsigned char) 0x00,
	PRV_ANTISHAKE_OFF 	= (unsigned char) 0x01,	
	PRV_ANTISHAKE_MAX	= (unsigned char) 0x02,
} PRV_ANTISHAKE;

typedef enum _REC_ADVANCEDENOISE
{
	REC_ADVANCEDENOISE_ON = (unsigned char) 0x00,
	REC_ADVANCEDENOISE_OFF = (unsigned char) 0x01,
	REC_ADVANCEDENOISE_MAX = (unsigned char) 0x02,
} REC_ADVANCEDENOISE;
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
#endif /* ___MENU_MICRO_H__ */
