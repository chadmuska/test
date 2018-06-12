/**************************************************************************
 *
 *       Copyright (c) 2002-2018 by iCatch Technology, Inc.
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
 *  File Description:
 *
 *  Author: Chris Wang
 *
 ******************************************************************************/
/**
 * @file app_sd_log.c
 * @brief Logging error message to storage
 * @author chris
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "sp5k_msg_def.h"
#include "sp5k_disk_api.h"
#include "sp5k_os_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_global_api.h"
#include "sp5k_utility_api.h"
#include "app_timer.h"
#include "cmd.h"
#include "debug_mod/dbg_api.h"
#include "middleware/proflog.h"
#include "gpio_custom.h"
#include "app_dead_process.h"
#include "customization/dev_init.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DEBUG_PRINT_LOG_SD              1           /* 0:disable , 1:enable */
#define LOG_SIGNATURE_FILE_NAME_NAND    "D:\\NANDMARK.LOG"
#define LOG_SIGNATURE_FILE_NAME_SD      "D:\\SDMARK.LOG"
#define LOG_BUFFER_SIZE (1024*16)

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if 0
    #define PROF_LOG_ADD(...)
    #define PROF_LOG_PRINTF(...)
    #define LOGDBG_PRINT(fmt, args...)
#else
    /* #define PROF_LOG_ADD		profLogAdd */
    #define PROF_LOG_PRINTF		profLogPrintf
    #define LOGDBG_PRINT(fmt, args...) printf(fmt, ##args)
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 logfname[]="C:\\ICATCH00.LOG";         /* log file name */
static UINT32 isEnableLog = APP_LOG_TO_SD_ENABLE;   /* SP5K_MSG_DISK_REMOVAL -> disable logging
														SP5K_MSG_DISK_INSERT  -> Enable logging
		                                   				SP5K_MSG_DISK_INSERT  -> Enable logging
														1 to store log, 0 no store */

static UINT32 logDevId = -1 ;
static UINT32 logBufferSize = LOG_BUFFER_SIZE;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 dbgPrintLogInit(UINT32 size);
extern void dbgPrintLogFnameSet(UINT8 *fname);
extern void dbgPrintLogConfirmCallbackSet(void *fp);
extern void ros_thread_sleep(UINT32 ms);
/* int uartWriteByte(UINT32 port,int ch); */
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#define MAX_SECTION 50
#define MAX_NAME 50
#define INI_ALLOW_BOM 0
#define INI_ALLOW_MULTILINE 0

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

/**
 * @brief Strip whitespace chars off end of given string, in place. Return s.
 *
 * @param[in] s
 * @return s
 */
static char* rstrip(char* s)
{
    char* p = s + strlen(s);
    while (p > s && isspace((UINT8)(*--p)))
        *p = '\0';
    return s;
}

/**
 * @brief Return pointer to first non-whitespace char in given
 *        string.
 *
 * @param[in] s
 * @return s
 */
static char* lskip(const char* s)
{
    while (*s && isspace((UINT8)(*s)))
        s++;
    return (char*)s;
}

/**
 * @brief Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment.
 *
 * @param[in] s
 * @return s
 */
static char* find_char_or_comment(const char* s, char c)
{
    int was_whitespace = 0;
    while (*s && *s != c && !(was_whitespace && *s == ';')) {
        was_whitespace = isspace((UINT8)(*s));
        s++;
    }
    return (char*)s;
}

/**
 * @brief Version of strncpy that ensures dest (size bytes) is null-terminated.
 *
 * @param[in] dest
 * @param[in] src
 * @param[in] size
 * @return s
 */
static char* strncpy0(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
}



/**
 * @brief get line of string from file
 *
 * @param[in] lineBuff
 * @param[in] lineSize
 * @param[in] fd
 * @return SUCCESS / FAIL
 */
UINT32
myfgets(
   UINT8* lineBuff,
   UINT32 lineSize,
   UINT32* fd)
{
    static UINT32 fileSize = 0 ;
    static UINT8* buff = 0 ;
    static UINT32 buffSize = 0 ;
    static UINT32 length = 0 ;
    static UINT32 buffPos = 0 ;

    UINT32 done = 0 ;
    UINT32 i = 0 ;

    if ( !buffSize ) {
        length = 0 ;
        buffPos = 0 ;
        fileSize = sp5kFsFileSizeGet(*fd);
        if ( fileSize == 0 ) {
            goto ERROR_RETURN;
        }

        if ( fileSize > 512 ) {
            buffSize = 512 ;
        } else {
            buffSize = fileSize ;
        }
        buff = sp5kMalloc(buffSize);
        if ( !buff ) {
            goto ERROR_RETURN;
        }
    }

    if ( ( fileSize == 0 ) && ( length == 0 ) ) {
        goto ERROR_RETURN;
    }

    for (i = 0 ;  !done && i < lineSize - 1; i++ , buffPos++ , length-- ) {
        if ( length == 0 ) {
            if ( fileSize == 0 ) {
                done = 1;
                break;
            } else {
                buffSize = sp5kFsFileRead(*fd, buff, buffSize);
                if ( buffSize == 0 ) {
                    goto ERROR_RETURN;
                }
                buffPos = 0 ;
                length = buffSize ;
                fileSize = fileSize - buffSize ;
                if (( fileSize != 0 ) && ( fileSize < buffSize )) {
                    buffSize = fileSize ;
                }
            }
        }
        lineBuff[i] = buff[buffPos];
        if ( lineBuff[i] == '\n' ) {
            done = 1;
        }
    }
    lineBuff[i] = '\0' ;
    return SUCCESS;

ERROR_RETURN:
    if (buff)
        sp5kFree(buff);
    buffSize = 0 ;
    fileSize = 0 ;
    return FAIL;
}

/**
 * @brief parse INI file
 *
 * @param[in] fd
 * @param[in] handler
 * @param[in] user
 * @return SUCCESS / FAIL
 */
UINT32
iniParseFile(
   UINT32* fd,
   UINT32 (*handler)(void*, const char*, const char*,const char*),
   void* user)
{

    /* Uses a fair bit of stack (use heap instead if you need to) */

    UINT8* line = 0 ;
    UINT32 ret = 0;
    UINT32 error = 0;


    char section[MAX_SECTION] = "";
    char prev_name[MAX_NAME] = "";

    char* start;
    char* end;
    char* name;
    char* value;
    UINT32 lineno = 0;


    line = sp5kMalloc(256);
    if (!line) {
        return FAIL;
    }
    ret = sp5kFsFileSeek(*fd, 0, SP5K_FS_SEEK_SET);
    if ( ret == FAIL ) {
        goto INIPARSE_EXIT;
    }

    /* Scan through file line by line */
    while (1) {
        ret = myfgets(line, 256, fd);
        if ( ret == FAIL ) {
            goto INIPARSE_EXIT;
        }
        lineno++;

        start = (char *)line;
#if INI_ALLOW_BOM
        if (lineno == 1 && start[0] == 0xEF &&
                           start[1] == 0xBB &&
                           start[2] == 0xBF) {
            start += 3;
        }
#endif
        start = lskip(rstrip(start));

        if (*start == ';' || *start == '#') {
            /* allow '#' comments at start of line */
        }
#if INI_ALLOW_MULTILINE
        else if (*prev_name && *start && start > line) {
            if (!handler(user, section, prev_name, start) && !error)
                error = lineno;
        }
#endif
        else if (*start == '[') {
            /* A "[section]" line */
            end = find_char_or_comment(start + 1, (char)']');
            if (*end == ']') {
                *end = '\0';
                strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            }
            else if (!error) {
                /* No ']' found on section line */
                error = lineno;
            }
        }
        else if (*start && *start != ';') {
            /* Not a comment, must be a name[=:]value pair */
            end = find_char_or_comment(start, (char)'=');
            if (*end != '=') {
                end = find_char_or_comment(start, (char)':');
            }
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = lskip(end + 1);
                end = find_char_or_comment(value, (char)'\0');
                if (*end == ';')
                    *end = '\0';
                rstrip(value);

                /* Valid name[=:]value pair found, call handler */
                strncpy0(prev_name, name, sizeof(prev_name));
                if (!handler(user, section, name, value) && !error) {
                    error = lineno;
                }
            }
            else if (!error) {
                /* No '=' or ':' found on name[=:]value line */
                error = lineno;
            }
        }
    }

INIPARSE_EXIT:
    if (line)
        sp5kFree(line);
    if ( error ) {
        printf("configure file ERROR line %u\n",error);
    }
    return SUCCESS;
}


/**
 * @brief file handle to check INI file
 *
 * @param[in] fd
 * @param[in] handler
 * @param[in] user
 * @return SUCCESS / FAIL
 */
static UINT32 configure(void* user, const char* section, const char* name,
                  const char* value)
{
/*
INI file content
[LOGCONFIG]
BUFFER_SIZE=1048576     ;1024KB
LOG_DISK=1              ;1->SD Card  or 0->NAND
POWEROFF=1              ;0->REBOOT or 1->POWEROFF
POWERKEY_DETECT = 1     ;0: Disable Power Key Detect , 1: Enable Power Key Detect
LOGFILE = TEST.LOG      ;8.3 format file name
*/
#if 1
    static char prev_section[50] = "";

    if (strcmp(section, prev_section)) {
        strncpy(prev_section, section, sizeof(prev_section));
        prev_section[sizeof(prev_section) - 1] = '\0';
    }
    PROF_LOG_PRINTF(0,"[%s]%d %s = %s\n",__FUNCTION__,__LINE__, name, value);
    LOGDBG_PRINT("[%s]%d %s = %s\n",__FUNCTION__,__LINE__, name, value);
#endif
    if (MATCH("LOGCONFIG", "BUFFER_SIZE")) {
        logBufferSize = atoi(value);
    } else if (MATCH("LOGCONFIG", "LOG_DISK")) {
        /* Add in the future */
    } else if (MATCH("LOGCONFIG", "POWEROFF")) {
#if 0
        if ( atoi(value) == 1 )
            bKeyASSERTPowerOff = 1 ;
        else
            bKeyASSERTPowerOff = 0 ;
#endif
    } else if (MATCH("LOGCONFIG", "POWERKEY_DETECT")) {
#if 0
        if ( atoi(value) == 1 )
            isKeyDetectRunning = 0 ;
        else
            isKeyDetectRunning = -1 ;
#endif
    } else if (MATCH("LOGCONFIG", "LOGFILE")) {
        /* Add in the future */
#if 0
    #if APWDG_UT
    } else if (MATCH("LOGCONFIG", "TESTCASE")) {
        /* Add in the future */
        testCase = atoi(value) ;
    #endif
#endif
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;

}

/**
 * @brief callback to decide store or not which is called before
 *  	  log store start return 1 to store log, return 0 no
 *  	  store
 * @param[in] mode the dump mode, 0=file create, 1=file append,
 *  	 2=file close
 * @return 1 or 0
 * @author chris
 */
static UINT32
appLogMsgToStorageCallback(
	UINT32 mode
)
{
    sp5kLogControl(SP5K_LOG_ENABLE);

    tmx_t rtc;

    printf("%s 0x00C0(0x%x)\n",__FUNCTION__,*((volatile UINT8 *)0xB00000C0));
    printf("%s 0x00C1(0x%x)\n",__FUNCTION__,*((volatile UINT8 *)0xB00000C1));

    sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION | SP5K_DATE_TIME_BLOCK_WAIT , &rtc);
    /* log crash time to log file */
    printf("%s Enable Option(%u) ASSERT Time - %d/%d/%d %d:%d:%d\n",__FUNCTION__,isEnableLog,
           rtc.tmx_year+1900,rtc.tmx_mon,rtc.tmx_mday,rtc.tmx_hour,rtc.tmx_min,rtc.tmx_sec);
	if(IS_CARD_EXIST){
		return isEnableLog ;
	} else {
		return 0 ;
	}
}


/**
 * @brief check SD card must have a file named "SD.LOG" or
 *        "NAND.LOG" in root directory.
 * @return SUCCESS or FAIL
 * @author chris
 */

UINT32
appInitLogMsgToStorage(
	void
)
{
    UINT32 fd;
	UINT32 devId;
    UINT32 ret = FAIL ;


	if ( logDevId != -1 ) {
		/* Already logging */
		return SUCCESS;
	}

	fd = sp5kFsFileOpen((UINT8 *)LOG_SIGNATURE_FILE_NAME_SD, SP5K_FS_OPEN_RDONLY);
#if DEBUG_PRINT_LOG_SD
	LOGDBG_PRINT("[%s] line %d: open file %s ", __FUNCTION__,
		__LINE__, LOG_SIGNATURE_FILE_NAME_SD);
#endif

	devId = SP5K_DRIVE_SD ;
	if ( fd == 0 ) {
		if(devId == SP5K_DRIVE_SD){//paul 2018 0402 test- Create file which does not exist  
			LOGDBG_PRINT("FAIL\n");
			fd = sp5kFsFileOpen((UINT8 *)LOG_SIGNATURE_FILE_NAME_SD, SP5K_FS_OPEN_CREATE);
			LOGDBG_PRINT("[%s] line %d: open file %s", __FUNCTION__,
				__LINE__, LOG_SIGNATURE_FILE_NAME_SD);
			//sp5kFsFileWrite(fd,"test",5);
			//sp5kFsFileClose(fd);
		}
		else{
			LOGDBG_PRINT("FAIL\n");
			fd = sp5kFsFileOpen((UINT8 *)LOG_SIGNATURE_FILE_NAME_NAND, SP5K_FS_OPEN_RDONLY);
			LOGDBG_PRINT("[%s] line %d: open file %s ", __FUNCTION__,
				__LINE__, LOG_SIGNATURE_FILE_NAME_NAND);
		}
		#if 0
		if ( fd == 0 ) {
			LOGDBG_PRINT("FAIL\n");
            fd = -1 ;
            #if 0
                #ifndef _EVB_HW_
                    goto ALWAYS_ENABLE;
                #endif
            #endif
			return FAIL;
		}
		devId = SP5K_DRIVE_NAND ;
		#endif
	}
	LOGDBG_PRINT("SUCCESS\n");
	if(fd){
        ret = iniParseFile(&fd, configure, NULL);
    	sp5kFsFileClose(fd);

        if ( ret == FAIL ) {
            printf("[SDLOG]%s %d,FAIL",__FUNCTION__,__LINE__);
            return FAIL;
        }

    	fd = 0 ;
    	logfname[0] = ( devId == SP5K_DRIVE_NAND ? 'C' : 'D' );
    	logDevId = devId ;
    	dbgPrintLogFnameSet(logfname);
    	LOGDBG_PRINT("%s Logging File Name (%s)\n",__FUNCTION__,logfname);
	}

    dbgPrintLogInit(logBufferSize);
    dbgPrintLogConfirmCallbackSet(appLogMsgToStorageCallback);

    sp5kLogControl(SP5K_LOG_ENABLE);

	return SUCCESS;
}



/**
 * @brief initization log debug message to SD card and check the
 *        sd card plug in/out message.
 * @param[in] msg receive message
 * @param[in] param receive parameter
 * @return SUCCESS or FAIL
 */

UINT32
appLogMsgToStorage(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = FAIL ;
	switch( msg )
	{
#if 0
	case SP5K_MSG_DISK_INSERT:
		LOGDBG_PRINT("%s[%s] SP5K_MSG_DISK_INSERT param %d\n",__FILE__,__FUNCTION__,param);
		break;
	case SP5K_MSG_DISK_REMOVAL:
		LOGDBG_PRINT("%s[%s] SP5K_MSG_DISK_REMOVAL param %d\n",__FILE__,__FUNCTION__,param);
		if ( logDevId == param ) {
			/* Need to check for SD card plug-out */
			isEnableLog = 0 ;
		}
		break;
#endif
	case SP5K_MSG_DISK_MOUNT_COMPLETE:
#if DEBUG_PRINT_LOG_SD
		LOGDBG_PRINT("%s[%s] SP5K_MSG_DISK_MOUNT_COMPLETE param %u\n",__FILE__,__FUNCTION__,param);
#endif
		if ( ( param == SP5K_DRIVE_NAND ) ||
		     ( param == SP5K_DRIVE_SD   ) ) {
			ret = appInitLogMsgToStorage();
			if (ret == SUCCESS) {
				if ( logDevId == param ) {
					/* Need to check for SD card plug-in */
					isEnableLog = 1 ;
				}
			}
		}
		break;
	default:
		break;
	}

	return ret;
}


/* init Log to SD function */
void
appSDLogMsg(
	void
)
{
#if DEBUG_PRINT_LOG_SD
    profLogPrintf(0,"[SDLog.int] init");
    printf("[SDLog.int] init\n");
#endif

	UINT32 ret = FAIL;

	if(IS_CARD_EXIST)
	{
		ret = sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_STATE, &ret);
		if(ret == SUCCESS){
            appLogMsgToStorage(SP5K_MSG_DISK_MOUNT_COMPLETE,SP5K_DRIVE_SD);
            printf("[SDLog.ISR]%s %d, SD ready.\n",__FUNCTION__,__LINE__);
			return ;
		}
        else
        {
            printf("[SDLog.ISR]%s %d, SD not ready.\n",__FUNCTION__,__LINE__);
        }
	}
    /* ret = sp5kTimerUsIsrReg(SP5K_TIMER_US_ISR_ID_1 , 1 , 1000000, appSDLogMsg); */
    /*Reg ISR again if SD not ready.*/
    ret = sp5kTimerIsrOneShotReg(0,1000,appSDLogMsg);


    if ( ret == FAIL ) {
        printf("[SDLog.ISR]%s %d, FAIL\n",__FUNCTION__,__LINE__);
        return;
    }
    else
    {
#if DEBUG_PRINT_LOG_SD
        printf("[SDLog.ISR]%s %d, SUCCESS\n",__FUNCTION__,__LINE__);
#endif
    }
}

