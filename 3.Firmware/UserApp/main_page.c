#include "lvgl.h"
#include "main_page.h"
#include "src/core/lv_obj_pos.h"


void page(void)
{
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
	LV_IMG_DECLARE(logo_ROG);
    lv_obj_t * logo = lv_img_create(lv_scr_act());
	lv_img_set_src(logo, &logo_ROG);
	lv_img_set_zoom(logo, 330);
    lv_obj_align(logo, LV_ALIGN_CENTER, -3, 8);
}

void main_page_init()
{
	page();
}
