#! /bin/bash

nasm -felf64 debug.asm -o debug.o
ld debug.o -o debug
objdump -D debug
