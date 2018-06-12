 /*--------------------------------------------------------------------------*
 *  File name	: 	App_cmd_gsensor.c
 *  Function 	: 	Get the G-sensor data and converter the data format 		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "api/sp5k_utility_api.h"
#include "api/sp5k_fs_api.h"
#include "api/sp5k_os_api.h"
#include "app_gSensor_common.h"
#include "app_General.h"
#include "app_infohandlerComm.h"
#define PGSENSOR_MAX	800
#define PGSENSOR_ZERO	400

extern void appGsensorInit(void);
extern void appGsensorCal(INT16* acc, UINT8* offset);
extern int appGsensorCalSave();
extern void appGsensorParkingMode(BOOL);
extern void appGsensorClearIRQ(void);
extern void appGsensorLevelCtrl(int mode, int level);
extern void appGsensorUpdateCompensation(UINT8 *compensation);
extern void appGsensorFilter(BOOL on);
extern void appGsensorAccXYZ(INT16 *x, INT16 *y, INT16 *z);
extern void _appGsensorAccXYZ(INT16 *x, INT16 *y, INT16 *z);
extern void appGsensorLpfSet(int lpf);
/*paul 2018.04.16 add for compiler warning*/
extern void appGsensorAccXYZMax(INT16 *x, INT16 *y, INT16 *z);


static UINT8 compensation[] = {0, 0, 0};   /* G sensor compensation for X, Y, and Z */

static SP5K_THREAD *gsensor_log = NULL;

static volatile BOOL gsensor_log_stop = 0;
static UINT32 log_period;


  /*--------------------------------------------------------------------------*
 * Function name	: appGetGsensorRawInfo
 * Function	       	: Get Gsensor RAW information
 * Return value   	: string data buffer pointer  (--)
 * Parameter1    	: void
 * Parameter2    	: buffer size (UINT32)
 * History	   :
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

char *appGetGsensorRawInfo()
{
    INT16 acc[3];
    static char buf[] = "16400,16400,16400";

    appGsensorAccXYZ(&acc[0], &acc[1], &acc[2]);
    snprintf(buf, sizeof(buf), "%d,%d,%d", acc[0], acc[1], acc[2]);

    return buf;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appGsesnorLogThread
 * Function	       	: Gsensor log thread process
 * Return value   	: void
 * Parameter1    	: parameter (ULONG)
 * History	   :
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appGsesnorLogThread(ULONG lparam)
{
    UINT32 fd = lparam;
    char buf[64];
    //UINT32 i;
    UINT32 sec_tick;
    tmx_t rtc;

    gsensor_log_stop = 0;
    sec_tick = sp5kOsTimeGet() + 100;
    sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION, &rtc);
    while (gsensor_log_stop == 0) {
		UINT32 i;
		UINT32 tick;
        tick = sp5kOsTimeGet();
        if (tick >= sec_tick) {
            sec_tick += 100;
            sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION, &rtc);
        }
        i = snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03u,%s\n",
                rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec,
                (tick - (sec_tick - 100)) * 10,
                appGetGsensorRawInfo());
        sp5kFsFileWrite(fd, (UINT8 *)buf, i);
        sp5kOsThreadSleep(log_period);
    }

    printf("gsensor log thread stop!!!\n");
    sp5kFsFileClose(fd);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appGsensorLog
 * Function	       	: Gsensor log to file
 * Return value   	: void
 * Parameter1    	: file name pointer(--)
 * Parameter2    	: set value - how much time to save the log once(UINT ms)
 * History	   :
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void appGsensorLog(char *filename, UINT32 ms)
{
    UINT32 fd, i;
    char buf[48];

    if (gsensor_log) {
        printf("gsensor log thread run already\n");
        return;
    }

    fd = sp5kFsFileOpen((const UINT8 *)filename,
            SP5K_FS_OPEN_CREATE | SP5K_FS_OPEN_RDWR);

    if (fd == 0) {
        printf("gsensor log thread open file %s fail\n", filename);
        return;
    }

    i = snprintf(buf, sizeof(buf), "compensation,%u,%u,%u\n",
            compensation[0], compensation[1], compensation[2]);
    sp5kFsFileWrite(fd, (UINT8 *)buf, i);

    log_period = ms;
    gsensor_log = sp5kOsThreadCreate("GSensorLog", appGsesnorLogThread,
            fd, 31, 31, 0, TX_AUTO_START);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appGsensorRaw2Pais
 * Function	       	: The function convert raw data of axis to PAIS format
 * Return value   	: PAIS format that multiply 100 and + 400 (UINT32)
 * Parameter1    	: uintG Raw data of G sensor(int16)
 * History	   :  2017.07.25  Roger.Huang  "First edit"
 *                :  2018.04.12  Paul.Chang     "Modify description"
 *--------------------------------------------------------------------------*/
 
UINT32 inline appGsensorRaw2Pais(INT16 intG)
{
    INT32 i;
    i = ((intG * 39) + 50) / 100 + 400;

    /* Check @return value is positive */
    if (i < 0)
	    i = 0;
    return (UINT32) i;
}


  /*--------------------------------------------------------------------------*
 * Function name	: appGsensorAccPXYZ
 * Function	       	: This function is followed PAIS Navi protocol for G sensor.
 * Return value   	: static string buffer pointer (--)
 * Parameter1    	: void
 * History	   :  2017.07.25  Roger.Huang  "First edit"
 *                :  2018.04.12  Paul.Chang     "Modify description"
 *--------------------------------------------------------------------------*/
 
char *appGsensorAccPXYZ(void)
{
    INT16 acc[3];
    UINT32 pacc[3];
    static char buf[] =  "65535,65535,65535";
    int i;

    appGsensorAccXYZMax(&acc[0], &acc[1], &acc[2]);

    for (i = 0; i < 3; i++) {
        pacc[i] = appGsensorRaw2Pais(acc[i]);

        /* check value out of bound */
        if (pacc[i] >= PGSENSOR_MAX)
            pacc[i] = PGSENSOR_ZERO;
    }

    snprintf(buf, sizeof(buf), "%u,%u,%u", pacc[0], pacc[1], pacc[2]);

    return buf;
}


  /*--------------------------------------------------------------------------*
 * Function name	: cmdGsensor
 * Function	       	: Gsensor command process
 * Return value   	: void
 * Parameter1    	: argument count (UINT32)
 * Parameter2    	: argument pointer (--)
 * Parameter3    	: value array (UINT32) 
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void cmdGsensor(UINT32 argc, UINT8 *argv[], UINT32 v[])
{
    INT16 acc[3];

    if (argc == 1) {
        printf("gsensor\n"
                "\tgsensor init: initail g-sensor\n"
                "\tgsensor xyz: get X Y Z data\n"
                "\tgsensor pxyz: get PAIS format X, Y, and Z\n"
                "\tgsensor cal: calibrate\n"
                "\tgsensor clear: clear IRQ status\n"
                "\tgsensor level mode level : mode 0 - disable, 1 - normal , "
                "2 - parking. level valid rang is 0 - 4\n"
                "\tgsensor log filename [ms]: start G-sensor logger thread and "
                "store data on filename every [ms]\n"
                "\tgsensor stop: Stop logger thread\n"
                "\tgsensor term: thread data collect back to OS\n"
                "\tgsensor lpf $LPF: set $LPF value\n");
    } else if (strcmp((char *) argv[1], "init") == 0) {
        appGsensorInit();
    } else if (strcmp((char *) argv[1], "xyz") == 0) {
        _appGsensorAccXYZ(&acc[0], &acc[1], &acc[2]);

        printf("X %d  Y %d  Z %d\nX %fG y %fG z %fG", acc[0], acc[1], acc[2],
                (float) acc[0] * 0.0039, (float) acc[1] * 0.0039,
                (float) acc[2] * 0.0039);

    } else if (strcmp((char *) argv[1], "pxyz") == 0) {
        printf("PXYZ : %s\n",
                appGsensorAccPXYZ());
    } else if (strcmp((char *) argv[1], "cal") == 0) {
        if (argc >= 3 && strcmp((char *) argv[2], "raw") == 0) {
            appGsensorFilter(1);
        } else
            appGsensorFilter(0);

        _appGsensorAccXYZ(&acc[0], &acc[1], &acc[2]);
        appGsensorCal(acc, compensation);
        printf("G sensor compensation - 0X%02hhX 0X%02hhX 0X%02hhX\n",
                compensation[0], compensation[1], compensation[2]);
    } else if (strcmp((char *) argv[1], "save") == 0) {
        printf("Cal's data saved and remain %d cycles\n", appGsensorCalSave());
    } else if (strcmp((char *) argv[1], "clear") == 0 && argc == 2) {
        appGsensorClearIRQ();
    } else if (strcmp((char *) argv[1], "log") == 0 && argc >= 3) {
        UINT32 ms = 50;

        if (argc > 3 && v[3] != 0)
            ms = v[3];
        appGsensorLog((char *) argv[2], ms);
    } else if (strcmp((char *) argv[1], "stop") == 0) {
        if (gsensor_log == NULL)
            return;
        gsensor_log_stop = 1;
    } else if (strcmp((char *) argv[1], "term") == 0) {
        UINT32 ret;

        if (gsensor_log_stop) {
            printf("gsensor thread still run\n");
            return;
        }

        if (gsensor_log == NULL)
            return;

        ret = sp5kOsThreadTerminate(gsensor_log);
        printf("thread terminate %u\n", ret);

        ret = sp5kOsThreadDelete(gsensor_log);
        printf("thread delete %u\n", ret);

        gsensor_log = NULL;
    } else if (strcmp((char *) argv[1], "level") == 0 && argc == 4) {
        appGsensorLevelCtrl(v[2], v[3]);
    } else if (strcmp((char *) argv[1], "compensation") == 0 && argc == 5) {
        UINT8 off[3];
        int i;

        for (i = 0; i < 3; i++) {
            off[i] = (UINT8) (0xff & v[2 + i]);
        }

        appGsensorUpdateCompensation(off);
    } else if (strcmp((char *)argv[1], "lpf") == 0 && argc == 3) {
	appGsensorLpfSet(v[2]);
    }
}
