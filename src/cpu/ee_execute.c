#include "ee_execute.h"

#include <stddef.h>

static int64_t sign_extend_16(uint16_t value)
{
    return (int64_t)(int16_t)value;
}

static void advance_after_instruction(ee_cpu_state *state, uint32_t old_pc,
                                      uint32_t old_next_pc, uint8_t was_delay_slot)
{
    if (was_delay_slot != 0u) {
        state->pc = old_next_pc;
        state->next_pc = state->pc + 4u;
        state->branch_pending = 0u;
    } else if (state->branch_pending != 0u) {
        state->pc = old_next_pc;
        /* Keep next_pc as the branch/jump target through the delay slot. */
    } else {
        state->pc = old_next_pc;
        state->next_pc = state->pc + 4u;
    }
    (void)old_pc;
}

ee_step_result ee_execute_instruction(ee_cpu_state *state, uint32_t instruction)
{
    ee_decoded_instruction d;
    uint64_t lhs;
    uint64_t rhs;
    uint64_t result;
    int64_t signed_immediate;
    uint32_t old_pc;
    uint32_t old_next_pc;
    uint8_t was_delay_slot;

    if (state == NULL)
        return EE_STEP_INVALID_ARGUMENT;

    d = ee_decode_instruction(instruction);
    if (d.kind == EE_OP_INVALID)
        return EE_STEP_UNSUPPORTED;

    old_pc = state->pc;
    old_next_pc = state->next_pc;
    was_delay_slot = state->branch_pending;
    lhs = state->gpr[d.rs];
    rhs = state->gpr[d.rt];
    signed_immediate = sign_extend_16(d.immediate);

    switch (d.kind) {
    case EE_OP_ADDIU:
        result = lhs + (uint64_t)signed_immediate;
        if (d.rt != 0u)
            state->gpr[d.rt] = result;
        break;
    case EE_OP_SLTI:
        if (d.rt != 0u)
            state->gpr[d.rt] = ((int64_t)lhs < signed_immediate) ? 1u : 0u;
        break;
    case EE_OP_SLTIU:
        if (d.rt != 0u)
            state->gpr[d.rt] = (lhs < (uint64_t)signed_immediate) ? 1u : 0u;
        break;
    case EE_OP_ANDI:
        if (d.rt != 0u)
            state->gpr[d.rt] = lhs & (uint64_t)d.immediate;
        break;
    case EE_OP_ORI:
        if (d.rt != 0u)
            state->gpr[d.rt] = lhs | (uint64_t)d.immediate;
        break;
    case EE_OP_XORI:
        if (d.rt != 0u)
            state->gpr[d.rt] = lhs ^ (uint64_t)d.immediate;
        break;
    case EE_OP_LUI:
        if (d.rt != 0u) {
            uint32_t value32 = (uint32_t)d.immediate << 16;
            state->gpr[d.rt] = (uint64_t)(int64_t)(int32_t)value32;
        }
        break;
    case EE_OP_BEQ:
    case EE_OP_BNE:
        if (was_delay_slot != 0u)
            return EE_STEP_UNSUPPORTED;
        if ((d.kind == EE_OP_BEQ && lhs == rhs) ||
            (d.kind == EE_OP_BNE && lhs != rhs)) {
            uint32_t displacement = (uint32_t)(signed_immediate * 4);
            state->next_pc = old_pc + 4u + displacement;
            state->branch_pending = 1u;
        }
        break;
    case EE_OP_J:
    case EE_OP_JAL:
        if (was_delay_slot != 0u)
            return EE_STEP_UNSUPPORTED;
        if (d.kind == EE_OP_JAL)
            state->gpr[31] = (uint64_t)(old_pc + 8u);
        state->next_pc = ((old_pc + 4u) & 0xF0000000u) | (d.target << 2);
        state->branch_pending = 1u;
        break;
    default:
        return EE_STEP_UNSUPPORTED;
    }

    state->gpr[0] = 0;
    advance_after_instruction(state, old_pc, old_next_pc, was_delay_slot);
    return EE_STEP_OK;
}
