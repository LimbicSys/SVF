#!/bin/sh

# $1 target file
target=$1
fileDir=$(cd "$(dirname "$0")"; pwd)
llvmPrePath=$fileDir/llvm-12.0.0.obj/bin
$llvmPrePath/clang++ -S -c -O0 -fno-discard-value-names -emit-llvm -g ${target} -o ${target}.ll
$llvmPrePath/opt -S -mem2reg ${target}.ll -o ${target}.ll


