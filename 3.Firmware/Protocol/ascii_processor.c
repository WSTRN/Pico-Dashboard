#include "ascii_processor.h"
#include "lfs.h"
#include "pico/bootrom.h"
#include "string.h"
#include "tusb.h"

#define MAX_LINE_LENGTH 40

extern lfs_size_t boot_count;
void OnUsbAsciiCmd(const char *_cmd, size_t _len)
{
    /*---------------------------- ↓ Add Your CMDs Here ↓ -----------------------------*/
    if (_cmd[0] == 'R')
    {
        reset_usb_boot(0, 0);
    }
    else if (_cmd[0] == 'B')
    {
        uint8_t bufs[20];
        int len = sprintf((char *)bufs, "boot_count: %d\n", (int)boot_count);
        tud_cdc_write(bufs, len);
        tud_cdc_write_flush();
    }

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
