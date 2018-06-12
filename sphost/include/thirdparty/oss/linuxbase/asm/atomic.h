/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 *
 * But use these as seldom as possible since they are much more slower
 * than regular operations.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996, 97, 99, 2000, 03, 04 by Ralf Baechle
 */

/*
 * As workaround for the ATOMIC_DEC_AND_LOCK / atomic_dec_and_lock mess in
 * <linux/spinlock.h> we have to include <linux/spinlock.h> outside the
 * main big wrapper ...
 */
#ifndef _ASM_ATOMIC_H
#define _ASM_ATOMIC_H

#include <linux/config.h>
#include <linux/spinlock.h>

typedef struct { volatile int counter; } atomic_t;

#define ATOMIC_LOCK		unsigned long flag; local_irq_save(flag)
#define ATOMIC_UNLOCK	local_irq_restore(flag)

#define ATOMIC_INIT(i)    { (i) }

/*
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.
 */
#define atomic_read(v)		((v)->counter)

/*
 * atomic_set - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
#define atomic_set(v,i)		((v)->counter = (i))

/*
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v.
 */
static __inline__ void atomic_add(int i, atomic_t * v)
{
	ATOMIC_LOCK;
	v->counter += i;
	ATOMIC_UNLOCK;
}

/*
 * atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v.
 */
static __inline__ void atomic_sub(int i, atomic_t * v)
{
	ATOMIC_LOCK;
	v->counter -= i;
	ATOMIC_UNLOCK;
}

static __inline__ int atomic_add_return(int i, atomic_t * v)
{
	unsigned long result;

	ATOMIC_LOCK;
	result = v->counter;
	result += i;
	v->counter = result;
	ATOMIC_UNLOCK;

	return result;
}

static __inline__ int atomic_sub_return(int i, atomic_t * v)
{
	unsigned long result;

	ATOMIC_LOCK;
	result = v->counter;
	result -= i;
	v->counter = result;
	ATOMIC_UNLOCK;

	return result;
}

/*
 * atomic_sub_if_positive - add integer to atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically test @v and decrement if it is greater than 0.
 * The function returns the old value of @v minus 1.
 */
static __inline__ int atomic_sub_if_positive(int i, atomic_t * v)
{
	unsigned long result;

	ATOMIC_LOCK;
	result = v->counter;
	result -= i;
	if (result >= 0)
		v->counter = result;
	ATOMIC_UNLOCK;

	return result;
}

#define atomic_dec_return(v) atomic_sub_return(1,(v))
#define atomic_inc_return(v) atomic_add_return(1,(v))

/*
 * atomic_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
#define atomic_sub_and_test(i,v) (atomic_sub_return((i), (v)) == 0)

/*
 * atomic_inc_and_test - increment and test
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
#define atomic_inc_and_test(v) (atomic_inc_return(v) == 0)

/*
 * atomic_dec_and_test - decrement by 1 and test
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
#define atomic_dec_and_test(v) (atomic_sub_return(1, (v)) == 0)

/*
 * atomic_dec_if_positive - decrement by 1 if old value positive
 * @v: pointer of type atomic_t
 */
#define atomic_dec_if_positive(v)	atomic_sub_if_positive(1, v)

/*
 * atomic_inc - increment atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1.
 */
#define atomic_inc(v) atomic_add(1,(v))

/*
 * atomic_dec - decrement and test
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1.
 */
#define atomic_dec(v) atomic_sub(1,(v))

/*
 * atomic_add_negative - add and test if negative
 * @v: pointer of type atomic_t
 * @i: integer value to add
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */
#define atomic_add_negative(i,v) (atomic_add_return(i, (v)) < 0)

#endif /* __ASM_ATOMIC_H */
