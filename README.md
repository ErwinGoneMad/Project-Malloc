
# Custom Memory Allocation Library

## Introduction
This project provides a custom reimplementation of standard memory allocation functions: `malloc`, `calloc`, `realloc`, and `free`. It is designed using `mmap` for efficient memory mapping and employs a bucket allocation strategy for optimal memory management.

## Features
- Custom implementations of `malloc`, `calloc`, `realloc`, and `free`.
- Memory mapping with `mmap`.
- Efficient bucket allocation strategy for memory management.
- Improved performance in memory allocation and deallocation.

## Installation
Clone the repository and navigate to the project directory. Use the provided Makefile to build the library:
```bash
make all
```
This will compile the source code and generate `libmalloc.so`, the shared library file.

## Testing
To test the custom memory allocation library, you can preload it with standard Unix commands. For example, to use it with the `ls` command, run:
```bash
LD_PRELOAD=./libmalloc.so ls
```
Similarly, you can test with other commands like `tree` or `seq` by preloading the library.
