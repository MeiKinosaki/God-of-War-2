# CPU foundation — Emotion Engine (PS2)

This document specifies the first CPU implementation milestone. It is a design and test plan, not a claim that an emulator or game is already running.

## Scope for the first implementation
- Fixed-width 32-bit and 64-bit types with explicit signed/unsigned behavior.
- CPU state structure for general-purpose registers, HI/LO, PC, and branch-delay bookkeeping.
- A pure instruction decoder that maps a 32-bit word to an instruction kind and fields.
- Initial arithmetic/logical and branch instruction tests using synthetic instruction words.
- Explicit handling of reserved/unsupported encodings; never silently execute unknown instructions.
- Document R5900-specific behavior separately from generic MIPS III behavior before implementing it.

## Suggested module boundaries
- `cpu/ee_state.h`: register and execution state.
- `cpu/ee_decode.h` / `cpu/ee_decode.c`: instruction fields and decoding.
- `cpu/ee_execute.h` / `cpu/ee_execute.c`: execution helpers, introduced only after decoder tests.
- `tests/test_ee_decode.c`: host-side deterministic tests.
- `tests/test_ee_arithmetic.c`: arithmetic semantics and overflow/edge cases.

These paths are proposed; create them only after checking the repository tree and existing conventions.

## Test requirements
- Test zero, sign-bit, maximum unsigned, and minimum signed values.
- Test immediate sign extension and zero extension separately.
- Test branch target calculation and delay-slot bookkeeping.
- Test register zero semantics where applicable.
- Verify decoder output for known encodings and reject unknown/reserved encodings.
- Run host tests in CI. Host tests do not prove Xenon compilation or game compatibility.

## Exit criteria
1. CPU state and decoder compile under the host compiler.
2. Unit tests pass in CI with recorded commit and logs.
3. The same modules are compiled with the Xenon toolchain only after that toolchain is independently verified.
4. No game-boot or compatibility percentage is reported until supported code is integrated and measured.
