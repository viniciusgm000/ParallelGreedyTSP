all:
	gcc -o tsp_par tsp_par.c -O3 -fopenmp -lm
	gcc -o tsp_seq tsp_seq.c -O3 -lm
