#ifndef _APP_LYT_SHOW_H_
#define _APP_LYT_SHOW_H_

#include "common.h"

typedef enum _app_lytCfg_e{
	APP_LYT_LAYOUT_SEL,	/*Select param1 layout */
	APP_LYT_LAYER_SEL,	/*Select param1 layer */
	APP_LYT_LAYOUT_SHOW,/*param1 is 1 Show current layout that select before, 0 Clean current layout*/
	APP_LYT_LAYER_SHOW,	/*param1 is 1, show current layer of current layout. 0 clean.*/
	APP_LYT_ITEM_SHOW,	/*param1: item id. param2 1 show the item, 0 clean the item*/

	APP_LYT_ITEM_MOVE,	/*param1: item id. param2: x move(negative, move left; positive, move right)
										   param3: y move(negative, move up; positive, move down).*/
	APP_LYT_ITEM_CHANGE,/*param1: item id. Change the id only.*/
	APP_LYT_ITEM_ISSHOW,/*param1: item id. setting the item show or hidden. 1 to show, 0 to hidden*/
}app_lytCfg_e;

UINT32 app_lytCommand(
	app_lytCfg_e paramId,
	UINT32 param1,
	UINT32 param2,
	UINT32 param3,
	UINT32 param4
);

UINT32
app_lytUSBView(
	void
);

void app_LayoutSceneShow(UINT8 showScene);

#endif
