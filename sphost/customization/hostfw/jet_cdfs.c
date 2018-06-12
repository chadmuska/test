/*--------------------------------------------------------------------------*
*  File name	 : Jet_cdfs.c
*  Function	     : "jet-opto cdfs function"
*  First editor: SC Chang
*  First date  : 2017.05.25
*  History:
*  2017.05.25 SC Chang Added
*  2018.04.10 Weikai Hsu Added File Header
*  
*  
*--------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "api/sp5k_global_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_dbg_api.h"
#include "app_sensor.h"
#include "sp5k_media_api.h"
#include "app_com_api.h"
#include "jet_cdfs.h"

extern BOOL appLeapYearCheck(UINT16 year);
extern CDFS_FILE_TAB_T *appCdfsFileListLastGet(UINT32 folder);
extern BOOL appFileExistCheck(UINT8 *filename);
extern UINT32 jet_appCdfsTheOldestFileDel(UINT8 cdfs_folder);


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_datetime_to_minute 
* Function		: convert datetime to minutes
* Return value	:  UINT32 
* Parameter1	: tmx_t time
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 jet_cdfs_datetime_to_minute(tmx_t time)
{
	int i=0, day=0;
	UINT8 j=0, months=0;
	UINT32 minute=0;
	UINT32 month[2][12] =
	{
		{ 31,28,31,30,31,30,31,31,30,31,30,31 },
		{ 31,29,31,30,31,30,31,31,30,31,30,31 }
	};
	//Ex) 2017/2/23 06:31 => 1,654,951 minutes, 0x1940A7
#if DEBUG_ON
	printf("[%s] %d, %d, %d, %d, %d\n", __FUNCTION__, time.tmx_year, time.tmx_mon, time.tmx_mday, time.tmx_hour,
	time.tmx_min);
#endif
	for(i=2014; i<=(time.tmx_year+1900); i++)
	{
		if(i == time.tmx_year+1900){
			months = time.tmx_mon-1;
		}else{
			months = 12;
		}
		for(j=0; j<months; j++){
			day += month[appLeapYearCheck(i)][j];
		}
	}
	day+=(time.tmx_mday-1);
	minute = (day*24+time.tmx_hour)*60+time.tmx_min;
	if(minute>=0xffffff){
		minute = 0xffffff;
		jetErrStorageState(ERROR_SAVE_FILENAME_OVER);
	}
#if DEBUG_ON
	printf("[%s] minute = %u, %u\n", __FUNCTION__, minute, minute);
#endif
	return minute;
}

#if 0
/**
 * get .
 * @param path
 * @return index - index or -1   
 * @author
 * @date 08/21/2017
 *
 */
int jet_cdfs_pathname_to_subfolder(char *path)
{
    char str[64], *p = NULL;
    int index = -1;

    strcpy(str, path);
    p = strrchr(str, '/');
    if (p == NULL)
        goto out;
    if (strcmp(p + 4, "_NML") != 0)
        goto out;
    *(p + 4) = '\0';
    index = atoi(p + 1);
    if (index < JET_CDFS_DCIM_SUBFOLDER_START_INDEX) {
        return -1;
    }
    index -= JET_CDFS_DCIM_SUBFOLDER_START_INDEX;
out:
    //printf("[%s] filename = %s, index = %d\n", __FUNCTION__, filename, index);
    return index;
}
#endif


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_pathname_to_cdfs 
* Function		: Set CDFS file path name 
* Return value	:  UINT32 
* Parameter1	: char *pathname
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 jet_cdfs_pathname_to_cdfs(char *pathname)
{
	UINT32 cdfs=CDFS_FILE_FOLDER_VIDEO;
	if(strstr(pathname , EVENT_PATH) != NULL){
		cdfs=CDFS_FILE_FOLDER_EVENT;
	}else if(strstr(pathname , USER_RECORD_PATH)!=NULL){
		cdfs=CDFS_FILE_FOLDER_USERRECORD;
	}else if(strstr(pathname , USERSAVE_PATH)!=NULL){
		cdfs=CDFS_FILE_FOLDER_USERSAVE;
	}

	printf("[%s] cdfs = %u\n",__FUNCTION__, cdfs);
	return cdfs;
}

#if 0
static cdfs_compare checkSubfolderFilename(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pCond)
{
	UINT32 i;
        i = pCur->folder;
        /* Check NORMAL video folder */
        if (i >= JET_CDFS_DCIM_SUBFOLDER_START_INDEX)
            i = CDFS_FILE_FOLDER_VIDEO_BIT;

	return strcmp(pCur->FileName, pCond->FileName) == 0 && (i & pCond->folder) != 0;
}
#endif

 
/*--------------------------------------------------------------------------*
* Function name : jet_folder_bit_2_cdfs_folder 
* Function		: Set CDFS file path name 
* Return value	:  UINT32 
* Parameter1	: UINT32 folder
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 jet_folder_bit_2_cdfs_folder(UINT32 folder)
{
	/* default VIDEO folder */
	UINT32 ret = CDFS_FILE_FOLDER_VIDEO;

	if (folder < JET_CDFS_DCIM_SUBFOLDER_START_INDEX) {
		int i = 0;

		while (folder) {
			if (folder & 1) {
				ret = i;
				break;
			}
			folder >>= 1;
			i++;
		}
	}

	return ret;
}

#if 0
/**
 * get cdfs subfloder.
 * @param filename - filename
 * @param cdfs_folder - cdfs folder name
 * @return subfloder of fail 
 * @aothur 
 * @date 08/21/2017
 *
 */
int jet_cdfs_subfolder_get(UINT32 cdfs_folder, char *filename)
{
	CDFS_FILE_TAB_T cond;
	CDFS_FILE_TAB_T *pCur;
	int subfolder = -1;

	memset(&cond, 0, sizeof(cond));
	cond.folder = cdfs_folder < CDFS_FILE_FOLDER_MAX ? 1 << cdfs_folder :
		CDFS_FILE_FOLDER_VIDEO_BIT;
	strncpy(cond.FileName, filename, sizeof(cond.FileName));

	pCur = appCdfsNextNodeFind(NULL, &cond, checkSubfolderFilename);
	if (pCur) {
		subfolder = jet_folder_bit_2_cdfs_folder(pCur->folder);
		printf("[%s] cdfs_folder = %d, subfolder = %d\n",
				__FUNCTION__, cdfs_folder, subfolder);
	} else {
		printf("%s: Not found!\n", __func__);
	}
	return subfolder;
}
#endif


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_next_subfolder_in_dcim_get 
* Function		: get cdfs oldest subfloder from dcim
* Return value	:  int 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
int jet_cdfs_next_subfolder_in_dcim_get(void)
{
	int subfolder;

	subfolder = appCdfsBiggestNmlGet();
	return subfolder;
}


static UINT32 pre_minutes = 0;
static int pre_index = -1;
/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_check_same_mins 
* Function		: get cdfs oldest subfloder from dcim
* Return value	:  void 
* Parameter1	: UINT8 *filename
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_cdfs_check_same_mins(UINT8 *filename)
{
	tmx_t chgFileTime;
	static UINT8 curFilename[16];

	if (pre_minutes == 0) {
		sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &chgFileTime);
		pre_minutes = jet_cdfs_datetime_to_minute(chgFileTime);
		sprintf((char *)curFilename, "%06X", pre_minutes);
	}

	if (strncmp((char *)filename, (char *)curFilename, 6) == 0) {
		char c;
		int idx;

		c = filename[6];

		if (c >= '0' && c <= '9')
			idx = c - '0';
		else if (c >= 'A' && c <= 'F')
			idx = 10 + ('A' - c);
		else idx = 0;

		if (pre_index <= idx) {
			pre_index = idx;
			pre_index++;
		}
	printf("%s(%d) %u - %s\n", __func__, __LINE__, pre_minutes, filename);
	}
}


/*--------------------------------------------------------------------------*
* Function name : *jet_cdfs_next_video_filename_get 
* Function		: get next video from cdfs subfloder
* Return value	:  char 
* Parameter1	: tmx_t time
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *jet_cdfs_next_video_filename_get(tmx_t time)
{
	int subfolder, i, cur_index, j=0;
	static char fname[64];
	UINT32 cur_minutes=0;

	subfolder = jet_cdfs_next_subfolder_in_dcim_get();
	cur_minutes = jet_cdfs_datetime_to_minute(time);
	printf("[%s] minutes = %x, pre_minutes = %x\n",__FUNCTION__, cur_minutes, pre_minutes);
	sprintf(fname, "D:/DCIM/%03d_NML", subfolder);
	sp5kFsDirMake((UINT8 *)fname);
	if (pre_index == -1)
		pre_minutes = 0;
	if(cur_minutes == pre_minutes){
		j = pre_index+1;
	}
	for(i=j; i<=0xf; i++){
		sprintf(fname, "D:/DCIM/%03d_NML/%06X%01X%01d.MP4", subfolder, cur_minutes, i, 0);
		if(appFileExistCheck((UINT8*)fname)==FALSE){
			break;
		}
	}
	cur_index = i;	
	if(cur_index>0xf||cur_index<0){
		cur_index=0;
		if(cur_minutes>=0xffffff){
			cur_minutes=0;
		}
	}
	pre_index = cur_index;
	pre_minutes = cur_minutes;
	sprintf(fname, "D:/DCIM/%03d_NML/%06X%01X%01d.MP4", subfolder, cur_minutes, cur_index, 0);
	printf("[%s] fname = %s\n", __FUNCTION__, fname);
	return fname;
}


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_subfolder_num_in_dcim_get 
* Function		: get subfloder number in dcim
* Return value	:  int 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
int jet_cdfs_subfolder_num_in_dcim_get(void)
{
	return appCdfsFolderCount(CDFS_FILE_FOLDER_VIDEO);
}


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_file_num_limit_by_card_size_get 
* Function		: calculate file number limit according to memory card size
* Return value	:  int 
* Parameter1	: UINT8 cdfs_folder
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
int jet_cdfs_file_num_limit_by_card_size_get(UINT8 cdfs_folder)
{
	appDiskInfo_t* pDisk = appCurDiskInfoGet();
	int limit_num = 0;
	int disk_size_gb = JET_FILE_NUM_LIMIT_VIDEO_8G;
	if(!IS_CARD_EXIST){
		return limit_num;
	}
	if(cdfs_folder == CDFS_FILE_FOLDER_JPG){
		limit_num = JET_FILE_NUM_LIMIT_PHOTO;
	}else{
		disk_size_gb = pDisk->dskMbSz/1000;
		if(disk_size_gb <= 1){
			limit_num = JET_FILE_NUM_LIMIT_VIDEO_8G;
			printf("*** SD 1G\n");
		}else if(disk_size_gb <= 2){
			limit_num = JET_FILE_NUM_LIMIT_VIDEO_8G;
			printf("*** SD 2G\n");
		}else if(disk_size_gb <= 4){
			limit_num = JET_FILE_NUM_LIMIT_VIDEO_8G;
			printf("*** SD 4G\n");
		}else if(disk_size_gb <= 8){
			limit_num = JET_FILE_NUM_LIMIT_VIDEO_8G;
			printf("*** SD 8G\n");
		}else if(disk_size_gb <= 16){
			limit_num = JET_FILE_NUM_LIMIT_VIDEO_16G;
			printf("*** SD 16G\n");
		}else if(disk_size_gb <= 32){
			limit_num = JET_FILE_NUM_LIMIT_VIDEO_32G;
			printf("*** SD 32G\n");
		}else if(disk_size_gb <= 64){
			limit_num = JET_FILE_NUM_LIMIT_VIDEO_64G;
			printf("*** SD 64G\n");
		}else if(disk_size_gb <= 128){
			limit_num = JET_FILE_NUM_LIMIT_VIDEO_128G;
			printf("*** SD 128G\n");
		}else{
			limit_num = JET_FILE_NUM_LIMIT_VIDEO_128G;
			printf("*** over SD 128G\n");		
		}
	}
	printf("[%s] limit_num = %d\n",__FUNCTION__, limit_num);
	return limit_num;
}


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_oldest_file_full_check 
* Function		: check specific folder full
* Return value	:  BOOL 
* Parameter1	: UINT8 cdfs_folder
* History	   : 2017.08.26
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL jet_cdfs_oldest_file_full_check(UINT8 cdfs_folder)
{
	/*int subfolder_oldest=-1, subfolder_latest=-1;
	UINT32 filenum_oldest=0, filenum_latest=0;*/
	BOOL del = FALSE;

	if(!IS_CARD_EXIST){
		return del;
	}
	if(cdfs_folder == CDFS_FILE_FOLDER_VIDEO){
		del = FALSE;
	}else{
		if(appCdfsFolderCount(cdfs_folder)
			>= jet_cdfs_file_num_limit_by_card_size_get(cdfs_folder)){
			del = TRUE;
		}
	}
	printf("[%s] del = %d\n", __FUNCTION__, del);
	return del;
}


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_latest_jpg_file_index_get 
* Function		: get the latest jpg file from index
* Return value	:  UINT16 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT16 jet_cdfs_latest_jpg_file_index_get(void)
{
	char *p=NULL, filename[32];
	CDFS_FILE_TAB_T *fileinfo;
	UINT16 index=0;
	fileinfo = appCdfsFileListLastGet(CDFS_FILE_FOLDER_JPG);
	if(fileinfo == NULL){
		return 0;
	}
	if(fileinfo->FileName == NULL){
		return 0;
	}
	strcpy(filename,(char*)fileinfo->FileName);
	printf("[%s] filename = %s\n", __FUNCTION__,filename);
	p = strrchr(filename,'.');
	if(p == NULL){
		return 0;
	}
	*p = '\0';
	p = strrchr(filename,'P');
	if(p == NULL){
		return 0;
	}
	index = atoi(p+4);
	printf("[%s] s = %s, index = %d\n", __FUNCTION__,p+4, index);
	return index;
}


/*--------------------------------------------------------------------------*
* Function name : *jet_cdfs_next_jpg_filename_get 
* Function		: get next jpg filename 
* Return value	:  char 
* Parameter1	: tmx_t time
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *jet_cdfs_next_jpg_filename_get(tmx_t time)
{
	int i;
	static char fname[64];
	UINT32 minutes=0;

	minutes = jet_cdfs_datetime_to_minute(time);
	for(i=0; i<=0xFF; i++){
		sprintf(fname, "D:/PHOTO/%06X%02X.JPG", minutes,i);
		if(appFileExistCheck((UINT8*)fname)==FALSE){
			break;
		}
	}
	printf("[%s] fname = %s\n", __FUNCTION__, fname);
	return fname;	
}

#if 0
/**
 * Get current play list  
 * @param index 
 * @return FILE_LIST_PANA_T
 * @aothur 
 * @date 08/21/2017
 *
 */
FILE_LIST_PANA_T * jet_cdfs_current_play_handle_get(UINT16 index)
{
	FILE_LIST_PANA_T *pCur=NULL;
	UINT16 i=0;
#if 0
	pCur = jet_file_list_filter_get();
#endif
	while(pCur!=NULL){
		if(i==index){
			break;
		}
		pCur=pCur->next;
		i++;
	}
	return pCur;
}
#endif
#if 0
/**
 * Change floder in playing  
 * @param void 
 * @return void
 * @aothur 
 * @date 08/21/2017
 *
 */

void jet_cdfs_pb_folder_change(void)
{
	FILE_LIST_PANA_T *pCur=NULL;;
	UINT16 total=0, cur_play_index=0;
	char path[64];
	printf("[%s]\n", __FUNCTION__);

#if 0
	pCur = jet_file_list_filter_get();
#endif
	if(pCur == NULL){
		return;
	}
	total = jet_file_list_filter_num_get();
	if(total == 0){
		return;
	}
	cur_play_index = jet_file_list_filter_current_play_get();
	pCur = jet_cdfs_current_play_handle_get(cur_play_index);
	appCdfsFileFolderSet((pCur)->cdfs_folder);
	strcpy(path, jet_cdfs_folder_to_path((pCur)->cdfs_folder, (pCur)->folder));
	sp5kFsDirChange((const UINT8 *)path);
	printf("[%s] path = %s\n", __FUNCTION__, path);
}
#endif


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_pb_image_next_index_get 
* Function		: Get next playback image index 
* Return value	:  UINT32 
* Parameter1	: void
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 jet_cdfs_pb_image_next_index_get(void)
{
	UINT16 total=0, next_play_index=0;
	printf("[%s]\n", __FUNCTION__);
	total = jet_file_list_filter_num_get();
	if(total == 0 || jet_avn_pb_para_get()==NULL){
		return 0;
	}
	next_play_index = jet_file_list_filter_current_play_get();
	if(jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_SKIP_NEXT){
		next_play_index++;
	}else if(jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_SKIP_PREVIOUS){
		if(next_play_index <= 1)
			next_play_index = total;
		else
			next_play_index--;
	}
	if(next_play_index>total){
		next_play_index=1;
	}
	printf("[%s] next_play_index = %d\n", __FUNCTION__, next_play_index);
	return next_play_index;
}


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_pb_dcf_index_get 
* Function		: Get index in current playing
* Return value	:  UINT32 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 jet_cdfs_pb_dcf_index_get(void)
{
#if 0
	FILE_LIST_PANA_T *pCur=NULL;
	CDFS_FILE_TAB_T *pHandle=NULL;
	UINT16 total=0, cur_play_index=0, dcf_index=0;
	UINT8 cdfs_folder=0;
	BOOL find = FALSE;
	printf("[%s]\n", __FUNCTION__);
#if 0
	pCur = jet_file_list_filter_get();
#endif
	if(pCur == NULL){
		return 0;
	}
	total = jet_file_list_filter_num_get();
	if(total == 0){
		return 0;
	}
	cur_play_index = jet_file_list_filter_current_play_get();
	pCur = jet_cdfs_current_play_handle_get(cur_play_index);
	cdfs_folder = (pCur)->cdfs_folder;

	pHandle = appCdfsFileListHeadGet(cdfs_folder);
	if(pHandle == NULL){
		return 0;
	}
	dcf_index = 0;
	find = FALSE;
	while(pHandle!=NULL){
		dcf_index++;
		pCur = jet_cdfs_current_play_handle_get(cur_play_index);
		if(cdfs_folder == CDFS_FILE_FOLDER_VIDEO){
			if(strcmp((char*)pHandle->FileName,(char*)pCur->FileName)==0
				&& pHandle->folder == (pCur)->folder){
				find = TRUE;
				break;
			}
		}else{	
			if(strcmp((char*)pHandle->FileName,(char*)pCur->FileName)==0){
				find = TRUE;
				break;
			}
		}
		pHandle = pHandle->next;
	}
	if(find == FALSE){
		dcf_index = 0;
	}

	#if 0
	strcpy(filename, jet_cdfs_folder_to_path((pCur+cur_play_index)->cdfs_folder, (pCur+cur_play_index)->subfolder));
	sprintf(filename,"%s/%s", filename, (pCur+cur_play_index)->FileName);
	printf("filename = %s\n", filename);
	sp5kDcfFsFileIdxGetByFilename(filename,&dcf_index);
	#endif
	printf("[%s] dcf_index = %d\n", __FUNCTION__, dcf_index);
	return dcf_index;
#else
	return jet_file_list_filter_current_play_get();
#endif
}


/*--------------------------------------------------------------------------*
* Function name : *jet_cdfs_folder_to_path 
* Function		: Get full file path of floder
* Return value	:  char 
* Parameter1	: int cdfs_folder
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *jet_cdfs_folder_to_path(int cdfs_folder)
{
	static char filepath[32];
	char *ret;

	ret = "D:/DCIM";
	if (cdfs_folder >= JET_CDFS_DCIM_SUBFOLDER_START_INDEX) {
		sprintf(filepath,"D:/DCIM/%03d_NML", cdfs_folder);
		ret = filepath;
	}else if(cdfs_folder == CDFS_FILE_FOLDER_JPG_BIT){
		ret = "D:/PHOTO";
	}else if(cdfs_folder == CDFS_FILE_FOLDER_EVENT_BIT){
		ret = "D:/GEVENT";
	}else if(cdfs_folder == CDFS_FILE_FOLDER_USERRECORD_BIT){
		ret = "D:/MANUAL";
	}else if(cdfs_folder == CDFS_FILE_FOLDER_USERSAVE_BIT){
		ret = "D:/USERSAVE";
#if 0
	}else if(cdfs_folder == CDFS_FILE_FOLDER_ANNIV_BIT){
		ret = "D:/ANNIV/";
#endif
	}

	//printf("[%s] filepath = %s\n",__FUNCTION__, filepath);
	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_oldest_file_delete_for_free_by_folder 
* Function		: delete oldest file for free space on specific folder
* Return value	:  void 
* Parameter1	: UINT8 cdfs_folder
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_cdfs_oldest_file_delete_for_free_by_folder(UINT8 cdfs_folder)
{
	while(jet_cdfs_oldest_file_full_check(cdfs_folder) == TRUE){
		jet_appCdfsTheOldestFileDel(cdfs_folder);
	}
}


/*--------------------------------------------------------------------------*
* Function name : jet_cdfs_pana_filename_rule_check 
* Function		: check file name is pana rule or not.
* Return value	:  BOOL 
* Parameter1	: char *filename
* History	   : 2017.08.03
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL jet_cdfs_pana_filename_rule_check(char *filename)
{
#if 0
	char str[64], *p;
#else
	char *str = filename;
#endif
	int i, j = 0;

	if(strcmp(str+8, ".JPG")==0
		|| strcmp(str+8, ".jpg")==0) {
		j = 8;
	} else if (strcmp(str+8, ".MP4")==0
		|| strcmp(str+8, ".mp4")==0){
		j = 7;
	}
	//printf("[%s] filename = %s\n", __FUNCTION__, filename);
	if(filename == NULL){
		printf("filename is null\n");
		return FALSE;
	}
#if 0
	p = strrchr(filename,'/');
	if(p!=NULL){
		strcpy(str, p+1);
	}else{
		strcpy(str, filename);
	}
#endif
	if(strlen(str) != 12){
		printf("filename length is error\n");
		return FALSE;
	}
	for(i=0; i<j; i++){
		if( (str[i]>='0' && str[i]<='9') || (str[i]>='A' && str[i]<='F')){
			continue;
		}
		return FALSE;
	}
	if(j == 7 && (str[7]<'0' || str[7]>'9')){
		return FALSE;
	}
	if (j > 0)
		return TRUE;
	//printf("sub file name is error\n");
	return FALSE;
}
