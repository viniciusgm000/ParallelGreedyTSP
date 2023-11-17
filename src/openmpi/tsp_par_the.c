// Code parallelized by Vinicius Gabriel Machado - Master's student in Computer Science at UFPR

// Sequential code by:

/* WSCAD - 9th Marathon of Parallel Programming 
 * Simple Brute Force Algorithm for the 
 * Traveling-Salesman Problem
 * Author: Emilio Francesquini - francesquini@ic.unicamp.br
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include <mpi.h>

double start, end;
double total_time;

#define STD_TAG 0

// Define MPI types and variables
int process_rank, n_process;
MPI_Status status;
MPI_Datatype MPI_D_INFO; // d info

// Define algorithm types and variables
int min_distance;
int nb_towns;

typedef struct {
    int to_town;
    int dist;
} d_info;

typedef enum {
    DEPTH,
    CURRENT_LENGTH,
    MIN_DISTANCE,
    PATH_START
} message_mapping;

d_info *d_matrix;
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
            min_distance = current_length;
    } else {
        int town, me, dist;
        me = path[depth - 1];

        for (int i = 0; i < nb_towns; i++) {
            town = d_matrix[me * nb_towns + i].to_town;

            if (!present (town, depth, path)) {
                path[depth] = town;
                dist = d_matrix[me * nb_towns + i].dist;

                tsp_recursive (depth + 1, current_length + dist, path);
            }
        }
    }
}

void tsp () {
    int *path, depth, current_length;
    int message[nb_towns + PATH_START];

    // Avoid copying by mapping path into the message
    path = message + PATH_START;

    if (process_rank == 0) {
        int town, dist, depth;
        for (int i = 0; i < nb_towns; i++) {
            current_length = d_matrix[i].dist;

            depth = 1;
            town = d_matrix[i].to_town;
            
            path[0] = 0;

            if (!present (town, depth, path)) {
                path[1] = town;
                depth = 2;

                for (int j = 0; j < nb_towns; j++) {
                    town = d_matrix[path[1] * nb_towns + j].to_town;
                    dist = current_length + d_matrix[path[1] * nb_towns + j].dist;
                    
                    if (!present (town, depth, path) && dist < min_distance) {
                        path[2] = town;

                        // Receive process min_distance and send more work (length/depth/path) with current min_distance
                        MPI_Recv (&message[MIN_DISTANCE], 1, MPI_INT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);

                        if (message[MIN_DISTANCE] < min_distance)
                            min_distance = message[MIN_DISTANCE];

                        message[DEPTH] = depth + 1;
                        message[CURRENT_LENGTH] = dist;
                        message[MIN_DISTANCE] = min_distance;
                        // path already mapped into the message

                        MPI_Ssend (&message, nb_towns + PATH_START, MPI_INT, status.MPI_SOURCE, STD_TAG, MPI_COMM_WORLD);
                    }
                }
            }
        }

        // No more work to be done, await for processes to send a finalize signal
        message[DEPTH] = -1;
        for (int i = n_process - 1; i > 0; i--) {
            MPI_Recv (&message[MIN_DISTANCE], 1, MPI_INT, MPI_ANY_SOURCE, STD_TAG, MPI_COMM_WORLD, &status);

            if (message[MIN_DISTANCE] < min_distance)
                min_distance = message[MIN_DISTANCE];

            MPI_Ssend (&message, nb_towns + PATH_START, MPI_INT, status.MPI_SOURCE, STD_TAG, MPI_COMM_WORLD);
        }
    } else {
        while (1) {
            // Send current min_distance (asking for more work) and receive more work (length/depth/path) with current min_distance
            MPI_Ssend (&min_distance, 1, MPI_INT, 0, STD_TAG, MPI_COMM_WORLD);
            MPI_Recv (&message, nb_towns + PATH_START, MPI_INT, 0, STD_TAG, MPI_COMM_WORLD, &status);

            // No more work to be done
            if (message[DEPTH] == -1)
                break;
            
            min_distance = message[MIN_DISTANCE];

            // No need to copy path
            tsp_recursive (message[DEPTH], message[CURRENT_LENGTH], message + PATH_START);
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
            d_matrix[i * nb_towns + j].to_town = town;
            dist = (int) sqrt (tmp);
            d_matrix[i * nb_towns + j].dist = dist;
            if (i == 0)
                dist_to_origin[town] = dist;
        }
    }

    free(tempdist);
}

// void show_distances() {
//     int i, j;

//     printf("To Town:\n");

//     for (i = 0; i < nb_towns; i++) {
//         for (j = 0; j < nb_towns; j++)
//             printf("%d\t", d_matrix[i * nb_towns + j].to_town);
//         printf("\n");
//     }

//     printf("Distance:\n");

//     for (i = 0; i < nb_towns; i++) {
//         for (j = 0; j < nb_towns; j++)
//             printf("%d\t", d_matrix[i * nb_towns + j].dist);
//         printf("\n");
//     }

//     printf("Distance to Origin:\n");

//     for (i = 0; i < nb_towns; i++)
//         printf("%d\t", dist_to_origin[i]);

//     printf("\n");
    
// }

void init_tsp() {
    int i, st;

    start = MPI_Wtime();

    min_distance = INT_MAX;

    if (process_rank == 0) {
        st = scanf("%u", &nb_towns);
        if (st != 1) exit(1);
    }

    end = MPI_Wtime();
    total_time = (end - start);

    // Send nb_towns
    MPI_Bcast(&nb_towns, 1, MPI_INT, 0, MPI_COMM_WORLD);

    start = MPI_Wtime();
 
    // Allocate contiguous memory for the matrix (so it can be sent)
    d_matrix = (d_info*) malloc (sizeof(d_info) * nb_towns * nb_towns);
    dist_to_origin = (int*) malloc(sizeof(int) * nb_towns);
    
    int *x, *y;

    // Allocate contiguous memory for both x and y
    x = (int*) malloc(sizeof(int) * nb_towns * 2);
    y = x + nb_towns;

    if (process_rank == 0)
        for (i = 0; i < nb_towns; i++) {
            st = scanf("%u %u", x + i, y + i);
            if (st != 2) exit(1);
        }

    end = MPI_Wtime();
    total_time += (end - start);

    // Send x and y - it's a little faster than sending the final matrix
    MPI_Bcast(x, nb_towns * 2, MPI_INT, 0, MPI_COMM_WORLD);

    start = MPI_Wtime();
    
    greedy_shortest_first_heuristic(x, y);

    free(x);

    end = MPI_Wtime();
    total_time += (end - start);
}

void run_tsp() {
    int i;

    init_tsp();

    // show_distances();
    
    tsp();

    free(d_matrix);
}

int main (int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_process);

    int num_instances, st, output_min;
    
    if (process_rank == 0) {
        st = scanf("%u", &num_instances);
        if (st != 1) exit(1);
    }

    // Send nb_towns
    MPI_Bcast(&num_instances, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Define d_info type
    int d_lengths[] = {1, 1};
    MPI_Aint d_offsets[] = {0, sizeof(int)};
    MPI_Datatype d_types[] = {MPI_INT, MPI_INT};
    MPI_Type_create_struct(2, d_lengths, d_offsets, d_types, &MPI_D_INFO);
    MPI_Type_commit(&MPI_D_INFO);

    while(num_instances-- > 0) {
        run_tsp();

        if (process_rank == 0)
	        printf("%d %lf\n", min_distance, total_time);
    }

    MPI_Finalize();

    return 0;
}
