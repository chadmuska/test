#ifndef _ASM_DMA_MAPPING_H
#define _ASM_DMA_MAPPING_H

#include <asm/types.h>
#include <asm/page.h>
#include <linux/slab.h>

extern void *dma_alloc_noncoherent_align(struct device *dev, size_t size,
					 dma_addr_t * dma_handle, int flag,
					 size_t align);

/* noncoherent (cached) */
static inline void *dma_alloc_noncoherent(struct device *dev, size_t size,
					  dma_addr_t * dma_handle, int flag)
{
	return dma_alloc_noncoherent_align(dev, size, dma_handle, flag,
					   PAGE_SIZE);
}

static inline void dma_free_noncoherent(struct device *dev, size_t size,
					void *vaddr, dma_addr_t dma_handle)
{
	void *alloc_addr;

	size = (size + 3) & ~((size_t) 3);
	alloc_addr = (void *)(*((TX_ADDRWORD *) (((__u8 *) vaddr) + size)));
	kfree(alloc_addr);
}

extern void *dma_alloc_coherent_align(struct device *dev, size_t size,
				      dma_addr_t * dma_handle, int flag,
				      size_t align);

/* coherent (uncached) */
extern void *dma_alloc_coherent(struct device *dev, size_t size, dma_addr_t * dma_handle, int flag);	/* see dma-mapping.c */

static inline void dma_free_coherent(struct device *dev, size_t size,
				     void *vaddr, dma_addr_t dma_handle)
{
	dma_free_noncoherent(dev, size, CAC_ADDR(vaddr), dma_handle);
}

static inline dma_addr_t dma_map_single(struct device *dev, void *ptr,
					size_t size,
					enum dma_data_direction direction)
{
	return (dma_addr_t) virt_to_phys(ptr);
}

static inline void dma_unmap_single(struct device *dev, dma_addr_t dma_addr,
				    size_t size,
				    enum dma_data_direction direction)
{
	/* nothing to do */
}

#endif /* _ASM_DMA_MAPPING_H */
