#!/bin/bash

gcc -o pipeline pipeline.c lib.c preditor.c

if [ $? -eq 0 ]; then
    echo "Compilation successful."

    ./pipeline perfect_squares.bin
else
    echo "Compilation failed."
fi