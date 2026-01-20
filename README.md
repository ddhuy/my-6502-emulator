# NES Emulator

A cycle-accurate NES emulator written in C++11 with comprehensive test coverage.

## Current Status: Phase 1-2 Complete ✓

### Implemented Features

- ✅ **CPU Infrastructure**
  - 6502 CPU registers (A, X, Y, SP, PC, P)
  - Status flags (Carry, Zero, Interrupt, Decimal, Break, Overflow, Negative)
  - Stack operations (push/pop 8-bit and 16-bit values)
  - Reset functionality

- ✅ **Memory System**
  - 64KB address space
  - Read/Write operations
  - Ready for memory-mapped I/O

- ✅ **All 13 Addressing Modes**
  - Implicit
  - Accumulator
  - Immediate
  - Zero Page
  - Zero Page,X
  - Zero Page,Y
  - Relative
  - Absolute
  - Absolute,X (with page crossing detection)
  - Absolute,Y (with page crossing detection)
  - Indirect (with 6502 page boundary bug)
  - Indexed Indirect (X) - (d,X)
  - Indirect Indexed (Y) - (d),Y (with page crossing detection)

- ✅ **Comprehensive Test Coverage**
  - 30+ unit tests covering all addressing modes
  - Edge case testing (wraparound, page crossing, etc.)
  - Stack operations validation
  - Flag operations testing

## Building the Project

### Prerequisites
- C++17 compatible compiler (g++ recommended)
- Google Test framework
- Make

### Build Commands

```bash
# Build the emulator
make

# Run all tests
make test

# Clean build artifacts
make clean
```

## Project Structure

```
nes-emulator/
├── src/
│   ├── bus/
│   │   ├── Bus.cpp           # Bus implementation
│   │   └── Bus.h             # Bus interface
│   ├── cpu/
│   │   ├── CPU.cpp           # CPU implementation
│   │   └── CPU.h             # CPU header with all registers and methods
│   ├── memory/
│   │   ├── Memory.cpp        # Memory system implementation
│   │   └── Memory.h          # Memory interface
│   └── emulator_main.cpp     # Entry point
├── tests/
│   ├── cpu_tests.cpp         # Basic CPU tests
│   └── addressing_mode_tests.cpp  # All addressing mode tests
├── build/                    # Generated directory for build artifacts
│   ├── obj/                  # Object files from source
│   ├── test_obj/             # Object files from tests
│   ├── emulator              # Main executable
│   └── test_runner           # Test executable
├── Makefile
└── README.md
```

## Next Steps: Phase 3

### Opcode Implementation Plan

The next phase involves implementing all 56 official 6502 opcodes:

1. **Load/Store Operations** (LDA, LDX, LDY, STA, STX, STY, TAX, TAY, TXA, TYA, TSX, TXS)
2. **Arithmetic** (ADC, SBC, INC, DEC, INX, DEX, INY, DEY)
3. **Logical** (AND, ORA, EOR, BIT)
4. **Shifts/Rotates** (ASL, LSR, ROL, ROR)
5. **Jumps/Calls** (JMP, JSR, RTS, RTI, BRK)
6. **Branches** (BCC, BCS, BEQ, BNE, BMI, BPL, BVC, BVS)
7. **Stack** (PHA, PHP, PLA, PLP)
8. **Status Flags** (CLC, SEC, CLI, SEI, CLD, SED, CLV)
9. **Comparison** (CMP, CPX, CPY)
10. **System** (NOP)

Each opcode will include:
- Correct cycle timing
- Additional cycles for page crossings
- Additional cycles for branches taken
- Comprehensive unit tests

## Design Patterns Used

- **RAII**: Memory management with std::unique_ptr
- **Separation of Concerns**: CPU, Memory, and Instructions in separate modules
- **Test-Driven Development**: Comprehensive test suite before implementation
- **Encapsulation**: Private helper methods for internal operations

## Testing Philosophy

Every feature is tested with:
- Normal cases
- Edge cases (boundary conditions)
- Error cases (underflow, overflow, wraparound)
- Hardware quirks (6502 bugs like indirect JMP page boundary)

## References

- [6502 Instruction Set](http://www.6502.org/tutorials/6502opcodes.html)
- [NES CPU Documentation](https://www.nesdev.org/wiki/CPU)
- [Klaus Dormann 6502 Functional Test](https://github.com/Klaus2m5/6502_65C02_functional_tests)

## License

[Your License Here]

## Contributors

[Your Name]