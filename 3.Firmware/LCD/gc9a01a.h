#pragma once
#include <stdint.h>

void lcd_init();
void lcd_setwin(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye);
void lcd_Ndata(uint8_t* data, unsigned int N);
void lcd_Ndatadma(uint8_t* data, unsigned int N);
