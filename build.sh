#!/bin/sh

set -xe

CC="${CXX:-cc}"
CFLAGS="-Wall -ggdb -O3 -std=c11 -pedantic"

$CC gpt.c $CFLAGS -o gpt -lm -lOpenCL
