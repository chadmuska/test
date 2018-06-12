#ifndef _SCSI_SCSI_DEVICE_H
#define _SCSI_SCSI_DEVICE_H

#include <linux/device.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <asm/atomic.h>

struct scsi_device 
{
    struct Scsi_Host *host;
    struct list_head siblings;   /* list of all devices on this host */
    unsigned int id;
    unsigned int lun;
} __attribute__((aligned(sizeof(unsigned long))));

#endif /* _SCSI_SCSI_DEVICE_H */
