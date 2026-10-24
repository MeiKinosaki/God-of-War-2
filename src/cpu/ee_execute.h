#ifndef GOW2_EE_EXECUTE_H
#define GOW2_EE_EXECUTE_H

#include <stdint.h>
#include "ee_state.h"
#include "ee_decode.h"

typedef enum {
    EE_STEP_OK = 0,
    EE_STEP_UNSUPPORTED,
    EE_STEP_MEMORY_FAULT,
    EE_STEP_INVALID_ARGUMENT
} ee_step_result;

/* Executes a deliberately limited subset of EE instructions.
 * The caller supplies instruction bits directly; instruction fetch and the
 * full PS2 exception/branch-delay model are not implemented in this stage.
 */
ee_step_result ee_execute_instruction(ee_cpu_state *state, uint32_t instruction);

#endif
