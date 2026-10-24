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
    assert(cpu.gpr[3] == 9);

    cpu.gpr[1] = 5;
    assert(ee_execute_instruction(&cpu, i_type(0x0A, 1, 9, 6)) == EE_STEP_OK);
    assert(cpu.gpr[9] == 1);
    assert(ee_execute_instruction(&cpu, i_type(0x0A, 1, 9, 4)) == EE_STEP_OK);
    assert(cpu.gpr[9] == 0);
    assert(ee_execute_instruction(&cpu, i_type(0x0A, 1, 9, 0xFFFF)) == EE_STEP_OK);
    assert(cpu.gpr[9] == 0);

    cpu.gpr[1] = UINT64_MAX;
    assert(ee_execute_instruction(&cpu, i_type(0x0B, 1, 10, 1)) == EE_STEP_OK);
    assert(cpu.gpr[10] == 0);
    cpu.gpr[1] = 0;
    assert(ee_execute_instruction(&cpu, i_type(0x0B, 1, 10, 0xFFFF)) == EE_STEP_OK);
    assert(cpu.gpr[10] == 1);

    cpu.gpr[4] = UINT64_C(0xF0F0);
    assert(ee_execute_instruction(&cpu, i_type(0x0C, 4, 5, 0x00FF)) == EE_STEP_OK);
    assert(cpu.gpr[5] == 0x00F0);
    assert(ee_execute_instruction(&cpu, i_type(0x0D, 4, 6, 0x000F)) == EE_STEP_OK);
    assert(cpu.gpr[6] == 0xF0FF);
    assert(ee_execute_instruction(&cpu, i_type(0x0E, 4, 7, 0x00FF)) == EE_STEP_OK);
    assert(cpu.gpr[7] == 0xF00F);

    assert(ee_execute_instruction(&cpu, i_type(0x0F, 0, 8, 0x8000)) == EE_STEP_OK);
    assert(cpu.gpr[8] == UINT64_C(0xFFFFFFFF80000000));

    /* Taken BEQ executes the next instruction as its delay slot. */
    ee_cpu_reset(&cpu);
    cpu.gpr[1] = 7;
    cpu.gpr[2] = 7;
    assert(ee_execute_instruction(&cpu, i_type(0x04, 1, 2, 2)) == EE_STEP_OK);
    assert(cpu.pc == 4 && cpu.next_pc == 12 && cpu.branch_pending == 1);
    assert(ee_execute_instruction(&cpu, i_type(0x09, 0, 3, 42)) == EE_STEP_OK);
    assert(cpu.gpr[3] == 42);
    assert(cpu.pc == 12 && cpu.next_pc == 16 && cpu.branch_pending == 0);

    /* Not-taken BNE still reserves the delay slot. */
    ee_cpu_reset(&cpu);
    cpu.gpr[1] = 7;
    cpu.gpr[2] = 7;
    assert(ee_execute_instruction(&cpu, i_type(0x05, 1, 2, 3)) == EE_STEP_OK);
    assert(cpu.pc == 4 && cpu.next_pc == 8 && cpu.branch_pending == 1);
    assert(ee_execute_instruction(&cpu, i_type(0x09, 0, 3, 11)) == EE_STEP_OK);
    assert(cpu.pc == 8 && cpu.next_pc == 12 && cpu.branch_pending == 0);
    
    /* JAL stores PC+8 and transfers control after its delay slot. */
    ee_cpu_reset(&cpu);
    assert(ee_execute_instruction(&cpu, (0x03u << 26) | 4u) == EE_STEP_OK);
    assert(cpu.gpr[31] == 8 && cpu.pc == 4 && cpu.next_pc == 16);
    assert(ee_execute_instruction(&cpu, i_type(0x09, 0, 3, 1)) == EE_STEP_OK);
    assert(cpu.pc == 16 && cpu.next_pc == 20 && cpu.branch_pending == 0);

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
