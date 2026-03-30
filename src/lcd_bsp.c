#include "lcd_bsp.h"
#include "esp_lcd_sh8601.h"
#include "lcd_config.h"
#include <assert.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_heap_caps.h"

#define LCD_HOST SPI2_HOST
/* Column offset: the SH8601 RAM on this board starts at physical column 20. */
#define LCD_X_OFFSET 0x14

static const char *TAG = "screen_bsp";
static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_panel_io_handle_t panel_io_handle = NULL;
volatile int g_lcd_init_step = 0;

/*
 * Init sequence from the Waveshare factory ESP-IDF demo (07_FactoryProgram).
 * 0x11 = Sleep Out (80 ms delay required)
 * 0x29 = Display On (10 ms delay)
 * 0x51 = Set Brightness (last command: full brightness 0xFF)
 */
static const uint8_t _d80[] = {0x80};
static const uint8_t _d20[] = {0x20};
static const uint8_t _dFF[] = {0xFF};
static const uint8_t _d00[] = {0x00};
static const sh8601_lcd_init_cmd_t lcd_init_cmds[] = {
    {0x11, _d00,  0, 80},
    {0xC4, _d80,  1, 0},
    {0x35, _d00,  1, 0},
    {0x53, _d20,  1, 1},
    {0x63, _dFF,  1, 1},
    {0x51, _d00,  1, 1},
    {0x29, _d00,  0, 10},
    {0x51, _dFF,  1, 0},
};

esp_err_t set_amoled_backlight(uint8_t brig)
{
    if (!panel_io_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    /* QSPI write-command format: (opcode=0x02 << 24) | (cmd << 8) */
    uint32_t lcd_cmd = (0x02UL << 24) | (0x51UL << 8);
    return esp_lcd_panel_io_tx_param(panel_io_handle, (int)lcd_cmd, &brig, 1);
}

void screen_init(void)
{
    g_lcd_init_step = 1;
    const spi_bus_config_t buscfg = SH8601_PANEL_BUS_QSPI_CONFIG(
        EXAMPLE_PIN_NUM_LCD_PCLK,
        EXAMPLE_PIN_NUM_LCD_DATA0,
        EXAMPLE_PIN_NUM_LCD_DATA1,
        EXAMPLE_PIN_NUM_LCD_DATA2,
        EXAMPLE_PIN_NUM_LCD_DATA3,
        EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * LCD_BIT_PER_PIXEL / 8);
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));
    g_lcd_init_step = 2;

    const esp_lcd_panel_io_spi_config_t io_config = SH8601_PANEL_IO_QSPI_CONFIG(
        EXAMPLE_PIN_NUM_LCD_CS, NULL, NULL);
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(
        (esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &panel_io_handle));
    g_lcd_init_step = 3;

    sh8601_vendor_config_t vendor_config = {
        .init_cmds      = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]),
        .flags          = { .use_qspi_interface = 1 },
    };
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .color_space    = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = LCD_BIT_PER_PIXEL,
        .vendor_config  = &vendor_config,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_sh8601(panel_io_handle, &panel_config, &panel_handle));
    g_lcd_init_step = 4;
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    g_lcd_init_step = 5;
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    g_lcd_init_step = 6;
    /* Give the AMOLED time to wake from sleep before sending pixel data. */
    vTaskDelay(pdMS_TO_TICKS(120));
    /* Explicitly set full brightness (belt-and-suspenders; also sent in init_cmds). */
    uint8_t full = 0xFF;
    esp_lcd_panel_io_tx_param(panel_io_handle,
        (int)((0x02UL << 24) | (0x51UL << 8)), &full, 1);
    g_lcd_init_step = 7;

    /* Allocate a DMA-capable line buffer (required for SPI color transfers). */
    uint16_t *chunk = (uint16_t *)heap_caps_malloc(
        EXAMPLE_LCD_H_RES * CHUNK_LINES * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (!chunk) {
        ESP_LOGE(TAG, "chunk alloc failed");
        return;
    }
    for (int i = 0; i < EXAMPLE_LCD_H_RES * CHUNK_LINES; i++) chunk[i] = 0xFFFF; /* white */
    for (int y = 0; y < EXAMPLE_LCD_V_RES; y += CHUNK_LINES) {
        int rows = (y + CHUNK_LINES <= EXAMPLE_LCD_V_RES) ? CHUNK_LINES
                                                           : (EXAMPLE_LCD_V_RES - y);
        esp_lcd_panel_draw_bitmap(panel_handle,
            LCD_X_OFFSET, y,
            LCD_X_OFFSET + EXAMPLE_LCD_H_RES, y + rows,
            chunk);
    }
    heap_caps_free(chunk);
    g_lcd_init_step = 100; /* success */
    ESP_LOGI(TAG, "done");
}

void screen_fill_color(uint16_t color)
{
    if (!panel_handle) return;

    uint16_t *line = (uint16_t *)heap_caps_malloc(
        EXAMPLE_LCD_H_RES * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (!line) return;
    for (int i = 0; i < EXAMPLE_LCD_H_RES; i++) line[i] = color;

    for (int y = 0; y < EXAMPLE_LCD_V_RES; y++) {
        esp_lcd_panel_draw_bitmap(
            panel_handle,
            LCD_X_OFFSET, y,
            LCD_X_OFFSET + EXAMPLE_LCD_H_RES, y + 1,
            line);
    }
    heap_caps_free(line);
}


