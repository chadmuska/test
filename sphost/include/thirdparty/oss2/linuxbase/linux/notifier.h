/*
 *	Routines to manage notifier chains for passing status changes to any
 *	interested routines. We need this instead of hard coded call lists so
 *	that modules can poke their nose into the innards. The network devices
 *	needed them so here they are for the rest of you.
 *
 *				Alan Cox <Alan.Cox@linux.org>
 */

#ifndef _LINUX_NOTIFIER_H
#define _LINUX_NOTIFIER_H

#include <linux/list.h>


#define NOTIFY_DONE		0x0000		/* Don't care */
#define NOTIFY_OK		0x0001		/* Suits me */
#define NOTIFY_STOP_MASK	0x8000		/* Don't call further */
#define NOTIFY_BAD		(NOTIFY_STOP_MASK|0x0002)
						/* Bad/Veto action */
/*
 * Clean way to return from the notifier and stop further calls.
 */
#define NOTIFY_STOP		(NOTIFY_OK|NOTIFY_STOP_MASK)


struct notifier_block;

typedef	int (*notifier_fn_t)(struct notifier_block *nb,
			unsigned long action, void *data);

struct notifier_block {
	notifier_fn_t notifier_call;
	struct list_head list;
};

struct raw_notifier_head {
	struct list_head head;
};

#define RAW_NOTIFIER_HEAD(name) \
	struct raw_notifier_head name =	{ LIST_HEAD_INIT(name) }

static inline int notifier_to_errno(int ret)
{
	ret &= ~NOTIFY_STOP_MASK;
	return ret > NOTIFY_OK ? NOTIFY_OK - ret : 0;
}

int raw_notifier_chain_register(struct raw_notifier_head *nh, struct notifier_block *n);
int raw_notifier_chain_unregister(struct raw_notifier_head *nh, struct notifier_block *n);
int raw_notifier_call_chain(struct raw_notifier_head *nh, unsigned long val, void *v);

/* netdevice notifier */
extern struct raw_notifier_head netdev_chain;
#define register_netdevice_notifier(nb)		raw_notifier_chain_register(&netdev_chain, nb)
#define unregister_netdevice_notifier(nb)	raw_notifier_chain_unregister(&netdev_chain, nb)
#define netdev_notifier_info_to_dev(ptr)	((struct net_device*)(ptr))
int call_netdevice_notifiers(unsigned long action, void *dev);

#endif
