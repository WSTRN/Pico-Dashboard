/*#include <stdio.h>*/
#include "hardware/regs/intctrl.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "gc9a01a.h"
#include "lvgl.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include <src/widgets/lv_roller.h>
#include <stdbool.h>
#include <stdint.h>
#include "display_interface.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"

#define gpio 3
#define PLL_SYS_KHZ (133 * 1000)


static void timer_irq_set_1ms();
static void timer_irq_callback();
static lv_disp_t* disp;

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

/**
 * An infinite roller with the name of the months
 */
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
void core1_entry()
{
	uint8_t i;
	while(1)
	{
		lv_roller_set_selected(roller1, i++ , LV_ANIM_ON);
		if(i==12)
			i=0;
		sleep_ms(2000);
	}
}

int main()
{
	set_sys_clock_khz(PLL_SYS_KHZ, true);
	clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_put(gpio, 1);
	lcd_init();
	lv_init();
	disp = lv_port();
	lv_example_roller_1();

	timer_irq_set_1ms();
	
	multicore_launch_core1(core1_entry);

	while(1)
    {
		lv_timer_handler();
		sleep_ms(2);
    }
}

static void timer_irq_callback()
{
	// Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << 0);
    uint64_t target = timer_hw->timerawl + 1000;
    timer_hw->alarm[0] = (uint32_t) target;
	lv_tick_inc(1);
}

static void timer_irq_set_1ms()
{
	// Enable the interrupt for our alarm (the timer outputs 4 alarm irqs)
    hw_set_bits(&timer_hw->inte, 1u << 0);
    // Set irq handler for alarm irq
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_callback);
    // Enable the alarm irq
	irq_set_priority(TIMER_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY);
    irq_set_enabled(TIMER_IRQ_0, true);
    // Enable interrupt in block and at processor

    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper
    // bits
    uint64_t target = timer_hw->timerawl + 1000;

    // Write the lower 32 bits of the target time to the alarm which
    // will arm it
    timer_hw->alarm[0] = (uint32_t) target;
}


