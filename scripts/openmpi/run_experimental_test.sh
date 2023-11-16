#!/bin/bash

INPUT_DIR="input"
OUTPUT_RESULTS_DIR="output/openmpi/experimental"

EXECUTIONS="50"
CITIES="13 14 15"
PROCESSES="1 2 3 4 5"

mkdir -p $OUTPUT_RESULTS_DIR

rm $OUTPUT_RESULTS_DIR/*

# for i in 1 2 3 4 5 6 7 8 9 10; do
#    for e in $EXECUTIONS; do
#        for c in $CITIES; do
#            echo $i"_seq_"$c"_cities_"$e"_executions"
#            taskset --cpu-list 0 ./tsp_seq < $INPUT_DIR/$c"_cities_"$e"_executions.in" > $OUTPUT_RESULTS_DIR/$i"_seq_"$c"_cities_"$e"_executions_result.out"
#        done
#    done
# done

for i in 1 2 3 4 5 6 7 8 9 10; do
    for p in $PROCESSES; do
        cp hostlist.template hostlist
        k=$(($p + "1"))
        sed -i 's/<NUMBER_PROCESSES>/'$k'/g' hostlist
        for e in $EXECUTIONS; do
            for c in $CITIES; do
                echo $i"_par_"$c"_cities_"$e"_executions_"$p"_processes"
                # mpirun --hostfile hostlist --map-by L1cache --bind-to hwthread tsp_par < $INPUT_DIR/$c"_cities_"$e"_executions.in" > $OUTPUT_RESULTS_DIR/$i"_par_"$c"_cities_"$e"_executions_"$p"_processes_result.out"
            done
        done
    done
done