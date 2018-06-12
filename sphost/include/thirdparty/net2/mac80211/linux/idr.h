#ifndef __IDR_H__
#define __IDR_H__

#include <linux/list.h>
#include <linux/mutex.h>

struct idr {
	struct list_head list;
	struct mutex lock;
	int start, end, cur;
};

void idr_init(struct idr *idp);
void *idr_find(struct idr *idp, int id);
int  idr_alloc(struct idr *idp, void *ptr, int start, int end, gfp_t gfp_mask);
int  idr_for_each(struct idr *idp, int (*fn)(int id, void *p, void *data), void *data);
void idr_remove(struct idr *idp, int id);
void idr_destroy(struct idr *idp);

#endif
