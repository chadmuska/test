
#ifndef _APP_INFOHANDLER_FUNC_H_
#define _APP_INFOHANDLER_FUNC_H_



#include "app_infohandlerComm.h"
#include "app_rtc.h"





#if 0
extern UINT8 appInfoCharToInt(UINT8 data);
#endif
extern UINT32 appInfoSDFormat(void);
extern ModeChangeStatus appInfoModeChange(UINT8 *buf);
extern void appInfoSetRes(UINT8 *buf);
extern void appInfoDISKINFOGet( UINT32 * totalSize , UINT32 * used  ,UINT32 * sdhealth );
extern void appInfoFileReName(UINT8 * srcPath  , UINT8 * dstPath , UINT8 * srcFileName , UINT8 *dstFileName);
extern UINT8 * appInfoFileNameAdd(UINT8 * fileName , UINT8 addChar);
extern UINT8 * appinfoMoveFileNameGet(UINT8 * path , UINT8 * fileName);
extern UINT8 * appInfoFileNameGet(UINT8 * fileNameWithPath );
extern BOOL appInfoSetRecorderSound(const char *buf);
extern void appInfoSetBeepSound(UINT8 *buf);
extern UserRecFBState appInfoUserRecStateCheck(void);
extern UINT32 appInfoBatteryLvGet(void);
extern void appInfoSetParkingMode(UINT8 * buf);
extern UINT32 appInfoParkingSettingGet(void);
extern UINT32 appInfoParkingBatterySettingGet(void);
extern UINT32 appInfoParkingReCordingTimeGet(void);
extern void appInfoParkingModeCtrl( UINT32 count ,UINT8 * filename);
extern ParkingModeTrigger appInfoParkingModeTriStateGet( void );
extern void appInfoParkingLowPowerCB(void);
extern UINT8 appInfoParkingGSensorLevelGet(void);
extern void appInfoPositionSet(char * buf);
extern char *appInfoPositionSync();
extern char * appInfoPostionALLStampGet(void);
extern BOOL appInfoGSensorSetFunc(const char *buf);
extern void appInfoPathGet(UINT8 * fileNameWithPath  , UINT8 * Path);
#if 0
extern void appInfoWaitInitSeqFinishCB(void);
#endif
extern void appUserResSet(UINT8 size);
extern void appBuzzerSet(UINT8 set);
extern void appGsensorSet(UINT8 set);
extern void appInfHWWDT(UINT32 ms);

extern void PlayPosCountStart(void);
extern void PlayPosCountStop(void);
extern void PosReqCountStart(void);
extern void PosReqCountStop(void);
extern void RegularCountStart(void);
extern void RegularCountStop(void);
extern void ConnectDevCountStart(void);
extern void ConnectDevCountStop(void);

extern int appInfo3AGet(char *buf, int buf_size);
extern SnapState appInfoSnapStateGet(void);
extern char * appInfoPoisitonEventGet(void);
extern void appInfoPositionEventStampSet(char * event);
extern void appInfoSWWDTCB(void);
extern void appInfSWWDT(void);
extern void appInfoSWWDTWakeup(UINT32);
void appInfoSWWDTReset();
extern BOOL appInfoGSensorCompensation(void);
extern void appInfoGSensorCompensationSet(void);
extern void appInfoFileNameEventAdd(CDFS_FILE_TAB_T*);
extern BOOL appInfoDualBootRAWCInit(void);
extern void appInfodualBootRAWCSuccess(void);
extern void appInfoChangePreFileName(UINT32 event);
extern void appInfoVideoAutoRecRetryCB(void);
extern void appInfoUdatHeaderPosEventClean(void);
extern UINT8 appInfoParkingToAVNGet(void);
extern void appinfoPosReqTimeoutClear(void);
extern void appInfoTemperaturDetectCB(void);
extern  void appInfoUpdataErrSet(ErrTypeUpdate type , ErrCodeUpdate code);
extern  ErrTypeUpdate appInfoUpdataErrGet(ErrCodeUpdate * code,UINT8 * count);
extern  ErrTypeUpdate appInfoUpdateTypeGet(UINT8 * filename);
extern void appInfoBootErrSet(void);
extern UINT8 appInfoBootErrGet(void);
extern void appInfoBootErrClear(void);
extern BOOL appInfoRTCChangedGet(void);
extern void appInfoRTCChangedSet(BOOL set);
extern appRTC_t *appInfoAVNTimeGet(void);
extern void appInfoLastPositionSave(void);
extern void appInfoLastPositionGet(void);
extern BOOL appInfoDiskSizeCheck(void);
extern char *appInfoPositionNIndicatorGet(void);
extern char *appInfoPositionEIndicatorGet(void);
extern char *appInfoPositionLatitudeGet(void);
extern char *appInfoPositionLongitudeGet(void);
extern char *appInfoPositionAngleGet(void);
extern char *appInfoPositionSpeedGet(void);
extern UINT32 appInfoUdatHeaderPEGet(void);
extern void appInfoUdatHeaderPESet(UINT32 data);
extern void appInfoUdatHeaderPEClean();
extern void appInfoStoragePosDataStampGet(char * src , char * dst);
extern TemperatureState appInfoTemperaturStateGet(void);
extern BOOL appInfoNewDayCheck(tmx_t * time ,int sec);
extern BOOL appInfoNewMonthCheck(tmx_t * time ,int sec);
extern BOOL appInfoNewHourCheck(tmx_t * time ,int sec);
extern BOOL appInfoNewMinCheck(tmx_t * time ,int sec);
extern BOOL appInfoNewYearCheck(tmx_t * time ,int sec);
extern void appInfoSecTimeToRealGet(UINT32 totalsec ,tmx_t * time);
extern UINT32 appInfoSecTimeBaseGet(tmx_t * time);
extern void appInfoPositionEventSet(char * event );
extern void appInfoUdatHeaderPosEventSet(char * event );
extern void appInfoFactoryReset(void);
extern void appInfoParkingModeTriStateSet(ParkingModeTrigger state);
extern  BOOL appInfoFWUPDateVerGet(UINT8 * filename);
extern void appInfoSDCardSlowSet(void);
extern void appInfoSDCardSlowClr(void);
#endif /* _APP_INFOHANDLER_FUNC_H_ */

