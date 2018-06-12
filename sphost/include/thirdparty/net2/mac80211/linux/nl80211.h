#ifndef __MAC80211_NL80211_H__
#define __MAC80211_NL80211_H__

#include <uapi/linux/nl80211.h>

/* Replace get phy name by reading from /sys/class/net/%s/phy80211/name */
BOOL cfg80211_get_phy_name(char phyname[16], const char *devname);

const char *nl80211_cmd2name(enum nl80211_commands cmd);
const char *nl80211_attr2name(enum nl80211_attrs attr);

#endif
