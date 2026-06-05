# Using ST-LINK/V2-1 on Linux

The ST-LINK/V2-1 debugger (found on Nucleo and Discovery boards) works natively on Linux using open-source tools. 

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

## 3. Connect to Virtual COM Port
The ST-LINK/V2-1 includes a built-in virtual serial port for UART communication.

* **Identify port:** It usually mounts at `/dev/ttyACM0`.
* **Connect via CLI:** Use `picocom` or `minicom` to view logs:
  ```bash
  picocom -b 115200 /dev/ttyACM0
  ```

## 4. Flash and Debug
Run these commands to interact with your target microcontroller:

* **Flash a binary (stlink tools):**
  ```bash
  st-flash write your_binary.bin 0x08000000
  ```
* **Start a debug session (OpenOCD):**
  ```bash
  openocd -f interface/stlink-v2-1.cfg -f target/stm32[your_chip_family].cfg
  ```


