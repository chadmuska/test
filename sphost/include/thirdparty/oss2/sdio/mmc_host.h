/**
 * @file mmc_host.h
 * @brief The header file of SDIO host functions
 * @author
 *
 */
#ifndef LINUX_MMC_HOST_H
#define LINUX_MMC_HOST_H

struct mmc_request;

struct mmc_ios {
	int	clock;			/* clock rate */

/* vdd stores the bit number of the selected voltage range from below. */

	int	power_mode;		/* power supply mode */

#define MMC_POWER_OFF		0
#define MMC_POWER_UP		1
#define MMC_POWER_ON		2

	int	bus_width;		/* data bus width */

#define MMC_BUS_WIDTH_1		0
#define MMC_BUS_WIDTH_4		2
#define MMC_BUS_WIDTH_8		3
};

struct mmc_host;
extern int  sdhc_enable(struct mmc_host *host);
extern int  sdhc_disable(struct mmc_host *host, int lazy);
extern void sdhc_request(struct mmc_host *host, struct mmc_request *req);
extern void sdhc_set_ios(struct mmc_host *host, struct mmc_ios *ios);
extern int  sdhc_get_cd(struct mmc_host *host);
extern void sdhc_enable_sdio_irq(struct mmc_host *host, int enable);
extern int  sdhc_probe_sdio(struct mmc_host* host);
extern void sdhc_enter_irq_cs(void);
extern void sdhc_exit_irq_cs(void);
extern int sdio_irq_safety(void);

struct mmc_card;
struct mmc_host {
	int			index;
	unsigned int		f_min;
	unsigned int		f_max;
	unsigned long		caps;		/* Host capabilities */

#define MMC_CAP_4_BIT_DATA	(1 << 0)	/* Can the host do 4 bit transfers */
#define MMC_CAP_MMC_HIGHSPEED	(1 << 1)	/* Can do MMC high-speed timing */
#define MMC_CAP_SD_HIGHSPEED	(1 << 2)	/* Can do SD high-speed timing */
#define MMC_CAP_SDIO_IRQ	(1 << 3)	/* Can signal pending SDIO IRQs */
#define MMC_CAP_SPI		(1 << 4)	/* Talks only SPI protocols */
#define MMC_CAP_NEEDS_POLL	(1 << 5)	/* Needs polling for card-detection */
#define MMC_CAP_8_BIT_DATA	(1 << 6)	/* Can the host do 8 bit transfers */
#define MMC_CAP_DISABLE		(1 << 7)	/* Can the host be disabled */
#define MMC_CAP_NONREMOVABLE	(1 << 8)	/* Nonremovable e.g. eMMC */
#define MMC_CAP_WAIT_WHILE_BUSY	(1 << 9)	/* Waits while card is busy */

	unsigned int		max_req_size;	/* maximum number of bytes in one req */
	unsigned int		max_blk_size;	/* maximum size of one mmc block */
	unsigned int		max_blk_count;	/* maximum number of blocks in one req */

	/* private data */
	UINT32			lock;		/* lock for claim and bus ops */

	struct mmc_ios		ios;		/* current io bus settings */
	UINT32			ocr;		/* the current OCR setting */

	/* group bitfields together to minimize padding */
	unsigned int		bus_dead:1;	/* bus has been released */
	unsigned int		removed:1;	/* host is being removed */

	/* Only used with MMC_CAP_DISABLE */
	int			enabled;	/* host is enabled */

	struct mmc_card		card;		/* device attached to this host */

	const struct mmc_bus_ops *bus_ops;	/* current bus driver */

	unsigned int		sdio_irqs;
	UINT32			sdio_irq_thread;
	UINT32			sdio_irq_thread_evt;
};

extern int mmc_init_host(struct mmc_host* host);
extern int mmc_deinit_host(struct mmc_host* host);
extern int mmc_add_host(struct mmc_host * host);
extern void mmc_remove_host(struct mmc_host *host);

#define mmc_host_is_spi(host)	0

/* Always return a global area. */
const char* mmc_hostname(struct mmc_host* host);

extern int mmc_suspend_host(struct mmc_host *);
extern int mmc_resume_host(struct mmc_host *);

extern int mmc_power_save_host(struct mmc_host *host);
extern int mmc_power_restore_host(struct mmc_host *host);

extern void mmc_detect_change(struct mmc_host *);

#define mmc_signal_sdio_irq(host) do { \
	sdhc_enable_sdio_irq((host), 0); \
	if ((host)->sdio_irqs > 0) \
		ros_event_flag_set((host)->sdio_irq_thread_evt, 0x01, ROS_EVT_OR); \
} while(0)

int mmc_host_enable(struct mmc_host *host);
int mmc_host_disable(struct mmc_host *host);

void sdhc_disable_irq(void);
void sdhc_restore_irq(void);

#endif

