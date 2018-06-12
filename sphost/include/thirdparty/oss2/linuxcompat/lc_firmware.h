#ifndef _LINUX_FIRMWARE_H
#define _LINUX_FIRMWARE_H

#include <linux/types.h>

struct firmware {
	size_t size;
	u8_t   data[0];
};

struct device;
int request_firmware(const struct firmware **fw, const char *name,
		     struct device *device);

int __request_firmware_nowait(const char *name, void *context,
	void (*cont)(const struct firmware *fw, void *context));

#define request_firmware_nowait(module, uevent, name, device, gfp, context, cont) \
	__request_firmware_nowait(name, context, cont)

void release_firmware(const struct firmware *fw);

#endif

