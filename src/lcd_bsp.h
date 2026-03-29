#ifndef LCD_BSP_H
#define LCD_BSP_H

#include "Arduino.h"
#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"

#ifdef __cplusplus
extern "C" {
#endif

void screen_init(void);
void screen_fill_color(uint16_t color);
esp_err_t set_amoled_backlight(uint8_t brig);

#ifdef __cplusplus
}
#endif

#endif