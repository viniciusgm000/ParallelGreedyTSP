#!/bin/bash

INPUT_DIR="input"

mkdir -p $INPUT_DIR

for e in 50; do
    for c in 13 14 15; do
        python3 generateInput.py -c $c -e $e > $INPUT_DIR/$c"_cities_"$e"_executions.in"
    done
done