V2MP
====

V2MP is an acronym for the Vesper Virtual Micro Processor. This virtual processor is designed to be very simple, with a small set of instructions, and to live within a virtual environment managed by a host machine (referred to as the "supervisor").

The specification for the virtual processor is outlined in this file.

* [Documentation Conventions](#documentation-conventions)
* [CPU](#cpu)
* [Memory Model](#memory-model)
* [Device Ports](#device-ports)
* [Instruction Set](#instruction-set)
  * [0h HCF: Halt](#h-halt-hcf)
  * [1h LDST: Load/Store](#h-loadstore-ldst)
  * [2h ASGN: Assign](#h-assign-asgn)
  * [3h ADD: Add](#h-add-add)
  * [4h SUB: Subtract](#h-subtract-sub)
  * [5h SHFT: Bit Shift](#h-bit-shift-shft)
  * [6h BITW: Bitwise Operation](#h-bitwise-operation-bitw)
  * [7h CBX: Conditional Branch](#h-conditional-branch-cbx)
  * [8h DPQ: Device Port Query](#h-device-port-query-dpq)
  * [9h DPO: Device Port Operation](#h-device-port-operation-dpo)
* [Faults](#faults)

## Documentation Conventions

This documentation uses the following conventions when describing the functionality of the processor.

### Numeric Literals

All numeric literals are specified in base 10 by default. Suffixes on numeric literals indicate a different base:

* `h` indicates that the number is in base 16 (hexadecimal). `10h` == `16`.
* `b` indicates that the number is in base 2 (binary). `10b` == `2`.

If a bit is described as being "set" then this means its value is `1`. If a bit is described as being "cleared" then this means its value is `0`.

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

* A program counter (`PC`)
* A status register (`SR`)
* A link register (`LR`)
* Two general-purpose registers (`R0` and `R1`)

The processor also contains an "instruction register" (`IR`) which holds the current instruction as it is decoded, but this cannot be manipulated during operation. It is mainly useful for debugging, as it is preserved if a fault is raised.

### Program Counter (`PC`)

The program counter points to the next instruction in code memory. Its value must always be aligned to a word boundary (ie. `PC[0]` must be `0`), or an [`ALGN`](#faults) fault will be raised when the instruction it points to is fetched.

The program counter is automatically incremented after the execution of each instruction, unless the instruction wrote to the program counter. In this case, the value of the program counter is assumed to point to the next instruction, so no increment takes place.

### Status Register (`SR`)

The status register holds flags relating to the result of the execution of the previous instruction. It cannot be accessed directly by instructions, but its state is used to affect the operation of certain instructions (eg. for conditional branching).

Before each instruction, all register bits are set to `0`. After the instruction, one or more bits may be set to `1` based on the result of the instruction.

```
|..............CZ|
```

The contents of the register are as follows:

* Bit `[0]`, or `SR[Z]`, is the `ZERO` bit. This bit is set to `1` to indicate the absence of a result after an instruction is executed, or is set to `0` otherwise.
  * For arithmetic instructions, if this bit is set to `1` then it indicates that the result of the instruction was zero.
  * For other instructions, a value of `1` may indicate for example that some requested device or state was not available.
* Bit `[1]`, or `SR[C]`, is the `CARRY` bit. This bit is set to `1` if the previous instruction resulted in an overflow or an underflow, or is set to `0` otherwise.
* Bits `[15 2]` are reserved for future use, and are always set to `0`.

Any instructions that may affect the state of the status register will describe the nature of the state changes. Additionally, any instructions that operate based on the current state of the status register will describe how the state is used.

### Link Register (`LR`)

The link register is used by certain instructions to refer to an address memory. Instructions which make use of the link register will describe the nature of its use.

Outside of these instructions, the link register can also be used as a general-purpose register if desired.

### General-Purpose Registers (`R0` and `R1`)

The general-purpose registers do not have any special significance for most instructions - they are simply used to manipulate values. Any instructions that use `R0` or `R1` in specific ways will explain the nature of their use.

## Memory Model

The CPU addresses two separate memory segments: the read-only code segment (`CS`) and the read-write data segment (`DS`). The program counter's address **always** refers to the code segment, and instructions to read from or write to memory **always** refer to the data segment. This means that under this memory model, code cannot be executed from RAM, and data for use in general-purpose registers cannot be directly loaded from the code segment. Some instructions, however, do support packing numerical literals into their operand bits, and these can affect the values in general-purpose registers.

```
        CS                                          DS
+----------------+                          +----------------+
|................| <- |.......PC.......|    |................|
|................|    |.......LR.......| -> |................|
|................|    |.......R0.......| -> |................|
|................|    |.......R1.......| -> |................|
+----------------+                          +----------------+
```

Each memory segment lives in a 16-bit address space, and so can hold a maximum of 65536 bytes (64KB). A program that runs on the V2MP CPU consists of the `CS` and `DS` segments, where the `CS` segment holds all code for the program, and the `DS` segment holds any pre-compiled data that the program may wish to use. It is completely up to the program in question how the `DS` data is managed - any data originally supplied within `DS` may be modified.

The following restrictions apply to any memory access (either in `CS` or `DS`), via any register (`PC` or general-purpose):

* The address of the access must be aligned to a word (ie. to a 16-bit boundary). If this is not the case, an [`ALGN`](#faults) fault will be raised.
* The address of the access must be within the size of the segment. Although 64KB of memory from each segment is addressable, the entire memory space might not be used if the program is not that large, and so will not be allocated by the supervisor. If access is attempted to a memory address outside of either segment, a [`SEG`](#faults) fault will be raised.

The V2MP memory model does not support dynamic memory allocation: all memory must be statically allocated. However, supervisor calls may be used to manipulate memory pages themselves at runtime, eg. to swap one page out for another.

### Relevant Instructions

The [`LDST`](#h-loadstore-ldst) instruction loads or stores single words from or to `DS`. The [`DPO`](#h-device-port-operation-dpo) instruction can transfer data between a device port and an address in `DS`.

The [`CBX`](#h-conditional-branch-cbx) instruction makes reference to an address in `CS` which the program counter `PC` should be set to if the branch condition is met. Other arithmetic instructions may directly modify the contents of `PC` to point to different addresses in `CS`.

## Device Ports

V2MP programs can communicate with other devices in the system using device ports. These are identified using a 16-bit address and numbered starting from `0`. Port `0` is the supervisor port, and is always attached to the supervisor device; other ports may or may not be attached to devices.

### Mailboxes

Each port contains a mailbox. The mailbox has space for a certain number of bytes, and is used to perform half-duplex communications with the device on the port. The format of the messages between the program and the device depends entirely on what the device expects.

The program can query the number of bytes currently available in a device's mailbox, either for reading or for writing. If required, a device may change the size of its mailbox dynamically at runtime, though this is not recommended without the device first informing the program via a message. The mailbox is at all times expected to be large enough to accommodate any single, complete message - messages whose sizes are larger than the maximum space in the mailbox are not supported.

When performing a write to a port's mailbox, the program may indicate that the write does not yet result in a complete message. This is referred to as an "uncommitted" write, and usually occurs if the program needs to write the complete message over the course of multiple processor instructions.

The program may conversely indicate that it has finished writing a complete message, which is referred to as a "committed" write. Immediately after a committed write, the device's mailbox is no longer considered writeable, and the program must query the state of the port to determine when it is writeable again. After a committed write, the point at which the port's mailbox becomes readable or writeable again is controlled by the attached device.

It should be noted that, if the program performs an uncommitted write to the mailbox which fills up all the remaining space, the write is treated as if it were a committed write.

### Data Transfer

Two methods of data transfer are supported when reading from or writing to a device port mailbox:

* **Direct data transfer**: a single word from a CPU register is written to the mailbox, or a single word from the mailbox is read into a CPU register. This data transfer always completes in one clock cycle.
* **Indirect data transfer**: given a memory address and a maximum number of bytes, both of which are held in CPU registers, one or more bytes are either read from the mailbox into the specified `DS` address, or written from the specified `DS` address into the mailbox. The data transfer is performed by the supervisor, and may take more than one clock cycle.

When passing a message using indirect data transfer, it is assumed that a buffer of at least the specified size is present in `DS` at the given address. Until the indirect data transfer has completed, accessing this buffer in any way may cause undefined behaviour.

### State

If a device port has no device attached to it, it is considered "disconnected". A disconnected port cannot be operated on using a [`DPO`](#h-device-port-operation-dpo) instruction; if this occurs, an [`IDO`](#faults) fault will be raised.

If a port does have a device attached to it, it is considered "connected". Depending on the state of the port's mailbox, a connected port may be read from or written to by the program using [`DPO`](#h-device-port-operation-dpo) instructions.

A port's mailbox may be "readable", "writeable", or "unavailable". It may not be in more than one of these states at the same time.

* If a mailbox is readable, this means that there is at least one byte of message data ready to be consumed by the program.
* If a mailbox is writeable, this means that there is at least one byte of free space available to be written to by the program.
* If a mailbox is unavailable, this means that it is neither readable nor writeable. An unavailable mailbox may not be read from or written to by a program, or an [`IDO`](#faults) fault will be raised.

If a port's mailbox is readable or writeable, it may at the same time be either "busy" or "ready".

* If the mailbox is busy, this means that an indirect data transfer is taking place, either writing bytes to the mailbox, or reading bytes from it. A mailbox may not change state between being readable, writeable, or unavailable while it is busy - any existing data transfer must first be completed. No new data transfers to or from the mailbox may be initiated by the program while the mailbox is busy, or an [`IDO`](#faults) fault will be raised.
* If the mailbox is ready, this means that no indirect data transfer is taking place. A new data transfer may be initiated either to read from the mailbox or to write to it, depending on its readable or writeable state.

It should be noted that the mailbox for any disconnected port is implicitly unavailable. If a port's mailbox is unavailable, the mailbox is also implicitly busy.

### State Diagram

The following diagram describes the state transitions allowed for a port's mailbox when the port is connected to a device.

![Device port state diagram](img/device-port-state-diagram.png)

### Connected State Diagram

The following diagram describes the state transitions allowed when a device is connected to or disconnected from a port. If a device is disconnected from a port while the mailbox is busy, the data transfer in progress is immediately terminated. This will likely corrupt the message that was being read or written, so should be avoided.

![Device port connected state diagram](img/device-port-connected-state-diagram.png)

### Relevant Instructions

The [`DPQ`](#h-device-port-query-dpq) and [`DPO`](#h-device-port-operation-dpo) instructions are used to respectively query a device port's state, and to perform an operation on a device port.

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

### `0h`: Halt (`HCF`)

Also known as "halt and catch fire". This instruction stops the processor, leaves all registers as they are, and raises an [`HCF`](#faults) fault.

All operand bits are reserved for future use, and must be set to `0`. If this is not the case, the fault raised will instead be a [`RES`](#faults) fault.

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

* Operand bit `[11] (A)` specifies the mode of the operation:
  * `0b` means that the operation is a load.
  * `1b` means that the operation is a store.
* Operand bits `[10 9] (B)` specify the two-bit identifier of the register whose value will be used for the operation.

The memory location for the load or store is always specified by the value of `LR`. This always refers to the data segment `DS`.

Operand bits `[8 0]` of the instruction are reserved for future use. If any of these bits is not `0`, a [`RES`](#fault) fault will be raised.

If the memory address specified by `LR` is not aligned to a word boundary, an [`ALGN`](#faults) fault will be raised. If the address is not within the boundaries of `DS`, a [`SEG`](#faults) fault will be raised.

If the value that is loaded into the destination register is zero, `SR[Z]` is set; otherwise, it is cleared. All other bits in `SR` are always cleared.

### `2h`: Assign (`ASGN`)

Assigns a value to a register.

```
 ASGN
|0010|AABBCCCCCCCC|
```

* Operand bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Operand bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is copied to the destination register, and the source register remains unchanged. This behaviour is the same regardless of the destination register. Operand bits `[7 0] (C)` of the instruction must be set to `0` in this case, or a [`RES`](#faults) fault will be raised.

Assigning to the program counter `PC` (`11b`) is equivalent to performing an unconditional jump. Beware not to assign a value that is not aligned to a word boundary. If an assignment like this is performed, an [`ALGN`](#faults) fault will be raised upon fetching the next instruction.

If instead the source `A` and destination `B` register identifiers are the same, operand bits `[7 0] (C)` of the instruction are interpreted as a signed 8-bit literal to assign to the destination register. The behaviour of the assignment depends on the destination register.

If `A` and `B` are the same and the destination register is `R0`, `R1` or `LR`, the following assignment is performed:

* Bits `[6 0]` in the destination register are assigned the operand bits `[6 0]`.
* Bits `[15 8]` in the destination register are set to `0` if operand bit `[7]` was `0`; otherwise, bits `[15 8]` in the destination register are set to `1`.

```
 R0, R1, or LR, assigning operand 00000010b (2):
|0000000000000010|

 R0, R1, or LR, assigning operand 10000010b (-126):
|1111111110000010|
```

If instead `A` and `B` are the same and the destination register is `PC` (`11b`), operand bits `[7 0] (C)` are treated as a **signed offset in words** from `PC`'s current value. This means that `PC` can be incremented by up to `127` words (`254` bytes), or decremented by up to `128` words (`256` bytes).

```
 ASGN  PC     += 4W      PC - 400h (1024)
|0010|1111|00000100| -> |0000010000000000|
                        V                V
                         PC - 408h (1032)
                        |0000010000001000|


 ASGN  PC     -= 4W      PC - 400h (1024)
|0010|1111|11111100| -> |0000010000000000|
                        V                V
                         PC - 3F8h (1016)
                        |0000001111111000|
```

If `PC` is incremented or decremented, and the increment or decrement respectively overflows or underflows `PC`, `SR[C]` is set; otherwise, it is cleared. In all other cases, where a value is simply assigned verbatim to a register, `SR[C]` is cleared.

In all cases described for this instruction, `SR[Z]` is set if the eventual value in the destination register is zero; otherwise, it is cleared. All other bits in `SR` are cleared.

### `3h`: Add (`ADD`)

Increments the value in a register.

```
 ADD
|0011|AABBCCCCCCCC|
```

* Operand bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Operand bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is used to increment the destination register, and the source register remains unchanged. Operand bits `[7 0] (C)` must be set to `0` in this case, or a [`RES`](#faults) fault will be raised.

If instead the source `A` and destination `B` register identifiers are the same, operand bits `[7 0] (C)` are interpreted as an unsigned 8-bit literal, which is used to increment destination register. This behaviour is the same regardless of the destination register.

It should be noted that, contrary to the [`ASGN`](#h-assign-asgn) instruction which may increment or decrement the program counter `PC` by a certain number of _words,_ the [`ADD`](#h-add-add) instruction will always increment the destination register's absolute value, regardless of which register it is. This means that it is possible to increment `PC` by an odd number of byes, causing it to no longer be aligned to a word boundary. This type of operation should be avoided, or an [`ALGN`](#faults) fault will be raised upon fetching the next instruction.

If the add operation overflows the destination register, `SR[C]` is set; otherwise, it is cleared.

If the add operation results in a value of `0` in the destination register (ie. a 1-bit arithmetic overflow occurs), `SR[Z]` is set; otherwise, it is cleared.

All other bits in `SR` are always cleared.

### `4h`: Subtract (`SUB`)

Decrements the value in a register.

```
 SUB
|0100|AABBCCCCCCCC|
```

* Operand bits `[11 10] (A)` specify the two-bit identifier of the register to use as the source.
* Operand bits `[9 8] (B)` specify the two-bit identifier of the register to use as the destination.

If the source `A` and destination `B` register identifiers are different, the source register's value is used to decrement the destination register, and the source register remains unchanged. Operand bits `[7 0] (C)` must be set to `0` in this case, or a [`RES`](#faults) fault will be raised.

If instead the source `A` and destination `B` register identifiers are the same, operand bits `[7 0] (C)` are interpreted as an unsigned 8-bit literal, which is used to decrement destination register. This behaviour is the same regardless of the destination register.

It should be noted that, contrary to the [`ASGN`](#h-assign-asgn) instruction which may increment or decrement the program counter by a certain number of _words,_ the [`SUB`](#h-subtract-sub) instruction will always decrement the destination register's absolute value, regardless of which register it is. This means that it is possible to decrement the program counter by an odd number of byes, causing it to no longer be aligned to a word boundary. This type of operation should be avoided, or an [`ALGN`](#faults) fault will be raised upon fetching the next instruction.

If the subtraction operation underflows the destination register, `SR[C]` is set; otherwise, it is cleared.

If the subtraction operation results in a value of `0` in the destination register (ie. a 1-bit arithmetic underflow occurs), `SR[Z]` is set; otherwise, it is cleared.

All other bits in `SR` are always cleared.

### `5h`: Bit Shift (`SHFT`)

Shifts the bits in a register left or right.

```
 SHFT
|0101|AABB...CCCCC|
```

* Operand bits `[11 10] (A)` specify the two-bit identifier of the register whose value determines the magnitude of the shift. Only bits `[4 0]` of the register are used to determine the magnitude; the rest of the bits are ignored.
* Operand bits `[9 8] (B)` specify the two-bit identifier of the register whose value will be shifted.

If both register identifiers `A` and `B` are the same, operand bits `[4 0] (C)` are used to determine the magnitude of the shift. If register identifiers `A` and `B` are different, operand bits `[4 0] (C)` must be set to `0`, or a [`RES`](#faults) fault will be raised.

In all cases, operand bits `[7 5]` are reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

Regardless of whether the shift magnitude is taken from a register or from the instruction, the magnitude is treated as a signed 5-bit number, ranging from `-16` to `15`. A negative magnitude implies a right shift (dividing the value by 2), and a positive magnitude implies a left shift (multiplying the value by 2).

Bits that are shifted off either end of the register are discarded, and bits that are inserted into the register due to the shift are always `0`. A shift of `-16` is equivalent to clearing the register to all `0` bits.

If after the operation the remaining value in the register is zero, `SR[Z]` is set to `1`; otherwise, it is set to `0`. Additionally, if any `1` bits were shifted off the end of the register during the operation, `SR[C]` is set to `1`; otherwise, it is set to `0`. All other bits in `SR` are set to `0`.

### `6h`: Bitwise Operation (`BITW`)

Performs a bitwise operation between two register values.

```
 BITW
|0110|AABBCCD.EEEE|
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

### `7h` Conditional Branch (`CBX`)

Decided whether or not to modify the value of the program counter `PC`, depending on the current state of the status register `SR`.

```
 CBX
|0111|AB..CCCCCCCC|
```

* Operand bit `[11] (A)` specifies how the value of `PC` should be modified:
  * If `A` is `1`, the value in `LR` is treated as the address to jump to, and is assigned to `PC`. In this case, operand bits `[7 0] (C)` must be set to `0`; if they are not, a [`RES`](#faults) fault will be raised.
  * If `A` is `0`, the value of operand bits `[7 0] (C)` is treated as a signed 8-bit offset **in words** from the current `PC` address. `PC` is incremented (or decremented, depending on the sign) by this value.
* Operand bit `[10] (B)` specifies what aspect of `SR` is considered when deciding whether to branch.
  * If `B` is `0`, `SR[Z]` is considered. If `SR[Z]` is set (ie. the last instruction that was executed produced a result of zero), `PC` is modified according to the rules above. If `SR[Z]` is not set, `PC` is not modified and will be incremented as normal once the `CBX` instruction is complete.
  * If `B` is `1`, `SR[C]` is considered. If `SR[C]` is set (ie. the last instruction that was executed overflowed or underflowed its register), `PC` is modified according to the rules above. If `SR[C]` is not set, `PC` is not modified and will be incremented as normal once the `CBX` instruction is complete.

Operand bits `[9 8]` are reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

After this instruction, `SR[Z]` will be set to `1` if the condition was not met. If the condition was met, and `PC` was set, `SR[Z]` will be set to `0`. All other bits in `SR` will be set to `0`.

### `8h` Device Port Query (`DPQ`)

Queries the state of a device communications port, and sets the status register `SR` appropriately.

```
 DPQ
|1000|........AAA|
```

The number of the port to be queried is held in `R0`. Additionally, operand bits `[2 0] (A)` specify the type of query to perform:

* If operand bits `[2 0] (A)` are set to `000b`, the instruction queries the connected state of the port - ie. whether there is a device currently attached to the port.
* If operand bits `[2 0] (A)` are set to `001b`, the instruction queries whether the port's mailbox is readable _and_ ready - ie. whether a data transfer can be initiated for reading.
* If operand bits `[2 0] (A)` are set to `010b`, the instruction queries whether the port's mailbox is writeable _and_ ready - ie. whether a data transfer can be initiated for writing.
* If operand bits `[2 0] (A)` are set to `011b`, the instruction queries whether the port's mailbox is _either_ readable _or_ writeable. Whether the port is ready or busy does not affect the result of the query. This query is equivalent to checking whether the port is available.
* If operand bits `[2 0] (A)` are set to `100b`, the instruction queries whether the port's mailbox is busy - ie. whether a data transfer is currently in progress. This query is useful to determine whether the data transfer buffer in memory should currently be considered off-limits for the CPU to access.

Values `101b`, `110b`, and `111b` are reserved. If operand bits `[2 0] (A)` are set to any of these values, a [`RES`](#faults) fault will be raised.

Operand bits `[11 3]` in the instruction word are reserved, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

After the instruction is executed, `SR[Z]` is set based on the query type. The convention followed is to set `SR[Z]`, as the "zero" status bit, to indicate the _absence_ of the state being queried for. Therefore, conditionally branching on the presence of `SR[Z]` implies "branch on failure" - that the `PC` jump occurs if the state queried for was _not_ present.

* If `A` is `000b`, `SR[Z]` will be set if there _is not_ a device attached to the port, and will be cleared if there _is_ a device attached.
* If `A` is `001b`, `SR[Z]` will be set if the port's mailbox was _not_ readable, and will be cleared if the mailbox _was_ readable.
* If `A` is `010b`, `SR[Z]` will be set if the port's mailbox was _not_ writeable, and will be cleared if the mailbox _was_ writeable.
* If `A` is `011b`, `SR[Z]` will be set if the port's mailbox was _neither_ readable _nor_ writeable (ie. the port was unavailable), and will be cleared if the mailbox was _either_ readable _or_ writeable (ie. the port was available).
* If `A` is `100b`, `SR[Z]` will be set if the port's mailbox _was not_ busy (ie. the port was ready), and will be cleared if the mailbox _was_ busy.

All other bits in `SR` will be set to `0`.

### `9h` Device Port Operation (`DPO`)

Performs an operation on a device communications port.

```
 DPO
|1001|A........BB|
```

The number of the port to use is held in `R0`. Operand bit `[11] (A)` is used to specify the type of data transfer used for a read or a write:

* If operand bit `[11] (A)` is `0`, this means that a read or write will be performed using **direct data transfer**.
* If operand bit `[11] (A)` is `1`, this means that a read or write will be performed using **indirect data transfer**.

Additionally, operand bits `[1 0] (B)` are used to specify the type of operation that will take place:

* If operand bits `[1 0] (B)` are set to `00b`, the **usable byte count** is fetched.
* If operand bits `[1 0] (B)` are set to `01b`, a **read** is performed.
* If operand bits `[1 0] (B)` are set to `10b`, an **uncommitted write** is performed.
* If operand bits `[1 0] (B)` are set to `11b`, a **committed write** is performed.

For any operation, operand bits `[10 2]` are reserved for future use, and must be set to `0`. If this is not the case, a [`RES`](#faults) fault will be raised.

The different supported operations, specified by operand `B`, are explained below.

#### Usable Byte Count (`00b`)

If operand bits `[1 0] (B)` are set to `00b`, the operation fetches the number of usable bytes in the device port's mailbox, and places the value into `LR`.

The purpose of the usable bytes depends on whether the mailbox is in a readable or writeable state (remember that a mailbox cannot be both readable and writeable at the same time).

* If the mailbox is in a readable state, the number of usable bytes corresponds to the number of bytes left to read from the mailbox.
* If the mailbox is in a writeable state, the number of useable bytes corresponds to the number of bytes left that can be written to in the mailbox.

When the instruction is executed, operand bit `[11] (A)` must be set to `0`. If it is not, a [`RES`](#faults) fault will be raised.

If the operation is attempted when the mailbox is neither readable nor writeable, a [`IDO`](#faults) fault will be raised.

After the instruction has been executed, `SR[Z]` is set or cleared depending on the number of usable bytes returned. If the number of usable bytes was `0`, `SR[Z]` is set to `1`; otherwise, `SR[Z]` is set to `0`. All other bits in `SR` are set to `0`.

#### Read (`01b`)

TODO

Note that `SR[C]` should be set if there were more bytes to consume in the mailbox than were transferred in the read (ie. not all of the message has been read yet). `SR[Z]` should be set if the number of bytes transferred in the read exactly matched the size of the buffer they were transferred into (ie. the remaining space in the buffer was `0`).

#### Uncommitted Write (`10b`)

TODO

Note that `SR[Z]` should be set if there were no more free bytes available in the mailbox after the write. `SR[C]` should be set if more bytes were specified in the write command than were available to write to in the mailbox.

#### Committed Write(`11b`)

TODO

Note that `SR[Z]` should be set if there were no more free bytes available in the mailbox after the write. `SR[C]` should be set if more bytes were specified in the write command than were available to write to in the mailbox.

## Faults

The possible faults raised by the processor are described below.

| Index | Code | Name | Description | Triggered By |
| ----- | ---- | ---- | ----------- | ------------ |
| `00h` | `HCF` | Halt and Catch Fire | Raised by the `HCF` instruction to indicate that the processor has halted. | `HCF` |
| `01h` | `RES` | Reserved Bits Set | Raised when an instruction is decoded and one or more reserved bits are set to `1`. | Execution of any instruction |
| `02h` | `ALGN` | Alignment Violation | Raised when an unaligned memory address is dereferenced in `CS` or `DS`. | `LDST`, `DPO`, or upon fetching the next instruction using `PC`. |
| `03h` | `SEG` | Segment Access Violation | Raised when an address outside `CS` or `DS` is dereferenced. | `LDST`, `DPO`, or upon fetching the next instruction using `PC` |
| `04h` | `IDO` | Invalid Device Operation | Raised when an operation is attempted on a port which is not in the correct state for the operation. | `DPO` |

## Todo Notes

### Stack

Do we have enough remaining instructions to implement stack support? We're beginning to need it once we start wanting to check the results of device operations...
