#include "ascii_processor.h"
#include "filesystem.h"
#include "lfs.h"
#include "pico/bootrom.h"
#include "pico/time.h"
#include "string.h"
#include "tusb.h"
#include <stdint.h>
#include "gc9a01a.h"

#define MAX_LINE_LENGTH 40

extern lfs_size_t boot_count;
uint8_t bufs[10];
void OnUsbAsciiCmd(const char *_cmd, size_t _len)
{
    /*---------------------------- ↓ Add Your CMDs Here ↓ -----------------------------*/
    if (_cmd[0] == 'R')
    {
        reset_usb_boot(0, 0);
    }
    else if (_cmd[0] == 'B')
    {
		itoa((int)boot_count, (char*)bufs, 10);
        tud_cdc_write(bufs, 10);
        tud_cdc_write_flush();
    }
    else if (_cmd[0] == 'T')
    {
        uint32_t ms = to_ms_since_boot(get_absolute_time());
		itoa(ms, (char*)bufs, 10);
        tud_cdc_write(bufs, 10);
        tud_cdc_write_flush();
    }
    else if (_cmd[0] == 'L')
	{
		lcd_set_brightness((uint8_t)100*(_cmd[1]-48)+10*(_cmd[2]-48)+(_cmd[3]-48));
	}
    /*else if (_cmd[0] == 'F')*/
    /*{*/
        /*lfs_t lfs;*/
        /*lfs_file_t file;*/
        /*lfs_mount(&lfs, &cfg);*/
        /*lfs_file_open(&lfs, &file, "testfile", LFS_O_RDWR | LFS_O_CREAT);*/
		/*if(_cmd[1] == 'W')*/
		/*{*/
            /*lfs_file_write(&lfs, &file, &_cmd[2], _len-2);*/
		/*}*/
		/*else if(_cmd[1] == 'R')*/
		/*{*/
			/*uint8_t buf[30];*/
            /*lfs_ssize_t len = lfs_file_read(&lfs, &file, buf, 30);*/
            /*tud_cdc_write(buf, len);*/
            /*tud_cdc_write_flush();*/
		/*}*/
		/*else if(_cmd[1] == 'S')*/
		/*{*/
			/*lfs_soff_t size = lfs_file_size(&lfs, &file);*/
			/*itoa(size, (char*)bufs, 10);*/
            /*tud_cdc_write(bufs, 10);*/
            /*tud_cdc_write_flush();*/
		/*}*/
        /*lfs_file_close(&lfs, &file);*/
        /*lfs_unmount(&lfs);*/
    /*}*/

    /*---------------------------- ↑ Add Your CMDs Here ↑ -----------------------------*/
}

void ASCII_protocol_process_line(const uint8_t *buffer, size_t len)
{
    // copy everything into a local buffer so we can insert null-termination
    char cmd[MAX_LINE_LENGTH + 1];
    if (len > MAX_LINE_LENGTH)
        len = MAX_LINE_LENGTH;
    memcpy(cmd, buffer, len);

    cmd[len] = 0; // null-terminate

    OnUsbAsciiCmd(cmd, len);
}

void ASCII_protocol_parse_stream(const uint8_t *buffer, size_t len)
{
    static uint8_t parse_buffer[MAX_LINE_LENGTH];
    static bool read_active = true;
    static uint32_t parse_buffer_idx = 0;

    while (len--)
    {
        // if the line becomes too long, reset buffer and wait for the next line
        if (parse_buffer_idx >= MAX_LINE_LENGTH)
        {
            read_active = false;
            parse_buffer_idx = 0;
        }

        // Fetch the next char
        uint8_t c = *(buffer++);
        bool is_end_of_line = (c == '\r' || c == '\n');
        if (is_end_of_line)
        {
            if (read_active)
                ASCII_protocol_process_line(parse_buffer, parse_buffer_idx);
            parse_buffer_idx = 0;
            read_active = true;
        }
        else
        {
            if (read_active)
            {
                parse_buffer[parse_buffer_idx++] = c;
            }
        }
    }
}
