#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stddef.h>

typedef struct multiboot {    // TODO: get the size right goddamit
  uint32_t flags;             // required

  uint32_t mem_lower;         // if flags[0]
  uint32_t mem_upper;         // if flags[0]

  uint32_t boot_device;       // if flags[1]

  uint32_t cmdline;           // if flags[2]

  uint32_t mods_count;        // if flags[3]
  uint32_t mods_addr;         // if flags[3]

  uint8_t syms[12];           // if flags[4] or flags[5]

  uint32_t mmap_length;       // if flags[6]
  uint32_t mmap_addr;         // if flags[6]

  uint32_t drives_length;     // if flags[7]
  uint32_t drives_addr;       // if flags[7]

  uint32_t config_table;      // if flags[8]

  uint32_t bootloader_name;   // if flags[9]

  uint32_t apm_table;         // if flags[10]

  uint32_t vbe_control_info;  // if flags[11]
  uint32_t vbe_mode_info;     // ^
  uint16_t vbe_interface_seg; // ^
  uint16_t vbe_interface_off; // ^
  uint16_t vbe_interface_len; // ^

} __attribute__((packed)) multiboot_t;


#endif
