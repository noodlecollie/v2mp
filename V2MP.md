V2MP
====

V2MP stands for Vesper Virtual Micro Processor. This virtual processor is designed to be very simple, with a small set of instructions, and to live within a virtual environment managed by a host machine (referred to as the _supervisor_).

The specification for the virtual processor is outlined in this file.

## CPU

The CPU is 16-bit. It contains the following registers:

* A program counter (`PC`)
* A status register (`SR`)
* A link register (`LR`)
* Two general-purpose registers (`R0` and `R1`)

The processor also contains an "instruction register" (`IR`) which holds the current instruction as it is decoded, but this cannot be manipulated during operation. It is mainly useful for debugging, as it is preserved if a fault is raised.

## Memory Model

The CPU addresses two separate memory segments: the read-only code segment (`CS`) and the read-write data segment (`DS`). The program counter's address **always** refers to the code segment, and all other registers, if used to reference memory, **always** refer to the data segment. This means that there is no such thing as dynamically-loaded machine code in this memory model.

```
        CS                                          DS
+----------------+                          +----------------+
|................| <- |.......PC.......|    |................|
|................|    |.......LR.......| -> |................|
|................|    |.......R0.......| -> |................|
|................|    |.......R1.......| -> |................|
+----------------+                          +----------------+
```

Each memory segment lives in a 16-bit address space, and so can hold a maximum of 65536 bytes (64KB). A program that runs on the V2MP CPU consists of the `CS` and `DS` segments, where the `CS` segment holds all code for the program, and the `DS` segment holds any pre-compiled data that the program may wish to use. It is completely up to the progam in question how the `DS` data is managed - any data originally supplied with the program may be modified.

The following restrictions apply to any memory access (either to `CS` or `DS`) from any register:

* The address of the access must be aligned to a word (ie. to a 16-bit boundary). If this is not the case, a fault will be raised.
* The address of the access must be within the size of the segment. Although 64KB of memory from each segment is addressable, the entire memory space might not be used if the program is not that large, and so will not be allocated by the supervisor. If access is attempted to a memory address outside of either segment, a fault will be raised.

The V2MP memory model does not support dynamic memory allocation: all memory must be statically allocated. Since stack space is not provided automatically by the memory model, it is up to the program in question to allocate its own stack in `DS` and to ensure that it is used appropriately.

## Instruction Set

As the V2MP CPU is 16-bit, each instruction is also comprised of 16 bits. The highest 4 bits represent the opcode, and the other bits represent operands.

```
      Bits [15 12] represent the opcode
      V--V
High |XXXX|DDDDDDDDDDDD| Low
           ^----------^
           Bits [11 0] represent the operands (instruction-specific)
```

Operand bits in register diagrams are assigned letters based on which operands they correspond to. Operand bits represented by `.` are ignored by the instruction.

Some instructions make reference to a register in the CPU using a 2-bit identifier as an operand. Whenever one of these identifiers is used, it refers to the following register unless otherwise stated:

* `b00` refers to `R0`.
* `b01` refers to `R1`.
* `b10` refers to `LR`.
* `b11` refers to `PC`.

### 0x0: Halt (`HCF`)

Also known as "halt and catch fire". Stops the processor, leaves all registers as they are, and raises a fault. All operand bits in the instruction are ignored.

```
 HCF
|0000|............|
```

### 0x1: Load/Store (`LDST`)

Depending on the operands, either loads a value from a location in memory, or stores a value to a location in memory.

```
 LDST
|0001|ABB.........|
```

* Bit `[11] (A)` specifies the mode of the operation:
  * `b0` means that the operation is a load.
  * `b1` means that the operation is a store.
* Bits `[10 9] (B)` specify the two-bit identifier of the register whose value will be used for the operation.

The memory location for the load or store is always specified by the value of `LR`.

Bits `[8 0]` of the instruction are reserved for future use, and are ignored.

### 0x2: Assign (`ASGN`)

Assigns a value to a register. Assigning to `PC` (`b11`) is equivalent to an unconditional jump.

```
 ASGN
|0010|AABBCCCCCCCC|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is copied to the destination register, and the source register remains unchanged. This behaviour is the same regardless of the destination register.

If instead the source `A` and destination `B` register identifiers are the same, bits `[7 0] (C)` of the instruction are interpreted as an 8-bit literal to assign to the destination register. The behaviour of the assignment depends on the destination register.

If the destination register is:

* `R0` (`b00`),
* `R1` (`b01`), or
* `LR` (`b10`),

bits `[7 0] (C)` of the instruction are assigned to the destination register's bits `[7 0]`. Bits `[15 8]` in the destination register are assigned to `0`.

```
 R0, R1, or LR
|00000000CCCCCCCC|
```

If instead the destination register is `PC` (`b11`), bits `[7 0] (C)` of the instruction are treated as a _signed offset in words_ from `PC`'s current value. This means that `PC` can be incremented by up to 127 words (254 bytes), or decremented by up to 128 words (256 bytes).

```
 ASGN         += 4      PC - 0x400 (1024)
|0010|111100000100| -> |0000010000000000|
                       V                V
                        PC - 0x408 (1032)
                       |0000010000001000|

```

### 0x3: Add (`ADD`)

Increments the value in a register.

```
 ADD
|0011|AABBCCCCCCCC|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is used to increment the destination register, and the source register remains unchanged.

If instead the source `A` and destination `B` register identifiers are the same, bits `[7 0] (C)` of the instruction are interpreted as an unsigned 8-bit literal, which is used to increment destination register. This behaviour is the same regardless of the destination register.

### 0x4: Subtract (`SUB`)

Decrements the value in a register.

```
 SUB
|0100|AABBCCCCCCCC|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is used to decrement the destination register, and the source register remains unchanged.

If instead the source `A` and destination `B` register identifiers are the same, bits `[7 0] (C)` of the instruction are interpreted as an unsigned 8-bit literal, which is used to decrement destination register. This behaviour is the same regardless of the destination register.

### 0x5: Bit Shift (`SHFT`)

Shifts the bits in a register left or right.

```
 SHFT
|0101|AABB...CCCCC|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register whose value determines the magnitude of the shift. Only bits `[4 0]` of the value are used to determine the magnitude; the rest of the bits are ignored.
* Bits `[9 8] (B)` specify the two-bit identifier of the register whose value will be shifted.

If both register identifiers `A` and `B` are the same, bits `[4 0] (C)` of the instruction are used to determine the magnitude of the shift. Bits `[7 5]` of the instruction are reserved for future use, and are ignored.

Regardless of whether the shift magnitude is taken from a register or from the instruction, the magnitude is treated as a signed 5-bit number, ranging from -16 to 15. A negative magnitude implies a right shift (dividing the value by 2), and a positive magnitude implies a left shift (multiplying the value by 2).

Bits that are shifted off either end of the register are discarded, and bits that are inserted into the register due to the shift are always `0`. A shift of -16 is equivalent to clearing the register to all `0` bits.

### 0x6: Bitwise Operation (`BITW`)

Performs a bitwise operation between two register values.

```
 BITW
|0110|AABBCCD.EEEE|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source of the bit mask.
* Bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.
* Bits `[7 6] (C)` specify the type of bitwise operation to perform:
  * `b00` performs a bitwise `AND`.
  * `b01` performs a bitwise `OR`.
  * `b10` performs a bitwise `XOR`.
  * `b11` performs a bitwise `NOT` (ie. flips all bits).

If the source `A` and destination `B` register identifiers are different, bits `[5] (D)` and `[3 0] (E)` of the instruction are ignored, and the source register's value is used as the bit mask that is imposed on the destination register. In the case of a bitwise `NOT` operation (which does not require a bit mask), only the destination register is considered, and the identifier for the source register is ignored.

If the source `A` and destination `B` register identifiers are the same, the bit mask is not based on the value in a register. Instead, it is created based on a mask value of `b0000000000000001` which is manipulated by bits `[5] (D)` and `[3 0] (E)`:

* Bits `[3 0] (E)` specify the magnitude of the left shift of the mask value, in the range [0 15]. For example, if bits `[3 0] (E)` were set to `b0101` (a shift of 5), this would create the mask `b0000000000100000`.
* Bit `[5] (D)` specifies if the resulting mask should be inverted. If bit `[5] (D)` is `0` then the mask is not inverted; if it is `1` then the mask is inverted. The above example would be `b1111111111011111` if bit `[5] (D)` was `1`.

Similarly to the earlier case where register identifiers `A` and `B` were different, if the bitwise operation is `NOT` then the generated bit mask is redundant and not used.

In all cases, instruction bit `[4]` is reserved for future use, and is ignored.

### 0x7: Inter-Device Call (`IDC`)

Initiates an inter-device call.

```
 IDC
|0111|............|
```

To make the call, the port is specfied by the value in `R0`, and the ordinal by the value in `R1`. The arguments to the call live in the memory address specified by the value in `LR`. It is assumed that there is enough memory available at the address for the call to read the arguments; if there is not, a fault will be raised.

If the port specified by `R0` is out of range, a fault will be raised. If the ordinal specified by `R1` is not recognised, the error bit of the status register `SR` will be set.

Bits `[11 0]` of this instruction are reserved for future use, and are ignored.

### 0x8 Inter-Device Response (`IDR`)

Checks for the response from a previous inter-device call.

```
 IDC
|1000|............|
```

The port is specified by the value in `R0`.

### Todo Notes

Start with simplest instructions possible, ignoring as many operand bits as possible. Then, as time goes on and things develop, we can refine the instructions and make proper use of the operands.

* Load/store: either load into a specified register the value at the address indicated by LR, or store, in the address indicated by LR, the value held by a specified register. Which operation is performed is dependent on the operand of the instruction.
* Assign: assign the value of a source register to a target register, optionally based on some conditions. Assigning to the program counter results in jumping or branching.
* Subtract: subtract the value in a source from the value in a destination register, and store the result in the destination register.
* Bit operation: perform a bit operation using a source and target register. What registers are involved, and what the operation is (AND, OR, XOR) is specified by the operands.
* Bit shift: shift the bits in a destination register by the value in a source register. The lower 5 bits are used as a signed value (positive is left, negative is right).
* IDC: inter-device call. Value in R0 indicates port, and value in LR points to RAM where arguments are stored.
* IDR: inter-device response. Value in R0 indicates port, and the result of the call is written back to R0.

TODO: Status register, plus not incrementing PC if it was modified by the previous instruction.

TODO: Tachi recommendation - raise faults on unrecognised bits of instructions instead of ignoring them. That way, in future the instruction set can be refined without breaking existing software (as you know that the reserved bits are definitely 0 for working software).
