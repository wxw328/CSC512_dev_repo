#!/bin/bash

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <program>"
    exit 1
fi

PROGRAM=$1
shift 

# Run the program with Callgrind
valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./"$PROGRAM" "$@"

grep "totals" callgrind.out | awk '{print $2}'

# rm callgrind.out