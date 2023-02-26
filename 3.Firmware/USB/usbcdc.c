#include "usbcdc.h"

#include "bsp/board.h"
#include "class/cdc/cdc_device.h"
#include "lfs.h"
#include "pico/bootrom.h"
#include "tusb.h"

void usb_init()
{
    board_init();
    // init device stack on configured roothub port
    tud_init(BOARD_TUD_RHPORT);
}

extern lfs_size_t boot_count;
void cdc_task(void)
{
    if (tud_cdc_available())
    {
        uint8_t buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        uint8_t bufs[20];
        int len = sprintf((char *)bufs, "boot_count: %d\n", (int)boot_count);
        tud_cdc_write(bufs, len);
        tud_cdc_write_flush();

        tud_cdc_write(buf, count);
        tud_cdc_write_flush();
        if (buf[0] == 'R')
            reset_usb_boot(0, 0);
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
