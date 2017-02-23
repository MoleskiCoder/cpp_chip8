#cpp_chip8

[![Coverity Scan Build Status](https://scan.coverity.com/projects/11833/badge.svg)](https://scan.coverity.com/projects/moleskicoder-cpp_chip8)
[![Build Status](https://travis-ci.org/MoleskiCoder/cpp_chip8.svg?branch=master)](https://travis-ci.org/MoleskiCoder/cpp_chip8)

CHIP-8, SuperChip and XO-Chip Emulator written in C++

Uses SDL2 to provide graphics and audio processing.

The game loop is strongly related to the way MonoGame/XNA works.

## Compiling

### Prerequisites

* SDL
* boost

### Linux

`make`

### Windows

Compile with Visual Studio 2015 (via the solution)

## Running

### Arguments

* processor-type (schip) - Processor type.  Can be one of chip, schip or xochip
* allow-misaligned-opcodes (false) - Allow instuctions to be loaded from odd addresses
* rom - ROM to use
* graphics-count-row-hits - Graphics: count row hits
* graphics-count-exceeded-rows - Graphics: count exceeded rows
* graphics-clip (true) - Graphics: clip
* cycles-per-frame - cycles per frame

### examples

#### Linux

`./cpp_chip8 Roms/SGAMES/ANT`

#### Windows

`cpp_chip8 Roms\SGAMES\ANT`
