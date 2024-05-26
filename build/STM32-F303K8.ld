/* Minimal linker script for STM32F303K8 chipsets */

/* As per STM32F303K8 the flash rom size is 64KB and the on board sd ram is 12KB*/
__flash_begin = 0x08000000;
__flash_size = 64K;
__sdram_size = 12K;
__stack_size = 0;
__heap_size = 4K;

/* Memory layout definition */
/* First 16KB of onboard flash rom is allocated for the arm vector table for conviniency */
MEMORY
{
    VECTOR  (rx)    : ORIGIN = 0x08000000, LENGTH = 16K
    FLASH   (rx)    : ORIGIN = 0x08004000, LENGTH = __flash_size - 16K
    RAM     (rxw)   : ORIGIN = 0x20000000, LENGTH = __sdram_size
}

SECTIONS
{
    /* First entry has to be for the vector table */
    /* .vec_table - the symbol for the ARM vector interrupt table -- forcing optimisation OFF */
    .vec_table :
    {
        . = ALIGN(4);
        KEEP(*(.vec_table))
        . = ALIGN(4);

    } > VECTOR

    /* This is the section where we store the common code symbols - resides in Flash ROM */
    /* Source code symbols and readonly data can be stored in FlashROM since it does not need to change. */
    /* .text - symbols from common source code*/
    /* .rodata - read only symbols - constants and the like */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        KEEP (*(.init))     /* ARM linker complains if this is not included TODO: Find out what is this */
        KEEP (*(.fini))     /* ARM linker complains if this is not included TODO: Find out what is this */
        *(.glue_7)          /* ARM linker complains if this is not included TODO: Find out what is this */
		*(.glue_7t)         /* ARM linker complains if this is not included TODO: Find out what is this */
		*(.eh_frame)        /* ARM linker complains if this is not included TODO: Find out what is this */
        . = ALIGN(4);

    } > FLASH

    /* ARM linker complains if this is not included TODO: Find out what is this */
    .ARM : {
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} >FLASH

    /* Runtime data symbols */
    /* Runtime data will change during lifetime cycle so it is loaded into RAM */
    __data_flash = .;
    .data : AT (__data_flash)
    {
        . = ALIGN(4);
        __data_begin = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        __data_end = .;

    } > RAM

    .bss : AT (__data_end)
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;

    } > RAM

    /* Stack symbols */
    /* Stack addresses are meant unrolled back to front in ARM */
    __stack_end = ORIGIN(RAM) + LENGTH(RAM);
    __stack_begin = __stack_end - __stack_size;
    . = __stack_begin;
    .__stack :
    {
        . = . + __stack_size;

    } > RAM

    /* Dynamic heap memory region */
    .heap (NOLOAD) : 
    {
        . = ALIGN(4);
        _sheap = .;
        end = _sheap;
        . = . + __heap_size;
        _eheap = .;
        . = ALIGN(4);
    } > RAM
}

ENTRY(IRQ_RESET_Handler);