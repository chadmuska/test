/**************************************************************************
 *
 *          Copyright (c) 2013 by iCatch Technology Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology Inc. All rights are reserved by iCatch Technology Inc.
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology Inc.
 *
 *  iCatch Technology Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 12708 $
$Date:: 2014-03-24 20:07:09#$
 */
/**
 * \file time_ll.h
 * \author Anmin Deng
 * \brief declaring usage of "long long" symbols corresponding those
 * in standard libs <time.h>
 */

#ifndef _TIME_LL_H_
#define _TIME_LL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include <time.h>


#define TIMEOFDAY_TIMESTAMPS_ADJ_SUPPORT   1


/* defining the below if "time_t" is "long long" */
/*#define TIME_T_IS_LONGLONG   1*/


#if TIME_T_IS_LONGLONG

# define time_ll_t       time_t

# define timeval_ll      timeval

# define settimeofday_ll settimeofday
# define gettimeofday_ll gettimeofday

# define time_ll         time

# define mktime_ll       mktime
# define localtime_ll    localtime
# define localtime_ll_r  localtime_r
# define gmtime_ll       gmtime
# define gmtime_ll_r     gmtime_r

# define ctime_ll        ctime
# define ctime_ll_r      ctime_r

# define difftime_ll     difftime

#else  /*#if TIME_T_IS_LONGLONG*/

typedef long long time_ll_t;

struct timeval_ll {
	time_ll_t tv_sec;
	suseconds_t tv_usec;
};

int settimeofday_ll(const struct timeval_ll *tv, const struct timezone *tz);
int gettimeofday_ll(struct timeval_ll *tv, struct timezone *tz);

time_ll_t time_ll(time_ll_t *t);

time_ll_t mktime_ll(struct tm *tm);
struct tm *localtime_ll(const time_ll_t *t);
struct tm *localtime_ll_r(const time_ll_t *t, struct tm *res);
struct tm *gmtime_ll(const time_ll_t *t);
struct tm *gmtime_ll_r(const time_ll_t *t, struct tm *res);

char *ctime_ll(const time_ll_t *t);
char *ctime_ll_r(const time_ll_t *t, char *s);

double difftime_ll(time_ll_t t1, time_ll_t t2);

#endif /*#if TIME_T_IS_LONGLONG*/


/**
 * \param[in] tz specifies TZ string.
 * the TZ string given by tz must be in accessible space and remained constant
 * at all times of calling timezone related APIs
 * (tzset, ctime, mktime, localtime, strftime, etc).
 * \return 0:OK; o/w the TZ string cannot be interpreted, UTC is used;
 */
int tzenvset(const char *tz);

/**
 * \brief to get TZ environment string
 * \param[out] s to store the result string
 * \return number of characters in the result string, excluding tailing '\0'
 */
int tzenvget(char *s);


#ifdef __cplusplus
}
#endif

#endif /*_TIME_LL_H_*/
