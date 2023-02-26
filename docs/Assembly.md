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

The first token is the mnemonic for the instruction **in lowercase**, and subsequent tokens are arguments for this instruction.

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

A token that begins with `:` is considered a label. A label defines a point in code to which the processor may branch later. The address of a label is the address of the instruction immediately following the label.

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

Before a file is assembled, a preprocessing pass is run. This preprocessor pass supports a number of different features, which are detailed below.

### Including Files

`#include` includes another source file as part of the current file. Files may only be included once - subsequent includes of the same file are silently ignored.

```
#include "defs.inc"
```

### Constants

`#const` defines a numerical constant. Any valid number may be used as a constant, including decimal, hexadecimal, and binary numbers.

By convention, constant names are defined in uppercase, with underscores separating words. Constant names may only contain alphanumeric characters `A-Z a-z 0-9` and underscores `_`, and may not begin with a number.

Constants are different to [aliases](#aliases), in that they are intended solely to represent numbers. This means that they may be used unambiguously in mathematical expressions.

```
// Defines a constant for a "success" exit code.
#const EXIT_SUCCESS 0

// Defines a negative decimal constant.
#const NEGATIVE_FIVE -5

// Defines a constant specified in hexadecimal.
#const MAX_VAL_OF_UNSIGNED_BYTE 0xFF

// Defines a constant specified in binary.
#const MY_FLAG 0b100
```

### Macros

`#macro` defines a single-instruction macro, and `#begin_macro` and `#end_macro` define a macro over one or more instructions. At the point when a macro is defined, its name must be globally unique within the file being assembled.

By convention, macro names are defined in uppercase, with underscores separating words. Macro names may only contain alphanumeric characters `A-Z a-z 0-9` and underscores `_`, and may not begin with a number.

Arguments to macros are defined within `()` brackets. Multiple arguments in the macro definition are delimited by commas. If a macro takes no arguments, the empty pair of `()` brackets must still be provided on both defintion and invocation.

Argument names may only contain alphanumeric characters `A-Z a-z 0-9` and underscores `_`, and may not begin with a number.

Within a macro, an argument may be referred to by placing its name into a set of curly braces `{}`. The argument is pasted verbatim into the macro, so it is possible to generate invalid tokens if insufficient care is taken.

Macros are evaluated recursively when invoked, so one macro may be provided as an argument to another.

```
// A macro for adding a literal value to a register.
// Eg: ADD_LITERAL(0, 13) would add 13 to R0.
#macro ADD_LITERAL(reg, val) add {reg} {reg} {val}

// A macro intended for either an add or a sub
// to be performed on R0. Note that it is not
// generally recommended to use instructions this
// way, but this example demonstrates the
// flexibility of how arguments can be passed to macros.
// Eg: PERFORM_ON_R0(add, 1)
#macro PERFORM_ON_R0(operation, value) {operation} 0 0 {value}

// A macro for performing an unconditional jump,
// as per the earlier example given for labels.
// Eg: JUMP(:my_label) would jump to :my_label.
#begin_macro JUMP(dest_label)
asgn 2 2 <{dest_label}
shft 2 2 8
add 2 2 >{dest_label}
asgn 2 3 0
#end_macro

// An empty macro for performing a nop operation.
// Eg: DO_NOP()
#macro DO_NOP() nop
```

### Aliases

`#alias` defines an alias. Aliases are used to create a new user-defined name for an existing token. The existing token that an alias refers to may be:

* A numeric literal (eg. `0x12`).
* A label (eg. `:my_label`).
* A label reference (eg. `~+:my_label`).
* A previously-defined preprocessor token (eg. `MY_CONSTANT`). This includes other previously-defined constants, macros, and aliases.

An attempt to alias a preprocessor token that has not been encountered yet, or an attempt to alias any other kind of token not included in the list above, will produce an error. This is to ensure that an alias always represents a well-defined language token.

The syntax for defining an alias is `#alias name token`.

By convention, aliases are defined in uppercase, with underscores separating words. Aliases may only contain alphanumeric characters `A-Z a-z 0-9` and underscores `_`, and may not begin with a number.

```
// Defines an alias for a number.
#alias ZERO 0

// Defines an alias for an existing constant.
#const MY_CONSTANT 123
#alias MAGIC_NUMBER MY_CONSTANT

// Defines an alias for a label.
#alias END_OF_PROGRAM :end

// Defines an alias for the distance to the label.
#alias DIST_TO_END +~END_OF_PROGRAM
```

TODO: Would `#alias` be better implemented as simply a `#define`?
