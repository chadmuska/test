/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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
 *  Author:
 *
 **************************************************************************/
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_os_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_global_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_utility_api.h"
#include <ndk_types.h>

#include "jansson.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define UART2_INIT_THREAD_PRIORITY		29
#define UART2_INIT_THREAD_TIME_SLICE	0

#define UART_PIN_TX		26
#define UART_PIN_RX		27

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
SP5K_EVENT_FLAGS_GROUP Uart_sig;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appUartthread
 *------------------------------------------------------------------------*/
void
appUartthread(ULONG param)
{
    ULONG flags;
    //UINT32 err;
    UINT32 len;
    UINT8 stringBuf[128];

	for (;;) 
	{
		UINT32 err;
		err = sp5kOsEventFlagsGet(&Uart_sig, 1, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);
		HOST_ASSERT(err == SP5K_SUCCESS);

		while(1)
		{
            memset(stringBuf,0,128);
			len = sp5kUartBufRead(2, stringBuf , 128);
            if ( len > 0 ) {
                printf("read len(%u) %s\n",len,stringBuf);
            }
        }
	}
}

#if 0
int appUart2CallBack(int ch)
{

    static UINT8 lnIdx = 0;
	UINT8 c=(UINT8)ch;

    printf("%c",c);

    return 0;
}
#endif


/*-------------------------------------------------------------------------
 *  File Name : appUart2Init
 *------------------------------------------------------------------------*/
void 
appUart2Init(
	void
)	
{
	SP5K_THREAD *ptrdUart2;
	UINT32 err;

	profLogAdd(0, "uart2 init");
	sp5kGpioPullSet(SP5K_GPIO_GRP_FML, 0x0F000000, 0x0F000000 );
    sp5kUartModeSet(2, 1); /* 0:PIO, 1:DMA */
    sp5kUartCfgSet(2, SP5K_UART_FLOWCTRL, 0);   /* 0:SW flow ctrl, 1:HW flow ctrl */
    sp5kUartCfgSet(2, SP5K_UART_PIN_SEL, 0);    /* SP5K_UART_PORT_PERI_0, PIN1: 31(RX), 30(TX), 29(RTS), 28(CTS) */
    sp5kUartCfgSet(2, SP5K_UART_RX_TIMEOUT, 500);  /* Rx wait forever by default and the time unit is ms */ 
    sp5kUartCfgSet(2, SP5K_UART_PARITY, SP5K_UART_PARITY_DISABLE); /* SP5K_UART_PARITY_ODD, SP5K_UART_PARITY_EVEN */   
    sp5kUartInit(2, 115200);  /* High Speed up to 4Mbps */ 

    #if 0
    /* Support Only port 0 */
    sp5kUartRxCallbackSet(2,appUart2CallBack);
    #endif

    err = sp5kOsEventFlagsCreate(&Uart_sig, "sigApp");
	HOST_ASSERT(err == SP5K_SUCCESS);
	ptrdUart2 = sp5kOsThreadCreate( 
						"thrAppUart",
						appUartthread,
						0,
						UART2_INIT_THREAD_PRIORITY,
						UART2_INIT_THREAD_PRIORITY,
						UART2_INIT_THREAD_TIME_SLICE,
						TX_AUTO_START );
	HOST_ASSERT(ptrdUart2);

	err = sp5kOsEventFlagsSet(&Uart_sig, 1, TX_OR);
	HOST_ASSERT(err == SP5K_SUCCESS);
}

