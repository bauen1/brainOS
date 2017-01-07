#include "elf_loader.h"
#include <stdint.h>
#include "string.h"

void * elf_load(void * loc) {
  Elf32_Ehdr_t * header = (Elf32_Ehdr_t *)loc;
  return NULL;
}
