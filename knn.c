#include "header_thread.h"
#include "thread_functions.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "mergeSort.h"


void calcDistance(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    for (int i = data->start_row; i < data->end_row; i++) {
        float sum_distance = 0.0;
        for (int j = 0; j < data->num_columns; j++) {
            float test_point_value = data->test_point[j];
            float point_distance = sqrt(pow(data->data_matrix[i * data->num_columns + j] - data->test_point[j], 2));
            sum_distance += point_distance;
        }
        data->local_distances[i - data->start_row] = sum_distance;
    }
}

void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    printf("Thread %d:\n\n", data->thread_id);
    calcDistance(data);
    int num_rows = data->end_row - data->start_row;
    mergeSort(data->local_distances, 0, data->end_row - data->start_row - 1, data->label_matrix);
    // Stampa le distanze locali
    printf("Distances - Labels\n");
    for (int i = 0; i < num_rows; i++) {
        printf("%f       %d\n", data->local_distances[i], data->label_matrix[i]);
    }
    printf("\n");
    pthread_exit((void *)data);
}

int knn(int rank, int size, char *data_filename, char *label_filename, float *test_point) {
    if (rank > 3) {
        char line_data[MAX_ROW_LENGTH];
        char line_label[MAX_ROW_LENGTH];

        FILE *data_file = fopen(data_filename, "r");
        FILE *label_file = fopen(label_filename, "r");

        openAndCheckCSVFiles(data_filename, label_filename, &data_file, &label_file);

        int total_rows_data = countTotalRows(data_file, line_data);
        int total_rows_label = countTotalRows(label_file, line_label);

        checkEqualNumberRows(total_rows_data, total_rows_label);

        int start_row, end_row;
        calculateRowRange(rank, size, total_rows_data, &start_row, &end_row);

        int num_rows, num_columns;

        float *data_matrix, *all_distances;

        int *label_matrix;

        allocateMatrix(start_row, end_row, &num_columns, &data_matrix, &label_matrix, &all_distances, &num_rows);

        readAssignedRows(data_file, label_file, start_row, end_row, num_columns, data_matrix, label_matrix, line_data, line_label);

        printf("Processo %d:\n\n", rank);

        pthread_t threads[NTHREADS];
        ThreadData thread_data[NTHREADS];

        initializeThreads(threads, thread_data, num_rows, num_columns, data_matrix, label_matrix, test_point, all_distances);

         float *thread_distances = malloc(NTHREADS * 5 *sizeof(int));
         int *thread_labels = malloc(NTHREADS * 5 *sizeof(int));
        int index = 0;
        for (int i = 0; i < NTHREADS; i++) {
            pthread_create(&threads[i], NULL, threadFunction, (void *)&thread_data[i]);
            
            for (int j = 0; j < 5; j++)
            {
                pthread_join(threads[i], (void **)&thread_data[i]);
                 thread_distances[index] = thread_data[i].local_distances[j];
                 thread_labels[index] = thread_data[i].label_matrix[j];
                 index++;
            }
        }


         // Stampa l'array di etichette per il processo corrente
        printf("Processo %d:  Distances array - Labels array\n", rank);
        for (int i = 0; i < NTHREADS*5; i++) {
            printf("                 %f            %d\n", thread_distances[i], thread_labels[i]);
        }
        

        // Calcola l'etichetta più frequente
        int *label_counts = calloc(num_rows, sizeof(int));
        int max_label = -1, max_count = 0;

        for (int i = 0; i < NTHREADS; i++) {
            int current_label = thread_distances[i];
            label_counts[current_label]++;
            if (label_counts[current_label] > max_count) {
                max_count = label_counts[current_label];
                max_label = current_label;
            }
        }

        // Stampa l'etichetta più frequente per il processo corrente
        printf("Processo %d - Etichetta piu' frequente: %d\n\n", rank, max_label);

        free(thread_distances);

        cleanupAndClose(data_file, label_file, data_matrix, label_matrix);

        return max_label;
    }
}
