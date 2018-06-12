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
 *  Author: Chris.Hsu
 *
 **************************************************************************/
#ifndef _OSS_ROS_API_H_
#define _OSS_ROS_API_H_

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 @file   oss_ros_api.h
 @brief  Header file for ros wrapper api
 */
#include <../inc/common.h>

// ROS APIs
UINT32 ros_thread_create(char *name, UINT32 priority, void (*entry_func)(void), UINT32 slice, UINT32 stack_depth);
UINT32 ros_thread_create_ext(char *sz_name,UINT32 priority,void (*entry_func)(UINT32 param),UINT32 param,UINT32 slice,UINT32 stack_depth,UINT32 autostart);
UINT32 ros_thread_sleep(UINT32 ms);
UINT32 ros_thread_destroy(UINT32 thread_id);
SINT32 ros_thread_priority_change(UINT32 thread_id, UINT32 new_priority);
UINT32 ros_thread_identify(void);
UINT32 ros_thread_suspend(UINT32 thread_id);
UINT32 ros_thread_resume(UINT32 thread_id);
void ros_thread_trash(UINT32 thread_id);
void ros_thread_relinquish(void);
UINT32 ros_timer_create(char *pname, void (*pfunc)(UINT32), UINT32 ms, UINT32 arg, UINT32 activate);
UINT32 ros_timer_destroy(UINT32 handle);
UINT32 ros_timer_activate(UINT32 handle, UINT32 activate);
UINT32 ros_timer_change(UINT32 handle, UINT32 ms);
UINT32 ros_semaphore_create(char *name, UINT32 init_val);
UINT32 ros_semaphore_get(UINT32 semaphore_id, UINT32 timeout);
UINT32 ros_semaphore_put(UINT32 semaphore_id);
UINT32 ros_semaphore_cnt_get(UINT32 semaphore_id, UINT32 *pcnt);
UINT32 ros_semaphore_destroy(UINT32 semaphore_id);
UINT32 ros_queue_create(char *name, UINT32 msg_size, UINT32 nr_msg);
UINT32 ros_queue_send(UINT32 queue_id, void *msg, UINT32 timeout);
UINT32 ros_queue_send_ext(UINT32 queue_id, void *msg, UINT32 timeout, UINT32 min_size);
UINT32 ros_queue_receive(UINT32 queue_id, void *msg, UINT32 timeout);
UINT32 ros_queue_destroy(UINT32 queue_id);
UINT32 ros_mutex_create(char *name, UINT32 inherit);
UINT32 ros_mutex_destroy(UINT32 mutex_id);
UINT32 ros_mutex_put(UINT32 mutex_id);
UINT32 ros_mutex_get(UINT32 mutex_id, UINT32 timeout);
UINT32 ros_event_flag_create(char *name);
UINT32 ros_event_flag_get(UINT32 event_flag_id, UINT32 flags, UINT32 option, UINT32 *cur_flags, UINT32 timeout);
UINT32 ros_event_flag_set(UINT32 event_flag_id, UINT32 flags, UINT32 option);
UINT32 ros_event_flag_destroy(UINT32 event_flag_id);

/* block pool */
UINT32 ros_block_pool_create(char *name, UINT32 block_size, UINT32 block_num);
void * ros_block_allocate(UINT32 pool_id);
void * ros_block_allocate_uncached(UINT32 pool_id);
UINT32 ros_block_release(void *block_ptr);
UINT32 ros_block_pool_info_get(UINT32 pool_id, UINT32 *available, UINT32 *total_blocks);
UINT32 ros_block_pool_destroy(UINT32 pool_id);

UINT32 ros_pthread_check(UINT32 thread_id);
UINT32 ros_semaphore_getvalue(UINT32 semaphore_id, UINT32 *sval);
UINT32 ros_timer_gettime(UINT32 handle, UINT32* remaining_ticks);
UINT32 ros_thread_info_get(UINT32 thread_id, UINT32* state);
UINT32 ros_thread_terminate(UINT32 thread_id);
UINT32 ros_thread_wait_abort(UINT32 thread_id);

void *osMemRealloc(void *pori, UINT32 size);
void *osMemAlloc(UINT32 size);
void *osMemAllocCache(UINT32 size);
UINT32 osMemFree(void *pointer);

void  *memByteCreate(char *name, void *ptr, UINT32 size);
UINT32 memByteDestroy(void *ptr, UINT32 force);
void  *memByteAlloc(UINT32 size, void *pcb, void *psmall, UINT32 smallthrs, UINT32 isbot);

UINT32 ros_enter_critical(void);
void ros_exit_critical(UINT32 x);

int ros_usb_irq_val_get(void);

#ifdef  __cplusplus
}
#endif

#endif /* _OSS_ROS_API_H_ */
