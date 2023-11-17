#!/bin/bash

SRC_DIR="src"
SCRIPTS_DIR="scripts"

if [ "$1" == "generate" ]; then
    # run generate input
    cd $SRC_DIR/generate_input/
    ./run_generate_input.sh
    cd -
    mv $SRC_DIR/generate_input/input/ .
else
    # run experiments
    
    case "$1" in
    "openmp_exp")
        # if openmp and experimental
        cp $SCRIPTS_DIR/openmp/run_experimental_test.sh .
        cd $SRC_DIR
        make openmp_experimental
        cd -
        mv $SRC_DIR/tsp_* .
        
        ./run_experimental_test.sh

        rm run_experimental_test.sh tsp_*
    ;;
    "openmp_the")
        # if openmp and theoretical
        cp $SCRIPTS_DIR/openmp/run_theoretical_test.sh .
        cd $SRC_DIR
        make openmp_theoretical
        cd -
        mv $SRC_DIR/tsp_* .

        ./run_theoretical_test.sh

        rm run_theoretical_test.sh tsp_*
    ;;
    "openmpi_exp")
        # if openmpi and experimental
        cp $SRC_DIR/hostlist.template .
        cp $SCRIPTS_DIR/openmpi/run_experimental_test.sh .
        cd $SRC_DIR
        make openmpi_experimental
        cd -
        mv $SRC_DIR/tsp_* .

        ./run_experimental_test.sh

        rm run_experimental_test.sh hostlist* tsp_*
    ;;
    "openmpi_the")
        # if openmpi and theoretical
        cp $SRC_DIR/hostlist.template .
        cp $SCRIPTS_DIR/openmpi/run_theoretical_test.sh .
        cd $SRC_DIR
        make openmpi_theoretical
        cd -
        mv $SRC_DIR/tsp_* .

        ./run_theoretical_test.sh

        rm run_theoretical_test.sh hostlist* tsp_*
    ;;
    *)
        echo "Invalid Option - available options:"
        echo "generate - Generates random inputs in the ./input/ directory"
        echo "openmp_exp - Runs the experimental tests for OpenMP"
        echo "openmp_the - Runs the theoretical tests for OpenMP"
        echo "openmpi_exp - Runs the experimental tests for OpenMPI"
        echo "openmpi_the - Runs the theoretical tests for OpenMPI"
    ;;
    esac
fi