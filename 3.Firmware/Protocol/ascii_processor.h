#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

void ASCII_protocol_parse_stream(const uint8_t *buffer, size_t len);
