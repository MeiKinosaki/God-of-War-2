#ifndef GOW2_EE_STATE_H
#define GOW2_EE_STATE_H

#include <stdint.h>

typedef struct {
    uint64_t gpr[32];
    uint64_t hi;
    uint64_t lo;
    uint32_t pc;
    uint32_t next_pc;
    uint8_t branch_pending;
    uint8_t reserved[3];
} ee_cpu_state;

/* Reset the modeled CPU state to deterministic initial values.
 * This is a software model baseline, not a full PS2 hardware reset sequence.
 */
void ee_cpu_reset(ee_cpu_state *state);

#endif
