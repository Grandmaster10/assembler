# SIMPLEX Assembler & Emulator

A custom two-pass assembler and virtual machine emulator for the 32-bit SIMPLEX instruction set, written in C++17. Built as the midsem MiniProject for CS2206.

## Features
* **Two-Pass Assembler (`asm.cpp`):** Converts SIMPLEX assembly (`.asm`) into 32-bit machine code. Features robust error diagnostics, advanced `.lst` listing generation, and support for the `SET` pseudo-instruction.
* **Virtual Machine Emulator (`emu.cpp`):** A fetch-decode-execute cycle emulator that runs compiled `.o` object files. Generates detailed `.trace` execution logs and `.dump` memory states.
* **Algorithmic Test Cases:** Includes custom `.asm` programs for Bubble Sort, Fibonacci sequence generation, Array summation, and recursive Pascal's Triangle (Binomial Coefficients).

## Compilation
This project includes a `Makefile` for easy compilation using `g++`.

To build both the assembler and emulator, simply run:
```bash
make
```

To clean up compiled binaries and output files (.o, .lst, .log, .trace, .dump), run:
```bash
make clean
```

## Usage:
* **Assemble a Program**
  ```bash
  ./asm test01.asm
  ```
  Outputs: `test01.o` (Binary Object), `test01.lst` (Listing), `test01.log` (Error/Warning Logs)
  
* **Emulare a Program**
  ```bash
  ./emu test01.o
  ```
  Outputs: `test01.trace` (Execution Trace), `test01.dump` (Memory Dump)
