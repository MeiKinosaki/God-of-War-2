#include "ee_execute.h"

#include <stddef.h>

static int64_t sign_extend_16(uint16_t value)
{
    return (int64_t)(int16_t)value;
}

static void advance_sequential_pc(ee_cpu_state *state)
{
    state->pc = state->next_pc;
    state->next_pc = state->pc + 4u;
}

ee_step_result ee_execute_instruction(ee_cpu_state *state, uint32_t instruction)
{
    ee_decoded_instruction d;
    uint64_t lhs;
    uint64_t result;
    int64_t signed_immediate;

    if (state == NULL)
        return EE_STEP_INVALID_ARGUMENT;

    d = ee_decode_instruction(instruction);
    if (d.kind == EE_OP_INVALID)
        return EE_STEP_UNSUPPORTED;

    lhs = state->gpr[d.rs];
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
    default:
        return EE_STEP_UNSUPPORTED;
    }

    /* This stage intentionally models sequential execution only. Branches,
     * delay slots, exceptions and full R5900-width semantics remain pending.
     */
    state->gpr[0] = 0;
    advance_sequential_pc(state);
    return EE_STEP_OK;
}
