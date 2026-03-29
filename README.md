# Waveshare ESP32-S3-Touch-AMOLED-1.64

A [PlatformIO](https://platformio.org/) demo project for the
[Waveshare ESP32-S3-Touch-AMOLED-1.64](https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.64)
development board.

## Hardware

| Component | Details |
|-----------|---------|
| MCU | ESP32-S3R8 – dual-core 240 MHz, 512 KB SRAM, 8 MB PSRAM |
| Flash | 16 MB external (QSPI) |
| Display | 1.64″ AMOLED CO5300, 280 × 456 px, QSPI |
| Touch | FT3168 capacitive, I²C (SDA=GPIO47, SCL=GPIO48) |
| IMU | QMI8658C 6-axis (I²C, shared bus) |
| Power | USB-C, 3.7 V LiPo battery charging |

## Features

- AMOLED display initialised via [Arduino_GFX](https://github.com/moononournation/Arduino_GFX)
- [LVGL 9](https://lvgl.io/) UI framework
- FT3168 capacitive-touch driver (I²C)
- Demo screen: animated arc, colour swatches, live touch-coordinate readout

## Prerequisites

- [PlatformIO](https://platformio.org/install) (CLI or VS Code extension)
- USB-C cable connected to the board

## Build & Flash

```bash
# Build
pio run

# Flash and open serial monitor
pio run --target upload --target monitor
```

## Project Structure

```
├── platformio.ini      # Board + library configuration
├── include/
│   └── lv_conf.h       # LVGL feature configuration
└── src/
    └── main.cpp        # Application entry point
```

## Pin Reference

### Display (QSPI)

| Signal | GPIO |
|--------|------|
| CS | 9 |
| CLK | 10 |
| D0 | 11 |
| D1 | 12 |
| D2 | 13 |
| D3 | 14 |
| RST | 21 |

### Touch (I²C, address 0x38)

| Signal | GPIO |
|--------|------|
| SDA | 47 |
| SCL | 48 |

## License

MIT – see [LICENSE](LICENSE).
