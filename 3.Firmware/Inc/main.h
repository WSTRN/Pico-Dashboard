#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#define BL_value 150
#define BL_max 255
#define BL_min 0

typedef struct HWInfo
{
	float CPU_useage;
	float CPU_temperature;
	float CPU_frequency;
	float CPU_core_voltage;
	float MEMORY_useage;
	float GPU_useage;
	float GPU_temperature;
}HWInfo;
