#ifndef _PM_CONFIG_H
#define _PM_CONFIG_H

/*-------------PM CONFIG---------------*/
/*with these macro on,PM can work on*/
#define PM_SWITCH (0) /* PM moudule spinlock has deadlock, so default disable PM */

#if PM_SWITCH
#define CONFIG_PM 
#define CONFIG_PM_RUNTIME 
#define CONFIG_USB_SUSPEND
#endif

#endif
