#ifndef WS2812B_H
#define WS2812B_H

#include "espressif/esp_common.h"

struct rgb {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

void ws2812b_init(void);
void ws2812b_show(struct rgb* buffer, uint8_t num_leds);

#endif //WS2812B_H