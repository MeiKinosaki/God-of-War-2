#include "ee_decode.h"

ee_decoded_instruction ee_decode_instruction(uint32_t word)
{
    ee_decoded_instruction d = {0};
    d.opcode = (uint8_t)((word >> 26) & 0x3Fu);
    d.rs = (uint8_t)((word >> 21) & 0x1Fu);
    d.rt = (uint8_t)((word >> 16) & 0x1Fu);
    d.rd = (uint8_t)((word >> 11) & 0x1Fu);
    d.shamt = (uint8_t)((word >> 6) & 0x1Fu);
    d.funct = (uint8_t)(word & 0x3Fu);
    d.immediate = (uint16_t)(word & 0xFFFFu);
    d.target = word & 0x03FFFFFFu;

    switch (d.opcode) {
    case 0x00:
        /* This only identifies the SPECIAL encoding class. The funct field
         * still needs a separate, validated R5900 instruction table. */
        d.kind = EE_OP_SPECIAL;
        break;
    case 0x02: d.kind = EE_OP_J; break;
    case 0x03: d.kind = EE_OP_JAL; break;
    case 0x04: d.kind = EE_OP_BEQ; break;
    case 0x05: d.kind = EE_OP_BNE; break;
    case 0x09: d.kind = EE_OP_ADDIU; break;
    case 0x0A: d.kind = EE_OP_SLTI; break;
    case 0x0B: d.kind = EE_OP_SLTIU; break;
    case 0x0C: d.kind = EE_OP_ANDI; break;
    case 0x0D: d.kind = EE_OP_ORI; break;
    case 0x0E: d.kind = EE_OP_XORI; break;
    case 0x0F: d.kind = EE_OP_LUI; break;
    case 0x23: d.kind = EE_OP_LW; break;
    case 0x2B: d.kind = EE_OP_SW; break;
    default: d.kind = EE_OP_INVALID; break;
    }
    return d;
}
