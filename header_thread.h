// header_thread.h

#include <pthread.h>
#include <stdlib.h>

#ifndef HEADER_THREAD_H
#define HEADER_THREAD_H

#define NTHREADS 3
#define NTRAIN 135
#define NFEATURES 4
#define MAX_ROW_LENGTH 1024
#define NCLASSES 3 						// number of classes/ labels 
#define K 3 						  	// the hyperparameter K in KNN algorithm
#define TOPN 3 					      	// Print the closest top N classes

// Array of all classes/ label names 
char classLabels[NCLASSES][25] = {"Iris-setosa", "Iris-versicolor", "Iris-virginica"};

#define X_TRAIN_PATH "C:/Users/tecnico/Desktop/MPI/MS_MPI/Programs/Federated_Parallel_KNN/Dataset/Iris_dataset/X_train.csv"
#define Y_TRAIN_PATH "C:/Users/tecnico/Desktop/MPI/MS_MPI/Programs/Federated_Parallel_KNN/Dataset/Iris_dataset/y_train.csv"
//thread struct
typedef struct {
    int thread_id;
    int start_row;
    int end_row;
    int num_columns;
    float *data_matrix;
    int *label_matrix;
    float *test_point;
    float *local_distances; 
    int predicted_label;
} ThreadData;

// Funzione eseguita dai thread
void *threadFunction(void *arg);

int knn(int rank, int size, char *data_filename, char *, float *test_point);

int compareDistances(const void *a, const void *b);

void openAndCheckCSVFiles(const char *data_filename, const char *label_filename, FILE **data_file, FILE **label_file);

int countTotalRows(FILE *file, char *line);

void checkEqualNumberRows(int total_rows_data, int total_rows_label);

void calculateRowRange(int rank, int size, int total_rows_data, int *start_row, int *end_row);

void allocateMatrix(int start_row, int end_row, int *num_columns, float **data_matrix, int **label_matrix, float **all_distances, int *num_rows);

void readAssignedRows(FILE *data_file, FILE *label_file, int start_row, int end_row, int num_columns, float *data_matrix, int *label_matrix, char *line_data, char *line_label);

void initializeThreads(pthread_t *threads, ThreadData *thread_data, int num_rows, int num_columns, float *data_matrix, int *label_matrix, float *test_point, float *all_distances);

void cleanupAndClose(FILE *data_file, FILE *label_file, float *data_matrix, int *label_matrix);


#endif // HEADER_THREAD_Hs
