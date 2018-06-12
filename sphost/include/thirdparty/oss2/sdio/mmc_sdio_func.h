/**
 * @file mmc_sdio_func.h
 * @brief The header file of SDIO functions
 * @author
 *
 */

#ifndef MMC_SDIO_FUNC_H
#define MMC_SDIO_FUNC_H

#include <linux/types.h>
#include <linux/device.h>

#define SDIO_ANY_ID (~0)

struct sdio_device_id {
	UINT8	class;			/* Standard interface or SDIO_ANY_ID */
	UINT16	vendor;			/* Vendor or SDIO_ANY_ID */
	UINT16	device;			/* Device ID or SDIO_ANY_ID */
	unsigned long driver_data;
};

struct mmc_card;
struct sdio_func;
struct sdio_driver;

typedef void (sdio_irq_handler_t)(struct sdio_func *);

struct sdio_device {
	struct device		base;
	void			*platform_data;
	struct sdio_func	*func;
};

struct sdio_func {
	struct mmc_card		*card;		/* the card this device belongs to */
	struct sdio_driver	*drv;
	struct sdio_device	dev;

	sdio_irq_handler_t	*irq_handler;	/* IRQ callback */
	unsigned int		num;		/* function number */

	unsigned char		class;		/* standard interface class */
	unsigned short		vendor;		/* vendor id */
	unsigned short		device;		/* device id */

	unsigned		max_blksize;	/* maximum block size */
	unsigned		cur_blksize;	/* current block size */

	unsigned		enable_timeout;	/* max enable timeout in msec */

	unsigned int		state;		/* function state */
#define SDIO_STATE_PRESENT	(1<<0)		/* present in sysfs */

	union {
		UINT8		tmpbuf[4];	/* DMA:able scratch buffer */
		UINT16		tmpu16;
		UINT32		tmpu32;
	};
	char			initialized;
};

extern const char* sdio_func_id(struct sdio_func* f);

#define sdio_get_drvdata(f)	((f)->dev.platform_data)
#define sdio_set_drvdata(f,d)	(f)->dev.platform_data = (d)

struct sdio_driver {
	char *name;
	const struct sdio_device_id *id_table;
	int sdio_func_num;

	int (*probe)(struct sdio_func *, const struct sdio_device_id *);
	void (*remove)(struct sdio_func *);
};

#define SDIO_DEVICE(vend,dev) \
	.class = SDIO_ANY_ID, \
	.vendor = (vend), .device = (dev)

#define SDIO_DEVICE_CLASS(dev_class) \
	.class = (dev_class), \
	.vendor = SDIO_ANY_ID, .device = SDIO_ANY_ID

extern int sdhc_initialize(char second_sdio,UINT32 f_max);
extern void sdhc_deinitialize(void);
extern int sdio_register_driver(struct sdio_driver *);
extern void sdio_unregister_driver(struct sdio_driver *);

/*
 * SDIO I/O operations
 */
extern void sdio_claim_host(struct sdio_func *func);
extern void sdio_release_host(struct sdio_func *func);

extern int sdio_enable_func(struct sdio_func *func);
extern int sdio_disable_func(struct sdio_func *func);

extern int sdio_set_block_size(struct sdio_func *func, unsigned blksz);

extern int sdio_claim_irq(struct sdio_func *func, sdio_irq_handler_t *handler);
extern int sdio_release_irq(struct sdio_func *func);

extern unsigned int sdio_align_size(struct sdio_func *func, unsigned int sz);

extern UINT8 sdio_readb(struct sdio_func *func, unsigned int addr, int *err_ret);
extern UINT16 sdio_readw(struct sdio_func *func, unsigned int addr, int *err_ret);
extern UINT32 sdio_readl(struct sdio_func *func, unsigned int addr, int *err_ret);

extern int sdio_memcpy_fromio(struct sdio_func *func, void *dst,
	unsigned int addr, int count);
extern int sdio_readsb(struct sdio_func *func, void *dst,
	unsigned int addr, int count);

extern void sdio_writeb(struct sdio_func *func, UINT8 b,
	unsigned int addr, int *err_ret);
extern void sdio_writew(struct sdio_func *func, UINT16 b,
	unsigned int addr, int *err_ret);
extern void sdio_writel(struct sdio_func *func, UINT32 b,
	unsigned int addr, int *err_ret);

extern UINT8 sdio_writeb_readb(struct sdio_func *func, UINT8 write_byte,
	unsigned int addr, int *err_ret);

extern int sdio_memcpy_toio(struct sdio_func *func, unsigned int addr,
	void *src, int count);
extern int sdio_writesb(struct sdio_func *func, unsigned int addr,
	void *src, int count);

extern unsigned char sdio_f0_readb(struct sdio_func *func,
	unsigned int addr, int *err_ret);
extern void sdio_f0_writeb(struct sdio_func *func, unsigned char b,
	unsigned int addr, int *err_ret);
extern void sdio_f0_writeb_lenient(struct sdio_func *func, unsigned char b, unsigned int addr,
	int *err_ret);

extern struct sdio_func* sdioFuncGet(int num);

extern void sdhc_release_irq(void);
extern void sdhc_restore_irq(void);

extern void sdio_cmd53_rdwr_set(BOOL en);

#endif

