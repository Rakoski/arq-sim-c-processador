#!/bin/bash

gcc -o arq-sim arq-sim.c lib.c

if [ $? -eq 0 ]; then
    echo "Compilation successful."

    ./arq-sim perfect_squares.bin
else
    echo "Compilation failed."
fi