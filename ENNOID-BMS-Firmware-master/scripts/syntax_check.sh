#!/bin/sh
#
# Repeatable local syntax check for the active Phase 7 migration surface.
# This is not a full firmware build or link step. The repo is still primarily
# Keil/CubeMX-oriented, so this script only verifies that selected translation
# units parse cleanly with the same defines/include paths used during migration.

set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)

check() {
  file="$1"
  echo "syntax-check: $file"
  clang \
    -fsyntax-only \
    -DSTM32F303xC \
    -DUSE_HAL_DRIVER \
    -x c \
    -I "$ROOT_DIR/Main" \
    -I "$ROOT_DIR/Modules/Inc" \
    -I "$ROOT_DIR/Drivers/HWDrivers/Inc" \
    -I "$ROOT_DIR/Drivers/SWDrivers/Inc" \
    -I "$ROOT_DIR/Libraries/Inc" \
    -I "$ROOT_DIR/Libraries/Scr" \
    -I "$ROOT_DIR/Device" \
    -I "$ROOT_DIR/Drivers/STM32F3xx_HAL_Driver/Inc" \
    -I "$ROOT_DIR/Drivers/CMSIS/Include" \
    -I "$ROOT_DIR/Drivers/CMSIS/Device/ST/STM32F3xx/Include" \
    "$ROOT_DIR/$file"
}

check "Main/main.c"
check "Modules/Src/modPowerElectronics.c"
check "Modules/Src/modOperationalState.c"
check "Drivers/SWDrivers/Src/driverSWLTC6812.c"
check "Drivers/SWDrivers/Src/driverSWISL28022.c"
check "Drivers/HWDrivers/Src/driverHWSPI1.c"
check "Drivers/HWDrivers/Src/driverHWI2C1.c"
check "Drivers/HWDrivers/Src/driverHWI2C2.c"
check "Drivers/HWDrivers/Src/driverHWADC.c"
check "Drivers/HWDrivers/Src/driverHWSwitches.c"

echo "syntax-check: all requested translation units parsed successfully"
