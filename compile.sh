#!/bin/sh

mkdir -p bin
gcc -o bin/mipos mipos.c -lbcm2835 -lasound
