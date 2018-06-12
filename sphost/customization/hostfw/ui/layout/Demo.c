#include "app_lyt_structure.h"

lyt_res_sfn_t Demo_iconResTab[3] = {
    {
        (UINT8 *)"A:\\RO_RES\\UI\\ICON\\ICON56.SFN",
        {0,1,2,3,4,5,6,7,8,9,10,11,12,13,0,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
    },
    {
        (UINT8 *)"A:\\RO_RES\\UI\\ICON\\MUBODY.SFN",
        {0,1,2,3,4,5,6,7,8,9,10,11,12,13,0,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
    },
    {
        (UINT8 *)"A:\\RO_RES\\UI\\ICON\\ICONTK.SFN",
        {0,1,2,3,4,5,6,7,8,9,10,11,12,13,0,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
    },
};

lyt_res_str_t Demo_strResTab[1] = {
    {
        .sfnRes = {
            (UINT8 *)"A:\\RO_RES\\UI\\FONT\\FONTENG.SFN",
            {0,13,1,0,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
        },
        .pSstFilename = (UINT8 *)"A:\\RO_RES\\UI\\SST\\STR5KTK.SST",
    },
};
lyt_item_t i_Demo_ICON56_0x00B7 = {
    .roi = {
        .xStart = 73,
        .yStart = 9,
        .width = 56,
        .height = 40,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x00B7,
    .pItem = (void *)&Demo_iconResTab[0],
};

lyt_item_t i_Demo_ICON56_0x00B4 = {
    .roi = {
        .xStart = 17,
        .yStart = 9,
        .width = 56,
        .height = 40,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x00B4,
    .pItem = (void *)&Demo_iconResTab[0],
};

lyt_item_t i_Demo_MUBODY_0x0000 = {
    .roi = {
        .xStart = 17,
        .yStart = 45,
        .width = 292,
        .height = 173,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x0000,
    .pItem = (void *)&Demo_iconResTab[1],
};

lyt_item_t i_Demo_ICONTK_0x00D3 = {
    .roi = {
        .xStart = 205,
        .yStart = 2,
        .width = 36,
        .height = 32,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x00D3,
    .pItem = (void *)&Demo_iconResTab[2],
};

lyt_item_t i_Demo_STR5KTK_0x0013 = {
    .roi = {
        .xStart = 12,
        .yStart = 2,
        .width = 92,
        .height = 28,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_STR,
    .id = 0x0013,
    .pItem = (void *)&Demo_strResTab[0],
};

lyt_item_t i_Demo_ICONTK_0x002C = {
    .roi = {
        .xStart = 205,
        .yStart = 2,
        .width = 36,
        .height = 32,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x002C,
    .pItem = (void *)&Demo_iconResTab[2],
};

lyt_item_t i_Demo_STR5KTK_0x0020 = {
    .roi = {
        .xStart = 12,
        .yStart = 2,
        .width = 62,
        .height = 28,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_STR,
    .id = 0x0020,
    .pItem = (void *)&Demo_strResTab[0],
};

lyt_item_t i_Demo_ICONTK_0x00BE = {
    .roi = {
        .xStart = 205,
        .yStart = 2,
        .width = 36,
        .height = 32,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x00BE,
    .pItem = (void *)&Demo_iconResTab[2],
};

lyt_item_t i_Demo_STR5KTK_0x0024 = {
    .roi = {
        .xStart = 12,
        .yStart = 2,
        .width = 76,
        .height = 28,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_STR,
    .id = 0x0024,
    .pItem = (void *)&Demo_strResTab[0],
};

lyt_item_t i_Demo_ICONTK_0x00E6 = {
    .roi = {
        .xStart = 205,
        .yStart = 2,
        .width = 36,
        .height = 32,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x00E6,
    .pItem = (void *)&Demo_iconResTab[2],
};

lyt_item_t i_Demo_STR5KTK_0x0028 = {
    .roi = {
        .xStart = 12,
        .yStart = 2,
        .width = 128,
        .height = 28,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_STR,
    .id = 0x0028,
    .pItem = (void *)&Demo_strResTab[0],
};

lyt_item_t i_Demo_ICONTK_0x00B6 = {
    .roi = {
        .xStart = 205,
        .yStart = 2,
        .width = 36,
        .height = 32,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x00B6,
    .pItem = (void *)&Demo_iconResTab[2],
};

lyt_item_t i_Demo_STR5KTK_0x0033 = {
    .roi = {
        .xStart = 12,
        .yStart = 2,
        .width = 33,
        .height = 28,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_STR,
    .id = 0x0033,
    .pItem = (void *)&Demo_strResTab[0],
};

lyt_item_t i_Demo_ICONTK_0x0071 = {
    .roi = {
        .xStart = 205,
        .yStart = 2,
        .width = 36,
        .height = 32,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x0071,
    .pItem = (void *)&Demo_iconResTab[2],
};

lyt_item_t i_Demo_STR5KTK_0x0050 = {
    .roi = {
        .xStart = 12,
        .yStart = 2,
        .width = 100,
        .height = 28,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_STR,
    .id = 0x0050,
    .pItem = (void *)&Demo_strResTab[0],
};

lyt_item_t i_Demo_ICONTK_0x0071_1 = {
    .roi = {
        .xStart = 205,
        .yStart = 2,
        .width = 36,
        .height = 32,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_ICON,
    .id = 0x0071,
    .pItem = (void *)&Demo_iconResTab[2],
};

lyt_item_t i_Demo_STR5KTK_0x0055 = {
    .roi = {
        .xStart = 12,
        .yStart = 2,
        .width = 40,
        .height = 28,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_STR,
    .id = 0x0055,
    .pItem = (void *)&Demo_strResTab[0],
};

lyt_item_t * Demo_8_itemTable[] = {
    &i_Demo_ICON56_0x00B7,
    &i_Demo_ICON56_0x00B4,
    &i_Demo_MUBODY_0x0000,
};

lyt_layer_t Demo_layer_8 = {
    .roi = {
        .xStart = 0,
        .yStart = 0,
        .width = 320, 
        .height = 240,
    },
    .fmt = 0,
    .pageId = 8,
    .isWithAlpha = 0,
    .alpha = 128,
    .itemTotal = 3,
    .ppItem = Demo_8_itemTable,
};


lyt_item_t * Demo_menuList[] = {
    &i_Demo_ICONTK_0x00D3,
    &i_Demo_STR5KTK_0x0013,
    &i_Demo_ICONTK_0x002C,
    &i_Demo_STR5KTK_0x0020,
    &i_Demo_ICONTK_0x00BE,
    &i_Demo_STR5KTK_0x0024,
    &i_Demo_ICONTK_0x00E6,
    &i_Demo_STR5KTK_0x0028,
    &i_Demo_ICONTK_0x00B6,
    &i_Demo_STR5KTK_0x0033,
    &i_Demo_ICONTK_0x0071,
    &i_Demo_STR5KTK_0x0050,
    &i_Demo_ICONTK_0x0071_1,
    &i_Demo_STR5KTK_0x0055,
};

lyt_res_menu_t Demo_menuRes = {
    .menuItemTotal = 5,
    .atomsPerItem = 2,
    .ppMenuList = Demo_menuList,
};

lyt_item_t v_Demo_menuItem_0 = {
    .roi = {
        .xStart = 33,
        .yStart = 56,
        .width = 260,
        .height = 30,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_MENU,
    .id = 0,
    .pItem = (void *)&Demo_menuRes,
};

lyt_item_t v_Demo_menuItem_1 = {
    .roi = {
        .xStart = 33,
        .yStart = 86,
        .width = 260,
        .height = 30,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_MENU,
    .id = 1,
    .pItem = (void *)&Demo_menuRes,
};

lyt_item_t v_Demo_menuItem_2 = {
    .roi = {
        .xStart = 33,
        .yStart = 116,
        .width = 260,
        .height = 30,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_MENU,
    .id = 2,
    .pItem = (void *)&Demo_menuRes,
};

lyt_item_t v_Demo_menuItem_3 = {
    .roi = {
        .xStart = 33,
        .yStart = 146,
        .width = 260,
        .height = 30,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_MENU,
    .id = 3,
    .pItem = (void *)&Demo_menuRes,
};

lyt_item_t v_Demo_menuItem_4 = {
    .roi = {
        .xStart = 33,
        .yStart = 178,
        .width = 260,
        .height = 30,
    },
    .scaleX = 1000,
    .scaleY = 1000,
    .showMode = 1,
    .itemType = LYT_ITEM_TYPE_MENU,
    .id = 4,
    .pItem = (void *)&Demo_menuRes,
};

lyt_item_t * Demo_Menu_ItemTable[] = {
&v_Demo_menuItem_0,
&v_Demo_menuItem_1,
&v_Demo_menuItem_2,
&v_Demo_menuItem_3,
&v_Demo_menuItem_4,
};

lyt_layer_t Demo_layer_menu = {
    .roi = {
        .xStart = 0,
        .yStart = 0,
        .width = 320, 
        .height = 240,
    },
    .fmt = 0,
    .pageId = 8,
    .isWithAlpha = 0,
    .alpha = 0,
    .itemTotal = 5,
    .ppItem = Demo_Menu_ItemTable,
};

lyt_layer_t *  Demo_layer_tab[] = {
    &Demo_layer_8,
    &Demo_layer_menu,
};

lyt_layout_t Demo_layout = { 
    .roi = {
        .xStart = 0,
        .yStart = 0,
        .width = 320,
        .height = 240,
    },
    .layerTotal = 2,
    .ppLayer = Demo_layer_tab,
};


lyt_layout_t * Demo_layoutGet()
{
     return &Demo_layout;
}

