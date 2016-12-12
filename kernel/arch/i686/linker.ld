OUTPUT_FORMAT("binary")
ENTRY(_start)
phys = 0x00100000;
SECTIONS
{
      . = 1M;

      .text phys : AT(phys)
      {
        code = .;
        *(.text)
        *(.rodata)
      }

      .data BLOCK(4K) : ALIGN(4K)
      {
        data = .;
        *(.data)
      }

      .bss BLOCK(4K) : ALIGN(4K)
      {
        bss = .;
        *(COMMON)
        *(.bss)
      }

      end = .;
}
