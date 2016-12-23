#include "vmm.h"

extern void load_page_directory(uint32_t page_directory);
extern void enable_paging();

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_tables[1024][1024] __attribute__((aligned(4096)));

void vmm_init() { // Identity map 4GBs

  for(int i = 0; i < 1024; i++)
  {
      page_directory[i] = 0x00000002; // write enabled, not present
  }

  for (unsigned int j = 0; j < 1024; j++) { // Map 4GBs
    for(unsigned int i = 0; i < 1024; i++)
    {
        page_tables[j][i] = ((j * 1024 * 0x1000) + (i * 0x1000)) | 3; // supervisor level, write, present
    }

    page_directory[j] = ((unsigned int)page_tables[j]) | 3;
  }

  puthex("page_directory: ", page_directory);
  load_page_directory(page_directory);
  enable_paging();
}
