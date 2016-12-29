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

static int pmm_find_first_free_region (size_t size) {
  if (size == 0) {
    return -1;
  }

  if (size == 1) {
    return pmm_find_first_free();
  }

  for (uint32_t i = 0; i < pmm_mem_size; i++) {
    if (pmm_mem_map[i] != 0xFFFFFFFF) {
      for (uint8_t j = 0; j < 32; j++) {
        uint32_t bit = (1 << j);
        if (! (pmm_mem_map[i] & bit)) {
          uint32_t base = i * 32 + j;
          size_t len = 0;

          for (uint32_t count = 0; count <= size; count++) {
            if (! pmm_mem_test(base + count)) {
              len++;
            }
            if (len == size) {
              return i * 32 + j;
            }
          }
        }
      }
    }
  }
  return -1;
}

void * pmm_alloc_block() {
  int block = pmm_find_first_free();
  if (block == -1) {
    return NULL; // Out Of Memory
  }

  pmm_mem_set(block);
  pmm_used_blocks++;

  return (void *)(block * PMM_BLOCK_SIZE);
}

void pmm_free_block(void * p) {
  uint32_t base_aligned = ((uint32_t)p) / PMM_BLOCK_ALIGN;

  pmm_mem_unset(base_aligned);
  pmm_used_blocks--;
}

void * pmm_alloc_blocks(size_t size) {
  if (size == 0) {
    return NULL;
  }

  int block = pmm_find_first_free_region(size);
  if (block == -1) {
    return NULL; // Out Of Memory / No block big enough
  }

  for (size_t i = 0; i < size; i++) {
    puthex("block + i: ", block + i);
    pmm_mem_set(block + i);
    pmm_used_blocks++;
  }

  return (void *)(block * PMM_BLOCK_SIZE);
}

void pmm_free_blocks(void * p, size_t size) {
  if (size == 0) {
    return;
  }

  uint32_t base_aligned = ((uint32_t)p) / PMM_BLOCK_ALIGN;

  for (uint32_t i = 0; i < size; i++) {
    puthex("block + i: ", base_aligned + i);
    pmm_mem_unset(base_aligned + i);
    pmm_used_blocks--;
  }
}

void pmm_init(size_t memory_size, uint32_t mem_map) {
  pmm_mem_size = memory_size / 8;
  pmm_mem_map = (uint32_t *)mem_map;
  pmm_used_blocks = 0;

  memset(pmm_mem_map, 0xff, pmm_mem_size); // by default all blocks are allocated
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

uint32_t pmm_get_pmm_used_blocks() {
  return pmm_used_blocks;
}
