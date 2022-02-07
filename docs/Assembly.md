```
// This is a line comment.
/* This is a multi-line comment. */

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

// Labels are defined using:

#label LabelName

// Their address can be used in an instruction with
// the '&' prefix:

ASGN 2 &LabelName
```