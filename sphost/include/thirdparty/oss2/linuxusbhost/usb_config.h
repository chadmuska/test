#ifndef _USB_CONFIG_H
#define _USB_CONFIG_H

#define DEBUG_LJ (0)

#define DEBUG_ON (0)

#if DEBUG_ON
#define DEBUG_PM diag_printf
#define LJLOG() diag_printf("[LiJun]%s %s %d \n",__FILE__, __FUNCTION__,__LINE__);
#define CONFIG_USB_DEBUG (1)
#define SUPPORT_STEADY_DEBUG (1)  /*for urb info */
#define EHCI_VERBOSE_DEBUG (1)
#define EHCI_URB_TRACE     (1)
#define EHCI_DEBUG_DATA_BLOCK (1)
#define OHCI_VERBOSE_DEBUG (1)
#define OHCI_DEBUG_DATA_BLOCK (1)
#define USB_STORAGE_DEBUG (1)
#else
#define DEBUG_PM(format, arg...) do {  } while (0)
#define SJHLOG()
#define LJLOG() diag_printf("[LiJun]%s %s %d \n",__FILE__, __FUNCTION__,__LINE__);
#define CONFIG_USB_DEBUG (0)
#define SUPPORT_STEADY_DEBUG (0)  /*for urb info */
#define EHCI_VERBOSE_DEBUG (0)
#define EHCI_URB_TRACE     (0)
#define EHCI_DEBUG_DATA_BLOCK (0)
#define OHCI_VERBOSE_DEBUG (0)
#define OHCI_DEBUG_DATA_BLOCK (0)
#define USB_STORAGE_DEBUG (0)
#endif

#define CONFIG_ICATCHTEK_SPCA6350
#define CONFIG_HCD_DSR	(0)
#define HUB_ENABLE_LED_WORK (0)
#define GENERAL_USB_GET_DEVICE_STRING (0)
#define USB30_COMPATIBILITY  /*kf 110520 for usb3.0 */
#define USB30  0x04
#define USB_Version_21  0x0210

#define SUPPORT_USB_TEST_MODE (0)
#define CONFIG_HUB_RESET_TRSTRCY_EXTRA	(0)

/********************************/
/*  3 EHCI                      */
/********************************/
#define EHCI_MINIMIZE_MEMORY_USAGE (1)
#define CONFIG_EHCI_ASYNCHRONOUS_SCHEDULE 1
#define CONFIG_EHCI_PERIODIC_SCHEDULE 1



/********************************/
/*  3 storage                   */
/********************************/
#define USB_STORAGE_DEBUG_CARD_READER (0)

/*kf 20101119 manits 10233 some device wait 8s return data */
#define CONFIG_USB_STORAGE_TANSFER_TIMEOUT (10*HZ)

#define CONFIG_USB_STORAGE_CB (1)
#define CONFIG_USB_STORAGE_CBI (1)
#define CONFIG_USB_STORAGE_BULKONLY (1)
#define CONFIG_USB_STORAGE_RBC_SCSI (1)
#define CONFIG_USB_STORAGE_ATAPI (1)
#define CONFIG_USB_STORAGE_QIC (1)
#define CONFIG_USB_STORAGE_UFI (1)

#ifdef SUPPORT_USB_CAMERA
#define CONFIG_USB_EHCI_SPLIT_ISO (1)
#define CONFIG_FIX_EHCI_ISOC_BUG (1)
#define CONFIG_DMA_NONCOHERENT (1)
#define CONFIG_UVC_FRAMESIZE_FIX (1)
#define CONFIG_UVC_DSR	(1)
#define CONFIG_UVC_PROF_EN	(1)

#endif


#define SUPPORT_USB_HDD_STANDBY	1
#define ANTI_STANDBY_TIME	(300 * HZ)

#define SUPPORT_USB_STORAGE (1)

#endif
