#ifndef _APP_LYT_H_
#define _APP_LYT_H_

#include "common.h"

#define COLOR_INDEX_MAX  32

typedef struct roi_s {
	SINT32 xStart;	/* ROI startx */
	SINT32 yStart;	/* ROI starty */
	UINT32 width;	/* ROI width */
	UINT32 height;	/* ROI height */
} roi_t;

typedef enum {
	LYT_ITEM_TYPE_ICON = 0,
	LYT_ITEM_TYPE_STR,
	LYT_ITEM_TYPE_MENU,
	LYT_ITEM_TYPE_MAX
}lyt_item_type_e;

typedef struct lyt_item_s{
	roi_t roi;
	UINT32 scaleX;
	UINT32 scaleY;	
	UINT8 showMode; /*0: hide, 1: show*/
	lyt_item_type_e itemType;
	UINT32 id; 
	void *pItem;
}lyt_item_t;

typedef struct res_sfn_s{
	UINT8 *sfnFile;
	UINT8 colorTbl[COLOR_INDEX_MAX]; 
}lyt_res_sfn_t;

typedef struct lyt_res_str_s{
	lyt_res_sfn_t sfnRes;
	UINT8 *pSstFilename;
}lyt_res_str_t;

typedef struct lyt_res_menu_s{
	UINT32 menuItemTotal;
	UINT8 atomsPerItem; 
	lyt_item_t **ppMenuList;
}lyt_res_menu_t;

typedef struct lyt_layer_s{
	roi_t roi;
	UINT32 fmt;
	UINT32 pageId;
	UINT8  isWithAlpha;
	UINT32 alpha;
	UINT32 itemTotal;
	lyt_item_t **ppItem;
}lyt_layer_t;

typedef struct lyt_layout_s{
	roi_t roi;
	UINT8 layerTotal;
	lyt_layer_t **ppLayer;
}lyt_layout_t;

#endif
