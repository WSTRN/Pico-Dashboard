#include "main.h"
#include "gc9a01a.h"
#include "hardware/regs/intctrl.h"
#include "pico/time.h"
#include "pico/types.h"
#include "waveshare_rp2040_lcd_1.28.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"


uint dma_tx;
dma_channel_config c;
extern lv_disp_drv_t disp_drv;


void lcd_gpio_init()
{
	// GPIO Config
	gpio_init(WAVESHARE_RP2040_LCD_RST_PIN);
	gpio_init(WAVESHARE_RP2040_LCD_DC_PIN);
	gpio_init(WAVESHARE_RP2040_LCD_CS_PIN);
	/*gpio_init(WAVESHARE_RP2040_LCD_BL_PIN);*/
	gpio_set_dir(WAVESHARE_RP2040_LCD_RST_PIN, GPIO_OUT);
	gpio_set_dir(WAVESHARE_RP2040_LCD_DC_PIN, GPIO_OUT);
	gpio_set_dir(WAVESHARE_RP2040_LCD_CS_PIN, GPIO_OUT);
	/*gpio_set_dir(WAVESHARE_RP2040_LCD_BL_PIN, GPIO_OUT);*/
	gpio_set_function(WAVESHARE_RP2040_LCD_BL_PIN, GPIO_FUNC_PWM);
	uint bl_slice_num = pwm_gpio_to_slice_num(WAVESHARE_RP2040_LCD_BL_PIN);
	pwm_config bl_config = pwm_get_default_config();
	pwm_config_set_clkdiv(&bl_config, 2);
	pwm_init(bl_slice_num, &bl_config, true);
	pwm_set_gpio_level(WAVESHARE_RP2040_LCD_BL_PIN, 0);//square the value to make brightness appear more linear
	
	gpio_put(WAVESHARE_RP2040_LCD_DC_PIN, 0);
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 1);
    gpio_put(WAVESHARE_RP2040_LCD_RST_PIN, 1);
	/*gpio_put(WAVESHARE_RP2040_LCD_BL_PIN, 1);*/
}

void lcd_set_brightness(uint8_t b)
{
	pwm_set_gpio_level(WAVESHARE_RP2040_LCD_BL_PIN, b*b);
}

void lcd_reset()
{
    gpio_put(WAVESHARE_RP2040_LCD_RST_PIN, 1);
    sleep_ms(100);
    gpio_put(WAVESHARE_RP2040_LCD_RST_PIN, 0);
    sleep_ms(100);
    gpio_put(WAVESHARE_RP2040_LCD_RST_PIN, 1);
}

void lcd_cmd(uint8_t reg){
	gpio_put(WAVESHARE_RP2040_LCD_DC_PIN, 0);
	spi_write_blocking(spi1, &reg, 1);
}

void lcd_data(uint8_t data){
	gpio_put(WAVESHARE_RP2040_LCD_DC_PIN, 1);
	spi_write_blocking(spi1, &data, 1);
}

void lcd_Ndata(uint8_t* data, unsigned int N){
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 0);
	gpio_put(WAVESHARE_RP2040_LCD_DC_PIN, 1);
	spi_write_blocking(spi1, data, N);
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 1);
}

void lcd_reg_init()
{
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 0);
    sleep_ms(5);
	//set RGB
	lcd_cmd(0x36);
	lcd_data(0x08);
	//
	lcd_cmd(0xEF);
	lcd_cmd(0xEB);
	lcd_data(0x14);

  	lcd_cmd(0xFE);
	lcd_cmd(0xEF);

	lcd_cmd(0xEB);
	lcd_data(0x14);

	lcd_cmd(0x84);
	lcd_data(0x40);

	lcd_cmd(0x85);
	lcd_data(0xFF);

	lcd_cmd(0x86);
	lcd_data(0xFF);

	lcd_cmd(0x87);
	lcd_data(0xFF);

	lcd_cmd(0x88);
	lcd_data(0x0A);

	lcd_cmd(0x89);
	lcd_data(0x21);

	lcd_cmd(0x8A);
	lcd_data(0x00);

	lcd_cmd(0x8B);
	lcd_data(0x80);

	lcd_cmd(0x8C);
	lcd_data(0x01);

	lcd_cmd(0x8D);
	lcd_data(0x01);

	lcd_cmd(0x8E);
	lcd_data(0xFF);

	lcd_cmd(0x8F);
	lcd_data(0xFF);


	lcd_cmd(0xB6);
	lcd_data(0x00);
	lcd_data(0x20);

	lcd_cmd(0x36);
	lcd_data(0x08);//bgr

	lcd_cmd(0x3A);
	lcd_data(0x55);//16bits/pixel


	lcd_cmd(0x90);
	lcd_data(0x08);
	lcd_data(0x08);
	lcd_data(0x08);
	lcd_data(0x08);

	lcd_cmd(0xBD);
	lcd_data(0x06);

	lcd_cmd(0xBC);
	lcd_data(0x00);

	lcd_cmd(0xFF);
	lcd_data(0x60);
	lcd_data(0x01);
	lcd_data(0x04);

	lcd_cmd(0xC3);
	lcd_data(0x13);
	lcd_cmd(0xC4);
	lcd_data(0x13);

	lcd_cmd(0xC9);
	lcd_data(0x22);

	lcd_cmd(0xBE);
	lcd_data(0x11);

	lcd_cmd(0xE1);
	lcd_data(0x10);
	lcd_data(0x0E);

	lcd_cmd(0xDF);
	lcd_data(0x21);
	lcd_data(0x0c);
	lcd_data(0x02);

	lcd_cmd(0xF0);
	lcd_data(0x45);
	lcd_data(0x09);
	lcd_data(0x08);
	lcd_data(0x08);
	lcd_data(0x26);
 	lcd_data(0x2A);

 	lcd_cmd(0xF1);
 	lcd_data(0x43);
 	lcd_data(0x70);
 	lcd_data(0x72);
 	lcd_data(0x36);
 	lcd_data(0x37);
 	lcd_data(0x6F);


 	lcd_cmd(0xF2);
 	lcd_data(0x45);
 	lcd_data(0x09);
 	lcd_data(0x08);
 	lcd_data(0x08);
 	lcd_data(0x26);
 	lcd_data(0x2A);

 	lcd_cmd(0xF3);
 	lcd_data(0x43);
 	lcd_data(0x70);
 	lcd_data(0x72);
 	lcd_data(0x36);
 	lcd_data(0x37);
 	lcd_data(0x6F);

	lcd_cmd(0xED);
	lcd_data(0x1B);
	lcd_data(0x0B);

	lcd_cmd(0xAE);
	lcd_data(0x77);

	lcd_cmd(0xCD);
	lcd_data(0x63);


	lcd_cmd(0x70);
	lcd_data(0x07);
	lcd_data(0x07);
	lcd_data(0x04);
	lcd_data(0x0E);
	lcd_data(0x0F);
	lcd_data(0x09);
	lcd_data(0x07);
	lcd_data(0x08);
	lcd_data(0x03);

	lcd_cmd(0xE8);
	lcd_data(0x34);

	lcd_cmd(0x62);
	lcd_data(0x18);
	lcd_data(0x0D);
	lcd_data(0x71);
	lcd_data(0xED);
	lcd_data(0x70);
	lcd_data(0x70);
	lcd_data(0x18);
	lcd_data(0x0F);
	lcd_data(0x71);
	lcd_data(0xEF);
	lcd_data(0x70);
	lcd_data(0x70);

	lcd_cmd(0x63);
	lcd_data(0x18);
	lcd_data(0x11);
	lcd_data(0x71);
	lcd_data(0xF1);
	lcd_data(0x70);
	lcd_data(0x70);
	lcd_data(0x18);
	lcd_data(0x13);
	lcd_data(0x71);
	lcd_data(0xF3);
	lcd_data(0x70);
	lcd_data(0x70);

	lcd_cmd(0x64);
	lcd_data(0x28);
	lcd_data(0x29);
	lcd_data(0xF1);
	lcd_data(0x01);
	lcd_data(0xF1);
	lcd_data(0x00);
	lcd_data(0x07);

	lcd_cmd(0x66);
	lcd_data(0x3C);
	lcd_data(0x00);
	lcd_data(0xCD);
	lcd_data(0x67);
	lcd_data(0x45);
	lcd_data(0x45);
	lcd_data(0x10);
	lcd_data(0x00);
	lcd_data(0x00);
	lcd_data(0x00);

	lcd_cmd(0x67);
	lcd_data(0x00);
	lcd_data(0x3C);
	lcd_data(0x00);
	lcd_data(0x00);
	lcd_data(0x00);
	lcd_data(0x01);
	lcd_data(0x54);
	lcd_data(0x10);
	lcd_data(0x32);
	lcd_data(0x98);

	lcd_cmd(0x74);
	lcd_data(0x10);
	lcd_data(0x85);
	lcd_data(0x80);
	lcd_data(0x00);
	lcd_data(0x00);
	lcd_data(0x4E);
	lcd_data(0x00);

  	lcd_cmd(0x98);
	lcd_data(0x3e);
	lcd_data(0x07);

	lcd_cmd(0x35);
	lcd_cmd(0x21);

	lcd_cmd(0x11);
	sleep_ms(120);
	lcd_cmd(0x29);
	sleep_ms(20);
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 1);
}

void lcd_setwin(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye)
{
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 0);
  	//set the X coordinates
    lcd_cmd(0x2A);
    lcd_data(0x00);
    lcd_data(xs);
	lcd_data(0x00);
    lcd_data(xe);

	//set the Y coordinates
    lcd_cmd(0x2B);
    lcd_data(0x00);
	lcd_data(ys);
	lcd_data(0x00);
    lcd_data(ye);

    lcd_cmd(0X2C);
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 1);
}

void lcd_clr()
{
    uint16_t p=0x0000;
    lcd_setwin(0, 0, 239, 239);
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 0);
    gpio_put(WAVESHARE_RP2040_LCD_DC_PIN, 1);
    for(uint8_t j = 0; j < 240; j++){
		for (uint8_t i = 0; i<240; i++) {
			spi_write_blocking(spi1, (uint8_t*)&p, 2);
		}
        /*spi_write_blocking(SPI_PORT, (uint8_t *)&p[j*LCD_W], LCD_W*2);*/
    }
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 1);
}

void lcd_Ndatadma(uint8_t* data, unsigned int N)
{
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 0);
	gpio_put(WAVESHARE_RP2040_LCD_DC_PIN, 1);
    dma_channel_set_read_addr(dma_tx, data, false);
	dma_channel_set_trans_count(dma_tx, N, true);
}

void dma_handler()
{
	// Clear the interrupt request.
    dma_hw->ints1 = 1u << dma_tx;
	gpio_put(WAVESHARE_RP2040_LCD_CS_PIN, 1);
    lv_disp_flush_ready(&disp_drv);
}

void lcd_init()
{
	lcd_gpio_init();
	spi_init(spi1, 60000000);
	gpio_set_function(WAVESHARE_RP2040_LCD_TX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(WAVESHARE_RP2040_LCD_SCLK_PIN, GPIO_FUNC_SPI);
	//spi_dma
	dma_tx = dma_claim_unused_channel(true);
	c = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
	channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, spi_get_dreq(spi1, true));
    dma_channel_configure(dma_tx, &c,
                          &spi_get_hw(spi1)->dr, // write address
                          NULL, // read address
                          0, // element count (each element is of size transfer_data_size)
                          false); // don't start yet
	dma_channel_set_irq1_enabled(dma_tx, true);
	// Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_1, dma_handler);
	irq_set_priority(DMA_IRQ_1, PICO_DEFAULT_IRQ_PRIORITY-10);
    irq_set_enabled(DMA_IRQ_1, true);
	dma_hw->ints1 = 1u << dma_tx;

	lcd_reset();
	lcd_reg_init();
	/*lcd_clr();*/
}


