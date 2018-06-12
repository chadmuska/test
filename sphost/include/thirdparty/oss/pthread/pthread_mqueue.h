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
#ifndef _PTHREAD_MQUEUE_H_
#define _PTHREAD_MQUEUE_H_

#ifdef  __cplusplus
extern  "C" {
#endif


#include "common.h"
#include "pthread_def.h"

typedef int mqd_t;
/*typedef UINT32 ssize_t;*/

struct mq_attr
{      
 	UINT32 mq_flags;      
	UINT32 mq_maxmsg;      
	UINT32 mq_msgsize;      
	UINT32 mq_curmsgs;
} ;



int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
int mq_setattr(mqd_t mqdes, const struct mq_attr *attr, struct mq_attr *oattr);
mqd_t mq_open(const char *name, int oflag, ... /* mode_t mode, struct mq_attr *attr */);
int mq_send(mqd_t mqdes, const char * ptr, size_t len, unsigned int prio);
ssize_t mq_receive(mqd_t mqdes, char * ptr, size_t len , unsigned int *priop);
int mq_close(mqd_t mqdes);
mqd_t mq_timedreceive(mqd_t mqdes, char *msg_ptr,
                      size_t msg_len, unsigned *msg_prio,
                      const struct timespec *abs_timeout);
int mq_timedsend (mqd_t mqdes, const char *msg_ptr, size_t msg_len,  
							unsigned int msg_prio, const struct timespec *abs_timeout);
mqd_t mq_unlink(const char *name);
int mq_notify(mqd_t mqdes, const struct sigevent *notification);							



#ifdef  __cplusplus
}
#endif

#endif /* _PTHREAD_MQUEUE_H_ */
