#!/bin/sh

# $1 target file
target=$1
clang++ -S -c -O0 -fno-discard-value-names -emit-llvm -g ${target} -o ${target}.ll
opt -S -mem2reg ${target}.ll -o ${target}.ll


