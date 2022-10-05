V2MP
====

V2MP is an acronym for the Vesper Virtual Micro Processor. This virtual processor is designed to be very simple, with a small set of instructions, and to live within a virtual environment managed by a host machine (referred to as the "supervisor").

The specification for the virtual processor is outlined in this file.

* [Documentation Conventions](#documentation-conventions)
* [CPU](#cpu)
* [Memory Model](#memory-model)
* [Instruction Set](#instruction-set)
  * [0h NOP: No Operation](#0h-no-operation-nop)
  * [1h ADD: Add](#1h-add-add)
  * [2h SUB: Subtract](#2h-subtract-sub)
  * [3h MUL: Multiply](#3h-multiply-mul)
  * [4h DIV: Divide](#4h-divide-div)
  * [5h ASGN: Assign](#5h-assign-asgn)
  * [6h SHFT: Bit Shift](#6h-bit-shift-shft)
  * [7h BITW: Bitwise Operation](#7h-bitwise-operation-bitw)
  * [8h CBX: Conditional Branch](#8h-conditional-branch-cbx)
  * [9h LDST: Load/Store](#9h-loadstore-ldst)
  * [Ah STK: Stack Operation](#ah-stack-operation-stk)
  * [Bh SIG: Raise Signal](#bh-raise-signal-sig)
* [Faults](#faults)
* [Signals](#signals)

## Documentation Conventions

This documentation uses the following conventions when describing the functionality of the processor.

### Numeric Literals

All numeric literals are specified in base 10 by default. Suffixes on numeric literals indicate a different base:

* `h` indicates that the number is in base 16 (hexadecimal). `10h` == `16`.
* `b` indicates that the number is in base 2 (binary). `10b` == `2`.

If a bit is described as being "set" then this means its value is `1`. If a bit is described as being "cleared" then this means its value is `0`.

### Numeric Sign

When values are referred to as "signed", this means that the signed value is represented using two's complement over the specified number of bits, unless otherwise stated.

### Bit Indexing

Instruction and register bits are numbered starting from `0` and proceeding right to left. A specific bit, or range of bits, in a register or instruction are referred to using square brackets `[ ]`. If the bits are present in a register, the name of the register may also be prefixed to the brackets.

For example, `[0]` indicates bit `0` in some unspecified register or instruction. `R0[5 2]` indicates a range of bits in register `R0`, beginning at bit `5` on the left, and ending at bit `2` on the right (both bits `5` and `2` are included in this range).

In certain cases (such as for the status register `SR`), specific bits are named. In this case, the letter used to identify that specific bit may be enclosed in square brackets. For example, `SR[C]` specifies the `CARRY` status bit in the status register.

### Bit Layout

The layout of bits in instructions and registers is represented visually by simple layout diagrams.

```
           V Bit [15]
Register: |................|
                          ^ Bit [0]

              V--V Opcode [15 12]
Instruction: |....|............|
                   ^----------^ Operands [11 0]
```

Certain bits may be substituted by letters, to indicate the positions of various elements of an instruction or register. Bits that are not directly used by a certain instruction or register are indicated by `.`, and unless otherwise specified should always be `0`.

```
Instruction: |0101|AABC........|
                   ^-^^ Three operands A, B, and C indicated here
```

## CPU

The CPU is 16-bit and little-endian. It contains the following registers:

* An instruction register (`IR`)
* A program counter (`PC`)
* A stack pointer (`SP`)
* A status register (`SR`)
* A link register (`LR`)
* Two general-purpose registers (`R0` and `R1`)

### Instruction Register (`IR`)

The instruction register holds the current instruction that is being executed. It cannot be accessed directly by instructions.

### Program Counter (`PC`)

The program counter points to the next instruction in code memory. Its value must always be aligned to a word boundary (ie. `PC[0]` must be `0`), or an [`ALGN`](#faults) fault will be raised when the instruction it points to is fetched.

The program counter is automatically incremented after each instruction is fetched. The instruction may then modify the value of the program counter (ie. perform a code branch), to specify which instruction will be fetched on the next clock cycle.

### Stack Pointer (`SP`)

The stack pointer points to the location in the stack memory segment where the top of the stack resides.

### Status Register (`SR`)

The status register holds flags relating to the result of the execution of the previous instruction. It cannot be accessed directly by instructions, but its state is used to affect the operation of certain instructions (eg. for conditional branching).

After the instruction is executed, all status register bits are set to `0` by default, apart from those bits which are affected by the result of the instruction. Any instructions that may affect the state of the status register will describe the nature of the state changes. Additionally, any instructions that operate based on the current state of the status register will describe how the state is used.

The contents of the register are as follows:

```
|..............CZ|
```

* Bit `[0]`, or `SR[Z]`, is the `ZERO` bit. This bit is set to `1` to indicate the absence of a result after an instruction is executed, or is set to `0` otherwise.
  * For arithmetic instructions, if this bit is set to `1` then it indicates that the result of the instruction was zero.
  * For other instructions, a value of `1` may indicate for example that some requested device or state was not available. The specific instruction will describe how the bit is used.
* Bit `[1]`, or `SR[C]`, is the `CARRY` bit. It is used to indicate the propagation of some form of state outside of the scope of an instruction.
  * For arithmetic instructions, this bit is set to `1` if the instruction resulted in an overflow or an underflow, or is set to `0` otherwise.
  * For other instructions, a value of `1` may indicate for example that not all of the data used by the instruction was able to be processed. The specific instruction will describe how the bit is used.
* Bits `[15 2]` are reserved for future use, and are always set to `0`.

### Link Register (`LR`)

The link register is used by certain instructions to refer to an address memory. Instructions which make use of the link register will describe the nature of its use.

Outside of these instructions, the link register can also be used as a general-purpose register if desired.

### General-Purpose Registers (`R0` and `R1`)

The general-purpose registers do not have any special significance for most instructions - they are simply used to manipulate values. Any instructions that use `R0` or `R1` in specific ways will explain the nature of their use.

## Memory Model

The CPU addresses three separate memory segments: the read-only code segment (`CS`), the read-write data segment (`DS`), and the read-write stack segment (`SS`). The program counter's address **always** refers to the code segment; instructions to read from or write to memory **always** refer to the data segment; and instructions to push to or pop from the stack **always** refer to the stack segment. This means that under this memory model, code cannot be executed from writable memory, and data for use in general-purpose registers cannot be directly loaded from the code segment. Some instructions, however, do support packing numerical literals into their operand bits, and these can affect the values in general-purpose registers.

```
        CS                                              DS
+----------------+                              +----------------+
|................| <--- |.......PC.......|      |................|
|................|      |.......LR.......| ---> |................|
|................|      |.......R0.......| ---> |................|
|................|      |.......R1.......| ---> |................|
+----------------+  /-- |.......SP.......|      +----------------+
                    |
                    |           SS
                    |   +----------------+
                    \-> |................|
                        |................|
                        |................|
                        |................|
                        +----------------+
```

Each memory segment lives in a 16-bit address space, and so can hold a maximum of 65536 bytes (64KB). A program that runs on the V2MP CPU consists primarily of the `CS` and `DS` segments, where the `CS` segment holds all code for the program, and the `DS` segment holds any pre-compiled data that the program may wish to use. It is completely up to the program in question how the `DS` data is managed - any data originally supplied within `DS` may be modified.

The program may also specify how much memory is allocated for `SS`, but the contents of `SS` may not be specified in advance.

The following restrictions apply to any memory access (in `CS`, `DS`, or `SS`), via any register (`PC` or general-purpose):

* When accessing a unit of memory (eg. a 2-byte word), the address of the access must be aligned to a multiple of the size of the unit. If this is not the case, an [`ALGN`](#faults) fault will be raised.
* The address of the access must be within the size of the segment. Although 64KB of memory from each segment is addressable, the entire memory space might not be used if the program is not that large, and so will not be allocated by the supervisor. If access is attempted to a memory address outside of either segment, a [`SEG`](#faults) fault will be raised.

The V2MP memory model does not directly support dynamic memory allocation: memory segments are of a fixed size. However, supervisor functionality may be used to manipulate memory pages themselves at runtime, eg. to swap one page out for another.

### Relevant Instructions

The [`LDST`](#9h-loadstore-ldst) instruction loads or stores single words from or to `DS`.

The [`STK`](#ah-stack-operation-stk) instruction pushes or pops one or more registers onto or off of the stack in `SS`.

The [`CBX`](#8h-conditional-branch-cbx) instruction makes reference to an address in `CS` which the program counter `PC` should be set to if the branch condition is met. Other arithmetic instructions may directly modify the contents of `PC` to point to different addresses in `CS`.

The [`SIG`](#bh-raise-signal-sig) instruction raises signals that are acted upon by the supervisor. The supervisor can be used to perform system-level operations regarding the memory space that is accessible by the program.

## Instruction Set

As the V2MP CPU is 16-bit, each instruction is also comprised of 16 bits. The highest 4 bits represent the opcode, and the other bits represent operands.

```
      Bits [15 12] represent the opcode
      V--V
High |CCCC|DDDDDDDDDDDD| Low
           ^----------^
           Bits [11 0] represent the operands (instruction-specific)
```

Operand bits in register diagrams are assigned letters based on which operands they correspond to. Operand bits represented by `.` are not used by the instruction, and **must** be set to `0`. If this is not the case, a [`RES`](#fault) fault is raised.

Some instructions make reference to a register in the CPU using a 2-bit identifier as an operand. Whenever one of these identifiers is used, it refers to the following register unless otherwise stated:

* `00b` refers to `R0`.
* `01b` refers to `R1`.
* `10b` refers to `LR`.
* `11b` refers to `PC`.

### `0h`: No Operation (`NOP`)

Performs no operation.

```
 NOP
|0000|............|
```

All operand bits `[11 0]` must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

All registers are left at their current values apart from `PC`, which is incremented to the next instruction.

### `1h`: Add (`ADD`)

Increments the value in a register.

```
 ADD
|0001|AABBCCCCCCCC|
```

* Operand bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Operand bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is used to increment the destination register, and the source register remains unchanged. Operand bits `[7 0] (C)` must be set to `0` in this case, or a [`RES`](#faults) fault will be raised.

If instead the source `A` and destination `B` register identifiers are the same, operand bits `[7 0] (C)` are interpreted as an unsigned 8-bit literal, which is used to increment destination register.

Note that if the destination register is `PC`, the increment (either as a literal or from a register) is treated as the number of **words** to increment by, rather than the number of bytes. This is to avoid causing an address mis-alignment by setting `PC` to an odd value.

If the add operation overflows the destination register, `SR[C]` is set; otherwise, it is cleared.

If the add operation results in a value of `0` in the destination register, `SR[Z]` is set; otherwise, it is cleared.

All other bits in `SR` are always cleared.

### `2h`: Subtract (`SUB`)

Decrements the value in a register.

```
 SUB
|0010|AABBCCCCCCCC|
```

* Operand bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Operand bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is used to decrement the destination register, and the source register remains unchanged. Operand bits `[7 0] (C)` must be set to `0` in this case, or a [`RES`](#faults) fault will be raised.

If instead the source `A` and destination `B` register identifiers are the same, operand bits `[7 0] (C)` are interpreted as an unsigned 8-bit literal, which is used to decrement the destination register.

Note that if the destination register is `PC`, the decrement (either as a literal or from a register) is treated as the number of **words** to decrement by, rather than the number of bytes. This is to avoid causing an address mis-alignment by setting `PC` to an odd value.

If the subtraction operation underflows the destination register, `SR[C]` is set; otherwise, it is cleared.

If the subtraction operation results in a value of `0` in the destination register, `SR[Z]` is set; otherwise, it is cleared.

All other bits in `SR` are always cleared.

### `3h`: Multiply (`MUL`)

Multiplies a register by a value.

```
 MUL
|0011|ABC.DDDDDDDD|
```

* Operand bit `[11] (A)` specifies whether `R0` or `R1` is used as the destination register for the operation. If `A` is `0` then `R0` is used; if `A` is `1` then `R1` is used.
* Operand bit `[10] (B)` specifies the source of the multiplicand. If `B` is `0` then either `R0` or `R1` (the opposite to that which has been selected by operand `A`) is used as the multiplicand; if `B` is `1` then operand bits `[7 0] (D)` are treated as an 8-bit multiplicand whose sign depends on operand `C`.
* Operand bit `[9] (C)` specifies whether the operation is treated as signed or unsigned. If `C` is `0` then the destination and multiplicand are treated as unsigned; if `C` is `1` then the destination and multiplicand are treated as signed.

Operand bit `[8]` is reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

After the instruction is executed, the destination register as specified by operand `A` will hold the lower 16 bits of the result, and `LR` will hold the higher 16 bits of the result. If the result fitted entirely into the destination register, `LR` will be set appropriately depending on whether the operation was signed or unsigned, so that `LR` concatenated with the destination register would form a 32-bit word of the correct sign.

If the result of the multiplication did not fit into the target register, `SR[C]` is set; otherwise, it is cleared. If the result is zero, `SR[Z]` is set; otherwise, it is. All other bits in `SR` are cleared.

### `4h`: Divide (`DIV`)

Divides a register by a value.

```
 DIV
|0100|ABC.DDDDDDDD|
```

* Operand bit `[11] (A)` specifies whether `R0` or `R1` is used as the destination register for the operation. If `A` is `0` then `R0` is used; if `A` is `1` then `R1` is used.
* Operand bit `[10] (B)` specifies the source of the divisor. If `B` is `0` then either `R0` or `R1` (the opposite to that which has been selected by operand `A`) is used as the divisor; if `B` is `1` then operand bits `[7 0] (D)` are treated as an 8-bit divisor whose sign depends on operand `C`.
* Operand bit `[9] (C)` specifies whether the operation is treated as signed or unsigned. If `C` is `0` then the destination and divisor are treated as unsigned; if `C` is `1` then the destination and divisor are treated as signed.

Operand bit `[8]` is reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

If the divisor is zero when the instruction is executed, a [`DIV`](#faults) fault is raised.

After the instruction is executed, the destination register as specified by operand `A` will hold the quotient of the result, and `LR` will hold the remainder. Both values will be either signed or unsigned, depending on whether the operation was a signed or unsigned division.

If the remainder of the result is not zero, `SR[C]` is set; otherwise, it is cleared. If the quotient of the result is zero, `SR[Z]` is set; otherwise, it is cleared. All other bits in `SR` are cleared.

### `5h`: Assign (`ASGN`)

Assigns a value to a register.

```
 ASGN
|0101|AABBCCCCCCCC|
```

* Operand bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Operand bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is copied to the destination register, and the source register remains unchanged. This behaviour is the same regardless of the destination register. Operand bits `[7 0] (C)` of the instruction must be set to `0` in this case, or a [`RES`](#faults) fault will be raised.

Assigning to the program counter `PC` (`11b`) is equivalent to performing an unconditional jump. Beware assigning a value that is not aligned to a word boundary. If an assignment like this is performed, an [`ALGN`](#faults) fault will be raised upon fetching the next instruction.

If instead the source `A` and destination `B` register identifiers are the same, operand bits `[7 0] (C)` of the instruction are interpreted as a signed 8-bit literal to assign to the destination register. The behaviour of the assignment depends on the destination register.

If `A` and `B` are the same and the destination register is `R0`, `R1` or `LR`, the following assignment is performed:

* Bits `[6 0]` in the destination register are assigned the operand bits `[6 0]`.
* Bits `[15 7]` in the destination register are set to `0` if operand bit `[7]` was `0`; otherwise, bits `[15 7]` in the destination register are set to `1`.

```
 R0, R1, or LR, assigning operand 00000010b (2):
|0000000000000010|

 R0, R1, or LR, assigning operand 10000010b (-126):
|1111111110000010|
```

If `A` and `B` are the same, `PC` may not be assigned to, since the range of values that are passed in operand bits `[7 0] (C)` is too small to be useful. If `A` and `B` are the same and the register index `11b` is specified, a [`RES`](#faults) fault is raised.

In all cases described for this instruction, `SR[Z]` is set if the eventual value in the destination register is zero; otherwise, it is cleared. All other bits in `SR` are cleared.

### `6h`: Bit Shift (`SHFT`)

Shifts the bits in a register left or right.

```
 SHFT
|0110|AABB...CCCCC|
```

* Operand bits `[11 10] (A)` specify the two-bit identifier of the register whose value determines the magnitude of the shift. The contents of the register are treated as a signed 16-bit value.
* Operand bits `[9 8] (B)` specify the two-bit identifier of the register whose value will be shifted.

If both register identifiers `A` and `B` are the same, operand bits `[4 0] (C)` are used to determine the magnitude of the shift. The magnitude is treated as a signed 5-bit number, ranging from `-16` to `15`.

If register identifiers `A` and `B` are different, operand bits `[4 0] (C)` must be set to `0`, or a [`RES`](#faults) fault will be raised.

In all cases, operand bits `[7 5]` are reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

When shifting bits, a negative magnitude implies a right shift (dividing the value by 2), and a positive magnitude implies a left shift (multiplying the value by 2). Bits that are shifted off either end of the register are discarded, and bits that are inserted into the register due to the shift are always `0`.

If after the operation the remaining value in the register is zero, `SR[Z]` is set to `1`; otherwise, it is set to `0`. Additionally, if any `1` bits were shifted off the end of the register during the operation, `SR[C]` is set to `1`; otherwise, it is set to `0`. All other bits in `SR` are set to `0`.

### `7h`: Bitwise Operation (`BITW`)

Performs a bitwise operation between two register values.

```
 BITW
|0111|AABBCCD.EEEE|
```

* Operand bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source of the bit mask.
* Operand bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.
* Operand bits `[7 6] (C)` specify the type of bitwise operation to perform:
  * `00b` performs a bitwise `AND`.
  * `01b` performs a bitwise `OR`.
  * `10b` performs a bitwise `XOR`.
  * `11b` performs a bitwise `NOT` (ie. flips all bits).

If the source `A` and destination `B` register identifiers are different, the source register's value is used as the bit mask that is used to manipulate the bits in the destination register. In the case of a bitwise `NOT` operation (which does not require a bit mask), the identifier for the source register is ignored.

If the source `A` and destination `B` register identifiers are different, operand bits `[5] (D)` and `[3 0] (E)` are reserved for future use, and must be set to `0`. If they are not, a [`RES`](#faults) fault will be raised.

If instead the source `A` and destination `B` register identifiers are the same, the bit mask is not based on the value in a register. Instead, it is created based on a mask value of `0000000000000001b` which is manipulated by operand bits `[5] (D)` and `[3 0] (E)`:

* Operand bits `[3 0] (E)` specify the magnitude of the left shift of the mask value, in the range `0` to `15` inclusive. For example, if the operand bits `[3 0] (E)` are set to `0101b` (a shift of `5`), this creates the mask `0000000000100000b`.
* Operand bit `[5] (D)` specifies if the resulting mask should be inverted. If operand bit `[5] (D)` is `0` then the mask is not inverted; if it is `1` then the mask is inverted. The above example would be `1111111111011111b` if operand bit `[5] (D)` was `1`.

Similarly to the earlier case where register identifiers `A` and `B` were different, if the bitwise operation is `NOT` then the generated bit mask is redundant. In this case, operand bits `[3 0] (E)` must be set to `0`, or a [`RES`](#faults) fault will be raised.

In all cases, operand bit `[4]` is reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

If the resulting value in the destination register is zero, `SR[Z]` is set to `1`; otherwise, it is set to `0`. All other bits in `SR` are set to `0`.

### `8h` Conditional Branch (`CBX`)

Decided whether or not to modify the value of the program counter `PC`, depending on the current state of the status register `SR`.

```
 CBX
|1000|AB..CCCCCCCC|
```

* Operand bit `[11] (A)` specifies how the value of `PC` should be modified:
  * If `A` is `1`, the value in `LR` is treated as the address to jump to, and is assigned to `PC`. In this case, operand bits `[7 0] (C)` must be set to `0`; if they are not, a [`RES`](#faults) fault will be raised.
  * If `A` is `0`, the value of operand bits `[7 0] (C)` is treated as a signed 8-bit offset **in words** from the current `PC` address. `PC` is incremented (or decremented, depending on the sign) by this value.
* Operand bit `[10] (B)` specifies what aspect of `SR` is considered when deciding whether to branch.
  * If `B` is `0`, `SR[Z]` is considered. If `SR[Z]` is set (ie. the last instruction that was executed produced a result of zero), `PC` is modified according to the rules above. If `SR[Z]` is not set, `PC` is not modified and will be incremented as normal once the `CBX` instruction is complete.
  * If `B` is `1`, `SR[C]` is considered. If `SR[C]` is set (ie. the last instruction that was executed overflowed or underflowed its register), `PC` is modified according to the rules above. If `SR[C]` is not set, `PC` is not modified and will be incremented as normal once the `CBX` instruction is complete.

Operand bits `[9 8]` are reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

After this instruction, `SR[Z]` will be set to `1` if the condition was not met. If the condition was met, and `PC` was set, `SR[Z]` will be set to `0`. All other bits in `SR` will be set to `0`.

### `9h`: Load/Store (`LDST`)

Depending on the operands, either loads a value from a location in memory, or stores a value to a location in memory.

```
 LDST
|1001|ABB.........|
```

* Operand bit `[11] (A)` specifies the mode of the operation:
  * `0b` means that the operation is a load.
  * `1b` means that the operation is a store.
* Operand bits `[10 9] (B)` specify the two-bit identifier of the register whose value will be used for the operation.

The memory location for the load or store is always specified by the value of `LR`. This always refers to the data segment `DS`.

Operand bits `[8 0]` of the instruction are reserved for future use. If any of these bits is not `0`, a [`RES`](#fault) fault will be raised.

If the memory address specified by `LR` is not aligned to a word boundary, an [`ALGN`](#faults) fault will be raised. If the address is not within the boundaries of `DS`, a [`SEG`](#faults) fault will be raised.

If the value that is loaded or stored to or from the register is zero, `SR[Z]` is set; otherwise, it is cleared. All other bits in `SR` are always cleared.

### `Ah`: Stack Operation (`STK`)

Pushes or pops register values from the stack.

```
 STK
|1010|A.......BCDE|
```

* Operand bit `[11] (A)` specifies whether the operation is a push or pop. If `A` is set then the operation is a push; if `A` is not set then the operation is a pop.
* Operand bit `[3] (B)` specifies whether `PC` is included in the operation: it is included if `B` is set, and is not included if `B` is not set.
* Operand bit `[2] (C)` specifies whether `LR` is included in the operation: it is included if `C` is set, and is not included if `C` is not set.
* Operand bit `[1] (D)` specifies whether `R1` is included in the operation: it is included if `D` is set, and is not included if `D` is not set.
* Operand bit `[0] (E)` specifies whether `R0` is included in the operation: it is included if `E` is set, and is not included if `E` is not set.

Operand bits `[10 4]` are reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised. Additionally, including no registers in the operation (ie. leaving operand bits `B` - `E` as `0`) will also raise a [`RES`](#faults) fault.

Registers are always pushed onto the stack in the following order, and are popped from the stack in reverse order:

1. `R0`
2. `R1`
3. `LR`
4. `PC`

If a register is not specified in the instruction word, it is not included in the push or pop operation. After the operation, the stack is grown or shrunk by as many words as there were registers included in the operation.

If a push operation overflows the stack, or a pop operation underflows the stack, a [`SOF`](#faults) fault is raised.

The status register `SR` is unaffected by the instruction, and its existing value is maintained.

### `Bh`: Raise Signal (`SIG`)

Raises a signal to be handled by the supervisor.

```
 SIG
|1011|............|
```

All operand bits `[11 0]` are reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

The code representing the signal being raised is specified by `R0`. See the [Signals](#signals) section for a complete list of signal codes. If the value of `R0` does not correspond to a recognised signal, a [`INS`](#faults) fault is raised.

The registers `R1` and `LR` may be treated as arguments to the signal, depending on which signal is raised. If a signal does not make use of `R1` or `LR`, the value in the register is ignored and left unchanged. If a signal does make use of `R1` and/or `LR`, the values in the registers used by the signal may be modified in response to the signal.

Additionally, memory in any of the `CS`, `DS` or `SS` segments may be modified by the supervisor in response to the signal, depending on the functionality implemented by the supervisor when responding to the signal.

Signals which make use of any registers or memory segments will describe the nature of their use, and any side-effects that may occur in response to the signal being raised. See the [Signals](#signals) section for complete documentation on available signals.

## Faults

The possible faults raised by the processor are described below.

| Index | Code | Name | Description | Triggered By |
| ----- | ---- | ---- | ----------- | ------------ |
| `02h` | `RES` | Reserved Bits Set | Raised when an instruction is decoded and one or more reserved bits are set to `1`. | Execution of any instruction |
| `03h` | `ALGN` | Alignment Violation | Raised when an unaligned memory address is dereferenced in `CS` or `DS`. | [`LDST`](#9h-loadstore-ldst), or upon fetching the next instruction using `PC`. |
| `04h` | `SEG` | Segment Access Violation | Raised when an address outside `CS` or `DS` is dereferenced. | [`LDST`](#9h-loadstore-ldst), or upon fetching the next instruction using `PC` |
| `06h` | `INI` | Invalid Instruction | Raised when an unrecognised instruction opcode is decoded. | Decoding of an instruction |
| `09h` | `SOF` | Stack overflow or underflow | Raised when a stack operation overflows or underflows the stack space. | [`STK`](#ah-stack-operation-stk) |
| `0Ah` | `DIV` | Division by zero | Raised when a [`DIV`](#4h-divide-div) operation is performed with a divisor of `0`. | [`DIV`](#4h-divide-div) |
| `0Bh` | `INS` | Invalid Signal | Raised when an unrecognised signal code is provided to the [`SIG`](#bh-raise-signal-sig) instruction. | [`SIG`](#bh-raise-signal-sig) |

## Signals

**TODO**

## Points to Resolve

* Is it a good idea to treat LR and the other register involved in a MUL instruction as being "concatenated" into a 32-bit word? Would it be better to treat one as an unsigned factor, and the other as a signed factor?
