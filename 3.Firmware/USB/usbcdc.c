#include "usbcdc.h"
#include "bsp/board.h"
#include "tusb.h"
#include "pico/bootrom.h"
#include "lfs.h"

void usb_init()
{
	board_init();
	tusb_init();
}

static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count)
{
  for(uint32_t i=0; i<count; i++)
  {
    tud_cdc_n_write_char(itf, buf[i]);
  }
  tud_cdc_n_write_flush(itf);
}
extern lfs_size_t boot_count;

void cdc_task(void)
{
    if ( tud_cdc_n_available(0) )
    {
      uint8_t buf[64];

      uint32_t count = tud_cdc_n_read(0, buf, sizeof(buf));
	uint8_t bufs[20];
	int len = sprintf((char*)bufs, "boot_count: %d\n", (int)boot_count);
	echo_serial_port(0, bufs, len);

      echo_serial_port(0, buf, count);
	  if(buf[0] == 'R')
	  	reset_usb_boot(0, 0);
    }
}
