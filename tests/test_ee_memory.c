#include "ee_memory.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    uint8_t storage[16] = {0};
    uint8_t byte = 0;
    uint32_t word = 0;
    ee_memory memory;

    assert(ee_memory_init(&memory, storage, sizeof storage));
    assert(ee_memory_write32(&memory, 4, 0x1234ABCDu));
    assert(storage[4] == 0xCD && storage[5] == 0xAB);
    assert(storage[6] == 0x34 && storage[7] == 0x12);
    assert(ee_memory_read32(&memory, 4, &word));
    assert(word == 0x1234ABCDu);

    assert(ee_memory_write8(&memory, 15, 0x5A));
    assert(ee_memory_read8(&memory, 15, &byte) && byte == 0x5A);
    assert(!ee_memory_read8(&memory, 16, &byte));
    assert(!ee_memory_write32(&memory, 2, 0xFFFFFFFFu));
    assert(!ee_memory_read32(&memory, 12, &word) == 0); /* aligned final word is valid */
    assert(ee_memory_read32(&memory, 12, &word));
    assert(!ee_memory_write32(&memory, 16, 0));
    assert(!ee_memory_init(0, storage, sizeof storage));

    puts("EE memory tests passed");
    return 0;
}
