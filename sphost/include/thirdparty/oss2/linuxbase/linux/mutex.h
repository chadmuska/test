#include "lc_mutex.h"

#ifndef __OSS_LINUX_MUTEX_H__
#define __OSS_LINUX_MUTEX_H__

typedef struct mutex rwlock_t;

#define DEFINE_RWLOCK(name)		rwlock_t name = MUTEX_INITIALIZER(name)
#define write_lock_bh(m)		mutex_lock(m)
#define write_unlock_bh(m)		mutex_unlock(m)

#define read_lock_bh(m)			mutex_lock(m)
#define read_unlock_bh(m)		mutex_unlock(m)

#define rwlock_init(lock)		__mutex_init(lock, 0, __FILE__ ":" __STR_LINE__)
#define rwlock_release			mutex_destroy

#define read_lock(m)			mutex_lock(m)
#define read_unlock(m)			mutex_unlock(m)

#define write_lock(m)			mutex_lock(m)
#define write_unlock(m)			mutex_unlock(m)

#endif

