
 /*--------------------------------------------------------------------------*
 *  File name	: 	App_uart2.c
 *  Function 	: 	UART2 Data Process		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.04.16  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/


#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_os_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_global_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_utility_api.h"
#include "app_infohandlerComm.h"

#include <ndk_types.h>

#define UART_INIT_THREAD_PRIORITY	26
#define UART_INIT_THREAD_TIME_SLICE	0

#define UART_PING_GROUP SP5K_GPIO_GRP_FML
#if defined(HW_JETDVR)
#define UART_PIN_TX		26
#define UART_PIN_RX		27
#define UART_PORT       2
#elif defined(HW_17DVR)
#define UART_PORT       1
#define UART_PIN_TX		21
#define UART_PIN_RX		22
#elif defined(HW_19DVR)
#define UART_PORT       1
#define UART_PIN_TX		21
#define UART_PIN_RX		22
#endif

#define BUF_SIZE 128

#define DEBUG 0

static int flagSkip = TRUE;
static int workBufIndex = 0;
static int recvIndex = 0;

/* paul 2018.04.16 Add for modify compiler warning*/
extern void appInfoHandler(UINT8 *);

/*--------------------------------------------------------------------------*
* Function name : checkData
* Function		: function for Check uart2 buffer data
* Return value	: result (BOOL TRUE/FALSE)
* Parameter1	: buffer pointer (--) 
* Parameter2	: int index pointer (--)
* Parameter3	: buffer2 pointer (--) 
* History	   	: 
*                	:  2018.04.16  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
BOOL checkData(UINT8 *buf, int *index ,UINT8 *buf2)
{
    UINT8 *s, *t , *sp = NULL;
    UINT checkSum, l;
    BOOL ret = FALSE;

   /*clear second buf*/	
    *buf2 = '\0';

    l = *index;
   
    if (l > BUF_SIZE) {
        printf("UART2 buffer over run\n");
        goto receive_error;
    }
    /* find START character and move begin of START staring to
     * first location of buffer
     */
    s = (UINT8 *) memchr((void *) buf, '$', l);
    if (s == NULL) {
        printf("Not found START character in %x len\n", l);
        goto receive_error;
    }
    if (s != buf) {
	#if DEBUG
        printf("START character is not begin of buffer \n");
	#endif
        l -= (s - buf);
        memmove((char *) buf, (char *) s, l);
	*index = l;
    }
    /*
     * find END string, if none, return false to continue receive.
     */
    s = (UINT8 *) memmem((void *) buf, l, "\r\n", 2);
    if (s == NULL) {
	#if DEBUG
        printf("Not found END string in buffer \n");
	#endif
        goto receive_continue;
    }
    
    /*
     * If has data after \r\n , copy data to second buf
     */
    if (&buf[l] > (s + 2)) {
        sp = (UINT8 *) memchr((void *) (s + 2), '$',
                    (&buf[l] - (s + 2)));
    }
  
     *s = '\0';
    /* find begin of check sum, skip 5 char ($DCMD) */
    s = (UINT8 *) memchr((void *) (buf + 5), '*', l - 5);
    if (s == NULL) {
        /* drop all buffer data for error handler */
	#if DEBUG
        printf("Not found check sum delimiter \n");
	#endif
        goto receive_error;
    }
    *s = '\0';
    checkSum = (UINT) strtol((char *) (s + 1), (char **) &t, 16);
    if (*t != '\0' || (s + 1) == t) {
        /* checksum is invalid */
        printf("No Checksum\n");
        goto receive_error;

    }
    for (l = 0, t = buf + 1; t < s; t++) {
        l ^= *t;
    }

    l &= 0xff;
    if ((l) != checkSum) {
        printf("Invalid checksum (%x - %x)\n", l, checkSum);
        goto receive_error;
    }
    ret = TRUE;

receive_error:
    if( sp != NULL){
        memcpy((char *) buf2, (char *) sp, *index - (sp - buf));
        *index -= sp - buf;
#if DEBUG
        printf("Buf2 data(%d) %s\n" , *index, buf2);
#endif
    } else {
        *index = 0;
    }

receive_continue:
    return ret;
}

/*--------------------------------------------------------------------------*
* Function name : Uart2Send
* Function		: Function for uart2 data send
* Return value	: status (UINT32)
* Parameter1	: buffer pointer (--) 
* Parameter2	: length of buffer (UINT32)
* History	   	: 
*                	:  2018.04.16  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
UINT32 Uart2Send(UINT8 *buf, UINT32 len)
{

    UINT32 status = 0;

    if (flagSkip)
	    goto out;
#if defined(PRJ_17DVR) || defined(PRJ_JETDVR)||defined(PRJ_19DVR)
    status = sp5kUartBufWrite(UART_PORT, buf, len);
#endif
out:
    return status;
}

 /*--------------------------------------------------------------------------*
* Function name : appUart2Stop
* Function		: Stop UART2 transmition/reciving
* Return value	: void
* Parameter1	: enable (BOOL TRUE/FALSE) 
* History	   	:  2017.07.31  Roger.Huang  "First edit"
*                	:  2018.04.16  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void appUart2Stop(BOOL en)
{
    /* re-enable UART clean buffer */
    if (en == FALSE) {
        workBufIndex = 0;
        recvIndex = 0;
    }
	flagSkip = en;

	printf("Uart2 transimition/reciving %s\n",
	        flagSkip ? "skip": "enable");
}
/*--------------------------------------------------------------------------*
* Function name : appUartthread
* Function		: UART thread process
* Return value	: void
* Parameter1	: param (ULONG)
* History	   	: 
*                	:  2018.04.16  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void appUartthread(ULONG param)
{
#if defined(PRJ_17DVR) || defined(PRJ_JETDVR)||defined(PRJ_19DVR)
    UINT8 recvBuf[4][BUF_SIZE];
    while (1) {
		UINT32 len;
        len = sp5kUartBufRead(UART_PORT, &recvBuf[workBufIndex][recvIndex],
                BUF_SIZE - recvIndex - 1);

	if (flagSkip)
		continue;

        /*
         * received from AVN are all printable, so free to set to '\0' at
         * last character.           *
         */
        if (len > 0) {
            recvIndex += len;
            recvBuf[workBufIndex][recvIndex] = '\0';
	#if DEBUG
            printf("read length (%d - %d) %s\n", len, index,
                    index > 0 ? (char *) recvBuf[workBufIndex] : "");
	#endif
            if (checkData(recvBuf[workBufIndex], &recvIndex,
                    recvBuf[(workBufIndex + 1) % 4])) {
                    	appInfoHandler(recvBuf[workBufIndex]);
		#if 0
                sp5kHostMsgSend(INFO_QUEUEMSG_UARTDATAREV,
                        (UINT32) recvBuf[workBufIndex]);
		#endif
                workBufIndex++;
                workBufIndex %= 4; /* wrap to correct buffer index range. */
            }
        }
    }
#endif
}

/*--------------------------------------------------------------------------*
* Function name : appUart2Init
* Function		:Initial the UART2
* Return value	: void
* Parameter1	: void
* History	   	: 
*                	:  2018.04.16  Paul.Chang     "Add function description & remove unused variable"
*--------------------------------------------------------------------------*/
void appUart2Init(void)
{
    //SP5K_THREAD *ptrdUart;


    printf("uart2 init\n");
#if defined(PRJ_17DVR) || defined(PRJ_JETDVR)||defined(PRJ_19DVR)
	SP5K_THREAD *ptrdUart;
    sp5kGpioFuncSet(UART_PING_GROUP, (1 << UART_PIN_TX), 0);
    sp5kGpioFuncSet(UART_PING_GROUP, (1 << UART_PIN_RX), 0);

    sp5kUartModeSet(UART_PORT, 0); /* 0:PIO, 1:DMA */
    sp5kUartCfgSet(UART_PORT, SP5K_UART_FLOWCTRL, 0);
    sp5kUartCfgSet(UART_PORT, SP5K_UART_PIN_SEL, 0);
    sp5kUartCfgSet(UART_PORT, SP5K_UART_CHAR_LEN, 8);
    sp5kUartCfgSet(UART_PORT, SP5K_UART_STOP_BIT, 1);
    sp5kUartCfgSet(UART_PORT, SP5K_UART_RX_TIMEOUT, 500);
    sp5kUartCfgSet(UART_PORT, SP5K_UART_TX_TIMEOUT, 500);
    sp5kUartCfgSet(UART_PORT, SP5K_UART_PARITY, SP5K_UART_PARITY_EVEN);
    sp5kUartInit(UART_PORT, 19200);

    ptrdUart = sp5kOsThreadCreate("thrAppUart", appUartthread, 0,
                            UART_INIT_THREAD_PRIORITY,
                            UART_INIT_THREAD_PRIORITY,
                            UART_INIT_THREAD_TIME_SLICE,
                            TX_AUTO_START);

    HOST_ASSERT(ptrdUart);
    appUart2Stop(FALSE);
#endif
}
