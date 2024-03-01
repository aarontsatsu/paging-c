# Paging System Simulation README

## Overview

This program simulates a paging system with memory management, focusing on virtual and physical memory management through a simple paging mechanism. It showcases process creation, memory allocation, memory access simulation, and the utilization of a Translation Lookaside Buffer (TLB) for efficient address translation. The simulation offers two main functionalities:

1. **Creating processes and allocating memory**: Allows the specification of the number of processes and their memory allocation.
2. **Simulating memory accesses**: Simulates random memory accesses for each process to demonstrate page fault handling and TLB efficiency.

This simulation aids in understanding the memory management techniques used by operating systems.

## Prerequisites

Ensure you have the following to compile and run the program:

- GCC (GNU Compiler Collection) compiler.
- Make, for utilizing the Makefile for easy compilation.
- A Unix-like environment such as Linux, macOS, or WSL on Windows.

## Compilation

A Makefile is included for straightforward compilation. To compile the program, navigate to the project directory in your terminal and execute:

```bash
make final_mmu_paging
```

This command compiles the source code into an executable named `final_mmu_paging`.

## Running the Program
After compiling, run the program from the command line with arguments to choose the operation mode and define the number of processes. Use the syntax:

```bash
./final_mmu_paging <option> <number_of_processes>
```

`<option>`: `1` for creating and allocating memory to processes, or 2 for simulating memory accesses.

`<number_of_processes>`: An integer specifying the number of processes.

## Examples
Create and allocate memory for 5 processes:

```bash
./final_mmu_paging 1 5
```

Simulate memory accesses for 10 processes:

```bash
./final_mmu_paging 2 10
```

## Testing Different Workloads
Custom Makefile targets are provided for testing different workloads without manual command-line arguments entry:

`run_option1_small`: Option 1 with 5 processes.

`run_option1_large`: Option 1 with 10 processes.

`run_option2_small`: Option 2 with 5 processes.

`run_option2_large`: Option 2 with 10 processes.

Run these targets with make, followed by the target name, for example:
```bash
make run_option2_small
```

## Cleaning Up
To remove the compiled executable and clean up your directory, execute:

```bash
make clean
```