#ifndef LCD_BSP_H
#define LCD_BSP_H

#include "Arduino.h"
#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

void screen_init(void);
void screen_fill_color(uint16_t color);
esp_err_t set_amoled_backlight(uint8_t brig);

/* Diagnostic: step counter, >=100 means success */
extern volatile int g_lcd_init_step;

#ifdef __cplusplus
}
#endif

#endif