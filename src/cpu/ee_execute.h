#ifndef GOW2_EE_EXECUTE_H
#define GOW2_EE_EXECUTE_H

#include <stdint.h>
#include "ee_state.h"
#include "ee_decode.h"
#include "ee_memory.h"

typedef enum {
    EE_STEP_OK = 0,
    EE_STEP_UNSUPPORTED,
    EE_STEP_MEMORY_FAULT,
    EE_STEP_INVALID_ARGUMENT
} ee_step_result;

/* Limited instruction subset. The plain entry point intentionally has no
 * memory access; LW/SW require the explicit memory-aware entry point.
 */
ee_step_result ee_execute_instruction(ee_cpu_state *state, uint32_t instruction);
ee_step_result ee_execute_instruction_with_memory(ee_cpu_state *state,
                                                   uint32_t instruction,
                                                   ee_memory *memory);

#endif
