/**************************************************************************
 *
 *       Copyright (c) 2016 by iCatch Technology, Inc.
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

#ifndef __OSS_TTY_H__
#define __OSS_TTY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* tty terminal type */
typedef void *tty_term;

int  tty_term_open(tty_term *term, const char *driver_name);
void tty_term_close(tty_term term);
int  tty_term_write(tty_term term, const unsigned char *buf, unsigned int count);
int  tty_term_write_string(tty_term term, const char *str);
int  tty_term_read(tty_term term, unsigned char *buf, unsigned int count);
long tty_term_ioctl(tty_term term, unsigned int cmd, unsigned long arg);
/* \param timeout: milliseconds to wait. <=0 will return immediately
 * \return TRUE: has something to read. FALSE nothing to read.
 */
BOOL tty_term_poll_read(tty_term term, int timeout);
void tty_term_dump(tty_term term);

int  tty_global_init(const char *options);
void tty_global_exit();

#ifdef __cplusplus
}
#endif

#endif
