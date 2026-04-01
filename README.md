# MIL-STD-1750A Tools (milstd1750tools)

[![License: GPL](https://img.shields.io/badge/License-GPL-blue.svg)](LICENSE)

A suite of professional-grade tools for developing, assembling, and simulating software for the MIL-STD-1750A architecture. This repository contains a macro assembler/linker (`as1750`) and a software simulator (`sim1750`), primarily designed to support military and aerospace embedded systems development and legacy software maintenance.

---

## Background

**MIL-STD-1750A** is a 16-bit instruction set architecture (ISA) standard developed by the United States Department of Defense (DoD) in the late 1970s. It was designed to provide a standard microprocessor architecture for military airborne systems, reducing the costs associated with supporting numerous proprietary architectures.

The toolchain in this repository provides the necessary utilities to assemble 1750A assembly code (often outputted by cross-compilers like GNU C (`gcc`) configured for the 1750A target) into executable loadfiles, and a robust simulator to execute those binaries in software.

---

## Repository Contents & Capabilities

### 1. `as1750` - Macro Assembler and Linker
`as1750` is a comprehensive macro assembler and linker primarily intended to assemble the output of cross-compilers or hand-written MIL-STD-1750 (A or B) assembly code.

**Capabilities:**
*   **Macro Processing:** Built-in text-preprocessing phase for macro symbols, conditional assembly (`IF`/`ELSEIF`/`ENDIF`), loop generation (`WHILE`), and parameterized macros.
*   **Linking:** Automatically resolves symbols across multiple source files.
*   **Output Formats:** Generates loadfiles in both TLD Load Module (`.ldm`, default) and Tektronix Extended Hex (`.hex`) formats.
*   **Memory Sections:** Supports predefined program sections: `CODE`, `KONST` (constants), `DATA`, and `BSS` (uninitialized variables) for fine-grained memory layout.
*   **Library Mechanism:** Includes a simple mechanism to automatically resolve unbound global symbols from library files in `AS1750_LIB_PATH`.

### 2. `sim1750` - Software Simulator
`sim1750` provides a flexible, software-based execution environment for MIL-STD-1750A binaries, enabling development, debugging, and testing without requiring actual hardware.

**Capabilities:**
*   **Full ISA Support:** Complete simulation of the MIL-STD-1750A instruction set.
*   **Processor Timing Models:** Optional timing info/instruction cycle counting is supported for various chips, including:
    *   PACE
    *   F9450
    *   GVSC
    *   MA31750
    *   MAS281
*   **Interactive Debugging:** Built-in shell for interactive execution, inspecting registers, and memory manipulation.

---

## Usage Guide: `as1750` Assembler

`as1750` is invoked via the command line to compile assembly source files (`.s` or `.asm`) into an executable loadfile.

### Synopsis
```bash
as1750 [-a|-al|-ar|-as] [-D macro_symbol[=value]] [-I path] [-o loadfile] [-c hexaddr] [-k hexaddr] [-s hexaddr] [-t address] [-C mainname] [-S] [-u] [-v] files...
```

### Basic Examples

**1. Assemble a single file into the default TLD LDM load module:**
```bash
as1750 program.s
```
*(Produces `program.ldm` in the current directory).*

**2. Assemble multiple files into a Tektronix Hex file:**
```bash
as1750 -o output.hex file1.s file2.s file3.s
```

**3. Generate a full assembly listing alongside compilation:**
```bash
as1750 -a program.s
```

### Common Options
*   `-o loadfile`: Specify the output loadfile name and format (`.hex` for Tektronix Extended Hex, `.ldm` for TLD Load Module).
*   `-a`, `-al`, `-ar`, `-as`: Turn on assembly listings (`-a` for everything, `-al` before relocation, `-ar` after relocation, `-as` symbols only).
*   `-D symbol[=value]`: Define a macro symbol. Useful in combination with `IF DEF(symbol)` directives in the source code.
*   `-I path`: Add a path to the search list for `INCLUDE` directives.
*   `-c hexaddr`: Force the `CODE` section to start at the specified hexadecimal address.
*   `-k hexaddr`: Force the `KONST` section to start at the specified address.
*   `-s hexaddr`: Force the `DATA` section to start at the specified address.
*   `-t address`: Set the execution start address (overrides the `END` directive).

---

## Usage Guide: `sim1750` Simulator

Once you have built your `.ldm` or `.hex` loadfile, you can simulate it using `sim1750`.

### Basic Execution
Simply invoke the simulator from the command line:
```bash
sim1750
```
This drops you into the `sim1750` interactive prompt.

To see all available commands and short explanations within the simulator, type:
```
help
```

*Note: For the best experience, build `sim1750` with your target processor's instruction cycle times defined in `stime.h` to enable realistic operations on Timers A and B.*

---

## Building Instructions

To keep the repository clean and maintainable, detailed build instructions are located in their respective project directories.

*   **For `as1750`**: See the `Makefile` and source comments inside the [`as1750/`](as1750/) directory.
*   **For `sim1750`**: Please read the comprehensive build guide in [`sim1750/README`](sim1750/README) for instructions on UNIX, VMS, and DOS/Windows systems.

---

## License & Credits

See the `COPYING` or `LICENSE` files for licensing and distribution information.

Original tools developed by:
*   Daimler-Benz Aerospace AG, Space Systems Group (1994-1997)
*   O. Kellogg (okellogg@users.sourceforge.net)

---
*"Use the Source, Luke! The Source will always be with you."*
