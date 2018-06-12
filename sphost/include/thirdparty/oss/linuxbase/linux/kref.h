#ifndef _KREF_H_
#define _KREF_H_

#include <asm/atomic.h>

struct kref {
	atomic_t refcount;
};

#define kref_init(kref)	atomic_set(&(kref)->refcount,1)
#define kref_get(kref)	atomic_inc(&(kref)->refcount)
#define kref_put(kref, release)					\
	do {							\
		if (atomic_dec_and_test(&(kref)->refcount)) {	\
			release(kref);				\
		}						\
	} while(0)

#endif /* _KREF_H_ */
