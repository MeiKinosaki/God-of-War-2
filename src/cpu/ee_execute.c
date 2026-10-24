#include "ee_execute.h"

#include <stddef.h>

static int64_t sign_extend_16(uint16_t value)
{
    return (int64_t)(int16_t)value;
}

static uint64_t sign_extend_32(uint32_t value)
{
    return (uint64_t)(int64_t)(int32_t)value;
}

static void advance_after_instruction(ee_cpu_state *state, uint32_t old_next_pc,
                                      uint8_t was_delay_slot)
{
    if (was_delay_slot != 0u) {
        state->pc = old_next_pc;
        state->next_pc = state->pc + 4u;
        state->branch_pending = 0u;
    } else {
        state->pc = old_next_pc;
        if (state->branch_pending == 0u)
            state->next_pc = state->pc + 4u;
    }
}

ee_step_result ee_execute_instruction(ee_cpu_state *state, uint32_t instruction)
{
    ee_decoded_instruction d;
    uint64_t lhs, rhs, result;
    int64_t signed_immediate;
    uint32_t old_pc, old_next_pc;
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
    case EE_OP_SPECIAL:
        switch (d.funct) {
        case 0x00: /* SLL */
            result = sign_extend_32((uint32_t)rhs << d.shamt);
            break;
        case 0x02: /* SRL */
            result = sign_extend_32((uint32_t)rhs >> d.shamt);
            break;
        case 0x08: /* JR */
            if (was_delay_slot != 0u) return EE_STEP_UNSUPPORTED;
            state->next_pc = (uint32_t)lhs;
            state->branch_pending = 1u;
            result = 0;
            break;
        case 0x09: /* JALR */
            if (was_delay_slot != 0u) return EE_STEP_UNSUPPORTED;
            if (d.rd != 0u) state->gpr[d.rd] = (uint64_t)(old_pc + 8u);
            state->next_pc = (uint32_t)lhs;
            state->branch_pending = 1u;
            result = 0;
            break;
        case 0x21: /* ADDU: 32-bit result sign-extended */
            result = sign_extend_32((uint32_t)lhs + (uint32_t)rhs);
            break;
        case 0x23: /* SUBU */
            result = sign_extend_32((uint32_t)lhs - (uint32_t)rhs);
            break;
        case 0x24: result = lhs & rhs; break; /* AND */
        case 0x25: result = lhs | rhs; break; /* OR */
        case 0x26: result = lhs ^ rhs; break; /* XOR */
        case 0x27: result = ~(lhs | rhs); break; /* NOR */
        case 0x2A: result = ((int64_t)lhs < (int64_t)rhs) ? 1u : 0u; break;
        case 0x2B: result = (lhs < rhs) ? 1u : 0u; break;
        default:
            return EE_STEP_UNSUPPORTED;
        }
        if ((d.funct != 0x08u) && (d.funct != 0x09u) && d.rd != 0u)
            state->gpr[d.rd] = result;
        break;
    case EE_OP_ADDIU:
        result = lhs + (uint64_t)signed_immediate;
        if (d.rt != 0u) state->gpr[d.rt] = result;
        break;
    case EE_OP_SLTI:
        if (d.rt != 0u) state->gpr[d.rt] = ((int64_t)lhs < signed_immediate) ? 1u : 0u;
        break;
    case EE_OP_SLTIU:
        if (d.rt != 0u) state->gpr[d.rt] = (lhs < (uint64_t)signed_immediate) ? 1u : 0u;
        break;
    case EE_OP_ANDI:
        if (d.rt != 0u) state->gpr[d.rt] = lhs & (uint64_t)d.immediate;
        break;
    case EE_OP_ORI:
        if (d.rt != 0u) state->gpr[d.rt] = lhs | (uint64_t)d.immediate;
        break;
    case EE_OP_XORI:
        if (d.rt != 0u) state->gpr[d.rt] = lhs ^ (uint64_t)d.immediate;
        break;
    case EE_OP_LUI:
        if (d.rt != 0u)
            state->gpr[d.rt] = sign_extend_32((uint32_t)d.immediate << 16);
        break;
    case EE_OP_BEQ:
    case EE_OP_BNE: {
        int taken;
        if (was_delay_slot != 0u) return EE_STEP_UNSUPPORTED;
        taken = (d.kind == EE_OP_BEQ) ? (lhs == rhs) : (lhs != rhs);
        state->next_pc = taken ? old_pc + 4u + (uint32_t)(signed_immediate * 4)
                               : old_pc + 8u;
        state->branch_pending = 1u;
        break;
    }
    case EE_OP_J:
    case EE_OP_JAL:
        if (was_delay_slot != 0u) return EE_STEP_UNSUPPORTED;
        if (d.kind == EE_OP_JAL) state->gpr[31] = (uint64_t)(old_pc + 8u);
        state->next_pc = ((old_pc + 4u) & 0xF0000000u) | (d.target << 2);
        state->branch_pending = 1u;
        break;
    default:
        return EE_STEP_UNSUPPORTED;
    }

    state->gpr[0] = 0;
    advance_after_instruction(state, old_next_pc, was_delay_slot);
    return EE_STEP_OK;
}
