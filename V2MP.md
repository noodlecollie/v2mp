
V2MP
====

V2MP is an acronym for the Vesper Virtual Micro Processor. This virtual processor is designed to be very simple, with a small set of instructions, and to live within a virtual environment managed by a host machine (referred to as the _supervisor_).

The specification for the virtual processor is outlined in this file.

## Documentation Conventions

This documentation uses the following conventions when describing the functionality of the processor.

### Numeric Literals

All numeric literals are specified in base 10 by default. Suffixes on numeric literals indicate a different base:

* `h` indicates that the number is in base 16 (hexadecimal). `10h` == `16`.
* `b` indicates that the number is in base 2 (binary). `10b` == `2`.

### Bit Indexing

Instruction and register bits are numbered starting from `0` and proceeding right to left. A specific bit, or range of bits, in a register or instruction are referred to using square brackets `[ ]`. For example, `[0]` indicates bit `0`, and `[5 2]` indicates a range of bits beginning at bit `5` on the left, and ending at bit `2` on the right (both bits `5` and `2` are included in this range).

In certain cases (such as for the status register), specific bits are named. In this case, the letter used to identify that specific bit may be enclosed in square brackets, prefixed by the name of the register. For example, `SR[Z]` specifies the `zero` status bit in the status register.

### Bit Layout

The layout of bits in instructions and registers is represented visually by simple layout diagrams.

```
           V Bit 15
Register: |................|
                          ^ Bit 0

              V--V Opcode
Instruction: |....|............|
                   ^----------^ Operands
```

Certain bits may be substituted by letters, to indicate the positions of various elements of an instruction or register. Bits that are not directly used by a certain instruction or register are indicated by `.`, and unless otherwise specified should always be `0`.

## CPU

The CPU is 16-bit and little-endian. It contains the following registers:

* A program counter (`PC`)
* A status register (`SR`)
* A link register (`LR`)
* Two general-purpose registers (`R0` and `R1`)

The processor also contains an "instruction register" (`IR`) which holds the current instruction as it is decoded, but this cannot be manipulated during operation. It is mainly useful for debugging, as it is preserved if a fault is raised.

### Program Counter (`PC`)

The program counter points to the next instruction in code memory. Its value must always be aligned to a word boundary (ie. bit `[0]` must be `0`), or an `ALGN` (alignment) fault will be raised when the instruction it points to is fetched.

The program counter is automatically incremented after the execution of each instruction, unless the instruction wrote to the program counter. In this case, the value of the program counter is assumed to point to the next instruction, so no increment takes place.

### Status Register (`SR`)

The status register holds flags relating to the result of the execution of the previous instruction. It cannot be accessed directly by instructions, but its state is used to affect the operation of certain instructions (eg. for conditional branching).

Before each instruction, all register bits are set to `0`. After the instruction, one or more bits may be set to `1` based on the result of the instruction.

```
|..............CZ|
```

The contents of the register are as follows:

* Bit `[0] (Z)` is the `zero` bit. This bit is set to `1` to indicate the absence of a result after an instruction is executed. For arithmetic instructions, if this bit is set to `1` then it indicates that the result of the instruction was zero. For other instructions, a value of `1` may indicate for example that some requested device or state was not available.
* Bit `[1] (C)` is the `carry` bit. This bit is set to `1` if the previous instruction resulted in an overflow or an underflow.
* Bits `[15 2]` are reserved for future use, and are always set to `0`.

Any instructions that may affect the state of the status register will describe the nature of the state changes.

### Link Register (`LR`)

The link register is used by certain instructions (eg. `LDST`) to refer to data memory. Instructions which make use of the link register will describe the nature of its use.

Outside of these instructions, the link register can also be used as a general-purpose register if desired.

### General-Purpose Registers (`R0` and `R1`)

The general-purpose registers do not have any special significance for most instructions - they are simply used to manipulate values. Any instructions that use `R0` or `R1` in specific ways will explain the nature of their use.

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

* The address of the access must be aligned to a word (ie. to a 16-bit boundary). If this is not the case, an `ALGN` (alignment) fault will be raised.
* The address of the access must be within the size of the segment. Although 64KB of memory from each segment is addressable, the entire memory space might not be used if the program is not that large, and so will not be allocated by the supervisor. If access is attempted to a memory address outside of either segment, a `SEG` (segmentation) fault will be raised.

The V2MP memory model does not support dynamic memory allocation: all memory must be statically allocated. Since stack space is not provided automatically by the memory model, it is up to the program in question to allocate its own stack in `DS` and to ensure that it is used appropriately.

## Instruction Set

As the V2MP CPU is 16-bit, each instruction is also comprised of 16 bits. The highest 4 bits represent the opcode, and the other bits represent operands.

```
      Bits [15 12] represent the opcode
      V--V
High |CCCC|DDDDDDDDDDDD| Low
           ^----------^
           Bits [11 0] represent the operands (instruction-specific)
```

Operand bits in register diagrams are assigned letters based on which operands they correspond to. Operand bits represented by `.` are not used by the instruction, and **must** be set to `0`. If this is not the case, a `RES` (reserved bits set) fault will be raised.

Some instructions make reference to a register in the CPU using a 2-bit identifier as an operand. Whenever one of these identifiers is used, it refers to the following register unless otherwise stated:

* `00b` refers to `R0`.
* `01b` refers to `R1`.
* `10b` refers to `LR`.
* `11b` refers to `PC`.

### `0h`: Halt (`HCF`)

Also known as "halt and catch fire". Stops the processor, leaves all registers as they are, and raises an `HCF` fault.

All operand bits are reserved for future use, and must be set to `0`. If this is not the case, the fault raised will instead be a `RES` fault.

```
 HCF
|0000|............|
```

### `1h`: Load/Store (`LDST`)

Depending on the operands, either loads a value from a location in memory, or stores a value to a location in memory.

```
 LDST
|0001|ABB.........|
```

* Bit `[11] (A)` specifies the mode of the operation:
  * `0b` means that the operation is a load.
  * `1b` means that the operation is a store.
* Bits `[10 9] (B)` specify the two-bit identifier of the register whose value will be used for the operation.

The memory location for the load or store is always specified by the value of `LR`.

Bits `[8 0]` of the instruction are reserved for future use. If any of these bits is not `0`, a `RES` fault will be raised.

If the values that is loaded into the destination register is zero, the `Z` bit will be set in the status register `SR`.

### `2h`: Assign (`ASGN`)

Assigns a value to a register. Assigning to `PC` (`11b`) is equivalent to an unconditional jump.

```
 ASGN
|0010|AABBCCCCCCCC|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is copied to the destination register, and the source register remains unchanged. This behaviour is the same regardless of the destination register. Bits `[7 0] (C)` of the instruction must be set to `0` in this case, or a `RES` faut will be raised.

If assigning to the program counter (`PC`), beware not to assign a value that is not aligned to a word boundary. If an assignment like this is performed, an `ALGN` fault will be raised upon fetching the next instruction.

The `Z` bit in the status register `SR` will be set if the value assigned to the destination register is zero.

If instead the source `A` and destination `B` register identifiers are the same, bits `[7 0] (C)` of the instruction are interpreted as an 8-bit literal to assign to the destination register. The behaviour of the assignment depends on the destination register.

If the destination register is:

* `R0` (`00b`),
* `R1` (`01b`), or
* `LR` (`10b`),

bits `[7 0] (C)` of the instruction are assigned to the destination register's bits `[7 0]`. Bits `[15 8]` in the destination register are assigned to `0`. The `Z` bit in the status register `SR` will be set if the value assigned was zero.

```
 R0, R1, or LR
|00000000CCCCCCCC|
```

If instead the destination register is `PC` (`11b`), bits `[7 0] (C)` of the instruction are treated as a **signed offset in words** from `PC`'s current value. This means that `PC` can be incremented by up to 127 words (254 bytes), or decremented by up to 128 words (256 bytes).

```
 ASGN         += 4      PC - 400h (1024)
|0010|111100000100| -> |0000010000000000|
                       V                V
                        PC - 408h (1032)
                       |0000010000001000|

```

If the increment or decrement respectively overflows or underflows `PC`, the `C` bit will be set in the status register `SR`.

### `3h`: Add (`ADD`)

Increments the value in a register.

```
 ADD
|0011|AABBCCCCCCCC|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is used to increment the destination register, and the source register remains unchanged. Bits `[7 0] (C)` of the instruction must be set to `0` in this case, or a `RES` faut will be raised.

If instead the source `A` and destination `B` register identifiers are the same, bits `[7 0] (C)` of the instruction are interpreted as an unsigned 8-bit literal, which is used to increment destination register. This behaviour is the same regardless of the destination register.

It should be noted that, contrary to the `ASGN` instruction which may increment or decrement the program counter by a certain number of _words,_ the `ADD` instruction will always increment the destination register's absolute value, regardless of which register it is. This means that it is possible to increment the program counter by an odd number of byes, causing it to no longer be aligned to a word boundary. This type of operation should be avoided, or an `ALGN` fault will be raised upon fetching the next instruction.

If the add operation overflows the destination register, the `C` bit is set in the status register `SR`.

### `4h`: Subtract (`SUB`)

Decrements the value in a register.

```
 SUB
|0100|AABBCCCCCCCC|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is used to decrement the destination register, and the source register remains unchanged. Bits `[7 0] (C)` of the instruction must be set to `0` in this case, or a `RES` faut will be raised.

If instead the source `A` and destination `B` register identifiers are the same, bits `[7 0] (C)` of the instruction are interpreted as an unsigned 8-bit literal, which is used to decrement destination register. This behaviour is the same regardless of the destination register.

It should be noted that, contrary to the `ASGN` instruction which may increment or decrement the program counter by a certain number of _words,_ the `SUB` instruction will always decrement the destination register's absolute value, regardless of which register it is. This means that it is possible to decrement the program counter by an odd number of byes, causing it to no longer be aligned to a word boundary. This type of operation should be avoided, or an `ALGN` fault will be raised upon fetching the next instruction.

If the add operation underflows the destination register, the `C` bit is set in the status register `SR`.

### `5h`: Bit Shift (`SHFT`)

Shifts the bits in a register left or right.

```
 SHFT
|0101|AABB...CCCCC|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register whose value determines the magnitude of the shift. Only bits `[4 0]` of the value are used to determine the magnitude; the rest of the bits are ignored.
* Bits `[9 8] (B)` specify the two-bit identifier of the register whose value will be shifted.

If both register identifiers `A` and `B` are the same, bits `[4 0] (C)` of the instruction are used to determine the magnitude of the shift. If register identifiers `A` and `B` are different, bits `[4 0] (C)` must be set to `0`, or a `RES` fault will be raised.

In all cases, bits `[7 5]` of the instruction are reserved for future use, and must be set to `0`. If this is not the case, a `RES` fault will be raised.

Regardless of whether the shift magnitude is taken from a register or from the instruction, the magnitude is treated as a signed 5-bit number, ranging from -16 to 15. A negative magnitude implies a right shift (dividing the value by 2), and a positive magnitude implies a left shift (multiplying the value by 2).

Bits that are shifted off either end of the register are discarded, and bits that are inserted into the register due to the shift are always `0`. A shift of -16 is equivalent to clearing the register to all `0` bits.

If after the operation the remaining value in the register is zero, the `Z` bit is set in the status register `SR`. Additionally, if any `1` bits were shifted off the end of the register during the operation, the `C` bit is set in SR.

### `6h`: Bitwise Operation (`BITW`)

Performs a bitwise operation between two register values.

```
 BITW
|0110|AABBCCD.EEEE|
```

* Bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source of the bit mask.
* Bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.
* Bits `[7 6] (C)` specify the type of bitwise operation to perform:
  * `00b` performs a bitwise `AND`.
  * `01b` performs a bitwise `OR`.
  * `10b` performs a bitwise `XOR`.
  * `11b` performs a bitwise `NOT` (ie. flips all bits).

If the source `A` and destination `B` register identifiers are different, the source register's value is used as the bit mask that is used to manipulate the bits in the destination register. In the case of a bitwise `NOT` operation (which does not require a bit mask), the identifier for the source register is ignored.

In the above case, bits `[5] (D)` and `[3 0] (E)` of the instruction are reserved for future use, and must be set to `0`. If they are not, a `RES` fault will be raised.

If instead the source `A` and destination `B` register identifiers are the same, the bit mask is not based on the value in a register. Instead, it is created based on a mask value of `0000000000000001b` which is manipulated by bits `[5] (D)` and `[3 0] (E)`:

* Bits `[3 0] (E)` specify the magnitude of the left shift of the mask value, in the range [0 15]. For example, if bits `[3 0] (E)` were set to `0101b` (a shift of 5), this would create the mask `0000000000100000b`.
* Bit `[5] (D)` specifies if the resulting mask should be inverted. If bit `[5] (D)` is `0` then the mask is not inverted; if it is `1` then the mask is inverted. The above example would be `1111111111011111b` if bit `[5] (D)` was `1`.

Similarly to the earlier case where register identifiers `A` and `B` were different, if the bitwise operation is `NOT` then the generated bit mask is redundant. In this case, bits `[3 0] (E)` of the instruction are ignored.

In all cases, bit `[4]` is reserved for future use, and must be set to `0`. If this is not the case, a `RES` fault will be raised.

If the resulting value in the destination register is zero, the `Z` bit is set in `SR`.

### `7h` Conditional Branch (`CBX`)

Decided whether or not to modify the value of the program counter `PC`, depending on the current state of the status register `SR`.

```
 CBX
|0111|AB..CCCCCCCC|
```

* Bit `[11] (A)` specifies how the value of `PC` should be modified:
  * If `A` is `1`, the value in `LR` is treated as the address to jump to, and is assigned to `PC`. In this case, bits `[7 0] (C)` must be set to `0`; if they are not, a `RES` fault will be raised.
  * If `A` is `0`, the value of bits `[7 0] (C)` is treated as a signed 8-bit offset **in words** from the current `PC` address. `PC` is incremented (or decremented, depending on the sign) by this value.
* Bit `[10] (B)` specifies what aspect of `SR` is considered when deciding whether to branch.
  * If `B` is `0`, the `Z` bit of `SR` is considered. If `SR[Z]` is set (ie. the last instruction that was executed produced a result of zero), `PC` is modified according to the rules above. If `SR[Z]` is not set, `PC` is not modified and will be incremented as normal once the `CBX` instruction is complete.
  * If `B` is `1`, the `C` bit of `SR` is considered. If `SR[C]` is set (ie. the last instruction that was executed overflowed or underflowed its register), `PC` is modified according to the rules above. If `SR[C]` is not set, `PC` is not modified and will be incremented as normal once the `CBX` instruction is complete.

Bits `[9 8]` are reserved for future use, and must be set to `0`. If this is not the case, a `RES` fault will be raised.

### `8h` Device Port Query (`DPQ`)

Queries the state of a device communications port, and sets the status register `SR` appropriately.

```
 DPQ
|1000|AB.........|
```

The number of the port to be queried is held in `R0`. Additionally, the operand bits of the instruction word specify the type of query to perform:

* Bit `[11] (A)` specifies whether to query the _readable_ state of the port's mailbox - ie. whether there is a message (or a portion of a message) available to be read from the mailbox. If bit `[11] (A)` is set to `1` then the readable state is queried, and if it is set to `0` then the readable state is not queried.
* Bit `[10] (B)` specifies whether to query the _writeable_ state of the port's mailbox - ie. whether it is empty and whether the device in question is ready for data to be transferred into the mailbox. If bit `[10] (B)` is set to `1` then the writeable state is queried, and if it is set to `0` then the writeable state is not queried.

Note that one or more of `A` and `B` in the instruction word must be set. Specifying neither `A` nor `B` is a reserved combination of these two operands, and will cause a `RES` fault to be raised.

After the instruction is executed, the `Z` bit of the status register `SR` will be set based on the query type:

* If bit `[11] (A)` was set and bit `[10] (B)` was not set, `SR[Z]` will be set if the port's mailbox was _not_ readable, and will be cleared if the mailbox _was_ readable.
* If bit `[10] (B)` was set and bit `[11] (A)` was not set, `SR[Z]` will be set if the port's mailbox was _not_ writeable, and will be cleared if the mailbox _was_ writeable.
* If both bits `[11] (A)` and `[10] (B)` were set, `SR[Z]` will be set if the port's mailbox was _neither_ readable _nor_ writeable, and will be cleared if the mailbox was _either_ readable _or_ writeable. This is useful after fully committing a write to a port: the port may remain busy until the device has processed the message and passed back a response, which will then cause it to become readable. In this intermediate busy time, the port will be neither readable nor writeable.

After a device port query instruction, the convention followed is to set `SR[Z]`, as the "zero" status bit, to indicate the _absence_ of the state being queried for. Therefore, conditionally branching on the presence of `SR[Z]` implies "branch on failure" - that the `PC` jump will occur if the state queried for was _not_ present.

Bits `[9 0]` in the instruction word are reserved, and must be set to `0`. If this is not the case, a `RES` fault will be raised.

### `9h` Device Port Operation (`DPO`)

Performs an operation on a device communications port.

```
 DPO
|1001|...........|
```

The number of the port to use is held in `R0`. Additionally, the operand bits of the instruction word specify the type of operation to perform:

TODO: We need a write operation, a read operation, and a "how many bytes are left to read" operation.

TODO: Based on endianness, in what order are bytes read from/written to a register?

## Todo Notes

### Device Port Communications

The device port is always either:

* Readable,
* Writeable, or
* Busy (neither readable nor writeable).

The port is never both readable and writeable at the same time.

**READ**

The read operation should specify the maximum number of bytes to read in `R1`.

* If the number of bytes to read is non-zero, this implies complex data transfer. `LR` holds the address to read into (it is assumed that there is a buffer at this address which holds at least the specified max number of bytes).
* If the number bytes to read is zero, this implies simple data transfer. A word is read and the value is stored in `LR`. If there was only one byte to read, the second byte is always `00h`.

If the max number of bytes to read is less than the remaining number of bytes in the mailbox, the mailbox retains the remaining bytes and remains in a readable state. Otherwise, all the bytes are consumed from the mailbox, and afterwards it is no longer readable (and becomes writeable instead).

After the read, `R1` is set to indicate how many bytes were actually read. In the case of a simple data transfer, this will be either `1` or `2`, depending on how many bytes were left in the mailbox.

`SR[Z]` is set if the mailbox content has been read in its entirety, and otherwise is cleared.

**WRITE**

The write operation should specify the number of bytes to write in `R1`. The instruction should also specify a flag to say whether the write constitutes a complete message, or whether there is more data to come later.

* If the number of bytes to write is non-zero, this implies complex data transfer. `LR` holds the address of the data to be written (it is assumed that there is a buffer at this address which holds at least the specified number of bytes to write).
* If the number of bytes to write is zero, this implies simple data transfer. `LR` holds the word to write.

After the write, `R1` is set to indicate how many bytes were actually written. For a simple data transfer this should usually be `2`, but may be `1` if there was only 1 byte of space left in the mailbox. If there is more data to come in subsequent writes, and the mailbox still has space, then the port remains writeable. Otherwise, it switches to not being writeable. A query should be used to determine when the port has become readable again.

`SR[Z]` is set if the mailbox has been completely filled, and otherwise is cleared.

**AVAILABLE**

The availability operation requests how many bytes are waiting to be read (if the port is readable), or how many bytes of space are available to be written to (if the port is writeable). The result of the instruction is placed in `R1`.

If the result placed in `R1` is zero, `SR[Z]` is set.

### Stack

Do we have enough remaining instructions to implement stack support?
