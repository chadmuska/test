/**************************************************************************
 *
 *       Copyright (c) 2006-2014 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
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
/**
 * @file common.h
 *
 * @author Stephen Hung
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
#define SUCCESS  0
#define FAIL     1

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

/* legacy tool chains no pre-define */
#ifndef __SIZEOF_INT__
#define __SIZEOF_INT__       4
#endif /*##ifndef __SIZEOF_INT__*/
#ifndef __SIZEOF_POINTER__
#define __SIZEOF_POINTER__   4
#endif /*##ifndef __SIZEOF_POINTER__*/

#if __SIZEOF_POINTER__ == 4
#define POINTER_BITALIGN  2
#elif __SIZEOF_POINTER__ == 8
#define POINTER_BITALIGN  3
#else
# error __SIZEOF_POINTER__ not supported
#endif

#ifdef __ASSEMBLER__

#ifndef NULL
#define NULL  0
#endif

#else  /*#ifdef __ASSEMBLER__*/

#ifdef __STRICT_ANSI__
/** \note Paul 20101231
 * our stdio.h will disable some standard declaration when -ansi is
 * specified as compiler option.  So #ifdef is added to remove warning
 */
#include <stdarg.h>
#include <stddef.h>
#endif /*#ifdef __STRICT_ANSI__*/

#ifndef NULL
#define NULL  ((void *)0)
#endif

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define COMMON_ALIGNED      __attribute__ ((__aligned__))
#define UNUSED              __attribute__ ((__unused__))

#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

/*
 * Configuration clear.
 */
#define CFG_CLEAR(pcfg, size)  memset((void *)(pcfg), 0, size)

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef unsigned long long  UINT64;
typedef unsigned int        UINT32;
typedef unsigned short      UINT16;
typedef unsigned char       UINT8;

typedef signed long long    SINT64;
typedef signed int          SINT32;
typedef signed short        SINT16;
typedef signed char         SINT8;

typedef unsigned char       BOOL;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/*workaround for <stdio.h> in which putchar is a macro*/
/*"common.h" is advised to include after <stdio.h>*/
#undef putchar
int putchar(int ch);

/*legacy symbols*/
#undef  sio_printf
#define sio_printf   printf
#undef  sio_vprintf
#define sio_vprintf  vsprintf
void usb_printf(const char *format,...)
			__attribute__ ((format (printf, 1, 2)));

#ifdef __STRICT_ANSI__
/**
 * \note Edward 20101215
 * Comment snprintf here due to it is already in stdio.h.
 * If some .c file includes stdio.h and common.h and calls snprintf(),
 * then conflicting prototype error happened.
 * \note Paul 20101231
 * our stdio.h will disable some standard declaration when -ansi is
 * specified as compiler option.  So #ifdef is added to remove warning
 */
int snprintf(char *dest, size_t size, const char *format, ...)
			__attribute__ ((format (printf, 3, 4)));
int vsnprintf(char *dest, size_t size, const char *format, va_list ap);
char *strupr(char *string);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t len);
long long strtoll(const char *__restrict s, char **__restrict e, int b);
unsigned long long strtoull(const char *__restrict s, char **__restrict e,
				int b);
#endif

void sioRedirectOff(void);
void sioRedirectEnable(int enable);
void *sioPutsRedirect(void *pf);
void *sioPutcRedirect(void *pf);
void *sioPutsCallbackSet(void *pf);
void *sioPutcCallbackSet(void *pf);
void ltoa(UINT32 val, UINT8 *pbuf, UINT32 radix, UINT32 digitno);
void tmrUsWait(UINT32 us);

#endif /*#ifdef __ASSEMBLER__*/

#ifdef __cplusplus
}
#endif

#endif  /* _COMMON_H_ */

