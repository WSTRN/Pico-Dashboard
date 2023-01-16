#include "lvgl.h"
#include "main_page.h"

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Selected month: %s\n", buf);
    }
}

lv_obj_t * roller1;
void lv_example_roller_1(void)
{
    roller1 = lv_roller_create(lv_scr_act());
    lv_roller_set_options(roller1,
                          "January\n"
                          "February\n"
                          "March\n"
                          "April\n"
                          "May\n"
                          "June\n"
                          "July\n"
                          "August\n"
                          "September\n"
                          "October\n"
                          "November\n"
                          "December",
                          LV_ROLLER_MODE_INFINITE);

    lv_roller_set_visible_row_count(roller1, 4);
    lv_obj_center(roller1);
    lv_obj_add_event_cb(roller1, event_handler, LV_EVENT_ALL, NULL);
}
void timer_2s(lv_timer_t * timer)
{
	static uint8_t i;
	lv_roller_set_selected(roller1, i++ , LV_ANIM_ON);
	if(i==12)
		i=0;
}

void main_page()
{
	lv_example_roller_1();
	lv_timer_create(timer_2s, 2000, NULL);
}
