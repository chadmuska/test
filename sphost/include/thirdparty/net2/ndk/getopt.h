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
#ifndef __NdkGetOpt_H__
#define __NdkGetOpt_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ndk_optarg ndk_getopt_optarg()
#define ndk_optind (*ndk_getopt_optind())
#define ndk_opterr ndk_getopt_opterr()

typedef struct {
	char *optarg;   /* pointer to the start of the option argument  */
	int   optind;   /* number of the next argv[] to be evaluated    */
	int   opterr;   /* non-zero if a question mark should be returned
                           when a non-valid option character is detected */
	char *pIndexPosition; /* place inside current argv string */
} NDKOptArg;

int   ndk_getopt (int argc, char **argv, const char *optstring);
/* first_arg_is_cmd_name:
 *   1: the first argument in command line is command name, like the C main function.
 *   0: no command name, all options in command line is arguments
 */
void  ndk_getopt_reset(char first_arg_is_cmd_name);
char* ndk_getopt_optarg();	/* pointer to the start of the option argument  */
int*  ndk_getopt_optind();	/* number of the next argv[] to be evaluated    */
int   ndk_getopt_opterr();	/* non-zero if a question mark should be returned
				   when a non-valid option character is detected */

void ndk_optarg_init(NDKOptArg *optarg, char first_arg_is_cmd_name);
int  ndk_optarg_get(NDKOptArg *optarg, int argc, char **argv, const char *optstring);

/* Return: 0, compare failed. 1 or 2: compare succeed and number of args been processed. */
int  ndk_optarg_compare(int *argc, char ***argv, const char *optstr, char **param/*NULL=no param*/);
int  ndk_optarg_vcompare(int *argc0, char ***argv0, int *index, char **param, ...);

#ifdef __cplusplus
}
#endif

#endif
