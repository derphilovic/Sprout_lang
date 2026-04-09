# Sprout Lang

A register-based bytecode virtual machine written in C++20, built from scratch as a learning project. Sprout is designed to eventually run a custom high-level language — the VM is feature-complete enough to be targeted by a compiler frontend.

---

## Architecture Overview

Sprout is a **register-based VM** with a fixed 32-bit instruction format. Values are stored using **NaN-boxing** — all registers hold tagged 64-bit words, so no heap allocation is needed for common scalar types.

```
Instruction format (32 bits):
[ opcode: 8 ][ ra: 8 ][ rb: 8 ][ rc: 8 ]
```

### Value Representation

All register values are 64-bit NaN-boxed words. The tag occupies bits 51–48 of a NaN payload:

| Tag | Type | Notes |
|---|---|---|
| `TAG_DOUBLE` | IEEE 754 double | Detected by non-NaN exponent bits |
| `TAG_INT48` | Signed 48-bit integer | Sign-extended on decode |
| `TAG_POINTER` | Heap pointer | 48-bit address, masked on decode |
| `TAG_BOOL` | Boolean | `true` / `false` |
| `TAG_CHAR6` | 6-char packed string | Up to 6 ASCII characters |
| `TAG_NULL` | Null | |

---

## Components

```
Sprout_lang/
├── src/
│   ├── main.cpp
│   ├── shared/
│   │   ├── bytecode_format.h   # BCHeader definition, magic number, version
│   │   ├── opcodes.h           # Opcode enum
│   │   └── values.h            # NaN-boxing encode/decode functions
│   └── vm/
│       ├── vm.h / vm.cpp       # VM struct, init, run loop
│       ├── decode.h / .cpp     # Instruction decode, push/pop
│       ├── execution.h / .cpp  # Opcode implementations
│       ├── bytecode.h / .cpp   # Binary loader, header parser, function table
│       └── heap.h / .cpp       # Arena allocator, two-space GC
├── tools/
│   └── assembler/
│       └── assembler.py        # Python assembler: .spt → .spbt
├── Sprout_lang_EBNF.txt        # Formal grammar for the Sprout language
└── CMakeLists.txt
```

---

## Binary Format (`.spbt`)

All multi-byte values are **big-endian**.

```
[ BCHeader (32 bytes) ]
[ Function Table (8 bytes × functionCount) ]
[ Code Section ]
```

**BCHeader fields:**

| Field | Size | Description |
|---|---|---|
| `magic` | u32 | `0x53505254` ("SPRT") |
| `version` | u32 | Currently `0x00000001` |
| `flags` | u32 | Reserved |
| `importCount` | u32 | Number of imported modules |
| `functionCount` | u32 | Number of functions |
| `importOffset` | u32 | Byte offset of import table |
| `functionOffset` | u32 | Byte offset of function table |
| `codeOffset` | u32 | Byte offset of code section |

**Function table entry (8 bytes):**

| Field | Size | Description |
|---|---|---|
| `entryIP` | u32 | Absolute bytecode address of function start |
| `frameSize` | u32 | Number of stack slots to reserve on call |

---

## Instruction Set

| Mnemonic | Opcode | Operands | Description |
|---|---|---|---|
| `add` | 0x00 | ra, rb, rc | `ra = rb + rc` (int or double) |
| `sub` | 0x01 | ra, rb, rc | `ra = rb - rc` |
| `mul` | 0x02 | ra, rb, rc | `ra = rb * rc` |
| `div` | 0x03 | ra, rb, rc | `ra = rb / rc` |
| `end` | 0x04 | ra | Print `ra`, halt |
| `load` | 0x05 | ra, imm64 | Load NaN-boxed 64-bit immediate into `ra` |
| `cmp` | 0x06 | ra, rb | Set `jmpFlag` based on comparison |
| `dbgret` | 0x07 | ra | Debug print `ra`, continue |
| `je` | 0x08 | addr | Jump if `jmpFlag == EQ` |
| `jne` | 0x09 | addr | Jump if `jmpFlag != EQ` |
| `jl` | 0x0A | addr | Jump if `jmpFlag == LT` |
| `jg` | 0x0B | addr | Jump if `jmpFlag == GT` |
| `call` | 0x0C | idx24 | Push FP+IP, set FP, reserve frame, jump to function |
| `ret` | 0x0D | — | Restore SP, IP, FP |
| `push` | 0x0E | ra | Push `ra` onto stack |
| `pop` | 0x0F | ra | Pop into `ra` |
| `ins` | 0x10 | ra, rb, rc | `stack[(ra<<8)\|rb] = rc` |
| `read` | 0x11 | ra, rb, rc | `rc = stack[(ra<<8)\|rb]` |
| `mov` | 0x12 | dst, src, flag | Register copy or pointer load/store (see flags below) |
| `arr` | 0x13 | dst, rlen, rtype | Allocate array of `rlen` elements of `rtype` |

**`mov` flags:**

| Flag | Behaviour |
|---|---|
| 0 | `reg[dst] = reg[src]` |
| 1 | `reg[dst] = *reg[src]` (load through pointer) |
| 2 | `*reg[dst] = reg[src]` (store through pointer) |
| 3 | `*reg[dst] = *reg[src]` (pointer-to-pointer copy) |

---

## Memory & Garbage Collection

Sprout uses an **arena bump allocator** with 4 MB chunks, growing on demand. Every heap object is preceded by an `objHeader`:

```cpp
struct objHeader {
    uint32_t size;      // total allocation size including header
    uint16_t type;      // OBJ_STRING or OBJ_ARRAY
    uint16_t flags;     // marking flags for GC
    uint64_t forwarded; // forwarding pointer during GC
};
```

The GC is a **two-space compacting collector**:

1. **Mark** — scan all registers and stack slots for heap pointers; mark reachable objects. For `OBJ_ARRAY` values holding pointer-typed elements, interior pointers are also traced.
2. **Evacuate** — copy live objects from the active half-space to the inactive one using forwarding pointers.
3. **Patch roots** — update all root pointers (registers, stack) to point to the new locations.
4. **Free** — release the old half-space.

GC triggers when `totalAllocated > 70% of max` (default max: 512 MB per half-space).

---

## Call Convention

```
Before call:             After call:
  [ ... ]                  [ ... ]
  [ saved FP ]   ← FP      [ saved FP ]
  [ saved IP ]             [ saved IP ]
                           [ local vars ] ← FP
                           ...            ← SP
```

`call` pushes FP then IP, sets FP = SP, advances SP by `frameSize`.  
`ret` sets SP = FP, pops IP, pops FP.

---

## Assembler

A Python assembler (`tools/assembler/assembler.py`) converts `.spt` text assembly into `.spbt` binaries.

**Usage:**
```bash
python assembler.py input.spt -o output.spbt
```

**Example `.asm`:**
```asm
.func main frameSize=0
    load  r0, 10
    load  r1, 32
    add   r2, r0, r1
    end   r2          ; prints 42
.endfunc
```

**Features:**
- NaN-boxed integer, float, bool, and null literals
- Local labels with `@label` forward references
- Multi-function files with per-function label scopes
- `call <index>` using 24-bit function table indices

---

## Building

**Requirements:** CMake 4.0+, C++20 compiler.

```bash
cmake -B build
cmake --build build
./build/Sprout_lang program.spbt
```

---

## Language Design (Work in Progress)

The Sprout language grammar is defined in `Sprout_lang_EBNF.txt`. Key design points:

- Static types: `int`, `double`, `boolean`, `string`, `char6`
- `char6` literals hold up to 6 characters packed into a single 64-bit register value — no heap allocation
- Interpolated strings via `v"..."` syntax with `{identifier}` substitution
- Functions declared with `func name -> returnType(params): block`
- C-style control flow: `if/else if/else`, `while`, `for ... in`
- Module imports via `#import lib::path`
- Named arguments: `func: arg1=val1, arg2=val2`

---

## Status

| Component                        | Status     |
|----------------------------------|------------|
| Fetch/decode/execute loop        | Done       |
| NaN-boxing value system          | Done       |
| Arena allocator                  | Done       |
| Two-space compacting GC          | Done       |
| Call/return convention           | Done       |
| Function table loader            | Done       |
| `OP_MOV` with pointer flags      | Done       |
| Array allocation (`OP_ARR_INIT`) | Done       |
| Python assembler                 | Done       |
| Array access (`OP_ARR_GET`)      | Done  |
| Boolean Implementation           | Done started |
| Char6 Implementation             | Not started |
| Lexer                            | Done started |
| Compiler frontend                | Not started |
| Module / import system           | In design  |
| Standard library                 | Not started |