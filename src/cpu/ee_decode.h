#ifndef GOW2_EE_DECODE_H
#define GOW2_EE_DECODE_H

#include <stdint.h>

typedef enum {
    EE_OP_INVALID = 0,
    EE_OP_SPECIAL,
    EE_OP_J,
    EE_OP_JAL,
    EE_OP_BEQ,
    EE_OP_BNE,
    EE_OP_ADDIU,
    EE_OP_DADDIU,
    EE_OP_SLTI,
    EE_OP_SLTIU,
    EE_OP_ANDI,
    EE_OP_ORI,
    EE_OP_XORI,
    EE_OP_LUI,
    EE_OP_LW,
    EE_OP_SW
} ee_opcode_kind;

typedef struct {
    ee_opcode_kind kind;
    uint8_t opcode;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
    uint8_t funct;
    uint16_t immediate;
    uint32_t target;
} ee_decoded_instruction;

/* Decode a limited subset of 32-bit R5900 instruction encodings. */
ee_decoded_instruction ee_decode_instruction(uint32_t word);

#endif
