#ifndef _LINUX_REBOOT_H
#define _LINUX_REBOOT_H

struct notifier_block
{
	int (*notifier_call)(struct notifier_block *self, unsigned long, void *);
};

static inline int register_reboot_notifier(struct notifier_block * nb)
{
	diag_printf("register_reboot_notifier\n");
	return 0;
}

static inline int unregister_reboot_notifier(struct notifier_block * nb)
{
	diag_printf("unregister_reboot_notifier\n");
	return 0;
}

#endif /* _LINUX_REBOOT_H */
