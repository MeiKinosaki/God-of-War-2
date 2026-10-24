#include "ee_memory.h"

int ee_memory_init(ee_memory *memory, uint8_t *storage, size_t size)
{
    if (memory == 0 || storage == 0 || size == 0)
        return 0;
    memory->bytes = storage;
    memory->size = size;
    return 1;
}

int ee_memory_read8(const ee_memory *memory, uint32_t address, uint8_t *value)
{
    if (memory == 0 || memory->bytes == 0 || value == 0 ||
        (size_t)address >= memory->size)
        return 0;
    *value = memory->bytes[address];
    return 1;
}

int ee_memory_read32(const ee_memory *memory, uint32_t address, uint32_t *value)
{
    const uint8_t *p;
    if (memory == 0 || memory->bytes == 0 || value == 0 ||
        (address & 3u) != 0 || (size_t)address > memory->size ||
        memory->size - (size_t)address < 4)
        return 0;
    p = &memory->bytes[address];
    *value = (uint32_t)p[0] |
             ((uint32_t)p[1] << 8) |
             ((uint32_t)p[2] << 16) |
             ((uint32_t)p[3] << 24);
    return 1;
}

int ee_memory_write8(ee_memory *memory, uint32_t address, uint8_t value)
{
    if (memory == 0 || memory->bytes == 0 ||
        (size_t)address >= memory->size)
        return 0;
    memory->bytes[address] = value;
    return 1;
}

int ee_memory_write32(ee_memory *memory, uint32_t address, uint32_t value)
{
    uint8_t *p;
    if (memory == 0 || memory->bytes == 0 ||
        (address & 3u) != 0 || (size_t)address > memory->size ||
        memory->size - (size_t)address < 4)
        return 0;
    p = &memory->bytes[address];
    p[0] = (uint8_t)(value & 0xFFu);
    p[1] = (uint8_t)((value >> 8) & 0xFFu);
    p[2] = (uint8_t)((value >> 16) & 0xFFu);
    p[3] = (uint8_t)((value >> 24) & 0xFFu);
    return 1;
}
