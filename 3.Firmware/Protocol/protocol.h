#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

void usb_rx_process_packet(uint8_t *buf, uint32_t len);
void usb_server_task();
