#ifndef _JET_CDFS_H_
#define _JET_CDFS_H_
#include "jet_utility.h"

#define JET_CDFS_DCIM_SUBFOLDER_START_INDEX 101
#define JET_CDFS_DCIM_SUBFOLDER_NUM_MAX 899
#define JET_CDFS_DCIM_SUBFOLDER_INDEX_MAX 999
#define JET_CDFS_DCIM_FILE_NUM_IN_SUBFOLDER_MAX 200

#define JET_FILE_NUM_LIMIT_VIDEO_8G 15
#define JET_FILE_NUM_LIMIT_VIDEO_16G 30
#define JET_FILE_NUM_LIMIT_VIDEO_32G 60
#define JET_FILE_NUM_LIMIT_VIDEO_64G 120
#define JET_FILE_NUM_LIMIT_VIDEO_128G 240
#define JET_FILE_NUM_LIMIT_PHOTO 300


UINT32 jet_cdfs_datetime_to_minute(tmx_t time);
int jet_cdfs_pathname_to_subfolder(char *filename);
#if 0
int jet_cdfs_subfolder_get(UINT32 cdfs_folder, char *filename);
#endif
UINT32 jet_folder_bit_2_cdfs_folder(UINT32 folder);
char *jet_cdfs_next_jpg_filename_get(tmx_t time);
char *jet_cdfs_next_video_filename_get(tmx_t time);
void jet_cdfs_check_same_mins(UINT8* filename);
void jet_cdfs_pb_folder_change(void);
UINT32 jet_cdfs_pb_dcf_index_get(void);
char *jet_cdfs_folder_to_path(int cdfs_folder);
#if 0
FILE_LIST_PANA_T * jet_cdfs_current_play_handle_get(UINT16 index);
#endif
void jet_cdfs_oldest_file_delete_by_folder(UINT8 cdfs_folder);
void jet_cdfs_oldest_file_delete_for_free_by_folder(UINT8 cdfs_folder);
BOOL jet_cdfs_pana_filename_rule_check(char *filename);
int jet_cdfs_file_num_limit_by_card_size_get(UINT8 cdfs_folder);
UINT32 jet_cdfs_pathname_to_cdfs(char *pathname);
UINT32 jet_cdfs_pb_image_next_index_get(void);
#endif /* _JET_CDFS_H_ */

