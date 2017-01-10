// kmain.c kernel entry point
//
// Copyright (C) 2016,2017  bauen1
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdbool.h>
#include <assert.h>

#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "system.h"
#include "tty.h"
#include "idt.h"
#include "time.h"
#include "gdt.h"
#include "keyboard.h"
#include "mouse.h"
#include "pci.h"
#include "pmm.h"
#include "vmm.h"
#include "rtl8139.h"
#include "elf_loader.h"

void __abort(const char * function, const char * filename, int line) {
  kprintf("Assertion failed in function %s in file %s at line %d!\n", function, filename, line);
  halt();
}

const char * exception[32] = {
  "Division by Zero",
  "Debug Exception",
  "NMI",
  "Breakpoint",
  "into detected overflow",
  "Out of Bounds",
  "Invalid Opcode",
  "No coprocessor",
  "Double Fault",
  "coprocessor segment overrun",
  "Bad TSS",
  "Segment not present",
  "Stack fault",
  "General Protection Fault",
  "Page Fault",
  "Unknown interrupt exception",
  "coprocessor fault",
  "alignment check exception",
  "machine check exception",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
};

__attribute__((noreturn)) void kpanic (struct registers * registers) {
  //tty_set_attribute(get_attribute(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
  tty_clear();
  puts("KERNEL PANIC:\n");
  if (registers->isr_num < 32) {
    puts((char *)exception[registers->isr_num]);
    putc('\n');
  } else {
    puts("Exception number out of bounds\n");
  }
  //kprintf("ds:          0x%x", registers->ds);

  kprintf("ds:          0x%x\n"
          "edi:         0x%x\n"
          "esi:         0x%x\n"
          "ebp:         0x%x\n"
          "esp:         0x%x\n"
          "ebx:         0x%x\n"
          "edx:         0x%x\n"
          "ecx:         0x%x\n"
          "eax:         0x%x\n"
          "isr_num:     0x%x\n"
          "err_code:    0x%x\n"
          "eip:         0x%x\n"
          "eflags:      0x%x\n"
          "useresp:     0x%x\n"
          "ss:          0x%x\n",
    registers->ds, registers->edi,
    registers->esi, registers->ebp,
    registers->esp, registers->ebx,
    registers->edx, registers->ecx,
    registers->eax, registers->isr_num,
    registers->err_code, registers->eip,
    registers->cs, registers->eflags,
    registers->useresp, registers->ss
  );

  halt();
}

__attribute__((optimize(0))) static void stacksmash() {
  putc('A');
  char buffer[10];
  memcpy(buffer, (char *)"AAAAAAAAAAAAAAABBBB", 20);
  putc('B');
  putc('\n');
}

// TODO: this is hardcoded and yeah this is bad (even tho its a terminator canary)
// TODO: implement constructors so we can somewhat randomize this
uintptr_t __stack_chk_guard = 0x0a0dFF00;
__attribute__((noreturn)) void __stack_chk_fail() {
  puts("Someone tried to smash the stack on his own head :(");
  halt();
}

extern uint32_t end;
extern uint32_t start;

int kmain (multiboot_info_t * mbi, uint32_t stack_size, uintptr_t esp) {
  // Initialise Segmentation
  gdt_init();

  // Initialise Interrupt handlers
  idt_install();

  // Catch all exceptions with a kernel panic
  for (int i = 0; i < 32; i++) {
    set_isr_handler(i, kpanic);
  }

  // Initialise standard output
  tty_init((uintptr_t)mbi->framebuffer_addr, mbi->framebuffer_bpp, mbi->framebuffer_width, mbi->framebuffer_height, mbi->framebuffer_pitch, mbi->framebuffer_type);

  // Print our logo
  tty_set_foreground_color((struct color){.r=0x00, .g=0x7E, .b=0xA7});
  kprintf("\n"
          "________________________________________________________________________________\n\n"
          "                 brainOS - v0.1 GNU GPL Licence v3.0 2016 bauen1                \n"
          "________________________________________________________________________________\n\n");
  tty_set_foreground_color((struct color){.r=0x00, .g=0x00, .b=0x00});


  // calculate available memory
  kprintf("calculated memory (from mboot->mem_upper + mboot->mem_lower):    %dkb\n", mbi->mem_upper + mbi->mem_lower);

  //
  keyboard_install();
  mouse_init();

  // setup the clock to fire every second (1hz)
  time_init(1);

  uint32_t _end = (uint32_t)&end;

  if (mbi->mods_count > 0) {
    multiboot_module_t * mod_info;
    for (uint32_t i = 0; i < mbi->mods_count; i++) {
      mod_info = (multiboot_module_t *)(mbi->mods_addr + 0x10 * i);
      if (_end < mod_info->mod_end) {

        _end = mod_info->mod_end;
      } else {
        // if the above statement isn't true then the module is either loaded inside our kernel
        // or below it
        // in either case its no good, so warn the user that the kernel could crash
        kprintf("WARNING: module loaded below or inside our kernel!\n");
      }
    }
  }

  // Round up to full blocks
  _end = _end + 4096 - (_end % 4096);

  pmm_init(mbi->mem_upper * 1024, _end);

  // Parse the memory map supplied to us by grub
  multiboot_memory_map_t * mmap;
  for (mmap = (multiboot_memory_map_t *)mbi->mmap_addr;
    (((uint32_t) mmap) < (mbi->mmap_addr + mbi->mmap_length));
    mmap = (multiboot_memory_map_t *) ((uint32_t)(mmap + mmap->size + sizeof(mmap->size))) ){

    if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
      pmm_free_region(mmap->addr, mmap->len);
    } else {
      pmm_alloc_region(mmap->addr, mmap->len);
    }
  }

  // memory below 1 mb is for special purpose (virtual 8086 mode)
  pmm_alloc_region(0x00000000, 0x00100000);

  // The length of the kernel from start to end
  uint32_t kernel_length = (((uint32_t)&end) - ((uint32_t)&start));

  // lets just say we don't want to "allocate" the space where our code lives
  pmm_alloc_region((uint32_t)&start, kernel_length);

  // Initialise Paging (this just identity maps 4GBs atm)
  vmm_init();

  if (mbi->mods_count > 0) {
    multiboot_module_t * module_info;
    kprintf("Trying to load %d module(s)\n", mbi->mods_count);

    for (uint32_t i = 0; i < mbi->mods_count; i++) {
      module_info = (multiboot_module_t *)(mbi->mods_addr + 0x10 * i);
      kprintf("module info:\n"
          "mod_start:   0x%x\n"
          "mod_end:     0x%x\n"
          "commandline: '%s'\n",
        module_info->mod_start,
        module_info->mod_end,
        (char *)module_info->cmdline);
      elf_load((void *)module_info->mod_start);
    }
  } else {
    kprintf("No modules loaded!\n");
  }

  // enable interrupts
  interrupts_enable();

  //rtl8139_init();

  char buffer[1024];
  while (true) {
    putc('>');
    getsn(&buffer[0], 1024);
    puts("You typed: ");
    puts(buffer);
    puts("\n");
    if (strncmp(buffer, "modules", 7) == 0) {
      printf("number of modules: %d\n", (int)mbi->mods_count);
    } else if (strncmp(buffer, "wait", 4) == 0) {
      puts("Waiting for 10 seconds\n");
      _wait(2 * 18.22 * 10);
    } else if (strncmp(buffer, "listpci", 7) == 0) {
      pci_list();
    } else if (strncmp(buffer, "panic", 5) == 0) {
      __asm__ __volatile__ ("int $0"); // pretend a division by zero
    } else if (strncmp(buffer, "stacksmash", 10) == 0) {
      stacksmash();
    } else if (strncmp(buffer, "clear", 5) == 0) {
      tty_clear();
    } else if (strncmp(buffer, "assert", 6) == 0) {
      assert(false);
    } else if (strncmp(buffer, "help", 4) == 0) {
      kprintf("Available commands:\n"
          "help:        prints a list of available commands\n"
          "modules:     prints the number of modules loaded by grub\n"
          "listpci:     lists all attached pci devices\n"
          "panic:       causes a kernel panic by triggering a interrupt 0\n"
          "stacksmash:  tests the stack smash protection will cause a kernel panic\n"
          "clear:       clears the screen\n"
          "assert:      causes a assertion fail\n"
        );
    }
  }

  return 1;
}
