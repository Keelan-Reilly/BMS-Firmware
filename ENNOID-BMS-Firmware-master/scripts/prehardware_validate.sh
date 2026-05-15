#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
ARM_GNU_DEFAULT=/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin

"$ROOT_DIR/scripts/syntax_check.sh"
if ! command -v arm-none-eabi-gcc >/dev/null 2>&1 && [ -d "$ARM_GNU_DEFAULT" ]; then
    PATH="$ARM_GNU_DEFAULT:$PATH"
    export PATH
fi

make -C "$ROOT_DIR" clean
make -C "$ROOT_DIR" -j4
python3 "$ROOT_DIR/scripts/check_flash_layout.py" --root "$ROOT_DIR"
python3 "$ROOT_DIR/tools/ui_protocol_golden.py" --ui-root "$ROOT_DIR/../ENNOID-BMS-Tool-master"
python3 "$ROOT_DIR/../ENNOID-BMS-Tool-master/tools/fake_bms_firmware.py" --self-test
