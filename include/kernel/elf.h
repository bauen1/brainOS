#ifndef ELF_H
#define ELF_H

#include <stdint.h>

// the structs are from:
// http://www.skyfree.org/linux/references/ELF_Format.pdf

typedef uint32_t Elf32_Addr_t;
typedef uint16_t Elf32_Half_t;
typedef uint32_t Elf32_Off_t;
typedef int32_t Elf32_Sword_t;
typedef uint32_t Elf32_Word_t;

#define EI_NIDENT 16

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half_t e_type;
  Elf32_Half_t e_machine;
  Elf32_Word_t e_version;
  Elf32_Addr_t e_entry;
  Elf32_Off_t e_phoff;
  Elf32_Off_t e_shoff;
  Elf32_Word_t e_flags;
  Elf32_Half_t e_ehsize;
  Elf32_Half_t e_phentsize;
  Elf32_Half_t e_phnum;
  Elf32_Half_t e_shentsize;
  Elf32_Half_t e_shnum;
  Elf32_Half_t e_shstrndx;
} __attribute__((packed)) Elf32_Ehdr_t;

typedef struct {
  Elf32_Word_t sh_name;
  Elf32_Word_t sh_type;
  Elf32_Word_t sh_flags;
  Elf32_Addr_t sh_addr;
  Elf32_Off_t sh_offset;
  Elf32_Word_t sh_size;
  Elf32_Word_t sh_link;
  Elf32_Word_t sh_info;
  Elf32_Word_t sh_addralign;
  Elf32_Word_t sh_entsize;
} __attribute__((packed)) Elf32_Shdr_t;

typedef struct {
  Elf32_Word_t st_name;
  Elf32_Addr_t st_value;
  Elf32_Word_t st_size;
  unsigned char st_info;
  unsigned char st_other;
  Elf32_Half_t st_shndx;
} __attribute__((packed)) Elf32_Sym_t;

typedef struct {
  Elf32_Addr_t r_offset;
  Elf32_Word_t r_info;
} __attribute__((packed)) Elf32_Rel_t;

typedef struct {
  Elf32_Addr_t r_offset;
  Elf32_Word_t r_info;
  Elf32_Sword_t r_addend;
} __attribute__((packed)) Elf32_Rela_t;

#define elf_check_magic(elf_header) (elf_header->e_ident[0] == 0x7f) && (elf_header->e_ident[1] == 'E') && (elf_header->e_ident[2] == 'L') && (elf_header->e_ident[3] == 'F')

#endif
