#ifndef __MAC80211_NETDEVICE_H__
#define __MAC80211_NETDEVICE_H__

#include <linux/netdev_common.h>
#include <linux/skbuff.h>

#include <ndk/netdriver.h>
#include <ndk/net.h>
#include <ndk/netif.h>

#define ICAT_SIOCDEVPRIVATE         (SIOCDEVPRIVATE+2)

struct net_device_ops {
	int             (*ndo_init)(struct net_device *dev);
	void            (*ndo_uninit)(struct net_device *dev);
	int             (*ndo_open)(struct net_device *dev);
	int             (*ndo_stop)(struct net_device *dev);
	int             (*ndo_start_xmit) (struct sk_buff *skb, struct net_device *dev);
///	u16		(*ndo_select_queue)(struct net_device *dev,
///					    struct sk_buff *skb,
///					    void *accel_priv,
///					    select_queue_fallback_t fallback);
	int             (*ndo_do_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
	struct net_device_stats*
			(*ndo_get_stats)(struct net_device *dev);
	int             (*ndo_set_mac_address)(struct net_device *dev, void *addr);
	void            (*ndo_set_multicast_list)(struct net_device *dev);
};

struct net_device {
	NETDEVCORE_MEMBERS;
	unsigned char		perm_addr[MAX_ADDR_LEN];
	const struct net_device_ops * netdev_ops;
	const struct iw_handler_def *wireless_handlers;
	struct wireless_dev	*ieee80211_ptr;
	unsigned		mtu;
	unsigned int		flags;
	unsigned int		netdev_flags;
	unsigned short		hard_header_len;
	unsigned short		needed_headroom;
	unsigned short		needed_tailroom;
	unsigned short		netdev_type;	// replace type
	netdev_features_t	features;
///	spinlock_t		addr_list_lock;
	const struct ethtool_ops *ethtool_ops;
	void			(*destructor)(struct net_device *dev);

	enum { NETREG_UNINITIALIZED=0,
	       NETREG_REGISTERED,	/* completed register_netdevice */
///	       NETREG_UNREGISTERING,	/* called unregister_netdevice */
	       NETREG_UNREGISTERED,	/* completed unregister todo */
///	       NETREG_RELEASED,		/* called free_netdev */
///	       NETREG_DUMMY,		/* dummy device for NAPI poll */
	} reg_state:8;
};

struct net {
	struct sock		*genl_sock;
};

static inline int eth_mac_addr(struct net_device *dev, void *p) {
	struct sockaddr *addr = p;
	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
	return 0;
}

static inline void skb_set_queue_mapping(struct sk_buff *skb, u16 queue_mapping) {
	skb->queue_mapping = queue_mapping;
}

static inline u16 skb_get_queue_mapping(const struct sk_buff *skb) {
	return skb->queue_mapping;
}

static inline void skb_copy_queue_mapping(struct sk_buff *to, const struct sk_buff *from) {
	to->queue_mapping = from->queue_mapping;
}

extern struct net init_net;

/* net */
#define get_net_ns_by_pid(pid)			(&(init_net))
#define get_net_ns_by_fd(fd)			(&(init_net))

/* socket */
#define sock_release(s)				ndk_sock_release((PSockInfo)s)
#define sock_net(s)				(&(init_net))
#define net_eq(a, b)				1

#define htons(a)				platform_htons(a)
#define ntohs(a)				platform_ntohs(a)
#define htonl(a)				platform_htonl(a)
#define ntohl(a)				platform_ntohl(a)

#define netdev_info(dev, fmt...)		ndk_info(fmt)
#define net_info_ratelimited(fmt...)		ndk_info(fmt)
#define netdev_err(dev, fmt...)			ndk_error(fmt)

#define netdev_hw_addr_list_count(l) ((l)->count)
#define netdev_hw_addr_list_empty(l) (netdev_hw_addr_list_count(l) == 0)
#define netdev_hw_addr_list_for_each(ha, l) \
	list_for_each_entry(ha, &(l)->list, list)

///static inline void netdev_set_default_ethtool_ops(struct net_device *dev,
///				    const struct ethtool_ops *ops)
///{
///	dev->ethtool_ops = ops;
///}

int	cfg80211_receive_skb(struct sk_buff *skb);
int	cfg80211_dev_close(struct net_device *dev);
void	cfg80211_ether_setup(struct net_device * dev);
struct net_device* cfg80211_alloc_netdev_mqs(int sizeof_priv, const char *name,
	unsigned char name_assign_type,
	void (*setup)(struct net_device *),
	unsigned int txqs, unsigned int rxqs);
int	cfg80211_dev_queue_xmit(struct sk_buff *skb);
int	cfg80211_register_netdevice(struct net_device *dev);
void	cfg80211_unregister_netdevice(struct net_device *dev);

#define ether_setup				cfg80211_ether_setup
#define dev_close(dev)				cfg80211_dev_close(dev)

#define alloc_netdev_mqs			cfg80211_alloc_netdev_mqs
#define alloc_netdev_mq(sizeof_priv, name, name_assign_type, setup, count) \
	alloc_netdev_mqs(sizeof_priv, name, name_assign_type, setup, count, \
			 count)

static inline struct net_device *alloc_etherdev_mqs(int sizeof_priv, unsigned int txqs, unsigned int rxqs) {
	return alloc_netdev_mqs(sizeof_priv, "eth%d", NET_NAME_UNKNOWN, ether_setup, txqs, rxqs);
}

#define alloc_etherdev_mq(sizeof_priv, count)	alloc_etherdev_mqs(sizeof_priv, count, count)
#define alloc_etherdev(sizeof_priv)		alloc_etherdev_mq(sizeof_priv, 1)

#define dev_queue_xmit(skb)			cfg80211_dev_queue_xmit(skb)
#define dev_receive_skb(skb)			cfg80211_receive_skb(skb)
#define register_netdevice(dev)			cfg80211_register_netdevice(dev)
#define register_netdev(dev)			register_netdevice(dev)
#define unregister_netdevice(dev)		cfg80211_unregister_netdevice(dev)
#define unregister_netdev(dev)			unregister_netdevice(dev)

#define netif_device_attach(dev)		((void)0)
#define netif_device_detach(dev)		((void)0)

/* sk-buff */
#define skb_clone(skb, flags)			((struct sk_buff*)skbc_clone((pskbc_t)(skb), NULL))
#define netif_skb_clone(skb, flags)		((struct sk_buff*)skbc_clone((pskbc_t)(skb), __netdrv_alloc_skb))

#define skb_copy(skb, mask)			((struct sk_buff*)skbc_clone((pskbc_t)(skb), NULL))
#define netif_skb_copy(skb, mask)		((struct sk_buff*)skbc_clone((pskbc_t)(skb), __netdrv_alloc_skb))

#define skb_copy_expand(skb, nhr, ntr, mask)	((struct sk_buff*)skbc_copy_expand((const pskbc_t)(skb), nhr, ntr, __netdrv_alloc_skb))
#define netif_skb_copy_expand(skb, nhr, ntr, mask) \
	((struct sk_buff*)skbc_copy_expand((const pskbc_t)(skb), nhr, ntr, __netdrv_alloc_skb))

#define skb_copy_bits(skb, offset, to, len)	skbc_copy_bits((const pskbc_t)(skb), offset, to, len)

#define netdev_alloc_skb(dev, size)     	ndk_netdrv_dev_alloc_skb(dev, size)
#define dev_alloc_skb(len)			netdev_alloc_skb(NULL, len)
#define __dev_alloc_skb(sz, flags)		netdev_alloc_skb(NULL, sz)
#define alloc_skb(len, priority)		netdev_alloc_skb(NULL, len)

#define kfree_skb(skb)				__skbc_unref((pskbc_t)(skb))
#define dev_kfree_skb(skb)			kfree_skb(skb)
#define dev_kfree_skb_any(skb)			kfree_skb(skb)

/* skb */
#define skb_linearize(...)			(0)
#define skb_vlan_tag_present(skb)		(0)

static inline int pskb_expand_head(struct sk_buff *skb, int nhead, int ntail,
		     gfp_t gfp_mask)
{
	ndk_error("skb %p, head/tailroom %d/%d; expand %d/%d",
		skb,
		skb_headroom(skb), skb_tailroom(skb),
		nhead, ntail);
	NDK_ASSERT(0);
	return 0;
}

static inline int pskb_may_pull(struct sk_buff *skb, unsigned int len) {
	if (len <= skb->len)
		return 1;
	return 0;
}

#define netif_carrier_on(dev)			do{ndk_info("carrier on %s",(dev)->name);ndk_netdrv_netif_carrier_on(dev);}while(0)
#define netif_carrier_off(dev)			do{ndk_info("carrier off %s",(dev)->name);ndk_netdrv_netif_carrier_off(dev);}while(0)
#define netif_carrier_ok(dev)			ndk_netdrv_netif_carrier_ok(dev)
#define netif_queue_stopped(dev)		ndk_netdrv_netif_queue_stopped(dev)
#define __netif_subqueue_stopped(dev, qidx)	ndk_netdrv_netif_queue_stopped(dev)
#define netif_start_queue(dev)			ndk_netdrv_netif_start_queue(dev)
#define netif_stop_queue(dev)			ndk_netdrv_netif_stop_queue(dev)
#define netif_wake_queue(dev)			ndk_netdrv_netif_wake_queue(dev)
#define netif_tx_wake_all_queues(dev)		ndk_netdrv_netif_wake_queue(dev)
#define netif_tx_start_all_queues(dev)		ndk_netdrv_netif_start_queue(dev)
#define netif_tx_stop_all_queues(dev)		ndk_netdrv_netif_stop_queue(dev)
#define netif_tx_disable(dev)			ndk_netdrv_netif_stop_queue(dev)
#define netif_start_subqueue(dev, b)		ndk_netdrv_netif_start_queue(dev)
#define netif_stop_subqueue(dev, b)		ndk_netdrv_netif_stop_queue(dev)
#define netif_wake_subqueue(dev, b)		ndk_netdrv_netif_start_queue(dev)
#define netif_running(dev)			ndk_netdrv_netif_running(dev)
#define netif_receive_skb(skb)			cfg80211_receive_skb(skb)
#define netif_rx(skb)				netif_receive_skb(skb)

#define dev_alloc_name(netdev, name)		ndk_netdrv_alloc_device_name(netdev, name)
static inline void free_netdev(struct net_device *dev) { ndk_netdrv_free_device(dev); }
#define netdev_priv(dev)			ndk_netdrv_netdev_priv(dev)
#define dev_get_by_index(net, ifindex)		ndk_netif_get_device_by_index(ifindex)
#define __dev_get_by_index(net, ifindex)	ndk_netif_get_device_by_index(ifindex)
#define SET_NETDEV_DEV(net, pdev)		((void)0)

const char *wext_sioc2str(int cmd);

#include <linux/wireless.h>

void cfg80211_wireless_send_event(struct net_device *	dev,
			 unsigned int		cmd,
			 union iwreq_data *	wrqu,
			 const char *		extra);
#define wireless_send_event(dev, cmd, wrqu, extra)	cfg80211_wireless_send_event(dev, cmd, wrqu, extra)

int cfg80211_wext_handle_ioctl(struct net_device *netdev, struct ifreq *ifr, unsigned int cmd, void *arg);
#define wext_handle_ioctl(dev, ifr, cmd, arg)		cfg80211_wext_handle_ioctl(dev, ifr, cmd, arg)

#endif /* __MAC80211_NETDEVICE_H__ */
