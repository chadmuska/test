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
#if (KIT_WITH_HDMI)
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"
#include "api/sp5k_cec_api.h"

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                         *
 **************************************************************************/
#define CEC_COMMAND_WAIT_BUSY_COUNT 40

typedef enum {		/* EIA-CEA Standard */
	DISP_480P_4X3_60HZ		= 2,
	DISP_720P_16X9_60HZ		= 4,
	DISP_1080I_16X9_60HZ	= 5,
	DISP_480I_4X3_60HZ		= 6,
	DISP_576P_4X3_50HZ 		= 17,
	DISP_720P_16X9_50HZ		= 19,
	DISP_1080I_16X9_50HZ 	= 20,
	DISP_576I_4X3_50HZ 		= 21,
} dispEDIDSet_t;

typedef struct CEC_CMD_FRAME
{
    UINT8  byAddr;
    UINT8  byOpCode;
    UINT8  byDataSize;
    UINT8  byData[SP5K_CEC_OPERAND_TBL_SIZE];
    UINT32 uiTimeStamp;

} CEC_CMD_FRAME, *PCEC_CMD_FRAME;

typedef enum _HCE_CEC_MENU_REQUEST_TYPE
{
	HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE 		= (UINT8 )0x0,
	HCE_CEC_MENU_REQUEST_TYPE_DEACTIVATE 	= 0x1,
	HCE_CEC_MENU_REQUEST_TYPE_QUERY		 	= 0x2
} HCE_CEC_MENU_REQUEST_TYPE, *PHCE_CEC_MENU_REQUEST_TYPE;


/**************************************************************************
 *                           D E C L A R A T I O N S                        *
 **************************************************************************/
void appCecStop(void);
BOOL IsHdmiPlugIn(void);
sp5kCecErrCode_e CecMsgSend( PCEC_CMD_FRAME psCecCmdBlock );
sp5kCecErrCode_e CecMsgSendLoop( PCEC_CMD_FRAME psCecCmdBlock );
void appCecHandle_Send_PhyAddr(void);
void appCecHandle_Send_DeviceVendorID(UINT8 desaddr);
void appCecHandle_Send_ImageViewOn(void);
void appCecHandle_Send_ActiveSource(UINT8 desaddr);
void appCecHandle_Send_GiveDevicePowerStatus(UINT8 desaddr);
void appCecHandleOneTouchPlay(void);
void appCecHandle_Send_FeatureAbort(UINT8 OpCode, UINT8 desaddr);
void appCecHandle_Send_ReportPowerStatus(UINT8 desaddr, UINT8 status);
void appCecHandle_Send_MenuStatus(UINT8 desaddr, UINT8 menustatus);
void appCecHandle_Send_UserControlPressed(UINT8 desaddr,UINT8  byKeyId);
void appCecCtrlLogicalAlloc(void);
BOOL appCecCapabilityGet(void);
void receiveMsgCallback(UINT32 instId,UINT32 iniAddr,UINT32 flwAddr,UINT32 opcode,UINT32 operandNum,UINT8 *operandTbl);
void appCecControlStateOnMsg(UINT32 msg,UINT32 param);
void appCecControlStateInit(void);

/**************************************************************************
 *                         G L O B A L    D A T A                        *
 **************************************************************************/
extern UINT8 resolution;
static UINT32 cecPhyAddr;
static UINT8 txOperandTbl[SP5K_CEC_OPERAND_TBL_SIZE];
static UINT32 pbDev;
static BOOL cecSupported = FALSE;
extern UINT32 previousStateGet;

/**************************************************************************
 *                             F U N C T I O N                         *
 **************************************************************************/

#if SPCA6330
/*-------------------------------------------------------------------------
 *  File Name : appCecStop
 *------------------------------------------------------------------------*/
void appCecStop(
	void
)
{
	sp5kCecControl(0, SP5K_CEC_CTRL_STOP);
	sp5kCecControl(0, SP5K_CEC_CTRL_CLOSE);
}

/*-------------------------------------------------------------------------
 *  File Name : IsHdmiPlugIn
 *------------------------------------------------------------------------*/
BOOL
IsHdmiPlugIn(
	void
)
{
	return sp5kDispHdmiHotPlugDetectGet(0);
}

/*-------------------------------------------------------------------------
 *  File Name : CecMsgSend
 *------------------------------------------------------------------------*/
sp5kCecErrCode_e CecMsgSend( PCEC_CMD_FRAME psCecCmdBlock )
{
    UINT32 ret;
	DBG_PRINT("cec: SP5K_CEC_CTRL_MSG_SEND_WAIT S OpCode=0x%x",psCecCmdBlock->byOpCode);
	ret = sp5kCecControl(0, SP5K_CEC_CTRL_MSG_SEND_WAIT, pbDev, psCecCmdBlock->byAddr, psCecCmdBlock->byOpCode, psCecCmdBlock->byDataSize, psCecCmdBlock->byData);
	DBG_PRINT("cec: SP5K_CEC_CTRL_MSG_SEND_WAIT E");
    return ret;
}

/*-------------------------------------------------------------------------
 *  File Name : CecMsgSendLoop
 *------------------------------------------------------------------------*/
sp5kCecErrCode_e CecMsgSendLoop( PCEC_CMD_FRAME psCecCmdBlock )
{
	DBG_PRINT("cec: >>>>>>>CecMsgSendLoop >>>>> S  OpCode = 0x%x\n",psCecCmdBlock->byOpCode);
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	while( byRes == SP5K_CEC_ERR_TX_BUSY || byRes == SP5K_CEC_ERR_TX_TIMEOUT){
		if(IsHdmiPlugIn())
			byRes = CecMsgSend(psCecCmdBlock);
		else{
				DBG_PRINT("cec: HDMI Unplug!!-1\n");
			break;
		}

		DBG_PRINT("cec: CEC MSG SEND ERROR CODE: %d\n",byRes);
		APP_SLEEP_MS(40);
		CEC_WaitCount --;
		if(CEC_WaitCount == 0 ) break;
	}
		DBG_PRINT("cec: >>>>>>>CecMsgSendLoop >>>>> E  OpCode = 0x%x\n",psCecCmdBlock->byOpCode);
	return byRes;
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_PhyAddr
 *------------------------------------------------------------------------*/
void
appCecHandle_Send_PhyAddr(
	void
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;

	sCmdFrame.byOpCode = SP5K_CEC_MSG_REPORT_PHYSICAL_ADDRESS;//0x84
	sCmdFrame.byAddr = SP5K_CEC_LOGI_ADDR_BROADCAST;
	sCmdFrame.byDataSize = 3;
	sCmdFrame.byData[0] = txOperandTbl[0];
	sCmdFrame.byData[1] = txOperandTbl[1];
	sCmdFrame.byData[2] = txOperandTbl[2];;

	if(pbDev == SP5K_CEC_LOGI_ADDR_UNREGISTERED)		return;
	//if( gbyIsTV_NACK && (sCmdFrame.byAddr==SP5K_CEC_DEV_TYPE_TV) )	return;
	byRes = CecMsgSendLoop(&sCmdFrame);

	if(byRes == SP5K_CEC_ERR_SENT_ACK || byRes == SP5K_CEC_ERR_SENT_NAK){
		DBG_PRINT("cec: Physical Addr Allocate SUCCESS ");
	}else{
		DBG_PRINT("cec: TV has no send ACK/NAK ");
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_DeviceVendorID
 *------------------------------------------------------------------------*/
void
appCecHandle_Send_DeviceVendorID(
	UINT8 desaddr
)
{
	CEC_CMD_FRAME	sCmdFrame={0};

	sCmdFrame.byOpCode = SP5K_CEC_MSG_DEVICE_VENDOR_ID;//0x87
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 3;
	sCmdFrame.byData[0] = 0x08;
	sCmdFrame.byData[1] = 0x00;
	sCmdFrame.byData[2] = 0x46;

   	DBG_PRINT("cec: send SP5K_CEC_MSG_DEVICE_VENDOR_ID//0x87");
	CecMsgSendLoop(&sCmdFrame);
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_ImageViewOn
 *------------------------------------------------------------------------*/
void
appCecHandle_Send_ImageViewOn(
	void
)
{
	CEC_CMD_FRAME	sCmdFrame={0};

	sCmdFrame.byOpCode = SP5K_CEC_MSG_IMAGE_VIEW_ON;//0x04
	sCmdFrame.byAddr = SP5K_CEC_LOGI_ADDR_TV;
	sCmdFrame.byDataSize = 0;

   	DBG_PRINT("cec: send SP5K_CEC_MSG_IMAGE_VIEW_ON//0x04");
	CecMsgSendLoop(&sCmdFrame);
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_ActiveSource
 *------------------------------------------------------------------------*/
void
appCecHandle_Send_ActiveSource(
	UINT8 desaddr
)
{
	CEC_CMD_FRAME	sCmdFrame={0};

	sCmdFrame.byOpCode = SP5K_CEC_MSG_ACTIVE_SOURCE;//0x82
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 2;
	sCmdFrame.byData[0] = txOperandTbl[0];
	sCmdFrame.byData[1] = txOperandTbl[1];

   	DBG_PRINT("cec: send SP5K_CEC_MSG_ACTIVE_SOURCE//0x82");
	CecMsgSendLoop(&sCmdFrame);
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_GiveDevicePowerStatus
 *------------------------------------------------------------------------*/
void
appCecHandle_Send_GiveDevicePowerStatus(
	UINT8 desaddr
)
{
	CEC_CMD_FRAME	sCmdFrame={0};

	sCmdFrame.byOpCode = SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS;//0x8F
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 0;

   	DBG_PRINT("cec: send SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS//0x8F");
	CecMsgSendLoop(&sCmdFrame);
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandleOneTouchPlay
 *------------------------------------------------------------------------*/
void
appCecHandleOneTouchPlay(
	void
)
{
	if( pbDev == SP5K_CEC_LOGI_ADDR_UNREGISTERED) 	return;
    DBG_PRINT("cec: --- OneTouchPlay S---");
	appCecHandle_Send_ImageViewOn();
	APP_SLEEP_MS(200);
	appCecHandle_Send_ActiveSource(SP5K_CEC_LOGI_ADDR_BROADCAST);
	APP_SLEEP_MS(200);
	appCecHandle_Send_GiveDevicePowerStatus(SP5K_CEC_LOGI_ADDR_TV);	/* Trigger TV power on from standby mode */
    DBG_PRINT("cec: --- OneTouchPlay E---");
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_FeatureAbort
 *------------------------------------------------------------------------*/
void
appCecHandle_Send_FeatureAbort(
	UINT8 OpCode,
	UINT8 desaddr
)
{
	UINT8 usErrorCode=0;
	CEC_CMD_FRAME	sCmdFrame={0};

	sCmdFrame.byOpCode = SP5K_CEC_MSG_FEATURE_ABORT;//0x00
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 2;
	sCmdFrame.byData[0] = OpCode;
	sCmdFrame.byData[1] = usErrorCode;
    DBG_PRINT("cec: send SP5K_CEC_MSG_FEATURE_ABORT//0x00");
	CecMsgSendLoop(&sCmdFrame);
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_ReportPowerStatus
 *------------------------------------------------------------------------*/
void
appCecHandle_Send_ReportPowerStatus(
	UINT8 desaddr,
	UINT8 status
)
{
	CEC_CMD_FRAME	sCmdFrame={0};

	sCmdFrame.byOpCode = SP5K_CEC_MSG_REPORT_POWER_STATUS;//0x90
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 1;
	sCmdFrame.byData[0] = status;

    DBG_PRINT("cec: send SP5K_CEC_MSG_REPORT_POWER_STATUS//0x90");
	CecMsgSendLoop(&sCmdFrame);
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_MenuStatus
 *------------------------------------------------------------------------*/
void
appCecHandle_Send_MenuStatus(
	UINT8 desaddr,
	UINT8 menustatus
)
{
	CEC_CMD_FRAME	sCmdFrame={0};

	sCmdFrame.byOpCode = SP5K_CEC_MSG_MENU_STATUS;//0x8E
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 1;
	sCmdFrame.byData[0] = menustatus;

    DBG_PRINT("cec: send SP5K_CEC_MSG_MENU_STATUS//0x8E");
	CecMsgSendLoop(&sCmdFrame);
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_UserControlPressed
 *------------------------------------------------------------------------*/
void
appCecHandle_Send_UserControlPressed(
	UINT8 desaddr,
	UINT8  byKeyId
)
{
	CEC_CMD_FRAME	sCmdFrame={0};

	sCmdFrame.byOpCode = SP5K_CEC_MSG_USER_CONTROL_PRESSED;//0x44
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 1;
	sCmdFrame.byData[0] = byKeyId;

    DBG_PRINT("cec: send SP5K_CEC_MSG_USER_CONTROL_PRESSED//0x44");
	CecMsgSendLoop(&sCmdFrame);
}

/*-------------------------------------------------------------------------
 *  File Name : appCecHandle_Send_SetOsdName
 *------------------------------------------------------------------------*/
void appCecHandle_Send_SetOsdName(
	UINT8 desaddr
)
{
	CEC_CMD_FRAME	sCmdFrame={0};
	sp5kCecErrCode_e byRes = SP5K_CEC_ERR_TX_BUSY;
	UINT16 CEC_WaitCount =CEC_COMMAND_WAIT_BUSY_COUNT;

	sCmdFrame.byOpCode = SP5K_CEC_MSG_SET_OSD_NAME;//0x47
	sCmdFrame.byAddr = desaddr;
	sCmdFrame.byDataSize = 1;
	sCmdFrame.byData[0] = 0x04;
	sCmdFrame.byDataSize = 8;
	//#define OSD_NAME 				"SPCA6330"

    DBG_PRINT("cec: send SP5K_CEC_MSG_SET_OSD_NAME//0x47");
	while( byRes == SP5K_CEC_ERR_TX_BUSY || byRes == SP5K_CEC_ERR_TX_TIMEOUT){
		byRes = sp5kCecControl(0, SP5K_CEC_CTRL_MSG_SEND_WAIT, pbDev, sCmdFrame.byAddr, sCmdFrame.byOpCode, sCmdFrame.byDataSize, "SPCA6330");
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cec: ERROR CODE:%d", byRes);
		APP_SLEEP_MS(40);
		CEC_WaitCount --;
		if(CEC_WaitCount == 0 ) break;
	}
}

BOOL appCecCapabilityGet(void)
{
	return cecSupported;
}

/*-------------------------------------------------------------------------
 *  File Name : appCecCtrlLogicalAlloc
 *------------------------------------------------------------------------*/
void appCecCtrlLogicalAlloc(
	void
)
{
    DBG_PRINT("cec: SP5K_CEC_CTRL_LOGICAL_ADDR_ALLOC S");
	sp5kCecControl(0, SP5K_CEC_CTRL_LOGICAL_ADDR_ALLOC, SP5K_CEC_DEV_TYPE_PLAYBACK_DEVICE, &pbDev);
    DBG_PRINT("cec: SP5K_CEC_CTRL_LOGICAL_ADDR_ALLOC E");

	if(sp5kCecControl(0, SP5K_CEC_CTRL_LOGICAL_ADDR_GET, SP5K_CEC_DEV_TYPE_PLAYBACK_DEVICE, &pbDev)==SUCCESS)
	{
        HOST_PROF_LOG_ADD(LEVEL_INFO, "cec: CEC Supported");
        cecSupported = TRUE;
    }
    else
	{
        HOST_PROF_LOG_ADD(LEVEL_WARN, "cec: CEC Not Supported");
        cecSupported = FALSE;
		return;
    }
    DBG_PRINT("cec: SP5K_CEC_CTRL_PHYSICALL_ADDR_GET S");
	sp5kCecControl(0, SP5K_CEC_CTRL_PHYSICALL_ADDR_GET, &cecPhyAddr);
	txOperandTbl[0] = cecPhyAddr >> 8;	 /* A.B. */
	txOperandTbl[1] = cecPhyAddr & 0xFF;  /* C.D. */
	txOperandTbl[2] = SP5K_CEC_DEV_TYPE_PLAYBACK_DEVICE;
    DBG_PRINT("cec: SP5K_CEC_CTRL_PHYSICALL_ADDR_GET E");

    DBG_PRINT("cec: cecPbDevAddr=0x%x, byPhysAddr [0]=0x%x,  byPhysAddr [1]=0x%x",pbDev,txOperandTbl[0],txOperandTbl[1]);
    DBG_PRINT("cec: playback device = %d", pbDev);

	appCecHandleOneTouchPlay();
	appCecHandle_Send_PhyAddr();
//	appCecHandle_Send_DeviceVendorID( SP5K_CEC_LOGI_ADDR_BROADCAST );

}

/*-------------------------------------------------------------------------
 *  File Name : receiveMsgCallback
 *------------------------------------------------------------------------*/
void
receiveMsgCallback(
	UINT32 instId,
	UINT32 iniAddr,
	UINT32 flwAddr,
	UINT32 opcode,
	UINT32 operandNum,
	UINT8 *operandTbl
)
{
    DBG_PRINT("cec: receiveMsgCallback S");
    DBG_PRINT("cec: -------------opcode=0x%x,operandNum=%d  ,  iniAddr=0x%x,flwAddr=0x%x",opcode,operandNum,iniAddr,flwAddr);
    DBG_PRINT("cec: --operandTbl[0]: 0x%02x  operandTbl[1]: 0x%02x  operandTbl[2]: 0x%02x   operandTbl[3]: 0x%02x", operandTbl[0],operandTbl[1],operandTbl[2],operandTbl[3]);

	if(pbDev == SP5K_CEC_LOGI_ADDR_UNREGISTERED)		return;

	switch ( opcode ) {
		case SP5K_CEC_MSG_USER_CONTROL_PRESSED: // 0x44
			if( operandNum != 1)	break;
			if( (iniAddr==SP5K_CEC_LOGI_ADDR_TV) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---Button 0x%02X Pressed---", operandTbl[0]);
				switch(operandTbl[0])
				{
					case SP5K_CEC_USER_CTRL_LEFT:
					sp5kHostMsgSend(SP5K_MSG_ADC_BUTTON_PRESS, 2);	/* Param: ADC key map */
						break;
					case SP5K_CEC_USER_CTRL_RIGHT:
					sp5kHostMsgSend(SP5K_MSG_ADC_BUTTON_PRESS, 3);
						break;
					default:
						appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_USER_CONTROL_PRESSED,iniAddr);
					break;
				}
			}
		break;

		case SP5K_CEC_MSG_REPORT_PHYSICAL_ADDRESS://0x84
			if((iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_REPORT_PHYSICAL_ADDRESS://0x84---");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_REPORT_PHYSICAL_ADDRESS,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_GIVE_PHYSICAL_ADDRESS://0x83
			if ( flwAddr == pbDev) {
                DBG_PRINT("cec: ---SP5K_CEC_MSG_GIVE_PHYSICAL_ADDRESS://0x83---");
				appCecHandle_Send_PhyAddr();
			}
		break;

		case SP5K_CEC_MSG_GIVE_DEVICE_VENDOR_ID:	//0x8c
			if ( flwAddr == pbDev) {
                DBG_PRINT("cec: ---SP5K_CEC_MSG_GIVE_DEVICE_VENDOR_ID://0x8c---");
				appCecHandle_Send_DeviceVendorID(iniAddr);
				#if 0
				operandTbl[0] = 0x00;
				operandTbl[1] = 0x07;
				operandTbl[2] = 0x33;
				sp5kCecControl(0, SP5K_CEC_CTRL_MSG_SEND, cecPbDevAddr, iniAddr, SP5K_CEC_MSG_DEVICE_VENDOR_ID, 3, operandTbl);
				#endif
			}
		break;

		case SP5K_CEC_MSG_VENDOR_COMMAND_WITH_ID:  // 0xA0
            DBG_PRINT("cec: ---SP5K_CEC_MSG_VENDOR_COMMAND_WITH_ID://0xA0---");
			UINT8 nDataCount =0;
			for(nDataCount=0;nDataCount<operandNum;nDataCount++)
			{
                DBG_PRINT("cec: operandTbl[%d]: 0x%02x", nDataCount,operandTbl[nDataCount]);
			}

			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_VENDOR_COMMAND_WITH_ID,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_SET_MENU_LANGUAGE:  // 0x32
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_SET_MENU_LANGUAGE://0x32---");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_SET_MENU_LANGUAGE,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_GET_MENU_LANGUAGE:  // 0x91
			if( flwAddr == pbDev )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_GET_MENU_LANGUAGE://0x91---");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_GET_MENU_LANGUAGE, iniAddr);
			}
		break;

		case SP5K_CEC_MSG_DEVICE_VENDOR_ID:  // 0x87
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev)  )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_DEVICE_VENDOR_ID://0x87---");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_DEVICE_VENDOR_ID,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS:  // 0x8f
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS://0x8f---");
				appCecHandle_Send_ReportPowerStatus(iniAddr,SP5K_CEC_POWER_ON);
			}
		break;

		case SP5K_CEC_MSG_MENU_REQUEST:		//0x8d
			if( operandNum != 1)	break;
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
				switch( operandTbl[0] )
				{
                    DBG_PRINT("cec: ---SP5K_CEC_MSG_MENU_REQUEST://0x8d---");
					case HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE:
					case HCE_CEC_MENU_REQUEST_TYPE_DEACTIVATE:
					case HCE_CEC_MENU_REQUEST_TYPE_QUERY:
					{
						appCecHandle_Send_MenuStatus(iniAddr, HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE);
					}
					break;

					default:
						appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_MENU_REQUEST,iniAddr);
					break;
				}
			}
		break;

		case SP5K_CEC_MSG_REPORT_POWER_STATUS:	//0x90
			if( operandNum != 1)	break;

			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_REPORT_POWER_STATUS: 0x%x//0x90---",  operandTbl[0]);
				if( (operandTbl[0] == SP5K_CEC_POWER_ON) || (operandTbl[0] == SP5K_CEC_POWER_STANDBY_TO_ON))
				{
                    DBG_PRINT("cec: TV is On now");
					#if 0
					appCecHandle_Send_ImageViewOn();
					APP_SLEEP_MS(200);
					appCecHandle_Send_ActiveSource(SP5K_CEC_LOGI_ADDR_BROADCAST);
					APP_SLEEP_MS(200);
					appCecHandle_Send_MenuStatus(SP5K_CEC_LOGI_ADDR_TV, HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE);
					#endif
				}
				else
				{
                    DBG_PRINT("cec: TV is stnadby");
					appCecHandle_Send_UserControlPressed(SP5K_CEC_LOGI_ADDR_TV,SP5K_CEC_USER_CTRL_POWER);
					appCecHandle_Send_GiveDevicePowerStatus(SP5K_CEC_LOGI_ADDR_TV);
				}
			}
		break;

		case SP5K_CEC_MSG_GET_CEC_VERSION:	//0x9F
            DBG_PRINT("cec: ---SP5K_CEC_MSG_GET_CEC_VERSION://0x9F---");
		break;

		/* get <CEC Version> return. */
		case SP5K_CEC_MSG_CEC_VERSION:	//0x9E
            DBG_PRINT("cec: ---SP5K_CEC_MSG_CEC_VERSION://0x9E---");
			if ( operandTbl[0] == 0x04 ) {
                DBG_PRINT("cec: CEC version 1.3.a\n");
			}
			else if ( operandTbl[0] == 0x05 ) {
                DBG_PRINT("cec: CEC version 1.4\n");
			}
		break;

		case SP5K_CEC_MSG_GIVE_OSD_NAME:	//0x46
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_GIVE_OSD_NAME://0x46---");
				appCecHandle_Send_SetOsdName(iniAddr);
			}
			else
			{
                DBG_PRINT("cec: HCE_CEC_OP_CODE_GIVE_OSD_NAME not match!");
			}
		break;

		case SP5K_CEC_MSG_SET_OSD_NAME:
		//	printf("%s, %d SP5K_CEC_MSG_SET_OSD_NAME\n", __FUNCTION__, __LINE__);
		//	printf("osd name=%s", rxOperandTbl);
			appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_HDMI_PLUG);
		break;

		case SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN:	//0x8a
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN://0x8a---");
				// key function
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_UP:	//0x8b
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_UP://0x8b---");
				// key function
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_UP,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_ACTIVE_SOURCE:  // 0x82
			DBG_PRINT( "cec: Data Size: %d\n", ucDataSize);
			DBG_PRINT( "cec: operandTbl[0]: 0x%02x\n", operandTbl[0]);
			DBG_PRINT( "cec: operandTbl[1]: 0x%02x\n", operandTbl[1]);
			DBG_PRINT( "cec: ucRcvInitAddr: 0x%02x\n", ucRcvInitAddr);
			DBG_PRINT( "cec: ucRcvDestAddr: 0x%02x\n", ucRcvDestAddr);
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==SP5K_CEC_LOGI_ADDR_BROADCAST) )
			{
				if( operandNum != 2)	break;
				//printf("*** Rcv <Active Source> ***\n");
                DBG_PRINT("cec: ---SP5K_CEC_MSG_ACTIVE_SOURCE://0x82---");

				if((operandTbl[0]!=txOperandTbl[0]) && (operandTbl[1]!=txOperandTbl[1]) )
				{
					DBG_PRINT( "cec: ---gbyIsDSCActive = %d://0x82---\n",gbyIsDSCActive);
					// if( gbyIsDSCActive )	HceCecHandle_Send_InactiveSource( iniAddr );
					// DBG_PRINT( "cec: SendMsg_1 0x%x\n",MSG_UI_CEC_RETURN_STANDBY);
					// SendUIMsg(MSG_UI_CEC_RETURN_STANDBY);
				}
			}
		break;

		case SP5K_CEC_MSG_REQUEST_ACTIVE_SOURCE:	//0x85
			if( flwAddr == SP5K_CEC_LOGI_ADDR_BROADCAST)
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_REQUEST_ACTIVE_SOURCE://0x85---");
				appCecHandle_Send_ActiveSource( SP5K_CEC_LOGI_ADDR_BROADCAST );
				appCecHandle_Send_MenuStatus(SP5K_CEC_LOGI_ADDR_TV, HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE);
			}
		break;

		case SP5K_CEC_MSG_SET_STREAM_PATH: //0x86
			if( operandNum != 2)	break;
			if( flwAddr == SP5K_CEC_LOGI_ADDR_BROADCAST)
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_SET_STREAM_PATH://0x86---");
				if( (operandTbl[0]==txOperandTbl[0]) && (operandTbl[1]==txOperandTbl[1]) )
				{
					appCecHandle_Send_ActiveSource( SP5K_CEC_LOGI_ADDR_BROADCAST );
					appCecHandle_Send_MenuStatus(SP5K_CEC_LOGI_ADDR_TV, HCE_CEC_MENU_REQUEST_TYPE_ACTIVATE);
				}
			}
		break;

		case SP5K_CEC_MSG_IMAGE_VIEW_ON:  // 0x04
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_IMAGE_VIEW_ON://0x04---");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_IMAGE_VIEW_ON,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_TEXT_VIEW_ON:  //0x0d
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_TEXT_VIEW_ON://0x0d---");
				appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_TEXT_VIEW_ON,iniAddr);
			}
		break;

		case SP5K_CEC_MSG_FEATURE_ABORT:  // 0x00
			if( (iniAddr!=SP5K_CEC_LOGI_ADDR_BROADCAST) && (flwAddr==pbDev) )
			{
                DBG_PRINT("cec: ---SP5K_CEC_MSG_FEATURE_ABORT://0x00---");
                DBG_PRINT("cec: *** Rcv <Feature Abort> ***\n");
			}
		break;

		case SP5K_CEC_MSG_ABORT:  // 0xff
		{
            DBG_PRINT("cec: ---SP5K_CEC_MSG_ABORT://0xff---");
			appCecHandle_Send_FeatureAbort(SP5K_CEC_MSG_ABORT, iniAddr);
		}
		break;
	}
    DBG_PRINT("cec: receiveMsgCallback	E");
}

#endif

/*-------------------------------------------------------------------------
 *  File Name : appCecControlStateOnMsg
 *------------------------------------------------------------------------*/
void appCecControlStateOnMsg(
	UINT32 msg,
	UINT param
)
{
    DBG_PRINT("cec: %s : [0x%x] [0x%x]", __FUNCTION__, msg, param);

	switch (msg)
	{
		case SP5K_MSG_CEC_PHYSICAL_ADDR_DISCOVER_FAIL:
            DBG_PRINT("cec: CEC_PHYSICAL_ADDR_DISCOVER_FAIL\n");
		//	appStateChange(appPreviousStateGet(), STATE_PARAM_HDMI_PLUG);
			break;
		case SP5K_MSG_CEC_PHYSICAL_ADDR_DISCOVER_DONE:
            DBG_PRINT("cec: CEC_PHYSICAL_ADDR_DISCOVER_SUCCESSFULL\n");
			break;
		default :
			break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appCecControlStateInit
 *------------------------------------------------------------------------*/
void appCecControlStateInit(
	void
)
{
	sp5kCecInstCfg_t cecCfg;
	cecCfg.ddcChnlId = 0;
	cecCfg.cecChnlId = 0;
	#if IS_CEC_ON
	cecCfg.rxMsgCallback = (sp5kCecRecieveMsgCallback_t)receiveMsgCallback;
	#else
	cecCfg.rxMsgCallback = NULL;
	#endif
	sp5kCecControl(0, SP5K_CEC_CTRL_OPEN, &cecCfg);
	sp5kCecControl(0, SP5K_CEC_CTRL_DBG_MSG_LOG_ENABLE_SET, 1);
	sp5kCecControl(0, SP5K_CEC_CTRL_START);		/* log start */
}

/*-------------------------------------------------------------------------
 *  File Name : appCecCapabilityCtrlState
 *------------------------------------------------------------------------*/
void
appCecCapabilityCtrlState(
	UINT32 msg,
	UINT32 param
)
{
    DBG_PRINT("cec: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appCecControlState");
		appCecControlStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;
	default:
		appCecControlStateOnMsg(msg,param);

			appStateChange(APP_STATE_HDMI_PLUG, STATE_PARAM_NORMAL_INIT);
		break;
	}
}
#endif
