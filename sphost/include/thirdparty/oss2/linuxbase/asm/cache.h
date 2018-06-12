#ifndef _ASM_CACHE_H
#define _ASM_CACHE_H

#define D_CACHE_LINE_SIZE	32	/* 6350 */
#define L1_CACHE_BYTES	D_CACHE_LINE_SIZE
#define ____cacheline_aligned_in_smp 	__attribute__((aligned(D_CACHE_LINE_SIZE)))

#define	NUMA_NO_NODE	(-1)

#endif