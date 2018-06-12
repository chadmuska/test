#ifndef _JET_UTILITY_H_
#define _JET_UTILITY_H_

#include "app_infohandlerComm.h"

typedef enum {
	FILE_TYPE_VIDEO_NORMAL,
	FILE_TYPE_VIDEO_MANUAL,
	FILE_TYPE_VIDEO_SMART_ASSIST,
	FILE_TYPE_VIDEO_GSENSOR,
	FILE_TYPE_VIDEO_PARKING,
	FILE_TYPE_VIDEO_RESERVED,
	FILE_TYPE_VIDEO_RISKY,
	FILE_TYPE_VIDEO_CONFLUENCE,
	FILE_TYPE_VIDEO_BIG_CURVE,
	FILE_TYPE_VIDEO_BTOB,
	FILE_TYPE_PHOTO,
	FILE_TYPE_TOTAL_FILE,
	FILE_TYPE_TOTAL_VIDEO,
	FILE_TYPE_NUM,
} fileTypeVideo_t;

typedef struct queryAvailableSpace_s
{
	UINT64 unuse_space;
	UINT64 deleted_file_size;
	UINT64 gsensor_movie_file_size;
}queryAvailableSpace_t;

typedef struct queryWriteEdurance_s
{
	UINT8 endurance;
	UINT64 nWritten;
	UINT8 ret;
}queryWriteEdurance_t;

typedef struct AVN_FILE_DELETE_PARA_S
{
	UINT16 filter;
	UINT16 folder;
	UINT16 total_count;
	UINT16 file_index;
	UINT8 del_all;
}AVN_FILE_DELETE_PARA_T;
typedef enum{
	AVN_PB_DELETE_STATUS_FAIL,
	AVN_PB_DELETE_STATUS_SUCCESS,
	AVN_PB_DELETE_STATUS_DELETING,
	AVN_PB_DELETE_STATUS_TIMEOUT
}AVN_FILE_DELETE_STATUS;

typedef struct FILE_UDATA_S
{
	/*[Format]
	$DCMD101,< UTC Position Time >,<Date>,< Datum >,<Latitude>,
	<N/S Indicator>,<Longitude>,<E/W Indicator>,<Speed>,<Angle>,
	<X-Axis>,<Y-Axis>,<Z-Axis>,<Events>,<File index>,< Play Status >
	*/
	char gpsTime[32];
	char gpsDate[32];
	UINT8 gpsDatum;
	char gpsLat[32];
	char gpsLong[32];
	char gpsNSIndicator[4];
	char gpsEWIndicator[4];
	char gpsSpeed[16];
	UINT16 gpsAngle;
	int gsensorX;
	int gsensorY;
	int gsensorZ;
	UINT16 event;
}FILE_UDATA_T;

typedef struct AVN_FILE_PB_PARA_S
{
	UINT16 filter;
	UINT8 folder;
	UINT16 file_index;
	UINT8 mode;
	UINT8 mode_branch;
	UINT8 speed;
	UINT8 speed_branch;
}AVN_FILE_PB_PARA_T;

typedef enum{
	AVN_PB_PARA_FOLDER_ROOT,
	AVN_PB_PARA_FOLDER_SAVE,
	AVN_PB_PARA_FOLDER_PHOTO,
	AVN_PB_PARA_FOLDER_ANNIV,
}AVN_PB_PARA_FOLDER;
typedef enum{
	AVN_PB_PARA_MODE_ONCE,
	AVN_PB_PARA_MODE_CONTINUE_ONE,
	AVN_PB_PARA_MODE_CONTINUE_ALL,
}AVN_PB_PARA_MODE;
typedef enum{
	AVN_PB_PARA_MODE_BRANCH_NORMAL,
	AVN_PB_PARA_MODE_BRANCH_SKIP_NEXT,
	AVN_PB_PARA_MODE_BRANCH_SKIP_PREVIOUS,
}AVN_PB_PARA_MODE_BRANCH;
typedef enum{
	AVN_PB_PARA_SPEED_X1,
	AVN_PB_PARA_SPEED_X4,
}AVN_PB_PARA_SPEED;
typedef enum{
	AVN_PB_PARA_SPEED_BRANCH_FF,
	AVN_PB_PARA_SPEED_BRANCH_RW,
}AVN_PB_PARA_SPEED_BRANCH;


void jetSDUseTimeWrite(UINT32 time);
void jetSDUseTimeInit(void);
UINT32 jetSDUseTimeRead(void);
void jetSDUseTimeCheck(void);
UINT32 jetSmartAssitModeTimerID(void);
void jetSmartAssitModeParseString(char *str, UINT8 *setting);
UINT8 jetSmartAssitModeInit(void);
UINT8 jetSmartAssitModeDestroy(void);
void jetSmartAssitModeTimer(void);
void jetSnapShotMarkParseString(char *str, UINT8 *type, char *filename);
UINT8 jetSnapShotMarkSet(UINT8 type, char *filename, SnapState *err);
UINT64 jetTotalFileSizeInFolderGet(UINT8 folder);
UINT8 jetQueryAvailableSpaceGet(queryAvailableSpace_t *status);
void jetSDLogParseString(char *str, UINT8 *setting);
void jetSDLogFileDelete(void);
UINT8 jetSDLogSet(UINT8 setting);
TemperatureState jetTempGet(UINT8 *temp);
void jetQueryFileCountParseString(char *str, UINT16 *filter, UINT8 *folder);
UINT8 jetQueryFileCountGet(UINT16 filter, UINT8 folder, UINT16 *total_count);
void jetQueryReset();
UINT16 jetFileCountInFolderGet(UINT32 folder, UINT16 filter);
void jetFileListParseString(char *str, UINT16 *filter, UINT8 *folder, UINT16 *file_index, UINT16 *file_count);
void jetFileListGet(UINT8 folder, UINT16 filter);
void jetQueryFileInfoParseString(char *str, UINT16 *filter, UINT8 *folder, UINT16 *file_index);
void jetFileDeleteParseString(char *str, AVN_FILE_DELETE_PARA_T *pFileDelete);
UINT8 jetFileListDelete(AVN_FILE_DELETE_PARA_T *pFileDelete);
BOOL jetFilePBStartParseString(char *str, AVN_FILE_PB_PARA_T *para);
#if 0
FILE_LIST_PANA_T *jet_file_list_filter_get(void);
#endif
UINT16 jet_file_list_filter_num_get(void);
UINT16 jet_file_list_filter_current_play_get(void);
void jet_file_list_filter_current_play_set(UINT16 idx);
AVN_FILE_PB_PARA_T *jet_avn_pb_para_get(void);
BOOL jet_file_udata_info_get(char *filename, FILE_UDATA_T *udataList, int *line_count, UINT32 *event);
BOOL jetTimeEventFormUdat(UINT8 *filename, UINT32 *base_time, UINT32 *event);
int jet_file_udata_info_line_num_get(void);
FILE_UDATA_T *jet_file_udata_info_handle_get(void);
void jet_file_list_filter_num_set(UINT16 num);
AVN_FILE_DELETE_PARA_T *jet_avn_file_delete_para_get(void);
void jet_avn_file_delete_para_set(AVN_FILE_DELETE_PARA_T para);
void jet_rec_video_post(CDFS_FILE_TAB_T *);
void jet_avn_pb_para_set(AVN_FILE_PB_PARA_T *p);
BOOL jet_file_udata_tag_seek(UINT32 fd);
void jet_filelist_delete_status_check(void);
void jet_usersave_movefile_status_check(void);
void jet_LatiLogi_string_to_rational(char *str, UINT32 *dwbuf);
void jet_LatiLogi_rational_to_string(UINT8 *buf, char *str);
void jet_exif_speed_rational_to_string(UINT8 *buf, char *str);
void jet_exif_direction_rational_to_string(UINT8 *buf, char *str);
UINT8 jetMoveFileHandle(UINT16 file_index, UINT8 *err);
void jetMoveFileToSaveFolderParseString(char *str, UINT16 *filter, UINT16 *file_index);
void jetErrStorageState(ErrSave state);
void jetSDState(ErrSave state);
void jet_file_udata_update(CDFS_FILE_TAB_T *pCur);

CDFS_FILE_TAB_T *jetFolderFilterFileNodeFind(UINT32 idx);
UINT32 jetFolderFilterTotal();
void jet_lineout_out(BOOL on);
void jet_exif_datetime_get(UINT8 *fname, tmx_t *time);
void jet_query_write_endurance_get(queryWriteEdurance_t *edu);
UINT64 jet_htoi(UINT8 *ptr);
#endif /* _JET_UTILITY_H_ */

