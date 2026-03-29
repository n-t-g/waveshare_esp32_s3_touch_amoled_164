/**
 * @file main.cpp
 * @brief Waveshare ESP32-S3-Touch-AMOLED-1.64 demo
 *
 * Hardware:
 *   MCU    : ESP32-S3R8 @ 240 MHz, 16 MB flash, 8 MB PSRAM
 *   Display: 1.64" AMOLED CO5300, 280×456, QSPI
 *   Touch  : FT3168 capacitive, I²C (SDA=GPIO47, SCL=GPIO48, addr=0x38)
 *
 * This sketch initialises the display via Arduino_GFX, wires LVGL 9 to it,
 * reads touch input over I²C and renders a simple interactive demo screen.
 */

#include <Arduino.h>
#include <Wire.h>
#include <esp_heap_caps.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>

// ---------------------------------------------------------------------------
// Display – CO5300 over QSPI
// ---------------------------------------------------------------------------
static Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    DISPLAY_CS  /* CS  */,
    DISPLAY_SCK /* SCK */,
    DISPLAY_D0  /* D0  */,
    DISPLAY_D1  /* D1  */,
    DISPLAY_D2  /* D2  */,
    DISPLAY_D3  /* D3  */
);

// col/row offsets taken from the official Waveshare example
static Arduino_GFX *gfx = new Arduino_CO5300(
    bus,
    DISPLAY_RST /* RST */,
    0           /* rotation: 0 = portrait */,
    false       /* IPS (not used for AMOLED) */,
    DISPLAY_WIDTH, DISPLAY_HEIGHT,
    20  /* col offset 1 */,
    0   /* row offset 1 */,
    180 /* col offset 2 */,
    24  /* row offset 2 */
);

// ---------------------------------------------------------------------------
// Touch – FT3168 over I²C
// ---------------------------------------------------------------------------
#define FT3168_ADDR      0x38
#define FT3168_REG_TOUCH 0x02   // number-of-touches register

static lv_indev_t  *touch_indev  = nullptr;
static lv_point_t   last_point   = {0, 0};
static bool         touch_active = false;

static bool ft3168_read(uint16_t *x, uint16_t *y)
{
    uint8_t buf[5] = {0};

    Wire.beginTransmission(FT3168_ADDR);
    Wire.write(FT3168_REG_TOUCH);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }
    if (Wire.requestFrom(static_cast<uint8_t>(FT3168_ADDR),
                         static_cast<uint8_t>(5)) != 5) {
        return false;
    }
    for (int i = 0; i < 5; i++) {
        buf[i] = Wire.read();
    }

    uint8_t touches = buf[0] & 0x0F;
    if (touches == 0) {
        return false;
    }

    *x = static_cast<uint16_t>(((buf[1] & 0x0F) << 8) | buf[2]);
    *y = static_cast<uint16_t>(((buf[3] & 0x0F) << 8) | buf[4]);
    return true;
}

// LVGL input-device callback
static void touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t x = 0, y = 0;
    if (ft3168_read(&x, &y)) {
        last_point.x  = static_cast<int32_t>(x);
        last_point.y  = static_cast<int32_t>(y);
        touch_active  = true;
    } else {
        touch_active = false;
    }

    data->point  = last_point;
    data->state  = touch_active ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

// ---------------------------------------------------------------------------
// LVGL display flush callback
// ---------------------------------------------------------------------------
static lv_display_t *lvgl_display = nullptr;

static void disp_flush_cb(lv_display_t *disp, const lv_area_t *area,
                           uint8_t *px_map)
{
    uint16_t w = static_cast<uint16_t>(area->x2 - area->x1 + 1);
    uint16_t h = static_cast<uint16_t>(area->y2 - area->y1 + 1);

    gfx->draw16bitRGBBitmap(
        static_cast<int16_t>(area->x1),
        static_cast<int16_t>(area->y1),
        reinterpret_cast<uint16_t *>(px_map),
        w, h
    );

    lv_display_flush_ready(disp);
}

// ---------------------------------------------------------------------------
// Demo UI helpers
// ---------------------------------------------------------------------------
static lv_obj_t *label_touch  = nullptr;
static lv_obj_t *arc_anim     = nullptr;

// Arc animation callback
static void arc_anim_cb(void *obj, int32_t val)
{
    lv_arc_set_value(static_cast<lv_obj_t *>(obj), static_cast<int16_t>(val));
}

static void build_demo_ui()
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0d1117), 0);

    // --- Title label ---
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Waveshare\nAMOLED 1.64\"");
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x58a6ff), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // --- Animated arc ---
    arc_anim = lv_arc_create(scr);
    lv_arc_set_range(arc_anim, 0, 100);
    lv_arc_set_value(arc_anim, 0);
    lv_obj_set_size(arc_anim, 120, 120);
    lv_obj_align(arc_anim, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_style_arc_color(arc_anim, lv_color_hex(0x238636), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_anim, lv_color_hex(0x21262d), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_anim, 10, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_anim, 10, LV_PART_MAIN);

    // Continuous 0→100 animation, looping
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc_anim);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, arc_anim_cb);
    lv_anim_start(&a);

    // --- Touch-coordinate label ---
    label_touch = lv_label_create(scr);
    lv_label_set_text(label_touch, "Touch the screen");
    lv_obj_set_style_text_color(label_touch, lv_color_hex(0x8b949e), 0);
    lv_obj_set_style_text_font(label_touch, &lv_font_montserrat_14, 0);
    lv_obj_align(label_touch, LV_ALIGN_BOTTOM_MID, 0, -30);

    // --- Colour swatch row ---
    static const uint32_t palette[] = {
        0xe74c3c, 0xe67e22, 0xf1c40f,
        0x2ecc71, 0x3498db, 0x9b59b6
    };
    static const uint8_t N = sizeof(palette) / sizeof(palette[0]);

    lv_obj_t *row = lv_obj_create(scr);
    lv_obj_set_size(row, DISPLAY_WIDTH - 20, 36);
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, -70);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(row, LV_SCROLLBAR_MODE_OFF);

    for (uint8_t i = 0; i < N; i++) {
        lv_obj_t *dot = lv_obj_create(row);
        lv_obj_set_size(dot, 28, 28);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dot, lv_color_hex(palette[i]), 0);
        lv_obj_set_style_border_width(dot, 0, 0);
    }
}

// Update touch label in the main loop
static void update_touch_label()
{
    if (touch_active && label_touch) {
        char buf[48];
        snprintf(buf, sizeof(buf), "x: %d  y: %d",
                 static_cast<int>(last_point.x),
                 static_cast<int>(last_point.y));
        lv_label_set_text(label_touch, buf);
    } else if (!touch_active && label_touch) {
        lv_label_set_text(label_touch, "Touch the screen");
    }
}

// ---------------------------------------------------------------------------
// Arduino entry points
// ---------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    Serial.println("Waveshare ESP32-S3-Touch-AMOLED-1.64 starting…");

    // --- I²C for touch ---
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
    Wire.setClock(400000);

    // --- Display ---
    if (!gfx->begin()) {
        Serial.println("ERROR: display init failed");
        while (true) { delay(100); }
    }
    gfx->fillScreen(BLACK);
    gfx->invertDisplay(false);

    Serial.printf("Display %dx%d ready\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // --- LVGL ---
    lv_init();

    // Draw buffer: 1/10 of screen height, allocated in PSRAM
    static const size_t BUF_LINES  = DISPLAY_HEIGHT / 10;
    static const size_t BUF_PIXELS = DISPLAY_WIDTH * BUF_LINES;
    static const size_t BUF_BYTES  = BUF_PIXELS * sizeof(lv_color_t);
    lv_color_t *draw_buf = static_cast<lv_color_t *>(
        heap_caps_malloc(BUF_BYTES, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (!draw_buf) {
        Serial.println("ERROR: failed to allocate LVGL draw buffer in PSRAM");
        while (true) { delay(100); }
    }

    lvgl_display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    lv_display_set_flush_cb(lvgl_display, disp_flush_cb);
    lv_display_set_buffers(lvgl_display, draw_buf, nullptr,
                           BUF_BYTES,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    // --- Touch input device ---
    touch_indev = lv_indev_create();
    lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touch_indev, touch_read_cb);

    // --- Build UI ---
    build_demo_ui();

    Serial.println("Setup complete");
}

void loop()
{
    lv_timer_handler();
    update_touch_label();
    delay(5);
}
