#ifndef __NDK_LIST_H__
#define __NDK_LIST_H__

#include <linux/list.h>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
/* Single Linked List */
#if 0
typedef struct SLListNode {
	struct SLListNode *next;
} SLListNode;

typedef struct SLList {
	SLListNode *head;
	SLListNode *tail;
} SLList;

#define SLLIST_OFFSET(TYPE, MEMBER)                     ((size_t) &((TYPE *)0)->MEMBER)
#define SLLIST_CONTAINER_OF(ptr, type, member)          ({ \
	char *__mptr = (char *)(ptr); \
	(type *)( (char *)__mptr - SLLIST_OFFSET(type,member) ); \
})

static inline void sllist_init_list(SLList *l)          { l->head = l->tail = NULL; }
static inline BOOL sllist_is_empty (SLList *l)          { return l->head == NULL; }
static inline SLListNode *sllist_get_head(SLList *l)    { return l->head; }
static inline SLListNode *sllist_get_tail(SLList *l)    { return l->tail; }

void sllist_add_head(SLListNode *n, SLList *l);
void sllist_add_tail(SLListNode *n, SLList *l);
void sllist_remove(SLListNode *n, SLList *l);
SLListNode *sllist_remove_head(SLList *l);
SLListNode *sllist_remove_tail(SLList *l);

// Remove node at position pos.
// Return the node to be removed.
// pos: 0, head; -1, tail; >0, position
SLListNode *sllist_remove_pos(int pos, SLList *l);

void sllist_dump(SLList *l);

#define sllist_for_each(p, l)         for (p = (l)->head; p; p = (p)->next)
#define sllist_for_each_pp(pp, l)     for (pp = &(l)->head; *(pp); pp = &(*pp)->next)
#endif
///////////////////////////////////////////////////////////////////////////////
// Helper functions for double-linked list
extern char list_find(plist_head_t head, plist_node_t node);

// cmp_func: return 0 if equal
typedef int (*list_cmp_func)(const void* list_node, const void* arg);
extern void *list_find_custom(plist_head_t head, const void* arg, list_cmp_func cmp_func);
extern void list_simple_free(plist_head_t head);
extern int  list_get_len(plist_head_t head);
extern void list_dump_nodes(plist_head_t head);

#ifdef __cplusplus
}
#endif

#endif

