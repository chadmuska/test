#ifndef _NET_ETH_INIT_H_
#define _NET_ETH_INIT_H_

/***************************************************************************/
/*  Types                                                                                                                   */
/***************************************************************************/
/*phy connect mode*/
enum {
    MII_MODE = 0, /*mii mode*/
    RMII_MODE,    /*rmii mode*/
};

/*phy clock select*/
enum {
    PHY_INTERNAL_CLK     = 0,  /*internal clock*/
    PHY_EXTERNAL_CLK_25M = 25, /*external clock, 25MHz*/
    PHY_EXTERNAL_CLK_50M = 50, /*external clock, 50MHz*/
};

enum {
    ETH_TIMEO_ENABLE  = 1, 
    ETH_TIMEO_DISABLE = 0, /*default*/
};

/*thread name*/
enum {
    ETH_TX_HANDLER_THR = 0,
    ETH_TX_FRESKB_THR,
    ETH_RX_HANDLER_THR,
};

/****************************************************************************/
/*  Initialization Functions.                                                                                            */
/****************************************************************************/
/* \brief   the thread priority setting
 * \param name: thread of name, example : ETH_TX_HANDLER_THR
 * \param priority: thread of priority
 * \return 0: succeed. < 0: failed.
 */
extern int _net_ethdrv_thread_priority_set(UINT32 name, UINT32 priority);

/* \brief  get the thread priority
 * \param name: thread of name, example : ETH_TX_HANDLER_THR
 * \return  0: failed  other:thread priority
 */ 
extern int _net_ethdrv_thread_priority_get(UINT32 name);

/* \brief  enable or disable time out function.
 * \param en: 1:enable 0: disable
 * \return 0: succeed. < 0: failed.
 */
extern int  _net_ethdrv_timeo_set(UINT32 en);

/* \brief  get the link state.
 * \param link: link state
 * \return 0: succeed. < 0: failed.
 */
extern int _net_ethdrv_carrier_get(int *link);

/* \brief Initialize ethernet driver.
 * \param connect_mode: phy connect mode
 * \param clk_M: phy clock select
 * \return 0: succeed. < 0: failed.
 */
extern int _net_ethdrv_init(const UINT8 connetc_mode, const UINT32 clk_M);

/* \brief uninstall ethernet driver.
 * \return 0: succeed. < 0: failed.
 */
extern int _net_ethdrv_exit(void);

#define net_ethdrv_init_def()         _net_ethdrv_init(MII_MODE,PHY_INTERNAL_CLK)
#define net_ethdrv_init(a,b)          _net_ethdrv_init(a,b)
#define net_ethdrv_exit()             _net_ethdrv_exit()
#define net_ethdrv_timeo_set(en)      _net_ethdrv_timeo_set(en)
#define net_ethdrv_carrier_get(link)  _net_ethdrv_carrier_get(link)
#define net_ethdrv_thread_priority_get(name)            _net_ethdrv_thread_priority_get(name)
#define net_ethdrv_thread_priority_set(name, priority)  _net_ethdrv_thread_priority_set(name, priority)
#endif
