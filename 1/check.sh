#! /bin/bash
nasm -f bin hello.asm
chmod +x hello
./hello
md5sum hello
ls -al hello

