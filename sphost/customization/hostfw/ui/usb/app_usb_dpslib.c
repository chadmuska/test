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

#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"
#include "app_dev_plug.h"
#include "sp5k_ptp_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_dcf_api.h"
#include "app_usb_dpslib.h"
#include "app_usb_dps_osd.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
extern dpsInfo_t  gDpsInfo;
extern dpsMenuOption_t  gDpsMenuOpt ;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

dpsPrintCapbilityPIMACode_t gPIMACodeTab={

	{PIMA_QUALITY_AUTO, PIMA_QUALITY_NORMAL, PIMA_QUALITY_DRAFT, PIMA_QUALITY_FINE},	
	{PIMA_PAPERSIZE_AUTO, PIMA_PAPERSIZE_L, PIMA_PAPERSIZE_2L, PIMA_PAPERSIZE_POSTCARD,PIMA_PAPERSIZE_CARDSIZE, PIMA_PAPERSIZE_4X6, PIMA_PAPERSIZE_8X10, PIMA_PAPERSIZE_LETTER,
	  PIMA_PAPERSIZE_11X17,PIMA_PAPERSIZE_A3,PIMA_PAPERSIZE_A4,PIMA_PAPERSIZE_A6,PIMA_PAPERSIZE_B0,PIMA_PAPERSIZE_B1,PIMA_PAPERSIZE_ROLL_L,PIMA_PAPERSIZE_ROLL_2L},	
	{PIMA_PAPERTYPE_AUTO, PIMA_PAPERTYPE_PLAIN, PIMA_PAPERTYPE_PHOTO, PIMA_PAPERTYPE_FASTPHOTO},						
	{PIMA_DATETIME_AUTO, PIMA_DATETIME_OFF, PIMA_DATETIME_ON},						
	{PIMA_FILENAME_AUTO, PIMA_FILENAME_OFF,PIMA_FILENAME_ON},							
	{PIMA_LAYOUT_AUTO, PIMA_LAYOUT_1, PIMA_LAYOUT_2,PIMA_LAYOUT_3,PIMA_LAYOUT_4,PIMA_LAYOUT_5,PIMA_LAYOUT_6,PIMA_LAYOUT_7,PIMA_LAYOUT_8,PIMA_LAYOUT_9,PIMA_LAYOUT_16},	
	{SP5K_DPS_DATETYPE_YYMMDD,SP5K_DPS_DATETYPE_DDMMYY,SP5K_DPS_DATETYPE_MMDDYY},
};

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

UINT32 gDateTypeConfigSet =SP5K_DPS_DATETYPE_YYMMDD;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_MenuCapabilityOptionSet
 *------------------------------------------------------------------------*/
void appUsbDps_MenuCapabilityOptionSet(
	UINT32 *databuf,
	unsigned char datalen,
	UINT32 *pimaTbl,
	unsigned char totalItem,
	PdpsMenuItemEn_t pEnOpt
)
{

	unsigned char i,j;

	for (i = 0; i<totalItem; i++){
		for (j = 0; j<datalen; j++)
		{
			if (databuf[j]==(*(pimaTbl+i))){
				pEnOpt->OsdEnOption |= 1<<i;
				break;
			}
		}

	}

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperSizeCapabilityGet
 *------------------------------------------------------------------------*/
void appUsbDps_PaperSizeCapabilityGet(
	PdpsMenuOption_t DpsCapOption
)
{
	UINT32 ret =0;
	UINT32 datdBuf[DPS_PAPERSIZE_CAP_BUF];
	UINT32 dataLen;
	
	ret=sp5kDpsPrtAttrGet(SP5K_DPS_PRT_CAPABILITY,SP5K_PAPERSIZES,0,datdBuf, &dataLen);
	
	if (ret !=DPS_CAPABILITY_SUCCESS){
        HOST_PROF_LOG_ADD(LEVEL_ERROR,"usb dps lib: PaperSizeCapabilityGet Error");	
	}

	DpsCapOption->PaperSize.OsdEnOption =0x00000000;

	appUsbDps_MenuCapabilityOptionSet(datdBuf,dataLen ,gPIMACodeTab.paperSize, DPS_PAPERSIZE_TOTAL, &DpsCapOption->PaperSize);

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperLayoutCapabilityGet
 *------------------------------------------------------------------------*/
void appUsbDps_PaperLayoutCapabilityGet(
	PdpsMenuOption_t DpsCapOption
)
{

	UINT32 ret =0;
	UINT32 datdBuf[DPS_PAPERSIZE_CAP_BUF];
	UINT32 dataLen;
	UINT32 paperSize;

	paperSize=gPIMACodeTab.paperSize[DpsCapOption->PaperSize.SelOption];

	ret=sp5kDpsPrtAttrGet(SP5K_DPS_PRT_CAPABILITY,SP5K_LAYOUTS,paperSize,datdBuf, &dataLen);
	if (ret !=DPS_CAPABILITY_SUCCESS){
        HOST_PROF_LOG_PRINT(LEVEL_ERROR,"usb dps lib: %s Error",__FUNCTION__);		        
	}

	DpsCapOption->Layout.OsdEnOption =0x00000000;
	appUsbDps_MenuCapabilityOptionSet(datdBuf,dataLen ,gPIMACodeTab.Layout, DPS_LAYOUT_TOTAL, &DpsCapOption->Layout);

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperTypeCapabilityGet
 *------------------------------------------------------------------------*/
void appUsbDps_PaperTypeCapabilityGet(PdpsMenuOption_t DpsCapOption)
{

	UINT32 ret =0;
	UINT32 datdBuf[DPS_PAPERSIZE_CAP_BUF];
	UINT32 dataLen;
	UINT32 paperSize;

	paperSize=gPIMACodeTab.paperSize[DpsCapOption->PaperSize.SelOption];
	
	ret=sp5kDpsPrtAttrGet(SP5K_DPS_PRT_CAPABILITY,SP5K_PAPERTYPES,paperSize,datdBuf, &dataLen);
	if (ret !=DPS_CAPABILITY_SUCCESS){
        HOST_PROF_LOG_PRINT(LEVEL_ERROR,"usb dps lib: %s Error",__FUNCTION__);	                
	}

	DpsCapOption->PaperType.OsdEnOption =0x00000000;
	appUsbDps_MenuCapabilityOptionSet(datdBuf,dataLen ,gPIMACodeTab.paperType, DPS_PAPERTYPE_TOTAL, &DpsCapOption->PaperType);

}



/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperQualityCapabilityGet
 *------------------------------------------------------------------------*/
void appUsbDps_PaperQualityCapabilityGet(PdpsMenuOption_t DpsCapOption)
{

	UINT32 ret =0;
	UINT32 datdBuf[DPS_PAPERSIZE_CAP_BUF];
	UINT32 dataLen;
	
	ret=sp5kDpsPrtAttrGet(SP5K_DPS_PRT_CAPABILITY,SP5K_QUALITIY,0,datdBuf, &dataLen);
	if (ret !=DPS_CAPABILITY_SUCCESS){
		HOST_PROF_LOG_PRINT(LEVEL_ERROR,"usb dps lib: %s Error",__FUNCTION__);	
	}

	DpsCapOption->Quality.OsdEnOption =0x00000000;
	appUsbDps_MenuCapabilityOptionSet(datdBuf,dataLen ,gPIMACodeTab.Quality, DPS_QUALITY_TOTAL, &DpsCapOption->Quality);

}


/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DateTimePrintCapabilityGet
 *------------------------------------------------------------------------*/
void appUsbDps_DateTimePrintCapabilityGet(
	PdpsMenuOption_t DpsCapOption
)
{

	UINT32 ret =0;
	UINT32 datdBuf[DPS_PAPERSIZE_CAP_BUF];
	UINT32 dataLen;
	
	ret=sp5kDpsPrtAttrGet(SP5K_DPS_PRT_CAPABILITY,SP5K_DATEPRINTS,0,datdBuf, &dataLen);
	if (ret !=DPS_CAPABILITY_SUCCESS){
		HOST_PROF_LOG_PRINT(LEVEL_ERROR,"usb dps lib: %s Error",__FUNCTION__);	
	}

	DpsCapOption->DateTime.OsdEnOption =0x00000000;
	appUsbDps_MenuCapabilityOptionSet(datdBuf,dataLen ,gPIMACodeTab.DateTime, DPS_QUALITY_TOTAL, &DpsCapOption->DateTime);


}


/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_FileNamePrintCapabilityGet
 *------------------------------------------------------------------------*/
void appUsbDps_FileNamePrintCapabilityGet(PdpsMenuOption_t DpsCapOption)
{

	UINT32 ret =0;
	UINT32 datdBuf[DPS_PAPERSIZE_CAP_BUF];
	UINT32 dataLen;

	ret=sp5kDpsPrtAttrGet(SP5K_DPS_PRT_CAPABILITY,SP5K_FILENAMEPRINTS,0,datdBuf, &dataLen);
	if (ret !=DPS_CAPABILITY_SUCCESS){
		HOST_PROF_LOG_PRINT(LEVEL_ERROR,"usb dps lib: %s Error",__FUNCTION__);	
	}
	
	DpsCapOption->FileNo.OsdEnOption =0x00000000;
	appUsbDps_MenuCapabilityOptionSet(datdBuf,dataLen ,gPIMACodeTab.FileNo, DPS_DATE_TOTAL, &DpsCapOption->FileNo);


}



/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_IndexPrintCapabilityGet
 *------------------------------------------------------------------------*/
void appUsbDps_IndexPrintCapabilityGet(
	PdpsMenuOption_t DpsCapOption
)
{

	UINT32 ret =0;
	UINT32 datdBuf[DPS_PAPERSIZE_CAP_BUF];
	UINT32 dataLen;
	UINT32 paperSize;
	unsigned char i;

	paperSize=gPIMACodeTab.paperSize[DpsCapOption->PaperSize.SelOption];
	
	ret=sp5kDpsPrtAttrGet(SP5K_DPS_PRT_CAPABILITY,SP5K_LAYOUTS,paperSize,datdBuf, &dataLen);
	if (ret !=DPS_CAPABILITY_SUCCESS){		
        HOST_PROF_LOG_PRINT(LEVEL_ERROR,"usb dps lib: %s Error",__FUNCTION__);	
	}

	for (i=0;i<dataLen;i++){
		if (datdBuf[i]==PIMA_LAYOUT_INDEX){
			 gDpsInfo.IndexPrintCap=TRUE;
			 gDpsMenuOpt.Print.OsdEnOption = gDpsMenuOpt.Print.OsdEnOption|0x00000002;
		}
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DPOFPrintCapabilityGet
 *------------------------------------------------------------------------*/
void appUsbDps_DPOFPrintCapabilityGet(PdpsMenuOption_t DpsCapOption)
{
	UINT32 ret =0;
	UINT32 datdBuf[DPS_PAPERSIZE_CAP_BUF];
	UINT32 dataLen;
	unsigned char i;

	ret=sp5kDpsPrtAttrGet(SP5K_DPS_PRT_CAPABILITY,SP5K_FILETYPES,0,datdBuf, &dataLen);
	if (ret !=DPS_CAPABILITY_SUCCESS){
		HOST_PROF_LOG_PRINT(LEVEL_ERROR,"usb dps lib: %s Error",__FUNCTION__);	
	}


	for (i=0;i<dataLen;i++){
		if (datdBuf[i]==PIMA_FILETYPE_DPOF){
			 gDpsInfo.DpofPrintCap=TRUE;
			 gDpsMenuOpt.Print.OsdEnOption = gDpsMenuOpt.Print.OsdEnOption|0x00000004;
		}
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperQualityConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_PaperQualityConfigSet(
	PdpsMenuOption_t DpsCapOption
)
{	
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_QUALITY,gPIMACodeTab.Quality[DpsCapOption->Quality.SelOption]);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperSizeConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_PaperSizeConfigSet(
	PdpsMenuOption_t DpsCapOption
)
{
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_PAPERSIZE,gPIMACodeTab.paperSize[DpsCapOption->PaperSize.SelOption]);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperSizeConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_PaperTypeConfigSet(
	PdpsMenuOption_t DpsCapOption
)
{
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_PAPERTYPE,gPIMACodeTab.paperType[DpsCapOption->PaperType.SelOption]);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_StandardPrintConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_StandardPrintConfigSet(
	void
)
{
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_FILETYPE,PIMA_FILETYPE_AUTO);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DPOFPrintConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_DPOFPrintConfigSet(
	void
)
{
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_FILETYPE,PIMA_FILETYPE_DPOF);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DPOFPrintConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_IndexPrintConfigSet(
	void
)
{
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_FILETYPE,PIMA_FILETYPE_EXIFJPG);
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_LAYOUT, PIMA_LAYOUT_INDEX);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperSizeConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_DateTimeConfigSet(
	PdpsMenuOption_t DpsCapOption
)
{
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_DATEPRINT,gPIMACodeTab.DateTime[DpsCapOption->DateTime.SelOption]);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_FileNameConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_FileNameConfigSet(
	PdpsMenuOption_t DpsCapOption
)
{
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_FILENAMEPRINT,gPIMACodeTab.FileNo[DpsCapOption->FileNo.SelOption]);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperLayoutConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_PaperLayoutConfigSet(
	PdpsMenuOption_t DpsCapOption
)
{
	sp5kDpsPrtAttrSet(SP5K_DPS_PRT_CAPABILITY,SP5K_SET_LAYOUT,gPIMACodeTab.Layout[DpsCapOption->Layout.SelOption]);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PaperDataTypeConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_PaperDataTypeConfigSet(
	PdpsMenuOption_t DpsCapOption
)
{
	gDateTypeConfigSet =gPIMACodeTab.DateType[DpsCapOption->DateType.SelOption];
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_TotalPrintSelImg
 *------------------------------------------------------------------------*/
UINT32 appUsbDps_TotalPrintSelImg(PdpsInfo_t pDpsInfo, unsigned int TotaImage)
{
	unsigned int i, ImgTotalSum=0;

	for (i=0; i<TotaImage; i++) {
		if (pDpsInfo->PicBuf[i]>=1) {

			ImgTotalSum+=pDpsInfo->PicBuf[i];
		}
	}
	return ImgTotalSum;
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_AddImageSet
 *------------------------------------------------------------------------*/
void appUsbDps_AddImageSet(UINT16 StartIndex,PdpsInfo_t pDpsInfo,UINT32 TotalImageCnt)
{

	UINT32 i;
	UINT32 ImageID;
	UINT8 filename[30];
	UINT32 ret;
	sp5kDcfAttrElm_t DcfFileAttr;
	sp5kDpsCropArea_t DpsCropArea;
	sp5kDpsDate_t DateAttr;

	DpsCropArea.wCropH = 0;
	DpsCropArea.wCropW = 0;
	DpsCropArea.wCropX = 0;
	DpsCropArea.wCropY = 0;

	for (i = StartIndex; i<TotalImageCnt; i++) {

		if (pDpsInfo->PicBuf[i]>=1) {
          	  	sp5kDcfFsFileAttrGet(( i+1),&DcfFileAttr,filename );
			DateAttr.wYear = DcfFileAttr.time.year;
			DateAttr.bMonth = DcfFileAttr.time.mon;
			DateAttr.bDay = DcfFileAttr.time.day;
			DateAttr.dateType = gDateTypeConfigSet;			
			sp5kDpsPrtAttrGet(SP5K_DPS_PRT_FILE_ID,filename,&ImageID);
			ret=sp5kDpsPrtAddImage(ImageID,&DpsCropArea,(UINT8*)&DcfFileAttr.filename,&DateAttr,pDpsInfo->PicBuf[i]);
			if (ret !=DPS_CAPABILITY_SUCCESS){
				HOST_PROF_LOG_PRINT(LEVEL_ERROR,"usb dps lib: %s Error",__FUNCTION__);	
			}
		}
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_StandardPrintingConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_StandardPrintingConfigSet(
	void
)
{

	appUsbDps_PaperQualityConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperSizeConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperTypeConfigSet(&gDpsMenuOpt);
	appUsbDps_StandardPrintConfigSet();
	appUsbDps_DateTimeConfigSet(&gDpsMenuOpt);
	appUsbDps_FileNameConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperLayoutConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperDataTypeConfigSet(&gDpsMenuOpt);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_IndexPrintingConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_IndexPrintingConfigSet(void)
{

	appUsbDps_PaperQualityConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperSizeConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperTypeConfigSet(&gDpsMenuOpt);
	appUsbDps_DateTimeConfigSet(&gDpsMenuOpt);
	appUsbDps_FileNameConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperDataTypeConfigSet(&gDpsMenuOpt);
	appUsbDps_IndexPrintConfigSet();

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DPOFPrintingConfigSet
 *------------------------------------------------------------------------*/
void appUsbDps_DPOFPrintingConfigSet(
	void
)
{

	appUsbDps_PaperQualityConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperSizeConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperTypeConfigSet(&gDpsMenuOpt);
	appUsbDps_DateTimeConfigSet(&gDpsMenuOpt);
	appUsbDps_FileNameConfigSet(&gDpsMenuOpt);
	appUsbDps_PaperDataTypeConfigSet(&gDpsMenuOpt);
	appUsbDps_DPOFPrintConfigSet();
	
}

