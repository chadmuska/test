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
 /*--------------------------------------------------------------------------*
 *  File name	: 	App_jet_sd_cert.c
 *  Function 	: 	SD Card certificate	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:2017.09.09  	             "First edited"
 *  			:2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/


#include "sp5k_global_api.h"
#include "ndk_global_api.h"
#include "app_com_def.h"
#include "app_sensor.h"
#include "app_disk.h"

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_infohandlerComm.h"
#include "app_jet_sd_cert.h"
#endif


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	TBD1,   		        
	TBD2,
	TBD3,
	TBD4,
	TBD5,
	TBD6,
	TBD7
}appJetSdCert;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


  /*--------------------------------------------------------------------------*
 * Function name	: appJetSdCertInit
 * Function	       	: Jet sdcert init sequence.
 * Return value   	: result (int true/false)
 * Parameter1    	: parameter (UINT32)
 * History	   :  2017.09.09  	                  "First edited"
 *                :  2018.04.12  Paul.Chang     "modify description"
 *--------------------------------------------------------------------------*/

int appJetSdCertInit(UINT32 param)
{
	UINT32 ret=0;

	if(IS_CARD_EXIST){
		
		printf("%s    %s\n" ,__FUNCTION__, "[sd cert]card is exist");
		switch (param)
		{
			UINT32 nsect;
			UINT32 type;
			UINT32 wProtect = 0 ;
			
			/**6
			Host use the INFO_TYPE in SD_ID_callback and sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_TOTAL_SECTOR, &nsect), 
			if (INFO_TYPE == SD_TYPE_V11 = 1) && (nsect >= 0x400000), then judge the SD as unusable.
			infomation about SD_ID_callback: http://k-public.icatchtek.com/doku.php?id=public:an:store:diskdriver#sd_identification_callback
			*/
			/** 28 7	
			Host use the INFO_TYPE in SD_ID_callback and sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_TOTAL_SECTOR, &nsect), 
			if (INFO_TYPE == SD_TYPE_SDHC = 3) && (nsect >= 0x4000000), then judge the SD as unusable.
			*/
			/** 14
			Host use sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_TOTAL_SECTOR, &nsect), 
			if ((nsect > 0x3FD8000) && (nsect < 0x4000000)), then judge the SD as unusable.
			*/
			/**29
			Host use the INFO_TYPE in SD_ID_callback, if (INFO_TYPE == SD_TYPE_UNKNOWN = 0), then judge the SD as unusable.	
			*/
			/** 16 17 27
			Host use sp5kDiskCfgGet(SP5K_DISK_WRITE_PROTECT_CFG, SP5K_DRIVE_SD, &wProtect), 
			if (wProtect != 0), then judge the SD as read only, can not record or format.
			*/
			case TBD1:
				sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_TOTAL_SECTOR, &nsect);
				sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_TYPE_QUERY, &type);
				sp5kDiskCfgGet(SP5K_DISK_WRITE_PROTECT_CFG, SP5K_DRIVE_SD, &wProtect);
				if ((wProtect != 0)||(type =0)||(((type = 1)||(type =3)) && (nsect >= 0x400000))||((nsect > 0x3FD8000) && (nsect < 0x4000000)))
					{
						printf("%s    %s\n" ,__FUNCTION__, "[sd cert]this card is not legal");
					
						//return 0; //unusable
						//break;
						ret=0;
					}
				else
				{
					ret=1;
					//return 1;
				}
			break;
			
			/**32
			Host can use sp5kDiskCfgSet( SP5K_DISK_SD18V_CTRL_PIN_CFG, SP5K_DRIVE_SD, SP5K_GPIO_GRP_NO_USE, 0); to disable SD 1.8V (UHS)
			*/
			case TBD2:
				
				sp5kDiskCfgSet( SP5K_DISK_SD18V_CTRL_PIN_CFG, SP5K_DRIVE_SD, SP5K_GPIO_GRP_NO_USE, 0); //to disable SD 1.8V (UHS)
				printf("%s    %s\n" ,__FUNCTION__, "[sd cert]this machine is not using uhs mode");
				
				//return 1;
				ret=1;
			break;
			
			default:
			break;
		}
	}
	
	//return FALSE;
	return ret;
}



  /*--------------------------------------------------------------------------*
 * Function name	: appJetSdcertState
 * Function	       	: Jet sdcert sequence state change.
 * Return value   	: void
 * Parameter1    	: state messages (UINT32)
 * Parameter2    	: messages  parameter (UINT32)
 * History	   :  2017.09.09  	                  "First edited"
 *                :  2018.04.12  Paul.Chang     "modify description"
 *--------------------------------------------------------------------------*/
void
appJetSdcertState(
	UINT32 msg,
	UINT32 param
)
{
	printf("%s    %s\n" ,__FUNCTION__, "[sd cert]in appJetSdcertState");
	switch (msg)
	{
		case APP_STATE_MSG_INIT:
			printf("%s    %s\n" ,__FUNCTION__, "[sd cert]in APP_STATE_MSG_INIT");
			BOOL result = appJetSdCertInit(param);
			printf("%s    %d\n" ,__FUNCTION__, result);
			appStateChange(appPreviousStateGet(),STATE_PARAM_NORMAL_INIT);
		break;
		case APP_STATE_MSG_CLOSE:
			//appJetPowerOffClose();
		    appStateCloseDone();
		    break;
		default:
			break;
	}
}


#define DEBUG_COLOR_CYAN          "\033[36m"


#define PRINT_CYAN(s, args...)     \
    printf(DEBUG_COLOR_CYAN STRINGIFY([sdxc] , s) DEBUG_COLOR_NORMAL, ##args);


//    printf(DEBUG_COLOR_CYAN STRINGIFY([js6n] ...., s) DEBUG_COLOR_NORMAL, ##args);


extern UINT32 Global_sdType;


  /*--------------------------------------------------------------------------*
 * Function name	: appInfoSD_CertCheck
 * Function	       	: Jet Check invalid SD Cards which probably fail to pass SDXC certification.
 * 				     According to the EXCEL File from Mr. Kadono.
 *
 * 				     NOTE: Call this subroutine after SD mount complete.
 
 * Return value   	: result (BOOL true/false
 						FALSE if fail to pass SDXC certification.)
 * Parameter1    	: state messages (UINT32)
 * Parameter2    	: messages  parameter (UINT32)
 * History	   :  2017.09.25  Jason	          "First edited"
 *                :  2018.04.12  Paul.Chang     "modify description"
 *                :  2018.04.16  Paul.Chang     "Remove unused variable"
 *--------------------------------------------------------------------------*/
BOOL appInfoSD_CertCheck(void)
{
    UINT32 sdType = Global_sdType; // passed from devDiskSdIdCallBack()
    UINT32 nSect = 0;
    UINT32 retval = 0;
    UINT32 FreeSizekBytes = 0;

#if (!ENABLE_SD_CERT_CHECK)
    /*
     *  if ENABLE_SD_CERT_CHECK is 0, it will disable the check of some
     *  special SD cards which might not pass SDXC certification.
     */
    return TRUE;
#endif

    //PRINT_CYAN("%s(%d) SP5K_DRIVE_SD=%d\n", __FUNCTION__, __LINE__, SP5K_DRIVE_SD);
    
    retval = sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_TOTAL_SECTOR, &nSect);
    PRINT_CYAN("%s(%d) ret=%d, SD nSect=0x%08x\n", __FUNCTION__, __LINE__, retval, nSect);

    retval = sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_FREE_SIZE_BY_KBYTE, &FreeSizekBytes);
    PRINT_CYAN("%s(%d) ret=%d, SD FreeSizekBytes=%d (0x%08x)\n", __FUNCTION__, __LINE__, retval, FreeSizekBytes, FreeSizekBytes);


    if ((nSect > 0x3FD8000) && (nSect < 0x4000000)) {
        PRINT_CYAN("SD is unusable. (SDXC No.14)\n");
        return FALSE;
    }

    switch(sdType) {
    case SD_TYPE_V11:
        PRINT_CYAN("SD_TYPE_V11\n");
        if (nSect > 0x400000) {
            PRINT_CYAN("SD is unusable. (SDXC No.6)\n");
            return FALSE;
        }
        else {
            return TRUE;
        }
        break;
    case SD_TYPE_V20:
        PRINT_CYAN("SD_TYPE_V20\n");
        return TRUE;
    
    case SD_TYPE_SDHC:
        PRINT_CYAN("SD_TYPE_SDHC\n");
        if (nSect >= 0x4000000) {
            PRINT_CYAN("SD is unusable. (SDXC No.7 and No.28)\n");
            return FALSE;
        }
        else {
            return TRUE;
        }
        return TRUE;

    case SD_TYPE_SDXC:
        PRINT_CYAN("SD_TYPE_SDXC\n");
        return TRUE;
    
    case SD_TYPE_MMC:
        PRINT_CYAN("SD_TYPE_MMC\n");
        return TRUE;

    default:
        PRINT_CYAN("SD_TYPE Zero.\n");  // SD_TYPE_UNKNOWN
        PRINT_CYAN("SD is unusable. (SDXC No.29)\n");
        return FALSE;
    } // END switch(sdType)

    return TRUE;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appInfoSD_CertCheck
 * Function	       	: Jet Check invalid SD Cards which probably fail to pass SDXC certification.
 * 				     According to the EXCEL File from Mr. Kadono.
 *
 * 				     NOTE: Call this subroutine after SD mount complete.
 *
 * Return value   	: result (BOOL true/false
 *						TRUE if this Card is write protected.)
 * Parameter1    	: void
 * History	   :  2017.09.25  Jason		  "First edited"
 *		   :  2017.10.21  Jason		  "After consulting with Kadono San, write-protected 
 * 								   SD Cards should be treated as NOT_SUPPORT_CARDS."
 *                :  2018.04.12  Paul.Chang     "modify description"
 *--------------------------------------------------------------------------*/
BOOL appInfoSD_isWriteProtect(void)
{
    UINT32 wProtect;

    //PRINT_CYAN("line=%d#\n", __LINE__);

    sp5kDiskCfgGet(SP5K_DISK_WRITE_PROTECT_CFG, SP5K_DRIVE_SD, &wProtect);
    PRINT_CYAN("line=%d, WP=%u\n", __LINE__, wProtect);  // wProtected(value=1)

    if (wProtect == 0) {
        return FALSE;
    } else if (wProtect == 1) {
        return TRUE;
    } else {
        printf("WARNING: wProtect=%u\n", wProtect);
        return TRUE;  // fail-safe
    }

}


