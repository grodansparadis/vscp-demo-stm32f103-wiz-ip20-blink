# vscp-demo-stm32f103-wiz-ip20-blink
[VSCP blinky demo](https://github.com/grodansparadis/vscp-firmware/wiki) for STM32F103C8T6 "Blue Pill" with [WIZnet IP20 module](https://wiznet.io/products/serial-to-ethernet-modules/wiz-ip20). The firmware is built with CMake and STM32CubeMX-generated code, using the GNU Arm Embedded Toolchain. It demonstrates basic GPIO control for blinking an LED and configures and sets up network connectivity. 

VSCP blink is the simplest possible VSCP application, toggling an LED on and off at a configurable interval. This project serves as a starting point for developing more complex VSCP applications on STM32 microcontrollers with (Ethernet) connectivity. The VSCP blink demo is designed to be simple and easy to understand, making it ideal for learning how to use the VSCP protocol and develop applications for embedded systems. It is implemented on different platforms, including STM32 microcontrollers, and can be used as a reference for building your own VSCP applications.

## Hardware
- [STM32F103C8T6 "Blue Pill" development board](https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill.html)
- [WIZnet W5500 Ethernet module](https://www.wiznet.io/product-item/w5500/) (SPI interface)
- [ST-Link V2 programmer/debugger](https://www.st.com/en/development-tools/st-link-v2.html)
- [WIZ-IP20 IO Module](https://docs.wiznet.io/Product/Modules/Serial-to-Ethernet-Module/WIZ-IP20/ip20-io)
- [W55RP20-S2E Command Manual](https://docs.wiznet.io/Product/Chip/MCU/Pre-programmed-MCU/W55RP20-S2E/command-manual-en)
- [WIZ-IP20 Product Page](https://wiznet.io/products/serial-to-ethernet-modules/wiz-ip20)
- [WIZnet S2E Tool GUI Getting Started Guide](https://github.com/Wiznet/WIZnet-S2E-Tool-GUI/wiki/Getting-started-guide_en)

### Resources

* TIM2 is used as a millisecond timebase.
* TIM3 is used as a 1 µs free-running counter, extended to 32 bits via software overflow.

## Persistent Flash Storage

2 KB of internal flash is reserved for persistent storage (registers, configuration) using the last two pages of the 32 KB flash:

| Region  | Start        | End          | Size  | Pages |
|---------|-------------|--------------|-------|-------|
| Firmware | `0x08000000` | `0x080077FF` | 30 KB | 0–29  |
| Storage  | `0x08007800` | `0x08007FFF` |  2 KB | 30–31 |

The linker script ([STM32F103XX_FLASH.ld](firmware/STM32F103XX_FLASH.ld)) reduces the `FLASH` region to 30 KB and defines a separate `STORAGE` region. The driver is in [Core/Inc/flash_storage.h](firmware/Core/Inc/flash_storage.h) and [Core/Src/flash_storage.c](firmware/Core/Src/flash_storage.c).

Key constraints:
- A full page (1 KB) must be **erased** before any byte in it can be written
- Writes are **16-bit (half-word)** aligned
- Flash endurance: ~**10 000** erase cycles per page

```c
#include "flash_storage.h"

// Erase both storage pages (required before first write)
flash_storage_erase();

// Write two half-words at offset 0
uint16_t cfg[] = { 0x0001, 0x1234 };
flash_storage_write(0, cfg, 2);

// Read back
uint16_t buf[2];
flash_storage_read(0, buf, 2);
```

### Setup
- Connect the W5500 to the STM32F103C8T6 according to the following pinout:
  - W5500 SCK -> PA5
  - W5500 MISO -> PA6
  - W5500 MOSI -> PA7
  - W5500 CS -> PB6
  - W5500 RST -> PB7
- Connect the ST-Link V2 to the STM32F103C8T6 for programming and debugging:
  - ST-Link SWCLK -> PA14
  - ST-Link SWDIO -> PA13
  - ST-Link GND -> GND
  - ST-Link VCC -> 3.3V (optional, can power the board from USB or external source)
- Connect an LED with a suitable resistor to PB0 for blinking (or use the onboard LED if available)
- Debug output is available on PA2 (USART2 TX) for printf debugging. Connect a serial TTL to USB adapter to PA2 and GND to view debug output on your computer.


## Build the project

Go to the firmware directory

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build -j$(nproc)
```

This produces build/firmware.elf and .hex/.bin.


## Flashing the project

```bash
# OpenOCD
openocd -f interface/stlink.cfg -f target/st m32f1x.cfg \
  -c "program build/firmware.elf verify reset exit"

# ST-Link CLI (install with 'sudo apt install stlink-tools')
st-flash write build/firmware.bin 0x08000000
```

### Build Tips

- Regenerating code — CubeMX overwrites CMakeLists.txt files in cmake/stm32cubemx/ but leaves your top-level alone; keep custom code in Core/Src and your own cmake files
- VS Code — install the CMake Tools extension, point it at the toolchain file via cmake.configureSettings in settings.json
- Ninja — add -G Ninja to the cmake command for faster builds

### Testing ST_Link CLI

```bash

# Check version of st-flash
st-flash --version

# Example reply
v1.8.0

# Check connected ST-Link devices 
st-info --probe      # detects connected ST-Link

# Example reply
Found 1 stlink programmers
  version:    V2J46S7
  serial:     19004300190000433132524E
  flash:      32768 (pagesize: 1024)
  sram:       10240
  chipid:     0x412
  dev-type:   STM32F1xx_LD
``` 

## Debugging

Use OpenOCD with GDB:

```bash
# Start OpenOCD in one terminal
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg

# In another terminal, connect with GDB
arm-none-eabi-gdb build/firmware.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) continue

# You can set breakpoints, inspect variables, etc. in GDB

# Alternatively use 'st-util' for a more user-friendly interface
# but with less features than OpenOCD
st-util
```
