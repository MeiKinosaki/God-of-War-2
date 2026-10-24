#ifndef GOW2_EE_MEMORY_H
#define GOW2_EE_MEMORY_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *bytes;
    size_t size;
} ee_memory;

/* Bind caller-owned storage. No allocation is performed. */
int ee_memory_init(ee_memory *memory, uint8_t *storage, size_t size);
int ee_memory_read8(const ee_memory *memory, uint32_t address, uint8_t *value);
int ee_memory_read32(const ee_memory *memory, uint32_t address, uint32_t *value);
int ee_memory_write8(ee_memory *memory, uint32_t address, uint8_t value);
int ee_memory_write32(ee_memory *memory, uint32_t address, uint32_t value);

#endif
