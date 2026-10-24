# God of War II — PS2 to Xbox 360 RGH Port Plan

## Goal
Develop a native Xbox 360 RGH port of the PlayStation 2 version of God of War II, progressing in small, verifiable milestones. The initial deliverable is a buildable Xenon ELF; the later packaging target is `default.xex`.

This document is a technical plan, not evidence that a game build already exists. No game executable, assets, or copyrighted game data are included.

## Parallel workstreams

### A — PS2 executable analysis and CPU
- Establish the exact supported PS2 release and document ELF/IRX/module inventory from a user-owned copy.
- Inventory Emotion Engine (MIPS R5900) instructions, delay slots, exceptions, and COP0 behavior.
- Prototype instruction decoding and deterministic unit tests using synthetic test cases.
- Track unsupported instructions explicitly; do not claim game compatibility from decoder tests alone.

### B — PS2 hardware services
- Specify memory map and access rules for EE RAM, scratchpad, registers, and relevant IOP interfaces.
- Map DMA channels, interrupts, timers, VIF/VU, GIF, and GS dependencies.
- Implement interfaces behind testable modules; prioritize boot-critical paths based on traces.
- Treat GS rendering, audio/SPU2, and FMV playback as separate milestones.

### C — Xbox 360 / Xenon platform
- Add a minimal platform abstraction for logging, files, timing, controller input, and memory.
- Verify the Free60/libXenon toolchain in CI before claiming target compilation.
- First produce a minimal Xenon ELF with a reproducible build; then integrate runtime modules.
- Add ELF-to-XEX packaging only after the ELF target is known to work. Target artifact: `default.xex`.

### D — Test, CI, and evidence
- Add host-side unit tests for decoders, memory access, DMA scheduling, and save-state serialization.
- Add CI checks for formatting, compilation, and tests where the available toolchain permits.
- Keep test results, toolchain version, commit SHA, and artifact names in build logs.
- Separate planned, implemented, compiled, and target-tested status in all progress reports.

### E — Integration and compatibility
- Maintain a dependency graph and integrate small changes frequently.
- Create a boot trace from startup through title screen, then progress scene-by-scene.
- Use regression tests after every integration change; do not mark a milestone complete solely because it compiles.

## Milestone sequence
1. **M0 — Repository baseline:** inventory existing files, license, build system, and CI.
2. **M1 — Build skeleton:** minimal host build and tests; document Xenon toolchain prerequisites.
3. **M2 — CPU foundation:** instruction decoder, register model, branch/delay-slot tests.
4. **M3 — Memory and exceptions:** address translation/access checks and exception tests.
5. **M4 — Platform layer:** logging, timing, input, and file abstractions.
6. **M5 — Hardware service stubs:** DMA/interrupt/timer interfaces with unit tests.
7. **M6 — First Xenon ELF:** compile the minimal target and retain verifiable CI output.
8. **M7 — Runtime integration:** connect implemented CPU and memory modules.
9. **M8 — XEX packaging:** generate `default.xex` from the integrated target and validate packaging.
10. **M9 — Game boot investigation:** test against a legally owned game copy and record exact failures.
11. **M10 — Graphics/audio progression:** implement and validate one subsystem at a time.
12. **M11 — Save/load and regression:** serialize CPU, memory, scheduler, and device state consistently.
13. **M12 — Compatibility milestone:** report scene-by-scene behavior, performance, and known defects.

## Status reporting rules
For every subsystem report four separate states:
- **Planned** — design exists, implementation not present.
- **Implemented** — code exists but has not necessarily compiled.
- **Compiled** — a named compiler/build completed successfully.
- **Tested** — a named reproducible test passed; specify whether host or Xbox 360 hardware.

Do not assign a completion percentage without a defined denominator and inspected evidence. Do not mark the port playable until the actual target build has been run and documented.

## Current verified status
- Repository README baseline inspected.
- This plan defines the workstreams and milestone sequence.
- No Xenon ELF, `default.xex`, gameplay boot, or Xbox 360 hardware test is claimed by this document.
