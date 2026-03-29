/**
 * @file lv_conf.h
 * LVGL configuration for Waveshare ESP32-S3-Touch-AMOLED-1.64
 * Display: 280x456 pixels, 16-bit color
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/
#define LV_COLOR_DEPTH 16

/*====================
    MEMORY SETTINGS
 *====================*/
/* Size of the memory available for `lv_malloc()` in bytes (>= 2kB) */
#define LV_MEM_SIZE (256 * 1024U)  /* 256 kB — uses PSRAM on ESP32-S3 */

/* Use the standard `memcpy` and `memset` instead of LVGL's own functions */
#define LV_MEMCPY_MEMSET_STD 0

/*====================
    HAL SETTINGS
 *====================*/
/* Default display refresh period in milliseconds */
#define LV_DEF_REFR_PERIOD 10

/* Input device read period in milliseconds */
#define LV_INDEV_DEF_READ_PERIOD 30

/*==================
 *  FONT USAGE
 *=================*/
/* Enable built-in fonts */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* Default font: Montserrat 16 */
#define LV_FONT_DEFAULT &lv_font_montserrat_16

/*===================
 *  TEXT SETTINGS
 *===================*/
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/*===================
 *  WIDGETS
 *===================*/
#define LV_USE_ARC        1
#define LV_USE_BAR        1
#define LV_USE_BTN        1
#define LV_USE_BTNMATRIX  1
#define LV_USE_CANVAS     1
#define LV_USE_CHECKBOX   1
#define LV_USE_DROPDOWN   1
#define LV_USE_IMG        1
#define LV_USE_LABEL      1
#define LV_USE_LINE       1
#define LV_USE_ROLLER     1
#define LV_USE_SLIDER     1
#define LV_USE_SWITCH     1
#define LV_USE_TEXTAREA   1
#define LV_USE_TABLE      1

/*==================
 * EXTRA COMPONENTS
 *==================*/
#define LV_USE_FLEX    1
#define LV_USE_GRID    1
#define LV_USE_ANIM    1
#define LV_USE_TIMER   1

/*==================
 * DRAW BACKENDS
 *==================*/
#define LV_USE_DRAW_SW 1

/*==================
 * LOG SETTINGS
 *==================*/
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 1

/*==================
 * OS / THREADING
 *==================*/
#define LV_USE_OS LV_OS_NONE

#endif /* LV_CONF_H */
