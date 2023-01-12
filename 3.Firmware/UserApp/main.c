#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define gpio 2

int main()
{
    // stdio_init_all();
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
    while(1)
    {
        gpio_put(gpio, 1);
        sleep_ms(300);
        gpio_put(gpio, 0);
        sleep_ms(300);
    }
}