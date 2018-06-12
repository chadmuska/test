#ifndef __LINUX_VERSION_H__
#define __LINUX_VERSION_H__

#define UTS_RELEASE "2.6.10-rc1-bk7"
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define LINUX_VERSION_CODE KERNEL_VERSION(2,6,32)

#endif /* __LINUX_VERSION_H__ */
