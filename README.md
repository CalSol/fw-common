# calsol-fw-libs
CalSol's open-source, shared libraries for firmware running on the mbed platform.

## Repository Structure
This is meant to be [submoduled](https://git-scm.com/book/en/v2/Git-Tools-Submodules) into a higher-level project.

- [drivers]: driver code for external ICs, dependent on the mbed API
- [utils]: utility code and classes, like RGB LEDs and long tiemrs, dependent on the mbed API
- [hal]: HAL (hardware abstraction layer) extensions to mbed

## Building
A SConscript ([SCons](http://scons.org/) build fragment) is included in this and can be invoked from a higher-level SCons script. This modifies the `env` passed in so `CPPPATH` includes the header locations and `LIBS` includes the built static library.

## SCons utilities
Several SCons build utils are included in separate `SConscript-*` files:
- [SConscript-git-utils]: includes submodule version check and getting the current git version
- [SConscript-openocd-utils]: adds a flash target using OpenOCD
- [SConscript-simplify]: simplifies the build console output to an abbreviation and output file, instead of the full command executed

These are not automatically invoked in the main SConscript file, and need to be invoked separately.
Usage examples are at the top of each file.
