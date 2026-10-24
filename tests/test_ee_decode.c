#include "ee_decode.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint32_t i_type(uint8_t op, uint8_t rs, uint8_t rt, uint16_t imm)
{
    return ((uint32_t)op << 26) | ((uint32_t)rs << 21) |
           ((uint32_t)rt << 16) | imm;
}

int main(void)
{
    ee_decoded_instruction d;

    d = ee_decode_instruction(i_type(0x09, 3, 4, 0xFFFE));
    assert(d.kind == EE_OP_ADDIU && d.rs == 3 && d.rt == 4);
    assert(d.immediate == 0xFFFE);

    d = ee_decode_instruction(i_type(0x19, 3, 4, 0xFFFE));
    assert(d.kind == EE_OP_DADDIU && d.rs == 3 && d.rt == 4);
    assert(d.immediate == 0xFFFE);

    d = ee_decode_instruction(i_type(0x0D, 1, 2, 0xABCD));
    assert(d.kind == EE_OP_ORI && d.immediate == 0xABCD);

    d = ee_decode_instruction((0x02u << 26) | 0x00123456u);
    assert(d.kind == EE_OP_J && d.target == 0x00123456u);

    d = ee_decode_instruction(i_type(0x3F, 0, 0, 0));
    assert(d.kind == EE_OP_INVALID);

    puts("EE decoder tests passed");
    return 0;
}
