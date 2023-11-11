// Code parallelized by Vinicius Gabriel Machado - Master's student in Computer Science at UFPR

// Sequential code by:

/* WSCAD - 9th Marathon of Parallel Programming 
 * Simple Brute Force Algorithm for the 
 * Traveling-Salesman Problem
 * Author: Emilio Francesquini - francesquini@ic.unicamp.br
 */

// Important environment variables:

// export OMP_PROC_BIND=spread
// export OMP_NUM_THREADS=n

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include <omp.h>

double start, end;
double total_time;

int min_distance;
int nb_towns;

typedef struct {
    int to_town;
    int dist;
} d_info;

d_info **d_matrix;
int *dist_to_origin;

int present (int town, int depth, int *path) {
    int i;
    for (i = 0; i < depth; i++)
        if (path[i] == town) return 1;
    return 0;
}

void tsp_recursive (int depth, int current_length, int *path) {
    if (current_length >= min_distance) return;
    if (depth == nb_towns) {
        current_length += dist_to_origin[path[nb_towns - 1]];
        if (current_length < min_distance)
            #pragma omp atomic write
            min_distance = current_length;
    } else {
        int town, me, dist;
        me = path[depth - 1];
        for (int i = 0; i < nb_towns; i++) {
            town = d_matrix[me][i].to_town;
            if (!present (town, depth, path)) {
                path[depth] = town;
                dist = d_matrix[me][i].dist;
                tsp_recursive (depth + 1, current_length + dist, path);
            }
        }
    }
}

void tsp () {
    #pragma omp parallel default(none) shared(nb_towns, d_matrix, min_distance)
    #pragma omp single nowait
    for (int i = 0; i < nb_towns; i++) {
        int path_threaded[nb_towns], town, dist;
        int current_length = d_matrix[0][i].dist;

        int depth = 1;
        town = d_matrix[0][i].to_town;
        
        path_threaded[0] = 0;
        if (!present (town, depth, path_threaded) & (current_length < min_distance)) {
            path_threaded[1] = town;
            depth = 2;

            for (int j = 0; j < nb_towns; j++) {
                town = d_matrix[path_threaded[1]][j].to_town;
                if (!present (town, depth, path_threaded)) {
                    dist = d_matrix[path_threaded[1]][j].dist;

                    path_threaded[2] = town;

                    #pragma omp task firstprivate(path_threaded)
                    tsp_recursive (depth + 1, current_length + dist, path_threaded);
                }
            }
        }
    }
}

void greedy_shortest_first_heuristic(int *x, int *y) {
    int i, j, k, dist;
    int *tempdist;

    tempdist = (int*) malloc(sizeof(int) * nb_towns);
    for (i = 0; i < nb_towns; i++) {
        for (j = 0; j < nb_towns; j++) {
            int dx = x[i] - x[j];
            int dy = y[i] - y[j];
            tempdist [j] = dx * dx + dy * dy;
        }
        for (j = 0; j < nb_towns; j++) {
            int tmp = INT_MAX;
            int town = 0;
            for (k = 0; k < nb_towns; k++) {
                if (tempdist [k] < tmp) {
                    tmp = tempdist [k];
                    town = k;
                }
            }
            tempdist [town] = INT_MAX;
            d_matrix[i][j].to_town = town;
            dist = (int) sqrt (tmp);
            d_matrix[i][j].dist = dist;
            if (i == 0)
                dist_to_origin[town] = dist;
        }
    }

    free(tempdist);
}

void show_distances() {
    int i, j;

    printf("To Town:\n");

    for (i = 0; i < nb_towns; i++) {
        for (j = 0; j < nb_towns; j++)
            printf("%d\t", d_matrix[i][j].to_town);
        printf("\n");
    }

    printf("Distance:\n");

    for (i = 0; i < nb_towns; i++) {
        for (j = 0; j < nb_towns; j++)
            printf("%d\t", d_matrix[i][j].dist);
        printf("\n");
    }

    printf("Distance to Origin:\n");

    for (i = 0; i < nb_towns; i++)
        printf("%d\t", dist_to_origin[i]);

    printf("\n");
    
}

void init_tsp() {
    int i, st;
    int *x, *y;

    min_distance = INT_MAX;

    st = scanf("%u", &nb_towns);
    if (st != 1) exit(1);
 
    d_matrix = (d_info**) malloc (sizeof(d_info*) * nb_towns);
    for (i = 0; i < nb_towns; i++)
        d_matrix[i] = (d_info*) malloc (sizeof(d_info) * nb_towns);
    dist_to_origin = (int*) malloc(sizeof(int) * nb_towns);
   
    x = (int*) malloc(sizeof(int) * nb_towns);
    y = (int*) malloc(sizeof(int) * nb_towns);
    

    for (i = 0; i < nb_towns; i++) {
        st = scanf("%u %u", x + i, y + i);
        if (st != 2) exit(1);
    }
    
    greedy_shortest_first_heuristic(x, y);
    
    free(x);
    free(y);
}

int run_tsp() {
    int i;

    init_tsp();

    // show_distances();
    
    tsp ();

    for (i = 0; i < nb_towns; i++)
        free(d_matrix[i]);
    free(d_matrix);

    return min_distance;
}

int main (int argc, char **argv) {
    int num_instances, st, output_min;
    st = scanf("%u", &num_instances);
    if (st != 1) exit(1);
    while(num_instances-- > 0) {
        start = omp_get_wtime();
        output_min = run_tsp();
        end = omp_get_wtime();
        total_time = (end - start);
        // printf("%d %lf\n", output_min, total_time);
	printf("%d\n", output_min);
    }
    return 0;
}
