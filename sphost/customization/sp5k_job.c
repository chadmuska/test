/**************************************************************************
 *
 *       Copyright (c) 2005-2018 by iCatch Technology, Inc.
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

#include "common.h"
#include "stdio.h"
#include "customization/dev_init.h"
#include "customization/app_init.h"
#include "api/sp5k_msg_def.h"
#include "api/sp5k_disk_api.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_dcf_api.h"
#include "api/sp5k_gfx_api.h"
#include "api/sp5k_rsvblk_api.h"
#include "api/sp5k_usb_api.h"
#include "api/sp5k_ae_api.h"
#include "middleware/proflog.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if SIMPLE_UI_FLOW

#if API_TEST_INIT
#define USB_CONNECT() do { \
	printf(" Set USB mode to SP5K_USB_MODE_APITEST...\n"); \
	sp5kUsbModeSet(SP5K_USB_MODE_APITEST);sp5kUsbSwCon(1); \
} while (0)
#else
#define	USB_CONNECT() do { \
	printf(" Set USB mode to SP5K_USB_MODE_PCCAM...\n"); \
	sp5kUsbModeSet(SP5K_USB_MODE_PCCAM);sp5kUsbSwCon(1);\
} while (0)
#endif

#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appMain(UINT32 msgId,UINT32 msgParam);
extern UINT32 usbModeGet(void);
UINT32 appHostMsgEnableGet();
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void evbSimpleJobDo(UINT32 ,UINT32);
void evbOsdInit(void);
/**************************************************************************
 *                                                                        *
 *  Function Name: sp5kJobDo                                              *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
sp5kJobDo(
	void
)
{
	UINT32 msgId=0, param=0;

	profLogAdd(0, "[host]sp5kjob: host enter");
	#if 0
	cmdCustomDef("sio + ###"); cmdCustomDef("sio 2");
	#endif

	while ( 1 )
	{
		#ifdef TEST_SUITE
			sp5kHostMsgReceive(&msgId, &param);
			evbSimpleJobDo(msgId, param);
		#else
		sp5kHostMsgReceive(&msgId, &param);
		appMain(msgId,param);
		#endif
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: evbSimpleJobDo                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

#if SIMPLE_UI_FLOW

void
evbSimpleJobDo(
	UINT32 msgId,
	UINT32 param
)
{
	static UINT32 cardIn = 0;
	static UINT32 vbusIn = 0;
	static UINT32 val;

	switch( msgId )
	{


		/* capture mode message*/
		case SP5K_MSG_STILL_VIEW_SETTING_NOTIFY:
			break;

		case SP5K_MSG_AE_READY:
		{
			aeResult_t aeResult;
			SINT32 expidx, step, nextexp;
			UINT32 ret;
			ret = sp5kAePreviewResultGet(param,&aeResult);
			if (ret != SUCCESS ||
				!(aeResult.valid & AAA_INFO_RESULT_AE_VALID))
			{
				printf("ae result overrun\n");
			}
			else
			{
				UINT32 agcidx, nextagc;
				/***TODO****DO NOT use fixed numbers*/
				/***TODO****evinfo != evsync???*/
				expidx = aeResult.evInfo.expIdx;
				agcidx = aeResult.evInfo.agcIdx;
				step = aeResult.aeStep;
				if (expidx == 80)
				{
					nextexp = expidx;
					if ((SINT32)agcidx < step)
					{
						nextagc = 0;
					}
					else
					{
						nextagc = agcidx - step;
					}
					if (nextagc > 120)
					{
						nextagc = 120;
					}
					else if (nextagc < 90)
					{
						nextexp = 80+90-agcidx+step;
						nextagc = 90;
						if (nextexp > 144)
						{
							nextexp=144;
						}
					}
				}
				else
				{
					nextexp = expidx + step;
					nextagc = agcidx;
					if (nextagc < 90)
					{
						nextexp += 90 - nextagc;
						nextagc = 90;
					}
					if (nextexp > 144)
					{
						nextexp = 144;
					}
					else if (nextexp < 80)
					{
						nextagc = agcidx +80 - nextexp;
						nextexp = 80;
						if (nextagc > 120)
						{
							nextagc=120;
						}
					}
				}
				if (expidx == nextexp)
				{
					nextexp = SP5K_AE_EXP_AGC_DONT_CHANGE;

				}
				if (agcidx == nextagc)
				{
					nextagc = SP5K_AE_EXP_AGC_DONT_CHANGE;
				}
				sp5kPreviewExpAgcSet(nextexp,nextagc);
			}
			break;
		}
		default:
			#if TEST_SUITE==1
			TP_Command_ALL( msgId, param ); /* Test suite */
			#endif
			break;
	}
}

#endif
