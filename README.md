# vending-machine

Firmware for a DIY vending machine built around Arduino Nano boards.

Right now this repo contains only a hardware smoke test: blink the built-in LED
and print `vending-machine alive` over serial. It exists to bring up real
hardware one component at a time.

## Hardware target

- Arduino Nano, ATmega328P-compatible clone
- PlatformIO board: `nanoatmega328new` (new bootloader, 115200 baud upload)

If this particular clone turns out to need the old bootloader, switch `board` in
`platformio.ini` to `nanoatmega328`.

## Prerequisites

- VS Code
- The [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) extension
  (it installs PlatformIO Core, which provides the `pio` CLI)

Open `vending-machine.code-workspace` in VS Code.

## Build, upload, monitor

From VS Code (Terminal → Run Task…, or Ctrl+Shift+B for the default build):

- `PlatformIO: Build`
- `PlatformIO: Upload`
- `PlatformIO: Serial Monitor`
- `PlatformIO: Upload and Monitor`

Or from a terminal:

```bash
pio run                  # build
pio run --target upload  # build + flash the Nano
pio device monitor       # serial monitor at 115200 baud
```

There is no debug launch configuration: the ATmega328P has no usable on-chip
debug interface without extra hardware.

## Scope

Only one Nano is in play at the moment. Everything lives in the single
`env:nanoatmega328new` environment.

## Future architecture

The machine is expected to end up as two Nanos:

- `motor-controller` — NEMA17 steppers via DRV8825 drivers, door relay/solenoid
- `front-controller` — keypad, I2C LCD, coin acceptor

talking to each other over a shared serial protocol. That split is not built
yet; it will become separate PlatformIO environments (and a shared `lib/`
protocol module) when the hardware justifies it.
