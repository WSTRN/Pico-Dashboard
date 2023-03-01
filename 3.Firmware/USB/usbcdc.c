#include "usbcdc.h"

#include "bsp/board.h"
#include "class/cdc/cdc_device.h"
#include "lfs.h"
#include "pico/bootrom.h"
#include "tusb.h"
#include "protocol.h"

void usb_init()
{
    board_init();
    // init device stack on configured roothub port
    tud_init(BOARD_TUD_RHPORT);
}

static uint8_t usb_cdc_buf[150];
void cdc_task(void)
{
    if (tud_cdc_available())
    {
        uint32_t count = tud_cdc_read(usb_cdc_buf, sizeof(usb_cdc_buf));
		usb_rx_process_packet(usb_cdc_buf, count);
		usb_server_task();

		tud_cdc_write(usb_cdc_buf, count);
		tud_cdc_write_flush();
    }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void)itf;
    (void)rts;

    // TODO set some indicator
    if (dtr)
    {
        // Terminal connected
    }
    else
    {
        // Terminal disconnected
    }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{

    (void)itf;
}
