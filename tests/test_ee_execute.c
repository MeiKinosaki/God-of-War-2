#include "ee_execute.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint32_t i_type(uint8_t opcode, uint8_t rs, uint8_t rt, uint16_t imm)
{
    return ((uint32_t)opcode << 26) | ((uint32_t)rs << 21) |
           ((uint32_t)rt << 16) | (uint32_t)imm;
}

static uint32_t r_type(uint8_t rs, uint8_t rt, uint8_t rd,
                       uint8_t shamt, uint8_t funct)
{
    return ((uint32_t)rs << 21) | ((uint32_t)rt << 16) |
           ((uint32_t)rd << 11) | ((uint32_t)shamt << 6) | funct;
}

int main(void)
{
    ee_cpu_state cpu;
    uint8_t storage[32];
    ee_memory memory;

    ee_cpu_reset(&cpu);
    cpu.gpr[1] = 10;
    assert(ee_execute_instruction(&cpu, i_type(0x09, 1, 2, 5)) == EE_STEP_OK);
    assert(cpu.gpr[2] == 15 && cpu.pc == 4 && cpu.next_pc == 8);
    assert(ee_execute_instruction(&cpu, i_type(0x09, 1, 3, 0xFFFF)) == EE_STEP_OK);
    assert(cpu.gpr[3] == 9);

    /* DADDIU retains the 64-bit source value and sign-extends its immediate. */
    ee_cpu_reset(&cpu);
    cpu.gpr[1] = UINT64_C(0x00000001FFFFFFFF);
    assert(ee_execute_instruction(&cpu, i_type(0x19, 1, 2, 1)) == EE_STEP_OK);
    assert(cpu.gpr[2] == UINT64_C(0x0000000200000000));
    assert(ee_execute_instruction(&cpu, i_type(0x19, 1, 3, 0xFFFF)) == EE_STEP_OK);
    assert(cpu.gpr[3] == UINT64_C(0x00000001FFFFFFFE));

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

    ee_cpu_reset(&cpu);
    cpu.gpr[1] = UINT32_C(0x7FFFFFFF);
    cpu.gpr[2] = 1;
    assert(ee_execute_instruction(&cpu, r_type(1, 2, 3, 0, 0x21)) == EE_STEP_OK);
    assert(cpu.gpr[3] == UINT64_C(0xFFFFFFFF80000000));
    assert(ee_execute_instruction(&cpu, r_type(1, 2, 4, 0, 0x23)) == EE_STEP_OK);
    assert(cpu.gpr[4] == UINT64_C(0x7FFFFFFE));
    assert(ee_execute_instruction(&cpu, r_type(1, 2, 5, 0, 0x24)) == EE_STEP_OK);
    assert(cpu.gpr[5] == 1);
    assert(ee_execute_instruction(&cpu, r_type(1, 2, 6, 0, 0x25)) == EE_STEP_OK);
    assert(cpu.gpr[6] == UINT32_C(0x7FFFFFFF));
    assert(ee_execute_instruction(&cpu, r_type(1, 2, 7, 0, 0x26)) == EE_STEP_OK);
    assert(cpu.gpr[7] == UINT32_C(0x7FFFFFFE));
    assert(ee_execute_instruction(&cpu, r_type(1, 2, 8, 0, 0x27)) == EE_STEP_OK);
    assert(cpu.gpr[8] == UINT64_C(0xFFFFFFFF80000000));
    assert(ee_execute_instruction(&cpu, r_type(0, 2, 9, 4, 0x00)) == EE_STEP_OK);
    assert(cpu.gpr[9] == 16);

    ee_cpu_reset(&cpu);
    cpu.gpr[1] = cpu.gpr[2] = 7;
    assert(ee_execute_instruction(&cpu, i_type(0x04, 1, 2, 2)) == EE_STEP_OK);
    assert(cpu.pc == 4 && cpu.next_pc == 12 && cpu.branch_pending == 1);
    assert(ee_execute_instruction(&cpu, i_type(0x09, 0, 3, 42)) == EE_STEP_OK);
    assert(cpu.gpr[3] == 42 && cpu.pc == 12 && cpu.next_pc == 16 && !cpu.branch_pending);

    ee_cpu_reset(&cpu);
    cpu.gpr[1] = cpu.gpr[2] = 7;
    assert(ee_execute_instruction(&cpu, i_type(0x05, 1, 2, 3)) == EE_STEP_OK);
    assert(cpu.pc == 4 && cpu.next_pc == 8 && cpu.branch_pending == 1);
    assert(ee_execute_instruction(&cpu, i_type(0x09, 0, 3, 11)) == EE_STEP_OK);
    assert(cpu.pc == 8 && cpu.next_pc == 12 && !cpu.branch_pending);

    ee_cpu_reset(&cpu);
    assert(ee_execute_instruction(&cpu, (0x03u << 26) | 4u) == EE_STEP_OK);
    assert(cpu.gpr[31] == 8 && cpu.pc == 4 && cpu.next_pc == 16);
    assert(ee_execute_instruction(&cpu, i_type(0x09, 0, 3, 1)) == EE_STEP_OK);
    assert(cpu.pc == 16 && cpu.next_pc == 20 && !cpu.branch_pending);

    ee_cpu_reset(&cpu);
    cpu.gpr[5] = 0x100;
    assert(ee_execute_instruction(&cpu, r_type(5, 0, 0, 0, 0x08)) == EE_STEP_OK);
    assert(cpu.pc == 4 && cpu.next_pc == 0x100 && cpu.branch_pending == 1);
    assert(ee_execute_instruction(&cpu, i_type(0x09, 0, 3, 2)) == EE_STEP_OK);
    assert(cpu.pc == 0x100 && cpu.next_pc == 0x104 && !cpu.branch_pending);

    memset(storage, 0, sizeof(storage));
    assert(ee_memory_init(&memory, storage, sizeof(storage)));
    ee_cpu_reset(&cpu);
    cpu.gpr[1] = 4;
    cpu.gpr[2] = UINT64_C(0xFFFFFFFF89ABCDEF);
    assert(ee_execute_instruction_with_memory(&cpu, i_type(0x2B, 1, 2, 0), &memory) == EE_STEP_OK);
    assert(storage[4] == 0xEF && storage[5] == 0xCD &&
           storage[6] == 0xAB && storage[7] == 0x89);
    assert(ee_execute_instruction_with_memory(&cpu, i_type(0x23, 1, 3, 0), &memory) == EE_STEP_OK);
    assert(cpu.gpr[3] == UINT64_C(0xFFFFFFFF89ABCDEF));

    {
        uint32_t old_pc = cpu.pc, old_next_pc = cpu.next_pc;
        cpu.gpr[1] = 31;
        assert(ee_execute_instruction_with_memory(&cpu, i_type(0x23, 1, 3, 0), &memory) == EE_STEP_MEMORY_FAULT);
        assert(cpu.pc == old_pc && cpu.next_pc == old_next_pc);
        assert(ee_execute_instruction(&cpu, i_type(0x23, 1, 3, 0)) == EE_STEP_UNSUPPORTED);
    }

    cpu.gpr[0] = 123;
    assert(ee_execute_instruction(&cpu, i_type(0x09, 0, 0, 99)) == EE_STEP_OK);
    assert(cpu.gpr[0] == 0);
    assert(ee_execute_instruction(NULL, 0) == EE_STEP_INVALID_ARGUMENT);
    puts("EE execute tests passed");
    return 0;
}
