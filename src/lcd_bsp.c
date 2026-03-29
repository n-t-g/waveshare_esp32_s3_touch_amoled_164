#include "lcd_bsp.h"
#include "esp_lcd_sh8601.h"
#include "lcd_config.h"
#include <assert.h>
#include "esp_log.h"

#define LCD_HOST SPI2_HOST

static const char *TAG = "screen_bsp";
static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_panel_io_handle_t panel_io_handle = NULL;

/* Init sequence taken from the Waveshare Arduino demo (06_LVGL_Test/lcd_bsp.c).
 * Key commands:
 *   0x11 - Sleep Out  (must wait 80 ms)
 *   0x29 - Display On (must wait 10 ms)
 *   0x51 - Write Brightness (last entry sets full brightness)
 */
static const sh8601_lcd_init_cmd_t lcd_init_cmds[] = {
    {0x11, (uint8_t []){0x00}, 0, 80},
    {0xC4, (uint8_t []){0x80}, 1, 0},
    {0x35, (uint8_t []){0x00}, 1, 0},
    {0x53, (uint8_t []){0x20}, 1, 1},
    {0x63, (uint8_t []){0xFF}, 1, 1},
    {0x51, (uint8_t []){0x00}, 1, 1},
    {0x29, (uint8_t []){0x00}, 0, 10},
    {0x51, (uint8_t []){0xFF}, 1, 0},
};

esp_err_t set_amoled_backlight(uint8_t brig)
{
    if (!panel_io_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    uint32_t lcd_cmd = 0x51;
    lcd_cmd &= 0xff;
    lcd_cmd <<= 8;
    lcd_cmd |= 0x02 << 24;
    return esp_lcd_panel_io_tx_param(panel_io_handle, lcd_cmd, &brig, 1);
}

void screen_init(void)
{
    const spi_bus_config_t buscfg = SH8601_PANEL_BUS_QSPI_CONFIG(
        EXAMPLE_PIN_NUM_LCD_PCLK,
        EXAMPLE_PIN_NUM_LCD_DATA0,
        EXAMPLE_PIN_NUM_LCD_DATA1,
        EXAMPLE_PIN_NUM_LCD_DATA2,
        EXAMPLE_PIN_NUM_LCD_DATA3,
        EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * LCD_BIT_PER_PIXEL / 8);

    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    const esp_lcd_panel_io_spi_config_t io_config = SH8601_PANEL_IO_QSPI_CONFIG(EXAMPLE_PIN_NUM_LCD_CS, NULL, NULL);
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &panel_io_handle));

    sh8601_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]),
        .flags = {
            .use_qspi_interface = 1,
        },
    };

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = LCD_BIT_PER_PIXEL,
        .flags = {.reset_active_high = 0},
        .vendor_config = &vendor_config,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_sh8601(panel_io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    /* The init sequence above already sends 0x29 (Display On) and sets full
     * brightness.  Call disp_on_off anyway as a safe belt-and-suspenders step. */
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    /* The SH8601 panel memory is offset by 20 columns (0x14) on this board. */
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 0x14, 0));

    ESP_ERROR_CHECK(set_amoled_backlight(0xFF));

    // Fill a simple color to show the display is on
    static uint16_t line[EXAMPLE_LCD_H_RES];
    for (int i = 0; i < EXAMPLE_LCD_H_RES; i++) {
        line[i] = 0xF800; // 16-bit RGB565 red
    }
    for (int y = 0; y < EXAMPLE_LCD_V_RES; y++) {
        esp_err_t err = esp_lcd_panel_draw_bitmap(panel_handle, 0, y, EXAMPLE_LCD_H_RES, y + 1, line);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "draw line %d failed (%d)", y, err);
        }
    }
}

void screen_fill_color(uint16_t color)
{
    if (!panel_handle) {
        ESP_LOGW(TAG, "panel_handle not ready");
        return;
    }
    static uint16_t line[EXAMPLE_LCD_H_RES];
    for (int i = 0; i < EXAMPLE_LCD_H_RES; i++) {
        line[i] = color;
    }

    for (int y = 0; y < EXAMPLE_LCD_V_RES; y++) {
        esp_err_t err = esp_lcd_panel_draw_bitmap(panel_handle, 0, y, EXAMPLE_LCD_H_RES, y + 1, line);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "screen_fill_color draw line %d failed (%d)", y, err);
        }
    }
}


