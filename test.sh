#!/bin/bash

./build/assembler -o handler.o tests/test1/handler.s
./build/assembler -o main.o tests/test1/main.s

./build/linker -relocatable -o output.o handler.o main.o
./build/linker -relocatable -o outputTest.o output.o