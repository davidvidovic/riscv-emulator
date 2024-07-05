# RISC-V Toolchain

A simple RISC-V toolchain containing ANSI C to RV64I compiler built from scratch, RV assembler and RV software emulator that takes binary file as input. Built on top of it is Qt application with interactive text field, compile button and text box to show assembly output. Additionaly, it highlights what ANSI C lines compiled into which assembly lines of code.  

## Table of Contents

- [Prerequisites](#prerequisites)
- [Usage](#usage)
- [Features](#features)

## Prerequisites

Compiler relays on yacc and lex as frontend tools for parsing and lexical analaysis of input code.
Before buiding the project, make sure you have installed yacc and lex:

- `yacc` and `lex` (also known as `bison` and `flex`)

### Checking Versions

```bash
# For yacc (bison) and lex (flex), check the version using
bison --version
flex --version
```

### Installing Prerequisites

```bash
# Update package lists
sudo apt update

# Install yacc (bison) and lex (flex)
sudo apt install bison flex
```

```bash
# Clone the repository
git clone https://github.com/davidvidovic/riscv-toolchain.git
```

## Usage

To compile input source code, edit `input.c` file and run `run.sh` bash script from terminal.

To run Qt app, an app should be first built from Qt Creator using `.pro` file in folder `app`.

## Features

Examples of app usage:

![Compile success is shown for correct input syntax](https://i.ibb.co/Z6Bz5qm/example1.png)

![Any error that compile reports is shown in UI textbox](https://i.ibb.co/1JM85cP/example2.png)
