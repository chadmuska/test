
#ifndef _APP_INFOHANDLERCOMM_H_
#define _APP_INFOHANDLERCOMM_H_

#include "middleware/sysg_msg_def.h"

#define USER_RECORD_PATH "D:/MANUAL"
#define EVENT_PATH "D:/GEVENT"
#define NORMAL_PATH "D:/DCIM"
#define USERSAVE_PATH "D:/USERSAVE"
/*
* Define name : APP_17DVR_FW
* Use for dual boot ctrl
* 1:Backup system
* 2:Woking system
*/
#ifndef APP_17DVR_FW
#define APP_17DVR_FW	1
#endif
#define PATH_SIZE 15
#define FILENAME_SIZE 30
/*
* Define name : APP_VERSION
* Define name : APP_SUB_VERSION
* Define name : APP_REVISION
* Version Ctrl for Jet
*/
#define APP_MODELNAME			50	/*VQ*/
#define APP_VERSION			868	/*VQ*/
#define APP_REVISION			0	/*VQ*/

typedef enum{

STATUS_NONE,
Fail_INIT,
Fail_PREVIEW,
Fail_RECORDING,
Fail_PLAYIDLE,
Fail_PLAYING,
Success_INIT,
Success_PREVIEW,
Success_RECORDING,
Success_PLAYIDLE,
Success_PLAYING,

}ModeChangeStatus;


typedef enum{
DISK_NORMAL ,  /*normal*/
DISK_NONE ,  /* No disk */
DISK_FULL ,  /*Disk is full */
DISK_ERROR ,  /*read/write error*/
DISK_FORMAT_ERROR,
DISK_FORMAT_WARNING,
DISK_FORMAT_BREAK,
DISK_FILE_DAMAGE,
DISK_SIZE_LACKING,
DISK_SIZE_FAIL,
DISK_FULL_USER ,  /*User Disk Full*/

}DiskStatus;


typedef enum{
MOVEFILE_NONE,
MOVEFILE_START,
MOVEFILE_STILL,
MOVEFILE_WAIT_VIDEOFINISH,
MOVEFILE_FINISH,
}MoveFileState;


typedef enum{
INFO_QUEUEMSG_NONE = SP5K_MSG_HEAD_CUSTOMER,
INFO_QUEUEMSG_STATECHANGE,	
#if 0
INFO_QUEUEMSG_VIDEOCALLBACK,
#endif
INFO_QUEUEMSG_UARTDATAREV,
INFO_QUEUEMSG_EVENTTIGGER,
INFO_QUEUEMSG_REGULAR,
INFO_QUEUEMSG_CONNECTDEV,
INFO_QUEUEMSG_POSREQUEST,
INFO_QUEUEMSG_PLAYPOS,
INFO_QUEUEMSG_DISKSTATE,
INFO_QUEUEMSG_SDPLUGTIMER,
INFO_QUEUEMSG_BUZZERONOFF,
INFO_QUEUEMSG_POWER,
INFO_QUEUEMSG_HWWDT,
#if 0
INFO_QUEUEMSG_PREAVNE,
#endif
}InfoThreadQueueMsg;

typedef enum{
BUZZER_CTRL_NONE,
BUZZER_CTRL_POWERON,
BUZZER_CTRL_GSENSOR,
BUZZER_CTRL_UPDATE,
BUZZER_CTRL_UPDATEFINISH,
BUZZER_CTRL_UPDATEERR,
BUZZER_CTRL_DISKERR,
BUZZER_CTRL_OTHERR,

}BuzzerCTRLState;




/*	Only Item 1~5 can get form iCatchtek state .the others need to do form function
	0: INIT
	1: PREVIEW
	2: RECORDING
	3: PLAY IDLE
	4: PLAYING
	5: USER_RECORDING
	6: PARKING_RECORDING
	7: FORMATTING
	8: UPDATING
	9: LOW_BATTERY
*/
typedef enum{
	PANA_STATE_INIT,
	PANA_STATE_PREVIEW,
	PANA_STATE_RECORDING,
	PANA_STATE_PLAY_IDLE,
	PANA_STATE_PLAYING,
	PANA_STATE_USER_RECORDING,
	PANA_STATE_PARKING_RECORDING,
	PANA_STATE_FORMATTING,
	PANA_STATE_UPDATING,
	PANA_STATE_LOW_BATTERY,
	PANA_STATE_MAX,
}PanaState;


typedef enum{
SD_STATE_IN,
SD_STATE_OUT,
SD_STATE_MOUNTFAIL,
SD_STATE_MAX,
}SDStatus;


typedef enum{
BUZZER_SETTING_NONE = 0x00,
BUZZER_SETTING_BOOT_ONOFF=0x01,
BUZZER_SETTING_WARNING_ONOFF=0x02,
BUZZER_SETTING_OTHER_ONOFF =0x04,
}BuzzerSettingState;

/*
typedef enum FOLDER_TYPE
{
	CDFS_FILE_FOLDER_VIDEO,
	CDFS_FILE_FOLDER_JPG,
	CDFS_FILE_FOLDER_EVENT,
#if defined(PRJ_JETDVR) ||defined(PRJ_SBC) ||defined(PRJ_17DVR)
	CDFS_FILE_FOLDER_USERRECORD,
	CDFS_FILE_FOLDER_USERSAVE,
	CDFS_FILE_FOLDER_ANVIV,
#endif
	CDFS_FILE_FOLDER_FAIL,
	CDFS_FILE_FOLDER_MAX = CDFS_FILE_FOLDER_FAIL,
};
*/
/*
<file name> - XXXXXXYZ.MP4
Suggest to use the file name as 8 characters (short filename) for better system performance.
* This rule is valid until the year 2045, because "XXXXXX" field characters can express for about 31 years from 2014/1/1.

XXXXXX: The minutes accumulated from the date 2014/1/1 0:00, expressed in hexadecimal values.
Ex) 2017/2/23 06:31 -> 1,654,951 minutes, 0x1940A7  base 2014010100
2017 		02 	23 	06 	31
1578240	43200	33120	360	31

Y: The counter in case where it is recorded in the same minute. The string is incremented in each record, expressed in 0 ~ F.
Z: The Event that occurred during recording, if some events are included in 1 minutes file, "Z" should add one event value (Dec) of the highest priority.
	<Priority>	<Event Video File>
	    1(highest)	  Manual Event (User trigger) Video File
	    2		  Smart Assist Video File
	    2		  G-sensor Event (ACC ON) Video File
	    2		  Parking Recording Video File
	    3		  Risky point Video File
	    3		  Confluence point Video File
	    3		  Big curve Video File
	    3		  BtoB Event File

*/


typedef enum{
USERREC_FB_NOERR,
USERREC_FB_DISKNONE,
USERREC_FB_STOPREC,
USERREC_FB_DISKFULL,
USERREC_FB_OTHERERR,
}UserRecFBState;



typedef enum{
POWER_DET_NONE,
POWER_DET_ACC_OFF_SIGNAL,
POWER_DET_ACC_OFF,
POWER_DET_ACC_ON,
POWER_DET_PARKING_WAKEUP,
}PowerDetect;


typedef struct ParkingModeStorage{
UINT8 setting;
UINT16 reCordingTime;
UINT8 batteryVoltageS;
UINT8 gSensorLevel;
UINT8 parkingEvntToAVN;
UINT16 byPassTime;
}ParkingMode_t;



typedef enum{
ParkingModeTigger_NONE,
ParkingModeTigger_EVENT,
ParkingModeTigger_LOWPOWER,
}ParkingModeTrigger;

/*
G sensor
0: OFF
1: Level 1
2: Level 2
3: Level 3
4: Level 4
5: Level 5
*/

typedef enum{
PANA_GSensor_OFF,
PANA_GSensor_Lv1,
PANA_GSensor_Lv2,
PANA_GSensor_Lv3,
PANA_GSensor_Lv4,
PANA_GSensor_Lv5,
}PANA_GSensor;

/*
0: No Error
1: Disk None (No disk)
2: Stop Recording
3: Disk Full
4: Over 5000 photo
5: String Error
6: Other Error

*/
typedef enum{
	SnapState_NoERROR,
	SnapState_DiskNone,
	SnapState_StopRec,
	SnapState_DiskFull,
	SnapState_Over5000PIC,
	SnapState_StringErr,
	SnapState_OtherErr,
}SnapState;


typedef enum{
REC_STATE_EVENT,
REC_STATE_USER,
}RECState;

/*
 * Enum name:appInfoTemperaturDetectCB
 * Defined temperature state
 * @param TEMP_NONE  - Doesn't get temperatur value
 * @param TEMP_NORMAL  - temperatur state <= 75
 * @param TEMP_HIGH  - temperatur state >= 90
 * @author Isaac Fong
 * @data   01/15/2018
 *
 */
typedef enum{
TEMP_NONE,
TEMP_NORMAL,
TEMP_HIGH,
TEMP_MAX,
}TemperatureState ;



typedef enum{

RAWC_UPDATA_ERRTYPE_INDEX ,
RAWC_UPDATA_ERRCODE_INDEX ,
RAWC_UPDATA_ERRCOUNT_INDEX,
RAWC_SDCARD_SLOW_INDEX,

}RAWCIndex;

/*
 * Enum name:ErrType
 * Defined Error type with update 
 * @param ERRTYPE_NOERR: update No err (0x00)
 * @param ERRTYPE_FW1: update FW1 error (0x01)
 * @param ERRTYPE_FW2: update FW2 Error (0x02)
 * @param ERRTYPE_RES: update RES Error (0x04)
 * @param ERRTYPE_MAX: update file read error (0x10)
 * @author Isaac Fong
 * @data 08/23/2017
 *
 */
typedef enum{
ERRTYPE_UPDATE_NOERR=0x00,
ERRTYPE_UPDATE_FW1=0x01,
ERRTYPE_UPDATE_FW2=0x02,
ERRTYPE_UPDATE_RES=0x04,
ERRTYPE_MAX =0x10,
}ErrTypeUpdate ;

/*
 * Enum name:ErrCode
 * Defined Error type with update or boot error
 *@param	ERRCODE_UPDATE_SUCCESS			0x00 Success 
 *@param	ERRCODE_UPDATE_FILEOPEN			0x01 File open error 
 *@param	ERRCODE_UPDATE_MEMFULL			0x02 Memory not enough 
 *@param	ERRCODE_UPDATE_FILEREAD			0x03 File read error 
 *@param	ERRCODE_UPDATE_SDDETECT			0x04 SD detect error 
 *@param	ERRCODE_UPDATE_SDMOUNT			0x05 SD mount error 
 *@param	ERRCODE_UPDATE_BACKUPDISK			0x06 Backup disk error 
 *@param	ERRCODE_UPDATE_FILEWRITE			0x07 File write error 
 *@param	ERRCODE_UPDATE_FILESIZE			0x08 File size error 
 *@param	ERRCODE_UPDATE_FILEFORMAT			0x09 File format error 
 *@param	ERRCODE_UPDATE_FILENAME			0x0A File name error 
 *@param	ERRCODE_UPDATE_FILECOPY			0x0B File copy error 
 *@param	ERRCODE_UPDATE_FOLDERCHANGE		0x0C Directory change error 
 *@param	ERRCODE_UPDATE_FOLDERRM			0x0D Directory remove error 
 *@param	ERRCODE_UPDATE_DRAMPARASIZE		0x0E Drampara size error 
 *@param	ERRCODE_UPDATE_DRAMPARAFORMAT	0x0F Drampara format error 
 *@param	ERRCODE_UPDATE_DISKSIZE			0x10 Disk size error 
 *@param	ERRCODE_UPDATE_DISKMOUNT			0x11 Disk mount error 
 *@param	ERRCODE_UPDATE_DISKWRITE			0x12 Disk write error 
 *@param	ERRCODE_UPDATE_DISKFULL			0x13 Disk full error 
 *@param	ERRCODE_UPDATE_FLASETYPE			0x14 Flash type error 
 *@param	ERRCODE_UPDATE_FWWRITE			0x15 Firmware write error 
 *@param	ERRCODE_UPDATE_RESSIZE			0x16 Resource size verify error 
 *@param	ERRCODE_UPDATE_FWVERIFY			0x17 Firmware verify error 
 *@param	ERRCODE_UPDATE_RESVERIFY			0x18 Resource verify error 
 *@param	ERRCODE_UPDATE_NOCANGE			0xFE not allowed(nothing changed)
 *@param	ERRCODE_UPDATE_FAIL				0xFF Fail(something might have been changed, reboot possible fail) 
 * @author Isaac Fong
 * @data 08/23/2017
 *	
 */
 
typedef enum{
	ERRCODE_UPDATE_SUCCESS,
	ERRCODE_UPDATE_FILEOPEN,
	ERRCODE_UPDATE_MEMFULL,
	ERRCODE_UPDATE_FILEREAD,
	ERRCODE_UPDATE_SDDETECT,
	ERRCODE_UPDATE_SDMOUNT,
	ERRCODE_UPDATE_BACKUPDISK,
	ERRCODE_UPDATE_FILEWRITE,
	ERRCODE_UPDATE_FILESIZE,
	ERRCODE_UPDATE_FILEFORMAT,
	ERRCODE_UPDATE_FILENAME,
	ERRCODE_UPDATE_FILECOPY,
	ERRCODE_UPDATE_FOLDERCHANGE,
	ERRCODE_UPDATE_FOLDERRM,
	ERRCODE_UPDATE_DRAMPARASIZE,
	ERRCODE_UPDATE_DRAMPARAFORMAT,
	ERRCODE_UPDATE_DISKSIZE,
	ERRCODE_UPDATE_DISKMOUNT,
	ERRCODE_UPDATE_DISKWRITE,
	ERRCODE_UPDATE_DISKFULL,
	ERRCODE_UPDATE_FLASETYPE,
	ERRCODE_UPDATE_FWWRITE,
	ERRCODE_UPDATE_RESSIZE,
	ERRCODE_UPDATE_FWVERIFY,
	ERRCODE_UPDATE_RESVERIFY,
	ERRCODE_UPDATE_NOCANGE,
	ERRCODE_UPDATE_FAIL,
	ERRCODE_MAX,
}ErrCodeUpdate;

/*
 * Enum name:ErrSave
 * Error state with need storage , follow panasonic functional spec
 *@param	ERROR_SAVE_SD_CANNOTUSE,
 *@param	ERROR_SAVE_SD_ABNORMAL,
 *@param	ERROR_SAVE_SD_USERFULL,
 *@param	ERROR_SAVE_SD_NORMALFULL,
 *@param	ERROR_SAVE_SD_USETIME,
 *@param	ERROR_SAVE_SD_SLOW,
 *@param	ERROR_SAVE_TEMP_HIGH,
 *@param	ERROR_SAVE_FILENAME_OVER,
 *@param	ERROR_SAVE_OTHERERROR,
 * @author Isaac Fong
 * @data 09/12/2017
 *	
 */

typedef enum{
ERROR_SAVE_SD_CANNOTUSE=0x01,
ERROR_SAVE_SD_ABNORMAL=0x02,
ERROR_SAVE_SD_USERFULL=0x04,
ERROR_SAVE_SD_NORMALFULL=0x08,
ERROR_SAVE_SD_USETIME=0x10,
ERROR_SAVE_SD_SLOW=0x20,
ERROR_SAVE_TEMP_HIGH=0x40,
ERROR_SAVE_FILENAME_OVER=0x80,
ERROR_SAVE_OTHERERROR = 0x100,
}ErrSave;



typedef enum{
INFO_QUEUEMSG_THREAD_PLAYPOS   = 0x01,
INFO_QUEUEMSG_THREAD_POSREQUEST= 0x02,
INFO_QUEUEMSG_THREAD_CONNECTDEV =0x04,
INFO_QUEUEMSG_THREAD_REGULAR = 0x08,
INFO_QUEUEMSG_THREAD_SDPLUGTIMER=0x10,
INFO_QUEUEMSG_THREAD_STATECHANGE =0x20,
INFO_QUEUEMSG_THREAD_MASK = INFO_QUEUEMSG_THREAD_PLAYPOS|INFO_QUEUEMSG_THREAD_POSREQUEST|\
INFO_QUEUEMSG_THREAD_CONNECTDEV|INFO_QUEUEMSG_THREAD_REGULAR|INFO_QUEUEMSG_THREAD_SDPLUGTIMER|INFO_QUEUEMSG_THREAD_STATECHANGE,
}INFO_QUEUEMSG_THREAD_EVENT;


#endif /* _APP_INFOHANDLERCOMM_H_ */


