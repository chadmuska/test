/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2009-2014 by iCatch Technology Co., Ltd.         *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch         *
 *  Technology Co., Ltd. All rights are reserved by iCatch Technology     *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of iCatch Technology Co., Ltd.                        *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this         *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                           *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Timothy Wu                                                    *
 *  Author: Anmin Deng                                                    *
 *  Author: Philip Lin                                                    *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 12606 $
$Date:: 2014-03-19 21:27:29#$
 */
#ifndef _PTHEAD_ERRNO_H_
#define _PTHEAD_ERRNO_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define PTHREAD_FAIL  -1
#define PTHREAD_SUCCESS 0
#define PTHREAD_EPERM  1
#define PTHREAD_ESRCH  3
#define PTHREAD_EAGAIN 11
#define PTHREAD_ENOMEM 12
#define PTHREAD_EBUSY 16
#define PTHREAD_EINVAL 22 /* Invalid argument */
#define PTHREAD_ETIMEDOUT 110
#define SEM_FAILED      ((sem_t *) 0)


#ifdef  __cplusplus
}
#endif

#endif
