Vesper Virtual Micro Processor (V2MP)
=====================================

The Vesper virtual micro processor is a
C implementation of a little-endian 16-bit
instruction set processor. There are specification
documents for:

* [The processor architecture and instruction
  set](docs/V2MP.md)
* [The assembly language](docs/Assembly.md)
* The linker and executable file format (docs
  still to be created)

Eventually, this repo will contain code and CMake
targets for:

* The processor and related virtual machine
  components, as libraries.
* An assembler and linker, both as libraries
  and executables.
* An example simulator application, capable of
  managing virtual machine components, and
  loading and executing programs.

## Why did you make this?

For a number of reasons:

* I wanted to practise designing and implementing
  behaviour in pure C, without the conveniences
  afforded by C++, which is my main day-to-day
  language as a developer. I'm beginning to
  believe that if you can design something that
  works well in C, you can design for any
  language. C forces you to know exactly what
  you're doing, and I wanted to challenge
  myself to make something of good quality in a
  language that's unforgiving.
    * The tools for assembling the machine code
    have been written in C++ in the end, because
    it turns out writing a parser in C is a big
    pain in the arse, and exception support makes
    it easier. However, the CPU simulator and
    all the other components that work with it
    are implemented in C.
* I wanted to practise test-driven development,
  which I've never followed particularly closely
  in the past (insert many excuses why here).
  If there's one domain where you want to be
  absolutely confident in the semantics of your
  code, it's when you're writing the very world
  within which the code operates.
* I wanted to make something that could
  potentially be used in a game. After a period
  attempting to reverse-engineer Windows 95
  driver code (don't ask), I found it was actually
  more fun doing the reverse-engineering than it
  would have been doing the thing I was originally
  planning. I thought this could work quite well
  in a game, and could also help teach people
  (myself included) about how to patch platform
  binaries.
* I wanted to learn more about instruction sets
  and how computer architectures actually work.
  Making my own architecture had turned out to
  be a fun way of doing this, and demonstrates
  a lot of the considerations that I imagine
  have to go into CPU and instruction set design.
