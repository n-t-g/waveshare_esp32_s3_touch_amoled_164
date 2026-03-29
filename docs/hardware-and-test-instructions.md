# Waveshare ESP32-S3-Touch-AMOLED-1.64 Hardware & Serial Unit Test

## Board details
- Product: USB JTAG/serial debug unit
- Manufacturer: Espressif
- SerialNumber: `10:20:BA:46:08:00`
- VID:PID: `0x303a`:`0x1001`
- MCU: ESP32-S3 QFN56
- Flash: 16MB
- RAM: 320KB

## PlatformIO configuration

`platformio.ini` section for board:

```
[env:waveshare_esp32_s3_touch_amoled_164]
platform = espressif32
framework = arduino
board = waveshare_esp32_s3_touch_amoled_164
monitor_speed = 115200
upload_speed = 921600
upload_port = /dev/ttyACM0
board_build.partitions = default.csv
board_build.flash_mode = dio
board_build.f_flash = 80000000L
board_build.f_cpu = 240000000L
board_build.arduino.memory_type = qio_opi
board_build.psram_type = opi
build_flags =
    -DBOARD_HAS_PSRAM
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1
```

## Board JSON settings

`boards/waveshare_esp32_s3_touch_amoled_164.json` must include:
- `core`: `esp32`
- `variant`: `esp32s3`
- `ARDUINO_USB_CDC_ON_BOOT=1` in `extra_flags`
- `hwids` mapping to `["0x303A", "0x1001"]`

## Serial test firmware (`src/main.cpp`)

The active test prints:
- startup info
- tick message every 500 ms
- echo incoming bytes
- toggles LED_BUILTIN

## Running the test

1. Build and upload:

```bash
pio run -e waveshare_esp32_s3_touch_amoled_164 -t upload --upload-port /dev/ttyACM0
```

2. Open monitor:

```bash
pio device monitor --port /dev/ttyACM0 --baud 115200
```

3. Verify output lines begin with:

`=== ESP32-S3 USB CDC serial test ===`

and ticks as:

`tick 000000 uptime ...`

## PlatformIO unit test

- File: `test/test_serial_monitor.cpp`
- Run:

```bash
pio test -e waveshare_esp32_s3_touch_amoled_164
```

- Example asserts:
  - `test_serial_is_ready`: `Serial.begin(115200)` and `TEST_ASSERT_TRUE(Serial)`
  - `test_dummy_counting`: simple sanity check
