/**************************************************************************
 *
 *       Copyright (c) 2015 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/

#ifndef __NDK_KEYFILE__
#define __NDK_KEYFILE__

#ifdef __cplusplus
extern "C" {
#endif

#define NDK_KEYFILE_KEYLEN_MAX		256
#define NDK_KEYFILE_KEYVAL_MAX		256

struct tagNDKKeyFile;
typedef struct tagNDKKeyFile NDKKeyFile;

typedef struct {
	NDKKeyFile *keyfile;
	void       *pos;
	const char *group;
	BOOL        reverse;
} NDKKeyFileIter;

NDKKeyFile *ndk_keyfile_new();
int   ndk_keyfile_load_from_file(NDKKeyFile *kf, const char *filename, BOOL create);
int   ndk_keyfile_save_to_file(NDKKeyFile *kf, const char *filename);
void  ndk_keyfile_delete(NDKKeyFile *kf);

void  ndk_keyfile_iter_begin(NDKKeyFileIter *iter, NDKKeyFile *kf, const char *grp, BOOL reverse);
BOOL  ndk_keyfile_iter_end(NDKKeyFileIter *iter);
void  ndk_keyfile_iter_next(NDKKeyFileIter *iter);
const char *ndk_keyfile_iter_get_key(NDKKeyFileIter *iter);
const char *ndk_keyfile_iter_get_value(NDKKeyFileIter *iter);

char  *ndk_keyfile_get_string(NDKKeyFile *kf, const char *grp, const char *key);
char **ndk_keyfile_get_string_list(NDKKeyFile *kf, const char *grp, const char *key, unsigned *length);
char **ndk_keyfile_get_groups(NDKKeyFile *kf, unsigned *length);
char **ndk_keyfile_get_keys(NDKKeyFile *kf, const char *grp, unsigned *length);

int   ndk_keyfile_set_string(NDKKeyFile *kf, const char *grp, const char *key, const char *string);
void  ndk_keyfile_set_string_list(NDKKeyFile *kf, const char *grp, const char *key, const char *list[], unsigned length);

BOOL  ndk_keyfile_has_group(NDKKeyFile *kf, const char *grp);
/* Number of keys to been removed. */
int   ndk_keyfile_remove_group(NDKKeyFile *kf, const char *grp);
BOOL  ndk_keyfile_remove_key(NDKKeyFile *kf, const char *grp, const char *key);

void  ndk_keyfile_dump(NDKKeyFile *kf);
void  ndk_keyfile_dump_string_list(char **slist);

#ifdef __cplusplus
}
#endif

#endif
