#!/bin/bash

assembler -o handler.o tests/test1/handler.s
assembler -o main.o tests/test1/main.s
linker -relocatable -o output.o handler.o main.o
linker -hex -place=my_code_main@0x40000000 -place=my_code_handler@0xc0000000 -o program.hex handler.o main.o
emulator program.hex

