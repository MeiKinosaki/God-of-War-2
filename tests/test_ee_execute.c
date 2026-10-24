#include "ee_execute.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint32_t i_type(uint8_t opcode, uint8_t rs, uint8_t rt, uint16_t imm)
{
    return ((uint32_t)opcode << 26) |
           ((uint32_t)rs << 21) |
           ((uint32_t)rt << 16) |
           (uint32_t)imm;
}

int main(void)
{
    ee_cpu_state cpu;

    ee_cpu_reset(&cpu);
    cpu.gpr[1] = 10;
    assert(ee_execute_instruction(&cpu, i_type(0x09, 1, 2, 5)) == EE_STEP_OK);
    assert(cpu.gpr[2] == 15);
    assert(cpu.pc == 4 && cpu.next_pc == 8);

    assert(ee_execute_instruction(&cpu, i_type(0x09, 1, 3, 0xFFFF)) == EE_STEP_OK);
    assert(cpu.gpr[3] == 9); /* ADDIU sign-extends the immediate. */

    cpu.gpr[4] = UINT64_C(0xF0F0);
    assert(ee_execute_instruction(&cpu, i_type(0x0C, 4, 5, 0x00FF)) == EE_STEP_OK);
    assert(cpu.gpr[5] == 0x00F0);
    assert(ee_execute_instruction(&cpu, i_type(0x0D, 4, 6, 0x000F)) == EE_STEP_OK);
    assert(cpu.gpr[6] == 0xF0FF);
    assert(ee_execute_instruction(&cpu, i_type(0x0E, 4, 7, 0x00FF)) == EE_STEP_OK);
    assert(cpu.gpr[7] == 0xF00F);

    assert(ee_execute_instruction(&cpu, i_type(0x0F, 0, 8, 0x8000)) == EE_STEP_OK);
    assert(cpu.gpr[8] == UINT64_C(0xFFFFFFFF80000000));

    cpu.gpr[0] = 123;
    assert(ee_execute_instruction(&cpu, i_type(0x09, 0, 0, 99)) == EE_STEP_OK);
    assert(cpu.gpr[0] == 0);

    {
        uint32_t old_pc = cpu.pc;
        uint32_t old_next_pc = cpu.next_pc;
        assert(ee_execute_instruction(&cpu, i_type(0x23, 0, 1, 0)) == EE_STEP_UNSUPPORTED);
        assert(cpu.pc == old_pc && cpu.next_pc == old_next_pc);
    }

    assert(ee_execute_instruction(NULL, 0) == EE_STEP_INVALID_ARGUMENT);
    puts("EE execute tests passed");
    return 0;
}
