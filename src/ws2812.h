#pragma once

#include <stdint.h>
#include "ch55x.h"

// Defined by code generation
// #define WS2812_PIN P3_4
// #define WS2812_COUNT 10

void ws2812_set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void ws2812_show(void);
