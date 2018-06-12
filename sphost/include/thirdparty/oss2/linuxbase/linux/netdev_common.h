#ifndef __NETDEVICE_COMMON_H__
#define __NETDEVICE_COMMON_H__

#include <uapi/linux/netdevice.h>
#include <linux/netdev_features.h>
#include <linux/list.h>

/*
 * CONSTANTS
 */
enum netdev_tx {
	__NETDEV_TX_MIN  = INT_MIN,     /* make sure enum is signed */
	NETDEV_TX_OK     = 0x00,        /* driver took care of packet */
	NETDEV_TX_BUSY   = 0x10,        /* driver tx path was busy*/
	NETDEV_TX_LOCKED = 0x20,        /* driver tx lock was already taken */
};
typedef enum netdev_tx          netdev_tx_t;

#define CHECKSUM_NONE		0
#define CHECKSUM_UNNECESSARY	1
#define CHECKSUM_COMPLETE	2
#define CHECKSUM_PARTIAL	3

/* Packet types */
#define PACKET_HOST		0		/* To us		*/
#define PACKET_BROADCAST	1		/* To all		*/
#define PACKET_MULTICAST	2		/* To group		*/
#define PACKET_OTHERHOST	3		/* To someone else 	*/
#define PACKET_OUTGOING		4		/* Outgoing of any type */
#define PACKET_LOOPBACK		5		/* MC/BRD frame looped back */
#define PACKET_USER		6		/* To user space	*/
#define PACKET_KERNEL		7		/* To kernel space	*/
/* Unused, PACKET_FASTROUTE and PACKET_LOOPBACK are invisible to user space */
#define PACKET_FASTROUTE	6		/* Fastrouted frame	*/

#define MSG_PEEK       0x01
#define MSG_WAITALL    0x02
#define MSG_OOB        0x04
#define MSG_DONTWAIT   0x08
#define MSG_MORE       0x10


#define NETDEV_UP	0x0001	/* For now you can't veto a device up/down */
#define NETDEV_DOWN	0x0002
#define NETDEV_REBOOT	0x0003	/* Tell a protocol stack a network interface
				   detected a hardware crash and restarted
				   - we can use this eg to kick tcp sessions
				   once done */
#define NETDEV_CHANGE	0x0004	/* Notify device state change */
#define NETDEV_REGISTER 0x0005
#define NETDEV_UNREGISTER	0x0006
#define NETDEV_CHANGEMTU	0x0007 /* notify after mtu change happened */
#define NETDEV_CHANGEADDR	0x0008
#define NETDEV_GOING_DOWN	0x0009
#define NETDEV_CHANGENAME	0x000A
#define NETDEV_FEAT_CHANGE	0x000B
#define NETDEV_BONDING_FAILOVER 0x000C
#define NETDEV_PRE_UP		0x000D
#define NETDEV_PRE_TYPE_CHANGE	0x000E
#define NETDEV_POST_TYPE_CHANGE	0x000F
#define NETDEV_POST_INIT	0x0010
#define NETDEV_UNREGISTER_FINAL 0x0011
#define NETDEV_RELEASE		0x0012
#define NETDEV_NOTIFY_PEERS	0x0013
#define NETDEV_JOIN		0x0014
#define NETDEV_CHANGEUPPER	0x0015
#define NETDEV_RESEND_IGMP	0x0016
#define NETDEV_PRECHANGEMTU	0x0017 /* notify before mtu change happened */
#define NETDEV_CHANGEINFODATA	0x0018
#define NETDEV_BONDING_INFO	0x0019
#define NETDEV_PRECHANGEUPPER	0x001A

#define NET_RX_SUCCESS		0	/* keep 'em coming, baby */
#define NET_RX_DROP		1	/* packet dropped */

/*
 * STRUCTURES
 */

/* forward declaration */
struct sk_buff;
struct net_device;

struct net_device_stats {
	unsigned long   rx_packets;             /* total packets received       */
	unsigned long   tx_packets;             /* total packets transmitted    */
	unsigned long   rx_bytes;               /* total bytes received         */
	unsigned long   tx_bytes;               /* total bytes transmitted      */
	unsigned long   rx_errors;              /* bad packets received         */
	unsigned long   tx_errors;              /* packet transmit problems     */
	unsigned long   rx_dropped;             /* no space in linux buffers    */
	unsigned long   tx_dropped;             /* no space available in linux  */
	unsigned long   multicast;              /* multicast packets received   */
	unsigned long   collisions;

	/* detailed rx_errors: */
	unsigned long   rx_length_errors;
	unsigned long   rx_over_errors;         /* receiver ring buff overflow  */
	unsigned long   rx_crc_errors;          /* recved pkt with crc error    */
	unsigned long   rx_frame_errors;        /* recv'd frame alignment error */
	unsigned long   rx_fifo_errors;         /* recv'r fifo overrun          */
	unsigned long   rx_missed_errors;       /* receiver missed packet       */

	/* detailed tx_errors */
	unsigned long   tx_aborted_errors;
	unsigned long   tx_carrier_errors;
	unsigned long   tx_fifo_errors;
	unsigned long   tx_heartbeat_errors;
	unsigned long   tx_window_errors;

	/* for cslip etc */
	unsigned long   rx_compressed;
	unsigned long   tx_compressed;
};

struct dev_addr_list {
	struct dev_addr_list *next;
	u8              da_addr[MAX_ADDR_LEN];
	u8              da_addrlen;
	u8              da_synced;
	int             da_users;
	int             da_gusers;
};
#define dev_mc_list     dev_addr_list
#define dmi_addr        da_addr

struct rcu_head {
	int		dummy;
};

struct netdev_hw_addr {
	struct list_head	list;
	unsigned char		addr[MAX_ADDR_LEN];
	unsigned char		type;
#define NETDEV_HW_ADDR_T_LAN		1
#define NETDEV_HW_ADDR_T_SAN		2
#define NETDEV_HW_ADDR_T_SLAVE		3
#define NETDEV_HW_ADDR_T_UNICAST	4
#define NETDEV_HW_ADDR_T_MULTICAST	5
	bool			global_use;
	int			sync_cnt;
	int			refcount;
	int			synced;
///	struct rcu_head		rcu_head;
};

struct netdev_hw_addr_list {
	struct list_head	list;
	int			count;
};

struct napi_struct {
	int			dummy;
};

typedef u16 (*select_queue_fallback_t)(struct net_device *dev,
				       struct sk_buff *skb);

#endif

