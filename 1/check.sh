#! /bin/bash
mkdir -p build
gcc main.c -s -o build/md5
strip -R .gnu.version build/md5

./build/md5
md5sum build/md5

ls -l build/md5
