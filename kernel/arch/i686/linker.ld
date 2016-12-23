ENTRY(_boot)
phys = 0x00100000;
SECTIONS
{
      . = 1M;

      start = .; _start = .; __start = .;

      .text phys : AT(phys)
      {
        code = .;
        *(._multiboot_header)
        *(.text)
        *(.rodata)
      }

      .data BLOCK(4K) : ALIGN(4096)
      {
        data = .;
        *(.data)
      }

      .bss BLOCK(4K) : ALIGN(4096)
      {
        bss = .;
        *(COMMON)
        *(.bss)
      }

      end = .; _end = .; __end = .;

      /DISCARD/ :
      {
        *(.comment)
      }
}
