/**************************************************************************
 *
 *	 Copyright (c) 2015 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/

#ifndef __NDK_BLUETOOTH_H__
#define __NDK_BLUETOOTH_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *   Bluetooth Argument ID
 *****************************************************************************/
enum {
	NDK_BT_ARG_END	= 0,	/* End Flag. Last argument for API with vairable arguments. */
};

/******************************************************************************
 *   Common Bluetooth API functions
 *****************************************************************************/
/* Hardware interface for Bluetooth */
enum {
	NDK_BT_HWIF_UART  = 0,
	NDK_BT_HWIF_USB,
	NDK_BT_HWIF_SDIO,

	NDK_BT_HWIF_MAX
};

typedef struct {
	UINT32 (*pf_init)();
	UINT32 (*pf_set_baudrate)(UINT32 baud);
	UINT32 (*pf_set_flowctrl)(BOOL bHwFlowCtrl);
	UINT32 (*pf_read)(void *buf, UINT32 len, UINT32 timeout);
	UINT32 (*pf_write)(void *buf, UINT32 len);
} NDKBtUartOps;

/* \brief Initialize BT global variables and register BT commands.
 */
int  ndk_bt_global_init(const char *options);

/******************************************************************************
 *   Adpater
 *****************************************************************************/
int  ndk_bt_adapter_enable(
	const char	*adapter_name,
	unsigned int	hwif,
	void		*ifops,
	const char	*config_dir, /* For firmware and configure files for BT adapter. */
	const char	*extra_opts);

void ndk_bt_adapter_disable();

/******************************************************************************
 *   HCI
 *****************************************************************************/
int  ndk_bt_hciattach(
	const char	*type,
	int		init_speed,
	int		speed,
	BOOL		flow,
	const char	*bdaddr);

void ndk_bt_hcidetach();

int  ndk_bt_hciconfig(
	int		dev_id,
	const char	*cmd,
	const char	*opt);

/******************************************************************************
 *   SDP Service
 *****************************************************************************/
typedef int (*NDKBtPinCodeCb)(char *pin_buf);

/* \brief Arg ID for ndk_bt_sdp_start_server. */
enum {
	NDK_BT_ARG_SDP_SERVER_STORAGE_DIR	= 1, /* const char*. Mandatory. */
	NDK_BT_ARG_SDP_SERVER_IO_CAPIBILITY	= 2, /* unsigned int. Mandatory. */
	NDK_BT_ARG_SDP_SERVER_PINCODE_CB	= 3, /* NDKBtPinCodeCb. Mandatory. */
	NDK_BT_ARG_SDP_SERVER_CLASS_OF_DEVICE	= 4, /* unsigned int. Mandatory. */
	NDK_BT_ARG_SDP_SERVER_BT_MODE		= 5, /* unsigned int. Bluetooth mode. Optional. */
	NDK_BT_ARG_SDP_SERVER_MGMT_DEBUG	= 6, /* int. 1: enable mgmt debug. 0: disable. Optional. */
};

/* \brief IO Capability */
enum {
	NDK_BT_IOCAP_DISPLAYONLY	= 0x00,
	NDK_BT_IOCAP_DISPLAYYESNO	= 0x01,
	NDK_BT_IOCAP_KEYBOARDONLY	= 0x02,
	NDK_BT_IOCAP_NOINPUTNOOUTPUT	= 0x03,
	NDK_BT_IOCAP_KEYBOARDDISPLAY	= 0x04,

	NDK_BT_IOCAP_INVALID		= 0xFF
};

/* \brief Bluetooth Mode. */
enum {
	NDK_BT_MODE_DUAL,
	NDK_BT_MODE_BREDR,
	NDK_BT_MODE_LE,
};

/* \brief  MW: set adapter discoverable on or off.
 */
int ndk_bt_adapter_discoverable(int onoff);

/* \brief  MW: set adapter powered on or off.
 */
int ndk_bt_adapter_powered(int onoff);

/* \brief Start SDP Server.
 */
int  ndk_bt_sdp_start_server(
	unsigned int	arg_id,		/* see NDK_BT_ARG_SDP_SERVER_XXX */
	...);

void ndk_bt_sdp_stop_server();

/* Service ID */
enum {
	NDK_BT_SDP_SVC_UNKNOWN		= 0,
	NDK_BT_SDP_SVC_SP		= 0x1101, /* Serial Port. Arg: int channel */
	NDK_BT_SDP_SVC_HS		= 0x1108, /* Headset */
	NDK_BT_SDP_SVC_HF		= 0x111e, /* Handsfree */
	NDK_BT_SDP_SVC_A2SRC		= 0x110a, /* Audio source */
	NDK_BT_SDP_SVC_A2SNK		= 0x110b, /* Audio Sink */
	NDK_BT_SDP_SVC_GATT		= 0x1801, /* Generic Attrib */
	NDK_BT_SDP_SVC_OPUSH		= 0x1105, /* OBEX Object Push */
	NDK_BT_SDP_SVC_AVRCT		= 0x110e, /* AV Remote Control */
};

/* \return <0: error; >0 service handle. */
int  ndk_bt_sdp_add_service(int svc_id, ...);

int  ndk_bt_sdp_del_service(int svc_handle);

/******************************************************************************
 *   Device Management
 *****************************************************************************/
/* Authentication type for pairing. */
enum {
	NDK_BT_AUTH_TYPE_PINCODE,
	NDK_BT_AUTH_TYPE_PASSKEY,
	NDK_BT_AUTH_TYPE_CONFIRM,
	NDK_BT_AUTH_TYPE_NOTIFY
};

typedef union
{
	char		pincode[16]; /* end with 0 */
	UINT32		passkey;
	BOOL		confirm;
} NDKBtAuthUserReply;

typedef BOOL (*NDKBtAuthUserCb)(int type, NDKBtAuthUserReply *aur, void *user_data);

/* \description: Start to discover nearby devices.
 */
int  ndk_bt_start_discovery();

/* \description: Stop discovery.
 */
void ndk_bt_stop_discovery();

/* \description: Pair device.
 * \param addr: Device mac address.
 */
int  ndk_bt_pair_device(const char *addr);

/* \description: Remove connected device.
 * \param addr: Device mac address. NULL to remove all devices.
 */
int  ndk_bt_remove_device(const char *addr);

/* \description: Connect to device.
 * \param addr: String of device BT MAC address.
 */
int  ndk_bt_connect_device(const char *addr);

int  ndk_bt_disconnect_device(const char *addr);

int ndk_bt_disconnect_all_devices();

typedef struct NDKBtDevInfo {
	char    name[64];    /* String. BT device name. */
	char    bdaddr[18];  /* String. BT Mac Address. */
	UINT32  devclass;    /* class of device. */
	BOOL    paired;      /* Have paired or not. */
	BOOL    connected;   /* Have connected or not. */
	BOOL    bredr;       /* Support BR/EDR mode. */
	BOOL    le;          /* Support LE mode. */
} NDKBtDevInfo;

/* \description: Get the BT device list stored in the adapter.
 * \paramm devices: OUT. Returns an array contains the BT devices.
 * \return: 0: No BT devices found. The devices will be set to NULL.
           >0: Number of devices. Free the devices array with the ndk_bt_free_device_list.
           <0: error.
 */
int ndk_bt_get_device_list(NDKBtDevInfo **devices);

/* \description: Free the devices array returned from ndk_bt_get_device_list.
 * \param devcies: Devices array.
 */
void ndk_bt_free_device_list(NDKBtDevInfo *devices);

/* \description Get other BT device informations which are not listed in the NDKBtDevInfo structure.
 *
 */
int ndk_bt_get_device_extra_info(const char *bdaddr, unsigned int code, void *value);

/* Device Event */
enum {
	NDK_BT_DEV_EVT_ADDED,
	NDK_BT_DEV_EVT_REMOVED,
};

typedef void (*NDKBtDeviceEventHandler)(const NDKBtDevInfo *di, int event, void *param);
void ndk_bt_device_event_handler_set(NDKBtDeviceEventHandler handler);

/******************************************************************************
 *   TTY Service
 *****************************************************************************/
enum {
	NDK_BT_TTYDEV_MSG_OPENED = 0,	/* Param: None */
	NDK_BT_TTYDEV_MSG_CLOSED,	/* Param: None */
	NDK_BT_TTYDEV_MSG_DATA, 	/* Param: NDKBtTtyData* */
};

typedef struct {
	UINT8		*buf;
	int		len;
} NDKBtTtyData;

typedef struct NDKBtTtyDev{}* NDKBtTtyDev;

/* \brief Create TTY device
 * \return 0: succeed; <0: failed.
 */
int  ndk_bt_ttydev_open(
	NDKBtTtyDev	*dev,
	int		chan,
	void		(*msgcb)(NDKBtTtyDev dev, void *user_data, int msg, void *param),
	void		*user_data);

void ndk_bt_ttydev_close(
	NDKBtTtyDev	dev);

int  ndk_bt_ttydev_write(
	NDKBtTtyDev	dev,
	void		*buf,
	unsigned int	count);

/******************************************************************************
 *   PAN & BNEP
 *****************************************************************************/
enum {
	NDK_BT_PAN_CFG_CONNECT_CB,    /* void connect_cb(const char *remote_bdaddr) */
	NDK_BT_PAN_CFG_DISCONNECT_CB, /* void disconnect_cb(const char *remote_bdaddr) */
	NDK_BT_PAN_CFG_USE_SDP,
	NDK_BT_PAN_CFG_ROLE,	      /* "PANU", "GN", "NAP" */
};

int  ndk_bt_pan_config(int id, void *value);
int  ndk_bt_pan_listen_start();
void ndk_bt_pan_listen_stop();
int  ndk_bt_pan_connect_to(const char *dst);
void ndk_bt_pan_kill_all_connections();
void ndk_bt_pan_print_info(void);

/******************************************************************************
 *   GATT
 *****************************************************************************/
typedef UINT32 NDKBtGattServiceHandle;
typedef UINT32 NDKBtGattChrcHandle;
typedef void (*NDKBtGattChrcReadCb)(void *buf, unsigned int *len, void *user_data);
typedef void (*NDKBtGattChrcWriteCb)(const void *buf, unsigned int len, void *user_data);

#define NDK_BT_GATT_SERV_INV_HANDLE		0
#define NDK_BT_GATT_CHRC_INV_HANDLE		0

/* Arg ID for ndk_bt_gatt_service_create */
enum {
	NDK_BT_ARG_GATT_SERV_UUID128		= 1, /* uuid128 string. Mandatory. */
	NDK_BT_ARG_GATT_SERV_CHRC_NUM		= 2, /* unsigned int. Mandatory. */
	NDK_BT_ARG_GATT_SERV_DESCRIPTION	= 3, /* const char*. optional.  */
};

NDKBtGattServiceHandle ndk_bt_gatt_service_create(
	unsigned int		arg_id,		/* see NDK_BT_ARG_GATT_SERV_XXX */
	...);

int  ndk_bt_gatt_service_active(NDKBtGattServiceHandle handle);
void ndk_bt_gatt_service_destroy(NDKBtGattServiceHandle handle);

/* Arg ID for ndk_bt_gatt_chrc_add */
enum {
	NDK_BT_ARG_GATT_CHRC_UUID128		= 1, /* uuid128 string. Mandatory. */
	NDK_BT_ARG_GATT_CHRC_PROPERTIES		= 2, /* unsigned int. Read/write properties. Mandatory. */
	NDK_BT_ARG_GATT_CHRC_DATA_SIZE		= 3, /* unsigned int. Max size of GATT data. Mandatory. */
	NDK_BT_ARG_GATT_CHRC_READ_CB		= 4, /* NDKBtGattChrcReadCb, Read callback function. optional. */
	NDK_BT_ARG_GATT_CHRC_WRITE_CB		= 5, /* NDKBtGattChrcWriteCb, Write callback function. optional. */
	NDK_BT_ARG_GATT_CHRC_USER_DATA		= 6, /* void*. User data for read/write callback function. optional. */
};

enum {
	NDK_BT_GATT_CHRC_PROP_READ		= 1 << 0,
	NDK_BT_GATT_CHRC_PROP_WRITE		= 1 << 1,
	NDK_BT_GATT_CHRC_PROP_INDICATE		= 1 << 2,
};

NDKBtGattChrcHandle
ndk_bt_gatt_chrc_add (
	NDKBtGattServiceHandle	serv_handle,
	unsigned int		arg_id,		/* see NDK_BT_ARG_GATT_CHRC_XXX */
	...);

/* Send indication to GATT client. */
int ndk_bt_gatt_chrc_indicate (
	NDKBtGattServiceHandle	serv_handle,
	NDKBtGattChrcHandle	chrc_handle,
	const void		*data,
	unsigned int		length);

/******************************************************************************
 *   Bluetooth IOCTLs
 *****************************************************************************/
/* Bluetooth IOCTL Codes */
enum {
	/* GET */
	/* v-args: (char addr[18]). */
	NDK_BT_IOCG_BDADDR			= 0x0000,

	/* SET */
	/* vargs: (BOOL discoverable). */
	NDK_BT_IOCS_DISCOVERABLE		= 0x1000,
	/* vargs: (unsigned int timeout). */
	NDK_BT_IOCS_DISCOVERABLE_TIMEOUT	= 0x1001,
	/* vargs: (unsigned int devclass) */
	NDK_BT_IOCS_CLASS_OF_DEVICE		= 0x1002,
	/* vargs: (const char *name) */
	NDK_BT_IOCS_LOCAL_NAME			= 0x1003,
};

int ndk_bt_ioctl(unsigned int code, ...);

/******************************************************************************
 *   Miscellaneous Functions
 *****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

