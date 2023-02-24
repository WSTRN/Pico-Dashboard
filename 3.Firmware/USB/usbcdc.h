#pragma once
#include "tusb.h"

void usb_init();
void cdc_task();
static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count);

