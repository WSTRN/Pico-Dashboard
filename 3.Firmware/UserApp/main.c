/*#include <stdio.h>*/
#include "device/usbd.h"
#include "hardware/regs/intctrl.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "gc9a01a.h"
#include "lvgl.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include <src/misc/lv_timer.h>
#include <src/widgets/lv_roller.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "display_interface.h"
#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "usbcdc.h"
#include "main_page.h"
#include "filesystem.h"

#define gpio 3
#define PLL_SYS_KHZ (133 * 1000)


static void timer_irq_set_1ms();
static void timer_irq_callback();
static lv_disp_t* disp;


void core1_entry()
{
	while(1)
	{
		tud_task();
		cdc_task();
	}
}

int main()
{
	set_sys_clock_khz(PLL_SYS_KHZ, true);
	clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, PLL_SYS_KHZ*1000, PLL_SYS_KHZ*1000);
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_put(gpio, 1);
	lcd_init();
	lv_init();
	disp = lv_port();
	usb_init();
	main_page_init();
	LittleFS_init();

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
    hw_clear_bits(&timer_hw->intr, 1u << 0);
    uint64_t target = timer_hw->timerawl + 1000;
    timer_hw->alarm[0] = (uint32_t) target;
	lv_tick_inc(1);
}

static void timer_irq_set_1ms()
{
    hw_set_bits(&timer_hw->inte, 1u << 0);
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_callback);
	irq_set_priority(TIMER_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY);
    irq_set_enabled(TIMER_IRQ_0, true);
    uint64_t target = timer_hw->timerawl + 1000;
    timer_hw->alarm[0] = (uint32_t) target;
}


