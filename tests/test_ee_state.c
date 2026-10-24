#include "ee_state.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    ee_cpu_state s;
    unsigned i;

    for (i = 0; i < 32; ++i)
        s.gpr[i] = 0xFFFFFFFFFFFFFFFFULL;
    s.hi = s.lo = 0xFFFFFFFFFFFFFFFFULL;
    s.pc = 0xFFFFFFFFu;
    s.next_pc = 0xFFFFFFFFu;
    s.branch_pending = 1;
    s.reserved[0] = s.reserved[1] = s.reserved[2] = 0xFF;

    ee_cpu_reset(&s);
    for (i = 0; i < 32; ++i)
        assert(s.gpr[i] == 0);
    assert(s.hi == 0 && s.lo == 0);
    assert(s.pc == 0 && s.next_pc == 4);
    assert(s.branch_pending == 0);
    assert(s.reserved[0] == 0 && s.reserved[1] == 0 && s.reserved[2] == 0);

    ee_cpu_reset(0);
    puts("EE state reset tests passed");
    return 0;
}
