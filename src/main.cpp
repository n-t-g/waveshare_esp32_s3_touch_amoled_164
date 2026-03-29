#include <Arduino.h>
#include "FT3168.h"
#include "lcd_bsp.h"

void setup() {
	Serial.begin(115200);
	delay(500);
	Serial.println("=== ESP32-S3-Touch-AMOLED-1.64 ===");

	Touch_Init();
	screen_init();

	Serial.println("Ready. Touch the screen.");
}

void loop() {
	static bool screen_green = false;
	uint16_t x, y;

	if (getTouch(&x, &y)) {
		Serial.printf("touch x=%u y=%u\n", x, y);
		if (!screen_green) {
			screen_fill_color(0x07E0); // green
			screen_green = true;
		}
	} else {
		if (screen_green) {
			screen_fill_color(0xF800); // red
			screen_green = false;
		}
	}

	delay(50);
}

