/*
 * GNU linker script for the migrated STM32F303CC application build.
 *
 * Flash layout matches the repo's documented split:
 * - 0x08000000..0x080007ff : application vectors / early code
 * - 0x08000800..0x080017ff : EEPROM emulation pages
 * - 0x08001800..0x08031fff : main application body
 * - 0x08032000..           : bootloader region
 */

ENTRY(Reset_Handler)

_estack = 0x2000A000;
_Min_Heap_Size = 0x200;
_Min_Stack_Size = 0x400;

MEMORY
{
  APPVEC  (rx)  : ORIGIN = 0x08000000, LENGTH = 0x00000800
  APPFLASH(rx)  : ORIGIN = 0x08001800, LENGTH = 0x00030800
  RAM     (xrw) : ORIGIN = 0x20000000, LENGTH = 40K
  CCMRAM  (rw)  : ORIGIN = 0x10000000, LENGTH = 8K
}

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >APPVEC

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
    KEEP(*(.init))
    KEEP(*(.fini))
    . = ALIGN(4);
    _etext = .;
  } >APPFLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >APPFLASH

  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >APPFLASH
  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >APPFLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >APPFLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >APPFLASH

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >APPFLASH

  _sidata = LOADADDR(.data);

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT>APPFLASH

  _siccmram = LOADADDR(.ccmram);

  .ccmram :
  {
    . = ALIGN(4);
    _sccmram = .;
    *(.ccmram)
    *(.ccmram*)
    . = ALIGN(4);
    _eccmram = .;
  } >CCMRAM AT>APPFLASH

  . = ALIGN(4);
  .bss :
  {
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM

  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >RAM

  /DISCARD/ :
  {
    libc.a(*)
    libm.a(*)
    libgcc.a(*)
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
