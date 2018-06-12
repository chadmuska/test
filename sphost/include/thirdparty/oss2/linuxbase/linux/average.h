#ifndef _LINUX_AVERAGE_H
#define _LINUX_AVERAGE_H

/* Exponentially weighted moving average (EWMA) */

static inline int ilog2(unsigned long v)
{
	int l = 0;
	while ((1UL << l) < v)
		l++;
	return l;
}

#define DECLARE_EWMA(name, _factor, _weight)				\
	struct ewma_##name {						\
		unsigned long internal;					\
	};								\
	static inline void ewma_##name##_init(struct ewma_##name *e)	\
	{								\
		e->internal = 0;					\
	}								\
	static inline unsigned long					\
	ewma_##name##_read(struct ewma_##name *e)			\
	{								\
		return e->internal >> ilog2(_factor);			\
	}								\
	static inline void ewma_##name##_add(struct ewma_##name *e,	\
					     unsigned long val)		\
	{								\
		unsigned long internal = ACCESS_ONCE(e->internal);	\
		unsigned long weight = ilog2(_weight);			\
		unsigned long factor = ilog2(_factor);			\
									\
		ACCESS_ONCE(e->internal) = internal ?			\
			(((internal << weight) - internal) +		\
				(val << factor)) >> weight :		\
			(val << factor);				\
	}

#endif /* _LINUX_AVERAGE_H */
