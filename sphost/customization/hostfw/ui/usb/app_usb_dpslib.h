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

#ifndef _APP_USBDPSLIB_H_
#define _APP_USBDPSLIB_H_

#include "app_menu.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

 typedef struct dpsImage_s
 {
	UINT32 		totalImg;
	UINT32		curImgIdx;
 } dpsImage_t,*PdpsImage_t;

typedef struct dpsInfo_s {
	UINT16		dpsModePara;
	UINT32		SelPic;
	UINT32 		TotalSelPics;
	UINT8 		*PicBuf;
	UINT8		IndexPrintCap;
	UINT8		DpofPrintCap;
	UINT8       PatialPics;
} dpsInfo_t, *PdpsInfo_t;

typedef struct dpsMenuItemEn_s{
	UINT32  OsdEnOption;
	UINT16  SelOption;
} dpsMenuItemEn_t, *PdpsMenuItemEn_t;

typedef struct dpsMenuOption_s {
	dpsMenuItemEn_t Print;
	dpsMenuItemEn_t Image;
	dpsMenuItemEn_t Quality;
	dpsMenuItemEn_t Layout;
	dpsMenuItemEn_t PaperSize;
	dpsMenuItemEn_t PaperType;
	dpsMenuItemEn_t DateTime;
	dpsMenuItemEn_t DateType;
	dpsMenuItemEn_t FileNo;
	dpsMenuItemEn_t Reset;
} dpsMenuOption_t, *PdpsMenuOption_t;

enum {
	DPS_SELECT_ONE  	= 0x00,
	DPS_SELECT_ALL  	= 0x01,
	DPS_SELECT_TOTAL  	= 0x02,
};

enum {
	DPS_PAPER_QUALITY_AUTO	 	= 0x00,
	DPS_PAPER_QUALITY_FINE  	= 0x01,
	DPS_PAPER_QUALITY_NORMAL 	= 0x02,
	DPS_PAPER_QUALITY_DRAFT	 	= 0x03,
	DPS_PAPER_QUALITY_TOTAL	 	= 0x04,
};

enum {
	DPS_PAPER_TYPE_AUTO			= 0x00,
	DPS_PAPER_TYPE_PLAIN		= 0x01,
	DPS_PAPER_TYPE_PHOTO		= 0x02,
	DPS_PAPER_TYPE_FAST_PHOTO	= 0x03,
	DPS_PAPER_TYPE_TOTAL		= 0x04,
};

enum {
	DPS_PAPER_SIZE_AUTO		    = 0x00,
	DPS_PAPER_SIZE_L		    = 0x01,
	DPS_PAPER_SIZE_2L		    = 0x02,
	DPS_PAPER_SIZE_POSTCARD	    = 0x03,
	DPS_PAPER_SIZE_100x150	    = 0x04,
	DPS_PAPER_SIZE_4X6		    = 0x05,
	DPS_PAPER_SIZE_8x10		    = 0x06,
	DPS_PAPER_LETTER		    = 0x07,
	DPS_PAPER_SIZE_A4		    = 0x08,
	DPS_PAPER_SIZE_A3		    = 0x09,
	DPS_PAPER_SIZE_TOTAL		= 0x0A,
};

enum {
	DPS_FILE_AUTO 				= 0x00,
	DPS_FILE_OFF 				= 0x01,
	DPS_FILE_ON					= 0x02,
	DPS_FILE_TOTAL				= 0x03,
};

enum {
	DPS_PRINT_STANDARD	        = 0x00,
	DPS_PRINT_INDEX		        = 0x01,
	DPS_PRINT_DPOF			    = 0x02,
	DPS_PRINT_TOTAL		        = 0x03,
};

enum {
	DPS_IMAGE_SIGNAL		    = 0x00,
	DPS_IMAGE_ALL			    = 0x01,
	DPS_IMAGE_TOTAL		        = 0x02,
};

enum {
	DPS_QUALITY_AUTO		    = 0x00,
	DPS_QUALITY_NORMAL	        = 0x01,
	DPS_QUALITY_DRAFT		    = 0x02,
	DPS_QUALITY_FINE		    = 0x03,
	DPS_QUALITY_TOTAL		    = 0x04,
};

enum {
	DPS_LAYOUT_AUTO		        = 0x00,
	DPS_LAYOUT_1			    = 0x01,
	DPS_LAYOUT_2			    = 0x02,
	DPS_LAYOUT_3			    = 0x03,
	DPS_LAYOUT_4			    = 0x04,
	DPS_LAYOUT_5			    = 0x05,
	DPS_LAYOUT_6			    = 0x06,
	DPS_LAYOUT_7			    = 0x07,
	DPS_LAYOUT_8			    = 0x08,
	DPS_LAYOUT_9			    = 0x09,
	DPS_LAYOUT_16			    = 0x0A,
	DPS_LAYOUT_TOTAL		    = 0x0B,
};

enum {
	DPS_PAPERSIZE_AUTO		    = 0x00,
	DPS_PAPERSIZE_L			    = 0x01,
	DPS_PAPERSIZE_2L			= 0x02,
	DPS_PAPERSIZE_POSTCARD	    = 0x03,
	DPS_PAPERSIZE_CARDSIZE	    = 0x04,
	DPS_PAPERSIZE_4X6			= 0x05,
	DPS_PAPERSIZE_8X10		    = 0x06,
	DPS_PAPERSIZE_LETTER		= 0x07,
	DPS_PAPERSIZE_11X17		    = 0x08,
	DPS_PAPERSIZE_A3			= 0x09,
	DPS_PAPERSIZE_A4			= 0x0A,
	DPS_PAPERSIZE_A6			= 0x0B,
	DPS_PAPERSIZE_B0			= 0x0C,
	DPS_PAPERSIZE_B1			= 0x0D,
	DPS_PAPERSIZE_ROLL_L		= 0x0E,
	DPS_PAPERSIZE_ROLL_2L		= 0x0F,
	DPS_PAPERSIZE_TOTAL		    = 0x10,
};

enum {
	DPS_PAPERTYPE_AUTO		    = 0x00,
	DPS_PAPERTYPE_PLAIN		    = 0x01,
	DPS_PAPERTYPE_PHOTO		    = 0x02,
	DPS_PAPERTYPE_FASTPHOTO	    = 0x03,
	DPS_PAPERTYPE_TOTAL		    = 0x04,
};

enum {
	DPS_DATE_AUTO		        = 0x00,
	DPS_DATE_OFF		        = 0x01,
	DPS_DATE_ON		            = 0x02,
	DPS_DATE_TOTAL	            = 0x03,
};

enum {
	DPS_DATETYPE_YYMMDD		    = 0x00,
	DPS_DATETYPE_DDMMYY		    = 0x01,
	DPS_DATETYPE_MMDDYY		    = 0x02,
	DPS_DATETYPE_TOTAL		    = 0x03,
};

enum {
	DPS_FILENO_AUTO	            = 0x00,
	DPS_FILENO_OFF	            = 0x01,
	DPS_FILENO_ON		        = 0x02,
	DPS_FILENO_TOTAL	        = 0x03,
};

enum {
	DPS_RESET_ON		        = 0x00,
	DPS_RESET_OFF		        = 0x01,
	DPS_RESET_TOTAL	            = 0x02,
};

typedef struct dpsPrintCapbilityPIMACode_s{
	UINT32 Quality[DPS_QUALITY_TOTAL];		
	UINT32 paperSize[DPS_PAPERSIZE_TOTAL];		
	UINT32 paperType[DPS_PAPERTYPE_TOTAL];	
	UINT32 DateTime[DPS_DATE_TOTAL];	
	UINT32 FileNo[DPS_FILENO_TOTAL];	
	UINT32 Layout[DPS_LAYOUT_TOTAL];	
	UINT32 DateType[DPS_DATETYPE_TOTAL];			
}dpsPrintCapbilityPIMACode_t, *PdpsPrintCapbilityPIMACode_t;


/**************************************************************************
 *                           VARIABLE                          *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

#define PIMA_QUALITY_AUTO					0x50000000
#define PIMA_QUALITY_NORMAL				    0x50010000
#define PIMA_QUALITY_DRAFT					0x50020000
#define PIMA_QUALITY_FINE					0x50030000

#define PIMA_PAPERSIZE_AUTO				    0x51000000
#define PIMA_PAPERSIZE_L					0x51010000
#define PIMA_PAPERSIZE_2L					0x51020000
#define PIMA_PAPERSIZE_POSTCARD			    0x51030000
#define PIMA_PAPERSIZE_CARDSIZE			    0x51040000
#define PIMA_PAPERSIZE_4X6					0x51060000
#define PIMA_PAPERSIZE_8X10				    0x51070000
#define PIMA_PAPERSIZE_LETTER				0x51080000
#define PIMA_PAPERSIZE_11X17				0x510A0000
#define PIMA_PAPERSIZE_A3					0x51130000
#define PIMA_PAPERSIZE_A4					0x51140000
#define PIMA_PAPERSIZE_A6					0x51160000
#define PIMA_PAPERSIZE_B0					0x51200000
#define PIMA_PAPERSIZE_B1					0x51210000
#define PIMA_PAPERSIZE_ROLL_L				0x51810000
#define PIMA_PAPERSIZE_ROLL_2L				0x51820000

#define PIMA_PAPERTYPE_AUTO				    0x52000000
#define PIMA_PAPERTYPE_PLAIN				0x52010000
#define PIMA_PAPERTYPE_PHOTO				0x52020000
#define PIMA_PAPERTYPE_FASTPHOTO			0x52030000

#define PIMA_DATETIME_AUTO					0x54000000
#define PIMA_DATETIME_OFF					0x54010000
#define PIMA_DATETIME_ON					0x54020000

#define PIMA_FILENAME_AUTO					0x55000000
#define PIMA_FILENAME_OFF					0x55010000
#define PIMA_FILENAME_ON					0x55020000

#define PIMA_LAYOUT_AUTO					0x57000000
#define PIMA_LAYOUT_1						0x57010000
#define PIMA_LAYOUT_2						0x57020000
#define PIMA_LAYOUT_3						0x57030000
#define PIMA_LAYOUT_4						0x57040000
#define PIMA_LAYOUT_5						0x57050000
#define PIMA_LAYOUT_6						0x57060000
#define PIMA_LAYOUT_7						0x57070000
#define PIMA_LAYOUT_8						0x57080000
#define PIMA_LAYOUT_9						0x57090000
#define PIMA_LAYOUT_16						0x57100000

#define PIMA_LAYOUT_INDEX					0x57FE0000

#define PIMA_FILETYPE_AUTO					0x53000000
#define PIMA_FILETYPE_EXIFJPG				0x53010000
#define PIMA_FILETYPE_JPG					0x53030000
#define PIMA_FILETYPE_DPOF					0x53180000


#define SP5K_DPS_DATETYPE_YYMMDD			0x01030001
#define SP5K_DPS_DATETYPE_DDMMYY			0x01030002
#define SP5K_DPS_DATETYPE_MMDDYY			0x01030003

 
#define DPS_CAPABILITY_SUCCESS		0x00

#define DPS_STANDBYMODE			    0x00
#define DPS_PRINTINGMODE			0x01
#define DPS_PAUSEMODE				0x02
#define DPS_HALTEDMODE				0x03
#define DPS_CANCELINGMODE			0x04
#define DPS_PRINTCOMPLETEMODE		0x05
#define DPS_PRINT_ALL_COMPLETEMODE	0x06
#define DPS_NO_IMAGEMODE			0x07
#define DPS_TIME_OUTMODE			0x08
#define DPS_PRINTER_ERRORMODE		0x09
#define DPSUSBERRORMODE				0x0A

#define DPS_PAPERSIZE_CAP_BUF		64

#define PIMA_SERVICE_STATUS_PRINTING    0x70000000
#define PIMA_SERVICESTATUS_IDLE   		0x70010000
#define PIMA_SERVICESTATUS_PAUSED   	0x70020000

#define PIMA_JOBEND_REASON_NOTEND   	0x71000000
#define PIMA_JOBEND_REASON_ABORTJOB   	0x71020000
#define PIMA_JOBEND_REASON_OTHERS   	0x71040000

#define PIMA_ERROR_STATUS_NO_ERROR   	0x72000000
#define PIMA_ERROR_STATUS_WARNING   	0x72010000
#define PIMA_ERROR_STATUS_FATAL_ERROR   0x72020000

#define PIMA_ERROR_NO_REASON   			0x73000000

#define PIMA_ERROR_PAPER_RELATED   		0x73010000
#define PIMA_ERROR_PAPER_EMPTY   		0x73010100
#define PIMA_ERROR_LOAD_ERROR   		0x73010200
#define PIMA_ERROR_EJECT_ERROR   		0x73010300
#define PIMA_ERROR_MEDIA_ERROR   		0x73010400
#define PIMA_ERROR_PAPER_JAM   			0x73010500
#define PIMA_ERROR_PAPER_NEARLY_EMPTY   0x73010600
#define PIMA_ERROR_PAPER_TYPE_NOTSUPPORT 0x73010700

#define PIMA_ERROR_INK_RELATED   		0x73020000
#define PIMA_ERROR_INK_EMPTY   			0x73020100
#define PIMA_ERROR_INK_LOW   			0x73020200
#define PIMA_ERROR_WASTE_INK_ERROR   	0x73020300

#define PIMA_ERROR_HARDWARE_RELATEDR	0x73030000
#define PIMA_ERROR_FATAL_ERROR  		0x73030100
#define PIMA_ERROR_SERVICE_CALL  		0x73030200
#define PIMA_ERROR_PRINT_UNAVAILABLE 	0x73030300
#define PIMA_ERROR_PRINT_BUSY 			0x73030400
#define PIMA_ERROR_LEVER_ERROR 			0x73030500
#define PIMA_ERROR_COVER_OPEN 			0x73030600
#define PIMA_ERROR_NO_MARKING_AGENT		0x73030700
#define PIMA_ERROR_INK_COVER_OPEN		0x73030800
#define PIMA_ERROR_NO_INK_CARTRIDGE		0x73030900

#define PIMA_ERROR_FILE_RELATED   		0x73040000
#define PIMA_ERROR_PRINTINFO_ERROR   	0x73040100
#define PIMA_ERROR_FILE_DECODE_ERROR   	0x73040200

#define	appUsbDps_PrintStart			sp5kDpsPrtAction(SP5K_ACTION_START)
#define	appUsbDps_PrintAbort			sp5kDpsPrtAction(SP5K_ACTION_ABORT)
#define	appUsbDps_PrintContinue			sp5kDpsPrtAction(SP5K_ACTION_CONTINUE)
#define	appUsbDps_PrintDpof			    sp5kDpsPrtAction(SP5K_ACTION_START_DPOF)

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

void appUsbDps_MenuCapabilityOptionSet(UINT32 *databuf,unsigned char datalen,UINT32 *pimaTbl,unsigned char totalItem,PdpsMenuItemEn_t pEnOpt);
void appUsbDps_PaperSizeCapabilityGet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_PaperLayoutCapabilityGet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_PaperTypeCapabilityGet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_PaperQualityCapabilityGet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_DateTimePrintCapabilityGet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_FileNamePrintCapabilityGet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_IndexPrintCapabilityGet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_DPOFPrintCapabilityGet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_PaperQualityConfigSet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_PaperSizeConfigSet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_StandardPrintConfigSet(void);
void appUsbDps_DPOFPrintConfigSet(void);
void appUsbDps_IndexPrintConfigSet(void);
void appUsbDps_DateTimeConfigSet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_FileNameConfigSet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_PaperLayoutConfigSet(PdpsMenuOption_t DpsCapOption);
void appUsbDps_PaperDataTypeConfigSet(PdpsMenuOption_t DpsCapOption);
UINT32 appUsbDps_TotalPrintSelImg(PdpsInfo_t pDpsInfo, unsigned int TotaImage);
void appUsbDps_AddImageSet(UINT16 StartIndex,PdpsInfo_t pDpsInfo,UINT32 TotalPrint);
void appUsbDps_StandardPrintingConfigSet(void);
void appUsbDps_IndexPrintingConfigSet(void);
void appUsbDps_DPOFPrintingConfigSet(void);
void appUsbDps_Dps_HostMsgProcess(UINT64  hMsg ,UINT32 param);
void appUsbDps_DpsPrintStatePrintInitProcess();

#endif /* _APP_USBDPSLIB_H_ */

