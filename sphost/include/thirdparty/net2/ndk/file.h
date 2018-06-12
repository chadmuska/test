/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by iCatch Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __NDK_COMPAT_FILE_OP_H__
#define __NDK_COMPAT_FILE_OP_H__

#ifdef FILENAME_MAX
#undef FILENAME_MAX
#endif

#define FILENAME_MAX 256

#undef  BUFSIZ
#define BUFSIZ 256

#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <api/sp5k_disk_fsm.h>

#define ndk_stdin       ((FILE*)0x73746469)

typedef unsigned int DIR;
typedef unsigned int mode_t;

/*struct dirent {
	char d_name[512];
	char d_shortname[13];
	unsigned int d_ino;
	int d_off;
	int d_size;
	unsigned int d_reclen;
	unsigned int d_type;
	int d_openid;
};
*/

BOOL    ndk_fexist(const char *path);

FILE*   ndk_fopen(const char *path, const char *mode);
size_t  ndk_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
char *  ndk_fgets(char *s, int size, FILE *stream);
int     ndk_fputs(const char *str, FILE *stream);
int     ndk_fgetc(FILE *stream);
int     ndk_ungetc(int c, FILE *stream);
int     ndk_feof(FILE *stream);
size_t  ndk_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int     ndk_fprintf(FILE *stream, const char * format, ... ) __attribute__ ((format (printf, 2, 3)));
int     ndk_fflush(FILE *stream);
int     ndk_fseeko(FILE *stream, off_t offset, int whence);
off_t   ndk_ftello(FILE *stream);
int     ndk_fclose(FILE *stream);
off_t   ndk_fsize(FILE *stream);
/* Allocate memory and read file data to the memory. */
off_t   ndk_fread2buf(FILE *stream, void **buf);

int     ndk_fs_open(const char *pathname, int flags, mode_t /*mode, not used*/);
int     ndk_fs_close(int fd);
ssize_t ndk_fs_write(int fd, const void *buf, size_t count);
ssize_t ndk_fs_read(int fd, void *buf, size_t count);
off_t   ndk_fs_lseek(int fd, off_t offset, int whence);
int     ndk_fs_remove(const char *pathname);
int     ndk_fs_rename(const char *oldpath, const char *newpath);
int     ndk_fs_mkdir(const char *pathname, mode_t mode);
char*   ndk_fs_getcwd(char *buf, size_t size);
int     ndk_fs_stat(const char *pathname, struct stat *st);
off_t   ndk_fs_size(const char* pathname);
size_t  ndk_fs_fsize(int fd);

// don't support re-entry
struct  dirent* ndk_fs_readdir(DIR *dirp);
DIR*    ndk_fs_opendir(const char *pathname);
int     ndk_fs_closedir(DIR *dirp);
// Create directories for file
int     ndk_fs_create_dirs(const char *filename);
int	ndk_fs_rmdir(const char *fullpath);

#ifdef __cplusplus
}
#endif

#endif

#ifdef NDK_COMPAT_FILE_API
#ifndef NDK_COMPAT_FILE_API_DEFINED
#define NDK_COMPAT_FILE_API_DEFINED
	#define fopen(a,b)              ndk_fopen(a,b)
	#define fread(a,b,c,d)          ndk_fread(a,b,c,d)
	#define fwrite(a,b,c,d)         ndk_fwrite(a,b,c,d)
	#define fprintf(stream, fmt...) ndk_fprintf(stream, fmt)
	#define fgets(a,b,c)            ndk_fgets(a,b,c)
	#define fseeko(a,b,c)           ndk_fseeko(a,b,c)
	#define ftello(a)               ndk_ftello(a)
	#define fclose(a)               ndk_fclose(a)

	#define open(a,b,c)             ndk_fs_open(a,b,c)
	#define close(a)                ndk_fs_close(a)
	#define write(a,b,c)            ndk_fs_write(a,b,c)
	#define read(a,b,c)             ndk_fs_read(a,b,c)
	#define lseek(a,b,c)            ndk_fs_lseek(a,b,c)
	#define remove(a)               ndk_fs_remove(a)
	#define rename(a, b)            ndk_fs_rename(a, b)
	#define mkdir(a,b)              ndk_fs_mkdir(a,b)
	#define getcwd(a,b)             ndk_fs_getcwd(a,b)
	#define readdir(a)              ndk_fs_readdir(a)
	#define closedir(a)             ndk_fs_closedir(a)
	#define opendir(a)              ndk_fs_opendir(a)
#endif
#endif

