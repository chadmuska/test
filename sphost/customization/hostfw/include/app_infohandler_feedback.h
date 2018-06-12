#ifndef _APP_INFOHANDLER_FEEDBACK_H_
#define _APP_INFOHANDLER_FEEDBACK_H_

extern UINT32 appInfoSDFormatFB( BOOL state  );
extern UINT32 appInfoSetResFB( PanaState state  );
extern UINT32 appInfoModeChangeFB( UINT8 state  , PanaState status);
extern UINT32 appInfoSDFormatOKFB( void  );
extern UINT32 appInfoDevFWVerFB(void);
extern UINT32 appInfoPOSReqFB(void);
extern UINT32 appInfoConnectDevFB(void);
extern void appInfoDiskErrorFB(DiskStatus state);
extern void appInfoDevInfo(PanaState state);
extern void appInfoSDInOutFB(SDStatus state);
extern void appInfoFactoryResetFB(BOOL state);
extern void appInfoSetRecorderSoundFB(BOOL state);
extern void appInfoSetBeepSoundFB(UINT8 *buf);
extern void appInfoStartUserRecordingFB(UserRecFBState state);
extern void appInfoSDUseTimeFB(UINT8 state, UINT32 time);
extern void appInfoSnapShotMarkFB(UINT8 state, UINT8 status);
extern void appInfoSmartAssitModeFB(UINT8 state);
extern void appInfoQueryAvailableSpaceFB(UINT8 state, queryAvailableSpace_t status);
extern void appInfoSDLogFB(UINT8 state);
extern void appInfoHighTempProtectedFB(UINT8 state, UINT8 temp);
extern void appInfoSetGSensorFB(BOOL state);
extern void appInfoAvailableSpaceFB(void);
extern void appInfoSDCardFormatNecessityFB(void);
extern void appInfoWriteEndUranceFB(queryWriteEdurance_t *edu);
extern void appInfoQueryFileCountFB(UINT8 state, UINT16 count);
extern void appInfoFileListFB(UINT16 index, UINT16 count);
extern void appInfoQueryFileInfoFB(UINT16 index);
extern void appFileDeleteFB(UINT8 state);
extern void jet_filelist_sending_status_set(BOOL s);
extern void appInfoGSensorCompensationFB(UINT8 state);
extern void appReportErrorStatusFB(UINT8 state, UINT8 error);
extern void appFilePBStartFB(UINT8 state);
extern void appFilePBStopFB(UINT8 state, AVN_FILE_PB_PARA_T *para, UINT16 file_index);
extern UINT32 appInfoPOSPlayFB(FILE_UDATA_T *pUdata, UINT16 file_index, UINT8 play_status);
extern void appInfoMoveFileToSaveFolderFB(UINT8 state, UINT8 error);
extern void appFilePBPauseFB(UINT8 state, AVN_FILE_PB_PARA_T *para);
extern BOOL jet_filelist_sending_status_get(void);
extern UINT32 appInfoDiskInfoFB( UINT32  totalSize , UINT32  used  ,UINT32  sdhealth ,DiskStatus state);
extern void appInfoStopUserRecordingFB(BOOL state);
extern void appInfoParkingModeFB(ParkingModeTrigger state);
extern void appInfoDevSetting();
extern void appInfoReportErrStatusFB();
extern void appInfoReportErrStatusFB();
extern void appInfoRegularFB(PanaState state);
#endif /* _APP_INFOHANDLER_FEEDBACK_H_ */

