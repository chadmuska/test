/*
 * include/linux/dmapool.h
 *
 * Allocation pools for DMAable (coherent) memory.
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef LINUX_DMAPOOL_H
#define	LINUX_DMAPOOL_H

#include <asm/io.h>
#include <linux/device.h>

struct dma_pool *__dma_pool_create(const char *name, struct device *dev,
			size_t size, size_t align, size_t allocation, int use_cache);

#define dma_pool_create(name, dev, size, align, allocation) \
	__dma_pool_create(name, dev, size, align, allocation, 0)

#define dma_pool_create_cached(name, dev, size, align, allocation) \
	__dma_pool_create(name, dev, size, align, allocation, 1)

void dma_pool_destroy(struct dma_pool *pool);

void *dma_pool_alloc(struct dma_pool *pool, int mem_flags, dma_addr_t *handle);

void dma_pool_free(struct dma_pool *pool, void *vaddr, dma_addr_t addr);

#endif

