#include "ee_state.h"

#include <stddef.h>

void ee_cpu_reset(ee_cpu_state *state)
{
    size_t i;
    if (state == NULL)
        return;

    for (i = 0; i < 32; ++i)
        state->gpr[i] = 0;

    state->hi = 0;
    state->lo = 0;
    state->pc = 0;
    state->next_pc = 4;
    state->branch_pending = 0;
    state->reserved[0] = 0;
    state->reserved[1] = 0;
    state->reserved[2] = 0;
}
