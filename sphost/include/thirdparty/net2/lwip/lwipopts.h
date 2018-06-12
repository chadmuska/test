#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#if NDK_DEBUG > 1
	#define LWIP_DEBUG
#else
	#define LWIP_NOASSERT
#endif

#define LWIP_HAVE_LOOPIF                    1
#define LWIP_DBG_MIN_LEVEL                  LWIP_DBG_LEVEL_WARNING
#define LWIP_COMPAT_SOCKETS                 0
#define LWIP_POSIX_SOCKETS_IO_NAMES         0

#define NOT_HAVE_SA_LEN

#define LWIP_DBG_OFF                  	    0
#define LWIP_DBG_TYPES_ON                   (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE)

#ifdef LWIP_DEBUG
	#define LWIP_DBG_ON                 (0x01<<0 )
	#define PBUF_DEBUG                  (0x01<<1 )
	#define API_LIB_DEBUG               (0x01<<2 )
	#define API_MSG_DEBUG               (0x01<<3 )
	#define TCPIP_DEBUG                 (0x01<<4 )
	#define NETIF_DEBUG                 (0x01<<5 )
	#define SOCKETS_DEBUG               (0x01<<6 )
	#define DEMO_DEBUG                  (0x01<<7 )
	#define IP_DEBUG                    (0x01<<8 )
	#define IP_REASS_DEBUG              (0x01<<9 )
	#define ICMP_DEBUG                  (0x01<<10)
	#define UDP_DEBUG                   (0x01<<11)
	#define TCP_DEBUG                   (0x01<<12)
	#define TCP_INPUT_DEBUG             (0x01<<13)
	#define TCP_OUTPUT_DEBUG            (0x01<<14)
	#define TCP_RTO_DEBUG               (0x01<<15)
	#define TCP_CWND_DEBUG              (0x01<<16)
	#define TCP_WND_DEBUG               (0x01<<17)
	#define TCP_FR_DEBUG                (0x01<<18)
	#define TCP_QLEN_DEBUG              (0x01<<19)
	#define TCP_RST_DEBUG               (0x01<<20)
	#define DHCP_DEBUG                  (0x01<<21)
	#define MEM_DEBUG                   (0x01<<22)
	#define MEMP_DEBUG                  (0x01<<23)
	#define PPP_DEBUG                   (0x01<<24)
	#define ETHARP_DEBUG                (0x01<<25)
	#define LWIP_DBG_TRACE              (0x01<<26)
	#define LWIP_DBG_STATE              (0x01<<27)

	#define LWIP_DBG_LEVEL_WARNING      (0x01<<29)/* bad checksums, dropped packets, ... */
	#define LWIP_DBG_LEVEL_SERIOUS      (0x01<<30)/* memory allocation failures, ... */
	#define LWIP_DBG_LEVEL_SEVERE       (0x01<<31)

	extern unsigned int lwip_dbg_flags;

	#define LWIPDBG(args...) do{ \
		printf("[%s:%d] ==>" , __FUNCTION__, __LINE__); \
		printf(" " args); \
		printf("\n"); \
	} while(0)
#else
	#define LWIP_DBG_ON                 0
	#define PBUF_DEBUG                  0
	#define API_LIB_DEBUG               0
	#define API_MSG_DEBUG               0
	#define TCPIP_DEBUG                 0
	#define NETIF_DEBUG                 0
	#define SOCKETS_DEBUG               0
	#define DEMO_DEBUG                  0
	#define IP_DEBUG                    0
	#define IP_REASS_DEBUG              0
	#define ICMP_DEBUG                  0
	#define UDP_DEBUG                   0
	#define TCP_DEBUG                   0
	#define TCP_INPUT_DEBUG             0
	#define TCP_OUTPUT_DEBUG            0
	#define TCP_RTO_DEBUG               0
	#define TCP_CWND_DEBUG              0
	#define TCP_WND_DEBUG               0
	#define TCP_FR_DEBUG                0
	#define TCP_QLEN_DEBUG              0
	#define TCP_RST_DEBUG               0
	#define DHCP_DEBUG                  0
	#define MEM_DEBUG                   0
	#define MEMP_DEBUG                  0
	#define PPP_DEBUG                   0
	#define LWIP_DBG_TRACE              0
	#define LWIP_DBG_STATE              0

	#define LWIP_DBG_LEVEL_WARNING      0
	#define LWIP_DBG_LEVEL_SERIOUS      0
	#define LWIP_DBG_LEVEL_SEVERE       0

	#define LWIPDBG(args...)
#endif

#define NO_SYS                              0
#define LWIP_COMPAT_MUTEX                   0
#define LWIP_CHECKSUM_ON_COPY               0
#define TCP_CHECKSUM_ON_COPY_SANITY_CHECK   0
#define LWIP_RAW                            1
#define MEM_USE_POOLS                       0

#if MEM_USE_POOLS
# warning mem pool is enabled.
	#define MEMP_USE_CUSTOM_POOLS           1
	#define MEM_LIBC_MALLOC                 0
	#define MEMP_MEM_MALLOC                 0

	#define MEM_USE_POOLS_TRY_BIGGER_POOL   0

	/*#define MEMP_NUM_RAW_PCB                32*/
	#define MEMP_NUM_PBUF                   48
	/*#define MEMP_NUM_UDP_PCB                16*/
	/*#define MEMP_NUM_TCP_PCB                15*/
	/*#define MEMP_NUM_TCP_PCB_LISTEN         24*/
	/*#define MEMP_NUM_TCP_SEG                48*/
	/*#define MEMP_NUM_REASSDATA              15*/
	/*#define MEMP_NUM_FRAG_PBUF              45*/
	#define MEMP_NUM_NETBUF                 8

	#define MEMP_NUM_NETCONN                16

	#define MEMP_NUM_TCPIP_MSG_API          8
	#define MEMP_NUM_TCPIP_MSG_INPKT        8

	#define MEMP_NUM_ARP_QUEUE              30
	#define MEMP_NUM_IGMP_GROUP             8

	#define MEMP_NUM_SNMP_ROOTNODE          30
	#define MEMP_NUM_SNMP_NODE              50
	#define MEMP_NUM_SNMP_VARBIND           2
	#define MEMP_NUM_SNMP_VALUE             3

	#define MEMP_NUM_NETDB                  1
	#define MEMP_NUM_LOCALHOSTLIST          1
	#define MEMP_NUM_PPPOE_INTERFACES       1
	#define PBUF_POOL_SIZE                  32

	#define MEMP_OVERFLOW_CHECK             2
	#define MEMP_SANITY_CHECK               1
#else
	#define MEMP_USE_CUSTOM_POOLS           0
	#define MEM_LIBC_MALLOC                 1
	#define MEMP_MEM_MALLOC                 1
#endif

#if !MEM_LIBC_MALLOC && MEMP_MEM_MALLOC
#define MEM_SIZE                                0x40000
#endif

#define MEMP_SANITY_CHECK                   1
#define MEMP_NUM_SYS_TIMEOUT                8
#define LWIP_NETIF_TX_SINGLE_PBUF           0

#define MEMP_NUM_TCP_PCB                    20
#define MEMP_NUM_UDP_PCB                    20
#define MEMP_NUM_RAW_PCB                    4
#define MEMP_NUM_TCP_PCB_LISTEN             4
#define MEMP_NUM_NETCONN                    (MEMP_NUM_TCP_PCB+MEMP_NUM_UDP_PCB+MEMP_NUM_RAW_PCB+MEMP_NUM_TCP_PCB_LISTEN)

/* Use errno from linux */
#undef  LWIP_PROVIDE_ERRNO

/*#define MEMP_NUM_SYS_TIMEOUT                8*/

/* ---------- Memory options ---------- */
#define MEM_ALIGNMENT                       4
#define ETH_PAD_SIZE                        0
#define SYS_LIGHTWEIGHT_PROT                1
#define LWIP_NETIF_API                      1
#define LWIP_DNS                            1

/* ---------- Socket options ---------- */
#define LWIP_SO_RCVBUF                      1
#define SO_REUSE                            1
#define SO_REUSE_RXTOALL                    1

/* ---------- TCP options ---------- */
#define LWIP_TCP                            1
#define LWIP_TCP_KEEPALIVE                  1
/*#define TCP_TTL                             255*/
#define TCP_LISTEN_BACKLOG                  1
#define TCP_QUEUE_OOSEQ                     1
#define TCP_MSS                             1460
#define TCP_MSS_BYTES                       TCP_MSS /* Alias */
#define TCP_RCVSCALE                        1

#if 0
#define TCP_WND                             (8*(TCP_MSS))
#define TCP_SND_BUF                         (24*(TCP_MSS))
#else
/*#define TCP_RCVSCALE                        2*/
#define TCP_CWND                            (40*(TCP_MSS))
#define TCP_WND                             (40*(TCP_MSS))
#define TCP_SND_BUF                         (40*(TCP_MSS))
#define TCP_SNDLOWAT                        (8*(TCP_MSS))
#endif

#define TCP_SND_QUEUELEN                    ((4*(TCP_SND_BUF) + ((TCP_MSS)-1))/(TCP_MSS))
#define TCP_SNDQUEUELOWAT                   16

/*#define TCP_MAXRTX                          12*/
/*#define TCP_SYNMAXRTX                       6*/
#define MEMP_NUM_TCP_SEG                    ((TCP_SND_QUEUELEN)*2)
#define TCP_MSL                             3000UL

/* ---------- ARP options ---------- */
#define ARP_TABLE_SIZE                      10
#define ARP_QUEUEING                        1

/* ---------- IP options ---------- */
#define IP_FORWARD                          0
#define IP_REASSEMBLY                       1
#define IP_FRAG                             1
#define IP_SOF_BROADCAST                    1
#define IP_SOF_BROADCAST_RECV               1

/* ---------- IGMP options ---------- */
#define LWIP_IGMP                           1
#define LWIP_RAND                           rand

/* ---------- ICMP options ---------- */
#define ICMP_TTL                            255

/* ---------- DHCP options ---------- */
#define LWIP_DHCP                           1
#define DHCP_DOES_ARP_CHECK                 0

/* ---------- UDP options ---------- */
#define LWIP_UDP                            1

/* ---------- Statistics options ---------- */
#define LWIP_STATS                          0

/* ---------- PPP options ---------- */
#define PPP_SUPPORT                         0   /* Set > 0 for PPP */

#if PPP_SUPPORT
	#define NUM_PPP                     1   /* Max PPP sessions. */
	#define PAP_SUPPORT                 1   /* Set > 0 for PAP. */
	#define CHAP_SUPPORT                1   /* Set > 0 for CHAP. */
	#define MSCHAP_SUPPORT              0   /* Set > 0 for MSCHAP (NOT FUNCTIONAL!) */
	#define CBCP_SUPPORT                0   /* Set > 0 for CBCP (NOT FUNCTIONAL!) */
	#define CCP_SUPPORT                 0   /* Set > 0 for CCP (NOT FUNCTIONAL!) */
	#define VJ_SUPPORT                  1   /* Set > 0 for VJ header compression. */
	#define MD5_SUPPORT                 1   /* Set > 0 for MD5 (see also CHAP) */

	/*
	 * Timeouts.
	 */
	#define FSM_DEFTIMEOUT              6   /* Timeout time in seconds */
	#define FSM_DEFMAXTERMREQS          2   /* Maximum Terminate-Request transmissions */
	#define FSM_DEFMAXCONFREQS          10  /* Maximum Configure-Request transmissions */
	#define FSM_DEFMAXNAKLOOPS          5   /* Maximum number of nak loops */

	#define UPAP_DEFTIMEOUT             6   /* Timeout (seconds) for retransmitting req */
	#define UPAP_DEFREQTIME             30  /* Time to wait for auth-req from peer */

	#define CHAP_DEFTIMEOUT             6   /* Timeout time in seconds */
	#define CHAP_DEFTRANSMITS           10  /* max # times to send challenge */

	/* Interval in seconds between keepalive echo requests, 0 to disable. */
	#if 1
		#define LCP_ECHOINTERVAL    0
	#else
		#define LCP_ECHOINTERVAL    10
	#endif

	/* Number of unanswered echo requests before failure. */
	#define LCP_MAXECHOFAILS            3

	/* Max Xmit idle time (in jiffies) before resend flag char. */
	#define PPP_MAXIDLEFLAG             100

	/*
	 * Packet sizes
	 *
	 * Note - lcp shouldn't be allowed to negotiate stuff outside these
	 *    limits.  See lcp.h in the pppd directory.
	 * (XXX - these constants should simply be shared by lcp.c instead
	 *    of living in lcp.h)
	 */
	#define PPP_MTU                     1500 /* Default MTU (size of Info field) */

	#if 0
		#define PPP_MAXMTU          65535 - (PPP_HDRLEN + PPP_FCSLEN)
	#else
		#define PPP_MAXMTU          1500 /* Largest MTU we allow */
	#endif

	#define PPP_MINMTU                  64
	#define PPP_MRU                     1500 /* default MRU = max length of info field */
	#define PPP_MAXMRU                  1500 /* Largest MRU we allow */
	#define PPP_DEFMRU                  296  /* Try for this */
	#define PPP_MINMRU                  128  /* No MRUs below this */
	#define MAXNAMELEN                  256  /* max length of hostname or name for auth */
	#define MAXSECRETLEN                256  /* max length of password or secret */
#endif /* PPP_SUPPORT */

/* ---------- Performance turning ---------- */
#ifdef ICAT_NDK_TUNING
# define LWIP_CHKSUM_ALGORITHM              3
# define LWIP_CHKSUM                        lwip_standard_chksum  /* ndk_chksum_compare ndk_chksum lwip_standard_chksum */

#ifdef SPCA6350
# define MEMCPY                             oss_memcpy
# define SMEMCPY                            oss_memcpy
#endif

# undef  MEMP_SANITY_CHECK
# define MEMP_SANITY_CHECK                  0

# undef  ETH_PAD_SIZE
# define ETH_PAD_SIZE                       2

#endif

#define LWIP_MBOX_SIZE		                128
#endif /* __LWIPOPTS_H__ */

