# Using Older ST-LINK (V2) on Linux

This is typical the low cost Chinese aluminum devices.

Older **ST-LINK/V2** debuggers (including standalone dongles and those on older Discovery boards) work natively on Linux but lack the built-in Virtual COM Port found on newer V2-1 or V3 models.

## 1. Install Required Software
Choose one of the standard open-source toolsets:

* **OpenOCD** (Best for IDE integrations):
  ```bash
  sudo apt install openocd
  ```
* **stlink Tools** (Best for quick command-line flashing):
  ```bash
  sudo apt install stlink-tools
  ```

## 2. Set Up Udev Rules (Fix Permissions)
To access the device without root/sudo privileges, configure your system's USB rules:

1. **Navigate** to the udev directory:
   ```bash
   cd /etc/udev/rules.d/
   ```
2. **Download** the official rule files from the `stlink-org` GitHub repository.
3. **Reload** the system rules to apply changes:
   ```bash
   sudo udevadm control --reload-rules
   sudo udevadm trigger
   ```

## 3. Connect to Serial Monitor (External Hardware Required)
Older ST-LINK/V2 hardware does **not** route UART data over the USB connection. 

* **Hardware requirement:** Connect an external USB-to-UART adapter (e.g., FTDI, CP2102) to your target microcontroller's TX/RX pins.
* **Identify port:** The external adapter usually mounts at `/dev/ttyUSB0`.
* **Connect via CLI:** Use `picocom` or `minicom` to view logs:
  ```bash
  picocom -b 115200 /dev/ttyUSB0
  ```

## 4. Flash and Debug
Run these commands to interact with your target microcontroller, noting the specific config file differences for older hardware:

* **Flash a binary (stlink tools):**
  ```bash
  st-flash write your_binary.bin 0x08000000
  ```
* **Start a debug session (OpenOCD):**
  ```bash
  openocd -f interface/stlink.cfg -f target/stm32[your_chip_family].cfg
  ```
  *(Note: Older OpenOCD setups may require `interface/stlink-v2.cfg` instead of `stlink.cfg`)*

