#include "protocol.h"
#include "ascii_processor.h"

typedef struct USBInterface
{
    uint8_t *rx_buf;
    uint32_t rx_len;
    bool data_pending;
}USBInterface;

static USBInterface CDC_interface = {
    .rx_buf = NULL,
    .rx_len = 0,
    .data_pending = false,
};

void usb_rx_process_packet(uint8_t *buf, uint32_t len)
{
    USBInterface *usb_iface;
	usb_iface = &CDC_interface;
    // We don't allow the next USB packet until the previous one has been processed completely.
    // Therefore it's safe to write to these vars directly since we know previous processing is complete.
    usb_iface->rx_buf = buf;
    usb_iface->rx_len = len;
    usb_iface->data_pending = true;
}

void usb_server_task()
{
	if (CDC_interface.data_pending)
    {
    	CDC_interface.data_pending = false;
		ASCII_protocol_parse_stream(CDC_interface.rx_buf, CDC_interface.rx_len);
	}
}
