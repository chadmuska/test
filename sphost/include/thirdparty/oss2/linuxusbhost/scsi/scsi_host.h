#ifndef _SCSI_SCSI_HOST_H
#define _SCSI_SCSI_HOST_H

#include <linux/device.h>
#include <linux/list.h>
#include <linux/types.h>

struct block_device;
struct module;
struct scsi_cmnd;
struct scsi_device;
struct Scsi_Host;
struct scsi_host_cmd_pool;
struct scsi_transport_template;

/*
 * The various choices mean:
 * NONE: Self evident.	Host adapter is not capable of scatter-gather.
 * ALL:	 Means that the host adapter module can do scatter-gather,
 *	 and that there is no limit to the size of the table to which
 *	 we scatter/gather data.
 * Anything else:  Indicates the maximum number of chains that can be
 *	 used in one scatter-gather request.
 */
#define SG_NONE 0
#define SG_ALL 0xff

struct Scsi_Host 
{
    struct list_head sdev_list; /* the scsi_devices related to this host */	
	
    spinlock_t host_lock;
    struct device shost_gendev;

    /*
     * We should ensure that this is aligned, both for better performance
     * and also because some compilers (m68k) don't automatically force
     * alignment to a long boundary.
     */
    unsigned long hostdata[0]  /* Used for storage of host specific stuff */
		__attribute__ ((aligned (sizeof(unsigned long))));
};

#define		dev_to_shost(d)		\
	container_of(d, struct Scsi_Host, shost_gendev)
#define		class_to_shost(d)	\
	container_of(d, struct Scsi_Host, shost_classdev)


extern struct Scsi_Host *scsi_host_alloc(int);
extern int __must_check scsi_add_host(struct Scsi_Host *, struct device *);
extern int scsi_scan_host(struct Scsi_Host *);
extern void scsi_scan_single_target(struct Scsi_Host *, unsigned int,
	unsigned int);
extern void scsi_rescan_device(struct device *);
extern void scsi_remove_host(struct Scsi_Host *);
extern struct Scsi_Host *scsi_host_get(struct Scsi_Host *);
extern void scsi_host_put(struct Scsi_Host *t);
extern struct Scsi_Host *scsi_host_lookup(unsigned short);

extern u64 scsi_calculate_bounce_limit(struct Scsi_Host *);


extern void scsi_unblock_requests(struct Scsi_Host *);
extern void scsi_block_requests(struct Scsi_Host *);

struct class_container;
/*
 * These two functions are used to allocate and free a pseudo device
 * which will connect to the host adapter itself rather than any
 * physical device.  You must deallocate when you are done with the
 * thing.  This physical pseudo-device isn't real and won't be available
 * from any high-level drivers.
 */
extern void scsi_free_host_dev(struct scsi_device *);
extern struct scsi_device *scsi_get_host_dev(struct Scsi_Host *);
int scsi_is_host_device(const struct device *);

#endif /* _SCSI_SCSI_HOST_H */
