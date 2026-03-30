#include <Arduino.h>
#include "FT3168.h"
#include "lcd_bsp.h"

void setup() {
	Serial.begin(115200);
	// Wait up to 3s for USB CDC host to connect (needed for early println).
	uint32_t t0 = millis();
	while (!Serial && millis() - t0 < 3000) {}
	delay(100);
	Serial.println("=== ESP32-S3-Touch-AMOLED-1.64 ===");
	printf("printf test: if you see this, printf goes to USB CDC\n");

	Touch_Init();
	Serial.println("Touch init done");
	screen_init();
	Serial.println("Screen init done - should be RED");
}

void loop() {
	static bool screen_green = false;
	static uint32_t tick = 0;
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

	if (++tick % 40 == 0) {
		Serial.printf("heartbeat tick=%u screen=%s\n",
			      tick, screen_green ? "green" : "red");
	}

	delay(50);
}

