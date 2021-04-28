# calsol-fw-libs
CalSol's open-source, shared libraries for firmware running on the mbed platform.

## Repository Structure
This is meant to be [submoduled](https://git-scm.com/book/en/v2/Git-Tools-Submodules) into a higher-level project.

- [drivers](drivers): driver code for external ICs, dependent on the mbed API
- [utils](utils): utility code and classes, like RGB LEDs and long timers, dependent on the mbed API
- [hal](hal): HAL (hardware abstraction layer) extensions to mbed

## Using
A [PlatformIO](https://platformio.org/) library.json file is included, and this repository can be submoduled into your `lib/` folder.
