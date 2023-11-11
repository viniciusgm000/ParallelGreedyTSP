#!/bin/bash

INPUT_DIR="input"
OUTPUT_RESULTS_DIR="output/openmp/experimental"

EXECUTIONS="50"
CITIES="13 14 15"
THREADS="1 2 3 6 9 12"

mkdir -p $OUTPUT_RESULTS_DIR

rm $OUTPUT_RESULTS_DIR/*

make openmp_experimental

for i in 1 2 3 4 5 6 7 8 9 10; do
   for e in $EXECUTIONS; do
       for c in $CITIES; do
           echo $i"_seq_"$c"_cities_"$e"_executions"
           taskset --cpu-list 0 ./tsp_seq < $INPUT_DIR/$c"_cities_"$e"_executions.in" > $OUTPUT_RESULTS_DIR/$i"_seq_"$c"_cities_"$e"_executions_result.out"
       done
   done
done

export OMP_PROC_BIND=spread

for i in 1 2 3 4 5 6 7 8 9 10; do
    for t in $THREADS; do
        export OMP_NUM_THREADS=$t
        for e in $EXECUTIONS; do
            for c in $CITIES; do
                echo $i"_par_"$c"_cities_"$e"_executions_"$t"_threads"
                ./tsp_par < $INPUT_DIR/$c"_cities_"$e"_executions.in" > $OUTPUT_RESULTS_DIR/$i"_par_"$c"_cities_"$e"_executions_"$t"_threads_result.out"
            done
        done
    done
done
