experimental:
	gcc -o tsp_par tsp_par.c -O3 -fopenmp -lm
	gcc -o tsp_seq tsp_seq.c -O3 -fopenmp -lm
    
theoretical:
	gcc -o tsp_par tsp_par_the.c -O3 -fopenmp -lm
	gcc -o tsp_seq tsp_seq_the.c -O3 -fopenmp -lm
