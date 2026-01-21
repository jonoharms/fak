# LED Control Guide

This guide covers how to set up the firmware and the host controller to control the WS2812 LEDs on the FAK keyboard, specifically targeting the CH552 microcontroller.

## 1. Firmware Setup

Before the LEDs can be controlled, the keyboard must be flashed with firmware that enables the LED and Raw HID features.

### Prerequisites
*   `uv` (Python package manager)
*   `sdcc` (C Compiler for 8051)
*   `nickel` (Configuration language)
*   `wchisp` (Flashing tool)

### Building and Flashing

1.  **Verify Configuration**: Ensure your `tests/keyboard.ncl` (or relevant config file) defines the LED pin and count:
    ```nickel
    led = {
      pin = 34, // Example: P3.4
      count = 6,
    },
    ```

2.  **Compile the Firmware**:
    Run the build script from the project root:
    ```bash
    uv run fak.py compile
    ```

3.  **Flash the Firmware**:
    Put your device into bootloader mode (hold the boot button while plugging in) and run:
    ```bash
    uv run fak.py flash
    ```

    *Note: The firmware now includes a startup LED chase sequence. If you see purple LEDs chasing on startup, the firmware is working correctly.*

## 2. LED Controller (Host Application)

The `led-controller` is a Rust CLI application that communicates with the keyboard via Raw HID.

### Prerequisites
*   Rust toolchain (`cargo`, `rustc`)

### Building (macOS / Windows / Linux Desktop)

1.  Navigate to the controller directory:
    ```bash
    cd led-controller
    ```

2.  Build the project:
    ```bash
    cargo build --release
    ```

3.  Run the test loop:
    ```bash
    ./target/release/led-controller --test
    ```

## 3. Raspberry Pi Setup

To run the controller on a Raspberry Pi, you need to handle Linux-specific permissions and dependencies.

### Dependencies

Install the required development libraries for `hidapi`:

```bash
sudo apt update
sudo apt install libudev-dev libusb-1.0-0-dev build-essential
```

### Udev Rules (Fixing Permissions)

To access the HID device without `sudo`, you must install a udev rule.

1.  **Copy the rule file**:
    The file `50-fak.rules` is located in the `led-controller` directory. Copy it to the system rules directory:
    ```bash
    sudo cp led-controller/50-fak.rules /etc/udev/rules.d/
    ```

2.  **Reload rules**:
    ```bash
    sudo udevadm control --reload-rules
    sudo udevadm trigger
    ```

3.  **Replug the Device**: Unplug and reconnect your keyboard for the permissions to take effect.

### Compiling on Raspberry Pi

1.  Transfer the `led-controller` folder to your Raspberry Pi.
2.  Navigate to the folder:
    ```bash
    cd led-controller
    ```
3.  Build the project:
    ```bash
    cargo build --release
    ```

### Usage

Run the controller test mode:

```bash
./target/release/led-controller --test
```

Or set a specific color manually:

```bash
# Set LED 0 to Red
./target/release/led-controller --index 0 --red 255 --green 0 --blue 0
```

## Troubleshooting

*   **"Failed to open device"**:
    *   Check USB connection.
    *   Verify Udev rules are installed (Linux).
    *   Ensure the firmware with `RAW_HID_ENABLE` is actually flashed.
    *   Try running with `sudo` temporarily to rule out permission issues.

*   **Command sent but LEDs don't light up**:
    *   If the startup chase works: The physical connection is good. The issue is likely the USB data packet format. (The codebase has been patched to send 65-byte packets to account for the Report ID offset on Linux).
    *   If the startup chase *doesn't* work: Check your wiring and the `pin` definition in `tests/keyboard.ncl`.
