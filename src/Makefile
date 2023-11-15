openmp_experimental:
	gcc -o tsp_par openmp/tsp_par.c -O3 -fopenmp -lm
	gcc -o tsp_seq sequential/tsp_seq.c -O3 -fopenmp -lm
    
openmp_theoretical:
	gcc -o tsp_par openmp/tsp_par_the.c -O3 -fopenmp -lm
	gcc -o tsp_seq sequential/tsp_seq_the.c -O3 -fopenmp -lm

openmpi_experimental:
	mpicc -o tsp_par openmpi/tsp_par.c -O3 -lm
	gcc -o tsp_seq sequential/tsp_seq.c -O3 -fopenmp -lm