/**************************************************************************
*                         H E A D E R   F I L E S                        *
**************************************************************************/
#define HOST_DBG 0

#include <stdio.h>
#include "app_com_def.h"
#include "common.h"

#include "sp5k_global_api.h"
#include "sp5k_dbg_api.h"
#include "api/sp5k_rsvblk_api.h"
#include "api/sp5k_usb_api.h"
// ----------------------------------------------------------------------
// Definition
// ----------------------------------------------------------------------
/* Video Class-Specific Request Codes */
#define RC_UNDEFINED    0x00
#define SET_CUR         0x01
#define SET_MIN         0x02
#define SET_MAX         0x03
#define GET_CUR         0x81
#define GET_MIN         0x82
#define GET_MAX         0x83 
#define GET_RES         0x84
#define GET_LEN         0x85
#define GET_INFO        0x86
#define GET_DEF         0x87

// ----------------------------------------------------------------------
// Gloabl variables
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------
static UINT32 u32UvcReq_PostOutDataExtensionUnit(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len);
static UINT32 u32UvcReq_PreInDataExtensionUnit(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len);

static UINT32 u32UvcReq_PreInDataExtensionUnit(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
       // Unit ID, wValue
       pData[0] = 0x00;
       DBG_PRINT("%s, %d\n", __FUNCTION__, __LINE__);
       return 1;
}

static UINT32 u32UvcReq_PostOutDataExtensionUnit(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
       // Unit ID, wValue
       DBG_PRINT("%s, %d\n", __FUNCTION__, __LINE__);
       return 1;
}

static UINT32 u32UvcReq_PreInGetMin(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
       pData[0] = 0x00;
       pData[1] = 0x00;
       DBG_PRINT("%s, %d\n", __FUNCTION__, __LINE__);
       return 1;
}

static UINT32 u32UvcReq_PreInGetMax(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
       pData[0] = 0x01;
       pData[1] = 0x00;
       DBG_PRINT("%s, %d\n", __FUNCTION__, __LINE__);
       return 1;
}

static UINT32 u32UvcReq_PreInGetRes(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
       pData[0] = 0x01;
       pData[1] = 0x00;
       DBG_PRINT("%s, %d\n", __FUNCTION__, __LINE__);
       return 1;
}

static UINT32 u32UvcReq_PreInGetInfo(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
       pData[0] = 0x03;
       DBG_PRINT("%s, %d\n", __FUNCTION__, __LINE__);
       return 1;
}

static UINT32 u32UvcReq_PreInGetDef(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
       pData[0] = 0x00;
       pData[1] = 0x00;
       DBG_PRINT("%s, %d\n", __FUNCTION__, __LINE__);
       return 1;
}

static UINT32 u32UvcReq_PreInGetLen(UINT16 wIndex, UINT16 wValue, UINT8 *pData, UINT16 len)
{
       pData[0] = 0x01;
       pData[1] = 0x00;
       DBG_PRINT("%s, %d  %d\n", __FUNCTION__, __LINE__,len);
       return 1;
}

void vUvcReq_ExtensionUnitCallbackInit(void)
{   
       HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvcrqst: %s %d",__FUNCTION__,__LINE__);
       sp5kUsbCfgSet( SP5K_USB_VNDR_REQ_WRITE_CB, SET_CUR, u32UvcReq_PostOutDataExtensionUnit ); 
       sp5kUsbCfgSet( SP5K_USB_VNDR_REQ_READ_CB, GET_CUR, u32UvcReq_PreInDataExtensionUnit ); 
       sp5kUsbCfgSet( SP5K_USB_VNDR_REQ_READ_CB, GET_MIN, u32UvcReq_PreInGetMin );        
       sp5kUsbCfgSet( SP5K_USB_VNDR_REQ_READ_CB, GET_MAX, u32UvcReq_PreInGetMax );               
       sp5kUsbCfgSet( SP5K_USB_VNDR_REQ_READ_CB, GET_RES, u32UvcReq_PreInGetRes );               
       sp5kUsbCfgSet( SP5K_USB_VNDR_REQ_READ_CB, GET_INFO, u32UvcReq_PreInGetInfo );                      
       sp5kUsbCfgSet( SP5K_USB_VNDR_REQ_READ_CB, GET_DEF, u32UvcReq_PreInGetDef );                             
       sp5kUsbCfgSet( SP5K_USB_VNDR_REQ_READ_CB, GET_LEN, u32UvcReq_PreInGetLen );                                    
}

