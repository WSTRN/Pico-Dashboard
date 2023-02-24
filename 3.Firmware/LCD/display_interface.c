#include "display_interface.h"
#include "lvgl.h"
#include "lv_conf.h"
#include "gc9a01a.h"
#include "pico/types.h"
#include <stdint.h>

/*A static or global variable to store the buffers*/
static lv_disp_draw_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[240 * 20];
static lv_color_t buf_2[240 * 20];

lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/

void flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	lcd_setwin(area->x1, area->y1, area->x2, area->y2);
	lcd_Ndatadma((uint8_t*)color_p, (area->x2 - area->x1 + 1)*(area->y2 - area->y1 + 1)*2);
    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    /*lv_disp_flush_ready(disp_drv);*/
}

lv_disp_t* lv_port()
{
	/*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
	lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, 240 * 20);
	lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
	disp_drv.flush_cb = flush_cb;        /*Set a flush callback to draw to the display*/
	disp_drv.hor_res = 240;                 /*Set the horizontal resolution in pixels*/
	disp_drv.ver_res = 240;                 /*Set the vertical resolution in pixels*/

	lv_disp_t* disp;
	disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
	
	return disp;
}

