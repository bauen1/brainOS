#ifndef PMM_H
#define PMM_H

#include <stddef.h>
#include <stdint.h>

#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE

void pmm_free_region(uint32_t base, size_t size);
void pmm_alloc_region(uint32_t base, size_t size);
void * pmm_alloc_block();
void pmm_free_block(void * p);
void pmm_init(size_t memory_size, uint32_t mem_map);

uint32_t pmm_get_pmm_used_blocks();

#endif
