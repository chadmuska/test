#ifndef __ASM_BUG_H
#define __ASM_BUG_H

#define BUG() do { diag_printf("BUG() at %s %d\n", __FILE__, __LINE__); *(int *)0=0; } while (0)

#ifndef HAVE_ARCH_BUG_ON
#define BUG_ON(condition) do { if (unlikely((condition)!=0)) BUG(); } while(0)
#endif

#define WARN_ON(condition) ({ \
	int __ret_warn_on = !!(condition); \
	if (__ret_warn_on!=0) { \
		diag_printf("Badness in %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
	} \
	__ret_warn_on; })

#define WARN_ON_FORMAT(condition, format...) ({ \
	int __ret_warn_on = !!(condition); \
	if (__ret_warn_on!=0) { \
		diag_printf(format); \
	} \
	__ret_warn_on; })

#define WARN(condition, format...)		WARN_ON_FORMAT(condition, format)
#define WARN_ON_ONCE(condition)			WARN_ON(condition)
#define WARN_ONCE(condition, format...)		WARN_ON_FORMAT(condition, format)

#define BUILD_BUG_ON(condition)			((void)sizeof(char[1 - 2*!!(condition)]))

#endif
