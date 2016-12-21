// Physical Memory Manager
#include "pmm.h"
#include "system.h"
#include <stdbool.h>

static uint32_t   pmm_mem_size; // size of pmm_mem_map in bytes
static uint32_t * pmm_mem_map; // Pointer to a bit map 0 = free 1 = used
// TODO: use this to skip already known used areas in pmm_find_first_free
//static uint32_t   pmm_mem_seek;
static uint32_t   pmm_used_blocks = 0;

static inline void pmm_mem_set (uint32_t bit) {
  pmm_mem_map[bit / 32] |= (1 << (bit % 32));
}

static inline void pmm_mem_unset (uint32_t bit) {
  pmm_mem_map[bit / 32] &= ~ (1 << (bit % 32));
}

static inline bool pmm_mem_test (uint32_t bit) {
  return pmm_mem_map[bit / 32] & (1 << (bit % 32));
}

// TODO: optimize this
static int pmm_find_first_free () {
  for (uint32_t i = 0; i < pmm_mem_size; i++) {
    if (pmm_mem_map[i] != 0xFFFFFFFF) {
      for (uint8_t j = 0; j < 32; j++) {
        uint32_t bit = (1 << j);
        if (! (pmm_mem_map[i] & bit)) {
          return i * 32 + j;
        }
      }
    }
  }

  return -1;
}

void pmm_free_region(uint32_t base, size_t size) {
  uint32_t base_aligned = base / PMM_BLOCK_ALIGN;
  uint32_t blocks = size / PMM_BLOCK_SIZE;

  while (blocks > 0) {
    pmm_mem_unset(base_aligned);
    pmm_used_blocks--;
    base_aligned++;
    blocks--;
  }

  pmm_mem_set(0); // make sure that the first block is always
}

void pmm_alloc_region(uint32_t base, size_t size) {
  uint32_t base_aligned = base / PMM_BLOCK_ALIGN;
  uint32_t blocks = size / PMM_BLOCK_SIZE;

  while (blocks > 0) {
    pmm_mem_set(base_aligned);
    pmm_used_blocks++;
    base_aligned++;
    blocks--;
  }
}

void * pmm_alloc_block() {
  int block = pmm_find_first_free();
  if (block == -1) {
    return NULL; // Out Of Memory
  }

  pmm_mem_set(block);
  uint32_t address = block * PMM_BLOCK_SIZE;
  //puts("-----------------------------\n");
  //puthex("address:        ", address);
  //puthex("block:          ", block);
  //puts("-----------------------------\n");
  pmm_used_blocks++;
  return (void *)address;
}

void pmm_free_block(void * p) {
  uint32_t base_aligned = ((uint32_t)p) / PMM_BLOCK_ALIGN;
  //puts("-----------------------------\n");
  //puthex("base_aligned:   ", base_aligned);
  //puthex("p:              ", p);
  //puts("-----------------------------\n");

  pmm_mem_unset(base_aligned);
  pmm_used_blocks--;
}

void pmm_init(size_t memory_size, uint32_t mem_map) {
  puthex("mem_map: ", mem_map);
  pmm_mem_size = memory_size / 8;
  pmm_mem_map = (uint32_t *)mem_map;
  pmm_used_blocks = 0;

  memset(pmm_mem_map, 0xff, pmm_mem_size); // by default all blocks are allocated
}

uint32_t pmm_get_pmm_used_blocks() {
  return pmm_used_blocks;
}
