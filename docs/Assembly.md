V2MP Assembly
=============

## Comments

The V2MP assembler uses `//` for line comments and `/* ... */` for multi-line comments. Comments are ignored by the assembler, and do not form part of the program.

```
// This is a line comment.

/* This is a
   multi-line comment. */
```

## Instructions

An assembly file is parsed one line at a time, and whitespace is used for delimiting tokens. If the line represents an instruction, it is expected to follow the format below:

```
<instruction name> [instruction args ...]
```

The first token is the mnemonic for the instruction in lowercase, and subsequent tokens are arguments for this instruction.

At the most basic level, an instruction's arguments are expected to be numerical values that correspond to the arguments for the instruction, as defined in `V2MP.md`. This means that for an instruction like `ADD`, it must be invoked with exactly three arguments:

```
// add <source_reg> <dest_reg> <literal_value>
// The following adds the value in R1 to that in R0:
add 0 1 0
```

This instruction would be assembled to the following machine code word:

```
// In binary:
// opcode source_reg dest_reg literal_value
   0001   00         01       00000000
```

If an instruction is specified with an incorrect number of arguments, an error is raised when the assembler parses the file. Aliases may be defined for convenience - see the [Preprocessor](#preprocessor) section later.

## Numerical Literals

A token that begins with a digit `0-9` or a plus or minus sign (`+` or `-`) is treated as a numeric literal. By default, numeric literals are treated as being decimal, but hexadecimal literals can be specified using the prefix `0x`, and binary literals using the prefix `0b`.

```
20       // 20 in decimal
0x14     // 20 in hexadecimal
0b10100  // 20 in binary
```

## Labels

A token that begins with `:` is considered a label. A label defines a point in code to which the processor may branch later.

The name of a label, formed by the string after the `:`, may only contain alphanumeric characters `A-Z a-z 0-9` and underscores `_`, and may not begin with a number.

```
// A label to mark the beginning of a loop:
:loop_begin
// ... further instructions below ...
```

Labels may be used as arguments to instructions. Since a label is a code memory address, and an there is not enough space to provide a full 16-bit address as an operand to an instruction, the following syntax is supported when referring to labels:

* Prefixing a label name with `~` evaluates to a **signed** byte representing the distance **in words** between the address of the next instruction and the label.
* Prefixing a label name with `~+` evaluates to an **unsigned** byte representing the distance **in words** between the address of the next instruction and the label.
* Prefixing a label name with `<` evaluates to the upper byte of the label's unsigned address.
* Prefixing a label name with `>` evaluates to the lower byte of the label's unsigned address.

This allows for the following methods of jumping to a new address. Note that in the case where `~` is used to add to or subtract from `PC`, the `ADD` and `SUB` commands treat the operand as the number of **words** to increment or decrement `PC` by.

```
// Loop by subtracting from PC:
:label1
add 0 0 1          // Add 1 to R0
add 1 1 2          // Add 2 to R1
sub 3 3 ~+:label1  // Subtract 3 words from PC, to begin executing from :label1 again
nop                // This is the next instruction that PC would otherwise be pointing to

// Jump by adding to PC:
add 3 3 ~:label2  // Add 3 words to PC, to continue execution from :label2
add 0 0 1         // +------------------------------------+
add 0 0 1         // | These instructions will be skipped |
add 0 0 1         // +------------------------------------+
:label2
nop               // No adds will have taken place by the time execution resumes from here

// Jump by bit shifting and assigning to PC:
:label3
asgn 2 2 <:label3  // Assign upper byte of label address to LR
shft 2 2 8         // Shift the value in LR left by 8 bits
add 2 2 >:label3   // Add lower byte of label address to LR
asgn 2 3 0         // Assign the value to PC

```

## Preprocessor

The following preprocessor commands are supported:

### Include

`#include` includes another source file as part of the current file. Files may only be included once - subsequent includes of the same file are silently ignored.

```
#include "defs.inc"
```

---

Original notes (outdated):

```
// Other files can be included for extra definitions:
#include "some_other_file.inc"

// Whitespace is used to delimit tokens,
// and newlines are used to terminate lines.

// By default, instructions are specified as:
// <mnemonic> <arg1> <arg2> ...
// For as many arguments as are present in the instruction word.
// For example:

ADD 0 1 0

// For a base instruction, all possible arguments must be provided.
// On top of this, aliases can be defined that make it easier to
// use the base instruction. For example:

#alias ADDL ADD $0 $0 $1

// Here, numbered arguments are prefixed with a '$'.
// Aliases are recursively evaluated.
// So:

#alias NOP ADD 0 0

// Evaluates to:

ADD 0 0 0

// Macros can define multiple instructions.
// For example:

#begin_macro BX_IF_R0_EQ_R1
ASGN 2 $0
SUB 0 1 0
CBX 1 0 0
#end_macro

// This macro defines a set of three instructions, where:
//   - LR is loaded with a target address
//   - R1 is subtracted from R0
//   - If the result was zero, PC is set to the address in LR
// It can then be used like so:

BX_IF_R0_EQ_R1 0x1000

// Macros are also recursively evaluated.

// Numerical literals can either be decimal (default),
// hex (prefixed by 0x), or binary (prefixed by 0b).

```
