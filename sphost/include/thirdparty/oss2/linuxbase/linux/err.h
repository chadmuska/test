#ifndef _LINUX_ERR_H
#define _LINUX_ERR_H

#include <linux/compiler.h>
#include <linux/types.h>

#include <asm/errno.h>

#include <stdio.h>

/* -1 should not be a valid pointer */
#define MAX_ERRNO			4095
#define IS_ERR_VALUE(x)			((x) >= (unsigned long)-MAX_ERRNO)

static inline bool __IS_ERR(const void *ptr) { return IS_ERR_VALUE((unsigned long)ptr); }
static inline bool __IS_ERR_OR_NULL(const void *ptr) { return !ptr || IS_ERR_VALUE((unsigned long)ptr); }
static inline void *__ERR_PTR(long error) { return (void *) error; }
static inline long __PTR_ERR(const void *ptr) { return (long) ptr; }

#if 0
#define IS_ERR(ptr) \
({ \
	void *_err_ptr = (void *)(ptr); \
	bool _is_err = __IS_ERR(_err_ptr); \
	if (_is_err) \
		printf("PTR_ERR %s:%d> %p\n", __func__, __LINE__, _err_ptr); \
	_is_err; \
})

#define IS_ERR_OR_NULL(ptr) \
({ \
	void *_err_ptr = (void*)(ptr); \
	bool _is_err = __IS_ERR_OR_NULL(_err_ptr); \
	if (_is_err) \
		printf("PTR_ERR %s:%d> %p\n", __func__, __LINE__, _err_ptr); \
	_is_err; \
})

#define ERR_PTR(error) \
({ \
	void *_err_ptr = __ERR_PTR(error); \
	if (__IS_ERR(_err_ptr)) \
		printf("PTR_ERR %s:%d> %p\n", __func__, __LINE__, _err_ptr); \
	_err_ptr; \
})

#else
#define IS_ERR(ptr)		__IS_ERR(ptr)
#define IS_ERR_OR_NULL(ptr)	__IS_ERR_OR_NULL(ptr)
#define ERR_PTR(error)		__ERR_PTR(error)
#endif

#define PTR_ERR(ptr)	__PTR_ERR(ptr)

static inline void *ERR_CAST(const void *ptr) { return (void *) ptr; }

static inline int PTR_ERR_OR_ZERO(const void *ptr)
{
	if (IS_ERR(ptr))
		return PTR_ERR(ptr);
	else
		return 0;
}

#endif /* _LINUX_ERR_H */
