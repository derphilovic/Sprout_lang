#!/usr/bin/env python3
"""
spt_asm.py  —  Assembler for the Sprout VM bytecode format
Reads a .spt text file, emits a .spbt binary.

Usage:
    python spt_asm.py input.spt [-o output.spbt]

Assembly syntax
---------------
Comments start with ;
Registers:  r0 .. r255
Labels:     label:           (defines a label at the current code position)
            @label           (reference to a label in branch instructions)

Instructions
------------
  add   rDST, rA, rB
  sub   rDST, rA, rB
  mul   rDST, rA, rB
  div   rDST, rA, rB
  load  rDST, <int|float>   ; LOAD64 — encodes value as NaN-boxed 64-bit word
  cmp   rA, rB
  je    @label
  jne   @label
  jl    @label
  jg    @label
  call  <funcIndex>          ; 24-bit function table index
  ret
  push  rA
  pop   rDST
  mov   rDST, rSRC [, flag]  ; flag 0=reg, 1=load ptr, 2=store ptr, 3=ptr-to-ptr
  arr     rDST, rLEN, rTYPE  ; ARR_INIT   — allocate heap array
  arrins  rSRC, rARR, rIDX   ; ARR_INSERT — array[rIDX] = rSRC
  arrread rDST, rARR, rIDX   ; ARR_READ   — rDST = array[rIDX]
  ins   rA, rB, rSRC         ; INSERT_INTO_STACK  stack[(rA<<8)|rB] = rSRC
  read  rA, rB, rDST         ; READ_FROM_STACK    rDST = stack[(rA<<8)|rB]
  end   rA
  dbgret rA                   ; DEBUG_RETURN

Function table entries (before code)
--------------------------------------
  .func <name> frameSize=<N>   ; opens a function block; must appear before any code
  .endfunc

  The assembler assigns function indices in declaration order (0, 1, 2, ...).
  Inside a .func block, labels are local to that function.

Example .spt
------------
.func main frameSize=0
    load  r0, 10
    load  r1, 20
    add   r2, r0, r1
    end   r2
.endfunc
"""

import struct
import sys
import math
import argparse
import os

# ---------------------------------------------------------------------------
# NaN-boxing constants  (must match decode.h)
# ---------------------------------------------------------------------------
NAN_BASE   = 0x7FF0000000000000
TAG_DOUBLE  = 0
TAG_INT48   = 1
TAG_POINTER = 2
TAG_BOOL    = 3
TAG_CHAR6   = 4
TAG_NULL    = 5

MAGIC   = 0x53505254   # 'SPRT'
VERSION = 0x00000001


def encode_int48(n: int) -> int:
    """Pack a signed 64-bit int into a NaN-boxed INT48 word."""
    lo48 = n & 0x0000_FFFF_FFFF_FFFF
    return NAN_BASE | (TAG_INT48 << 48) | lo48


def encode_double(d: float) -> int:
    """Reinterpret a Python float as a 64-bit word."""
    return struct.unpack('<Q', struct.pack('<d', d))[0]


def encode_bool(b: bool) -> int:
    return NAN_BASE | (TAG_BOOL << 48) | int(b)


def encode_null() -> int:
    return NAN_BASE | (TAG_NULL << 48)


def encode_char6(s: str) -> int:
    """Pack a string of up to 6 chars into a NaN-boxed CHAR6 word (little-endian layout)."""
    if len(s) > 6:
        raise ValueError(f"char6 literal too long ({len(s)} chars, max 6): {s!r}")
    raw = s.encode('ascii').ljust(6, b'\x00')
    value = int.from_bytes(raw[:6], 'little')
    return NAN_BASE | (TAG_CHAR6 << 48) | (value & 0x0000_FFFF_FFFF_FFFF)


def encode_value(token: str) -> int:
    """Try to encode a literal value token into a 64-bit NaN-boxed word."""
    if token == 'null':
        return encode_null()
    if token in ('true', 'false'):
        return encode_bool(token == 'true')
    # Quoted string → char6
    if (token.startswith('"') and token.endswith('"')) or \
       (token.startswith("'") and token.endswith("'")):
        return encode_char6(token[1:-1])
    # Try integer first
    try:
        return encode_int48(int(token, 0))
    except ValueError:
        pass
    # Then float
    try:
        return encode_double(float(token))
    except ValueError:
        pass
    raise ValueError(f"Cannot encode literal: {token!r}")


# ---------------------------------------------------------------------------
# Opcodes  (must match execution.h Opcode enum — order matters)
# ---------------------------------------------------------------------------
OPCODES = {
    'add':    0x00,
    'sub':    0x01,
    'mul':    0x02,
    'div':    0x03,
    'end':    0x04,
    'load':   0x05,   # OP_LOAD64
    'cmp':    0x06,
    'dbgret': 0x07,   # OP_DEBUG_RETURN
    'je':     0x08,
    'jne':    0x09,
    'jl':     0x0A,
    'jg':     0x0B,
    'call':   0x0C,
    'ret':    0x0D,
    'push':   0x0E,
    'pop':    0x0F,
    'ins':    0x10,   # OP_INSERT_INTO_STACK
    'read':   0x11,   # OP_READ_FROM_STACK
    'mov':    0x12,   # OP_MOV
    'arr':     0x13,  # OP_ARR_INIT
    'arrins':  0x14,  # OP_ARR_INSERT
    'arrread': 0x15,  # OP_ARR_READ
}

BRANCH_OPS = {'je', 'jne', 'jl', 'jg'}


def pack_instr(opcode: int, ra: int = 0, rb: int = 0, rc: int = 0) -> bytes:
    word = (opcode << 24) | (ra << 16) | (rb << 8) | rc
    return struct.pack('>I', word)


def pack_u32(n: int) -> bytes:
    return struct.pack('>I', n & 0xFFFF_FFFF)


def pack_u64(n: int) -> bytes:
    return struct.pack('>Q', n & 0xFFFF_FFFF_FFFF_FFFF)


# ---------------------------------------------------------------------------
# Tokeniser
# ---------------------------------------------------------------------------

def tokenise_line(line: str) -> list[str]:
    line = line.split(';', 1)[0]   # strip comments
    line = line.replace(',', ' ')
    return line.split()


def parse_reg(tok: str) -> int:
    if not tok.lower().startswith('r'):
        raise SyntaxError(f"Expected register, got {tok!r}")
    try:
        n = int(tok[1:])
    except ValueError:
        raise SyntaxError(f"Bad register: {tok!r}")
    if not 0 <= n <= 255:
        raise ValueError(f"Register out of range: {n}")
    return n


# ---------------------------------------------------------------------------
# Assembler — two-pass
# ---------------------------------------------------------------------------

class Assembler:
    def __init__(self):
        # Function table: list of {'name', 'frameSize', 'labels', 'patches', 'code'}
        # code is list of (offset_in_func_code, bytes_or_placeholder)
        self.functions: list[dict] = []
        self._cur_func: dict | None = None
        self._in_func = False

    # ------------------------------------------------------------------
    # Parsing helpers
    # ------------------------------------------------------------------

    def _require_func(self, mnemonic: str):
        if not self._in_func:
            raise SyntaxError(f"Instruction {mnemonic!r} outside .func block")

    def _emit(self, data: bytes):
        self._cur_func['raw'].extend(data)

    def _cur_offset(self) -> int:
        return len(self._cur_func['raw'])

    # ------------------------------------------------------------------
    # Two-pass assembly
    # ------------------------------------------------------------------

    def assemble_file(self, path: str) -> bytes:
        with open(path, 'r', encoding='utf-8') as fh:
            lines = fh.readlines()

        self._pass1(lines)
        return self._emit_binary()

    def _pass1(self, lines: list[str]):
        """Single logical pass; branches use a forward-reference patch list."""
        for lineno, raw in enumerate(lines, 1):
            toks = tokenise_line(raw)
            if not toks:
                continue
            try:
                self._handle_tokens(toks)
            except (SyntaxError, ValueError, KeyError) as e:
                sys.exit(f"Line {lineno}: {e}\n  > {raw.rstrip()}")

        if self._in_func:
            sys.exit("Unterminated .func block at end of file")

    def _handle_tokens(self, toks: list[str]):
        head = toks[0].lower()

        # Directives
        if head == '.func':
            self._begin_func(toks)
            return
        if head == '.endfunc':
            self._end_func()
            return

        # Label definition
        if head.endswith(':'):
            self._require_func('label')
            name = head[:-1]
            self._cur_func['labels'][name] = self._cur_offset()
            return

        # Instructions
        self._require_func(head)
        mnem = head

        if mnem not in OPCODES:
            raise SyntaxError(f"Unknown mnemonic: {mnem!r}")

        op = OPCODES[mnem]

        if mnem == 'load':
            # load rDST, <value>
            dst = parse_reg(toks[1])
            val = encode_value(toks[2])
            self._emit(pack_instr(op, dst))
            self._emit(pack_u64(val))

        elif mnem in ('add', 'sub', 'mul', 'div'):
            dst, a, b = parse_reg(toks[1]), parse_reg(toks[2]), parse_reg(toks[3])
            self._emit(pack_instr(op, dst, a, b))

        elif mnem == 'cmp':
            a, b = parse_reg(toks[1]), parse_reg(toks[2])
            self._emit(pack_instr(op, a, b))

        elif mnem in BRANCH_OPS:
            # je @label  — target is a 4-byte absolute code address
            # We record a patch because we may not know the code offset yet.
            label_tok = toks[1]
            if not label_tok.startswith('@'):
                raise SyntaxError(f"Branch target must be a label reference (@name), got {label_tok!r}")
            label_name = label_tok[1:]
            patch_offset = self._cur_offset()     # position of the instr word
            self._emit(pack_instr(op))            # placeholder ra/rb/rc = 0
            self._emit(b'\x00\x00\x00\x00')       # placeholder 32-bit target
            self._cur_func['patches'].append((patch_offset, label_name, 'branch'))

        elif mnem == 'call':
            idx = int(toks[1], 0)
            if not 0 <= idx <= 0xFFFFFF:
                raise ValueError(f"Function index out of 24-bit range: {idx}")
            ra = (idx >> 16) & 0xFF
            rb = (idx >> 8)  & 0xFF
            rc =  idx        & 0xFF
            self._emit(pack_instr(op, ra, rb, rc))

        elif mnem == 'ret':
            self._emit(pack_instr(op))

        elif mnem in ('push', 'pop', 'end', 'dbgret'):
            reg = parse_reg(toks[1])
            self._emit(pack_instr(op, reg))

        elif mnem == 'mov':
            dst = parse_reg(toks[1])
            src = parse_reg(toks[2])
            flag = int(toks[3], 0) if len(toks) > 3 else 0
            self._emit(pack_instr(op, dst, src, flag))

        elif mnem == 'arr':
            dst  = parse_reg(toks[1])
            rlen = parse_reg(toks[2])
            rtyp = parse_reg(toks[3])
            self._emit(pack_instr(op, dst, rlen, rtyp))

        elif mnem in ('arrins', 'arrread'):
            # arrins rSRC, rARR, rIDX  — store rSRC into array[rIDX]
            # arrread rDST, rARR, rIDX — load array[rIDX] into rDST
            ra_ = parse_reg(toks[1])
            rb_ = parse_reg(toks[2])
            rc_ = parse_reg(toks[3])
            self._emit(pack_instr(op, ra_, rb_, rc_))

        elif mnem == 'ins':
            # ins rA, rB, rSRC  -> insertIntoStack(a, b, reg[rc])
            ra_ = parse_reg(toks[1])
            rb_ = parse_reg(toks[2])
            rc_ = parse_reg(toks[3])
            self._emit(pack_instr(op, ra_, rb_, rc_))

        elif mnem == 'read':
            # read rA, rB, rDST
            ra_ = parse_reg(toks[1])
            rb_ = parse_reg(toks[2])
            rc_ = parse_reg(toks[3])
            self._emit(pack_instr(op, ra_, rb_, rc_))

        else:
            raise SyntaxError(f"Unhandled mnemonic: {mnem!r}")

    def _begin_func(self, toks: list[str]):
        if self._in_func:
            raise SyntaxError(".func inside another .func")
        # .func <name> frameSize=<N>
        name = toks[1] if len(toks) > 1 else f"func{len(self.functions)}"
        frame_size = 0
        for tok in toks[2:]:
            if tok.lower().startswith('framesize='):
                frame_size = int(tok.split('=', 1)[1], 0)
        self._cur_func = {
            'name':      name,
            'frameSize': frame_size,
            'labels':    {},       # name -> offset within this func's raw bytes
            'patches':   [],       # (offset_in_raw, label_name, kind)
            'raw':       bytearray(),
        }
        self.functions.append(self._cur_func)
        self._in_func = True

    def _end_func(self):
        if not self._in_func:
            raise SyntaxError(".endfunc without .func")
        self._in_func = False
        self._cur_func = None

    # ------------------------------------------------------------------
    # Binary emission
    # ------------------------------------------------------------------

    def _emit_binary(self) -> bytes:
        # Header layout (BCHeader — 8 uint32 fields = 32 bytes)
        # magic, version, flags, importCount, functionCount,
        # importOffset, functionOffset, codeOffset

        func_count    = len(self.functions)
        import_count  = 0

        # functionOffset = sizeof(BCHeader) = 32
        # Each functionInfo = entryIP (4) + frameSize (4 padded to 4) = 8 bytes
        # But looking at vm.h: frameSize is uint16_t stored as uint32_t via fetchAtAddr
        # fetchFuncMetadata reads two consecutive uint32_t words:
        #   f.entryIP   = fetchAtAddr(vm, addr)      — 4 bytes
        #   f.frameSize = fetchAtAddr(vm, addr + 4)  — 4 bytes (only low 16 used)
        # So each entry is 8 bytes.

        HEADER_SIZE     = 32
        FUNC_ENTRY_SIZE = 8   # entryIP(4) + frameSize padded to 4
        import_offset   = HEADER_SIZE                              # no imports
        func_offset     = HEADER_SIZE                              # function table starts right after header
        code_offset     = func_offset + func_count * FUNC_ENTRY_SIZE

        # Resolve patches (branch targets) — relative to code_offset
        # Each function's code is laid out sequentially after code_offset.
        func_base_addrs: list[int] = []
        pos = code_offset
        for f in self.functions:
            func_base_addrs.append(pos)
            pos += len(f['raw'])

        for fi, f in enumerate(self.functions):
            base = func_base_addrs[fi]
            for (raw_off, label_name, kind) in f['patches']:
                if label_name not in f['labels']:
                    sys.exit(f"Function {f['name']!r}: undefined label @{label_name}")
                label_abs = base + f['labels'][label_name]   # absolute bytecode address
                # The branch instruction is at raw_off; immediately after it (raw_off+4)
                # is the 4-byte target address written big-endian.
                struct.pack_into('>I', f['raw'], raw_off + 4, label_abs)

        # Build function table bytes
        func_table = bytearray()
        for fi, f in enumerate(self.functions):
            entry_ip = func_base_addrs[fi]
            func_table += pack_u32(entry_ip)
            func_table += pack_u32(f['frameSize'])   # stored as full uint32 per fetchAtAddr

        # Build code section
        code = bytearray()
        for f in self.functions:
            code += f['raw']

        # Build header
        header = struct.pack('>IIIIIIII',
                             MAGIC,
                             VERSION,
                             0,               # flags
                             import_count,
                             func_count,
                             import_offset,
                             func_offset,
                             code_offset,
                             )

        return bytes(header) + bytes(func_table) + bytes(code)


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description='Sprout assembler: .asm → .spbt'
    )
    parser.add_argument('input', help='Input .asm assembly file')
    parser.add_argument('-o', '--output', help='Output .spbt file (default: same name)')
    args = parser.parse_args()

    in_path = args.input
    if args.output:
        out_path = args.output
    else:
        base, _ = os.path.splitext(in_path)
        out_path = base + '.spbt'

    asm = Assembler()
    binary = asm.assemble_file(in_path)

    with open(out_path, 'wb') as fh:
        fh.write(binary)

    total_code = sum(len(f['raw']) for f in asm.functions)
    print(f"Assembled {len(asm.functions)} function(s), {total_code} bytes of code → {out_path}")
    print(f"Total binary size: {len(binary)} bytes")
    for i, f in enumerate(asm.functions):
        print(f"  [{i}] {f['name']}  frameSize={f['frameSize']}  codeSize={len(f['raw'])}")


if __name__ == '__main__':
    main()