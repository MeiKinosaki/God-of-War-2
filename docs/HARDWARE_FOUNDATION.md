# PS2 hardware foundation

This is an architecture and validation specification, not a completed hardware implementation.

## Parallel subsystem interfaces

### Memory bus
- Define explicit address ranges and access widths.
- Specify alignment behavior and side effects for memory-mapped registers.
- Test little-endian loads/stores, boundaries, and invalid accesses on the host.

### DMA and interrupts
- Model channel state, transfer progress, completion, and interrupt signaling.
- Keep transfer scheduling deterministic for reproducible tests.
- Test zero-length, aligned, boundary, and interrupted transfers with synthetic buffers.
- Do not assume all PS2 DMA behavior is covered by a generic implementation.

### Vector units and GIF
- Separate command/data transfer interfaces from vector instruction execution.
- Track unsupported operations and establish small conformance tests before integration.

### Graphics Synthesizer
- Separate command interpretation, frame buffer state, texture sampling, and presentation.
- Begin with deterministic software-side tests for formats and state transitions.
- A correct frame buffer test is not evidence that the full game renders correctly.

### Audio and video
- Keep SPU2/audio scheduling and FMV playback independent of the graphics path.
- Record synchronization and timing requirements; test with synthetic streams.

## Integration gates
1. Agree on stable interfaces and ownership of shared state.
2. Add unit tests for each module before connecting it to the CPU runtime.
3. Add tracing around DMA, interrupts, and memory-mapped I/O.
4. Integrate one subsystem at a time and run the full host regression suite.
5. Test on the Xenon target only after the toolchain and minimal target executable are verified.

## Status vocabulary
- Planned: specification exists.
- Implemented: source code exists.
- Compiled: build completed and logs are available.
- Tested: a named reproducible test passed.
- Target-tested: executed on Xbox 360 hardware with recorded evidence.

Do not report gameplay compatibility or performance without target measurements.
