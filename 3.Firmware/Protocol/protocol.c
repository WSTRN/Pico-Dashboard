#include "protocol.h"
#include "ascii_processor.h"
#include "pico/time.h"
#include "pico/util/queue.h"
#include "tusb.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct USBInterface
{
    uint8_t *rx_buf;
    uint32_t rx_len;
    bool is_xmodem;
    bool xmodem_initialized;
} USBInterface;

static USBInterface CDC_interface = {
    .rx_buf = NULL,
    .rx_len = 0,
    .is_xmodem = false,
    .xmodem_initialized = false,
};

queue_t xmodem_fifo;

void usb_rx_process_packet(uint8_t *buf, uint32_t len)
{
    USBInterface *usb_iface;
    usb_iface = &CDC_interface;
    // We don't allow the next USB packet until the previous one has been processed completely.
    // Therefore it's safe to write to these vars directly since we know previous processing is complete.
    usb_iface->rx_buf = buf;
    usb_iface->rx_len = len;
    if (usb_iface->is_xmodem)
    {
		uint8_t *xbuf = buf;
        for (uint32_t i = len; i != 0; i--)
        {
			queue_add_blocking(&xmodem_fifo, xbuf++);
        }
    }
}

void usb_server_task()
{
    if (CDC_interface.is_xmodem)
    {
        if (!CDC_interface.xmodem_initialized)
        {
            queue_init(&xmodem_fifo, 140, sizeof(uint8_t));
        }
    }
    else
    {
        ASCII_protocol_parse_stream(CDC_interface.rx_buf, CDC_interface.rx_len);
    }
}
void xmodem_transform_complete()
{
    CDC_interface.is_xmodem = false;
    CDC_interface.xmodem_initialized = false;
    queue_free(&xmodem_fifo);
}
