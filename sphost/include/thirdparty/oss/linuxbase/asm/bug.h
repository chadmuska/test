#ifndef __ASM_BUG_H
#define __ASM_BUG_H

#define BUG() do { diag_printf("BUG() at %s %d\n", __FILE__, __LINE__); *(int *)0=0; } while (0)

#ifndef HAVE_ARCH_BUG_ON
#define BUG_ON(condition) do { if (unlikely((condition)!=0)) BUG(); } while(0)
#endif

#define WARN_ON(condition) do { \
	if (unlikely((condition)!=0)) { \
		diag_printf("Badness in %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
	} \
} while (0)

#endif
